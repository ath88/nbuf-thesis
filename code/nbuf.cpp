#include <thread>
#include <future>
#include <mutex>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <cstdint>
#include <vector>
#include <chrono>

using namespace std;

class Accumulator {
    public:
        void accumulate(char *data) {
            for (int i = 0; i < 1024; i++) {
                double l = (double) data[i];
                sum += l;
                count++;
                if (l > max)
                    max = l;
                if (l < min)
                    min = l;
            }
            int acc = 0;
        };

        double getAverage() {
            if (count == 0) return 0;
            return sum / count;
        };

        friend Accumulator operator+=(Accumulator &acc1, const Accumulator &acc2) {
            acc1.count += acc2.count;
            acc1.sum += acc2.sum;
            if (acc1.max < acc2.max)
                acc1.max = acc2.max;

            if (acc1.min > acc2.min)
                acc1.min = acc2.min;
            return acc1;
        }

        friend ostream& operator<<(ostream &o, Accumulator &acc) {
            o << "Accumulator average is: " << acc.getAverage() << std::endl
                << acc.count << " items were counted." << std::endl
                << acc.min << " was the lowest value." << std::endl
                << acc.max << " was the highest value.";
            return o;
        };

    private:
        int64_t count = 0;
        double sum = 0;
        double min = std::numeric_limits<double>::max();
        double max = 0;
};

class VirtualOutputFilter {
    public:
        virtual void output_filter(ostream *os, char *data) {
            std::cout
                << "This is the default output filter. "
                << "You will want to overwrite this, "
                << "if you want to use the output filter. "
                << "Terminating program." << std::endl;
            exit(1);
            return;
        };
};

class nbuf {
    public:
        Accumulator run(
            istream &is, ostream &os, int threads,
            bool use_shared_acc, bool use_output_filter, VirtualOutputFilter *filter,
            int64_t available_megabytes, int datasize
        ) {
            int64_t available_bytes = available_megabytes * 1024 * 1024;
            int64_t bytes_per_worker = available_bytes / threads;

            std::future<Accumulator*> futures[threads];
            Accumulator acc;

            for (int i = 0; i < threads; i++) {
                std::packaged_task<Accumulator*()> task(
                    std::bind(&nbuf::worker,
                        &is, &os, &acc, bytes_per_worker, datasize,
                        use_shared_acc, use_output_filter, filter)); // wrap the function

                std::future<Accumulator*> future = task.get_future();  // get a future
                std::thread(std::move(task)).detach(); // launch on a thread
                futures[i] = std::move(future);
            }

            for (int i = 0; i < threads; i++) {
                try {
                    if (use_shared_acc) {
                        futures[i].get();
                    } else {
                        Accumulator *future_acc = futures[i].get();
                        acc += *future_acc;
                        delete future_acc;
                    }
                }
                catch (exception &e) {
                    cout << "Thread " << i << " threw an exception: " << e.what() << endl;;
                }
            }

            return acc;
        };

        static Accumulator* worker(istream *is, ostream *os,
            Accumulator *shared_acc, int64_t available_bytes, int datasize,
            bool use_shared_acc, bool use_output_filter, VirtualOutputFilter *filter) {
            static mutex im;
            static mutex om;
            static mutex shared_acc_mutex;
            Accumulator *acc;

            int64_t usable_bytes = (available_bytes / datasize) * datasize;

            char* data = new char[usable_bytes];

            if (use_shared_acc) {
                acc = shared_acc;
            } else {
                acc = new Accumulator;
            }

            bool eof = false;
            while(!eof) {
                int count = 0;
                {
                    lock_guard<mutex> lock(im);

                    is->read(data, usable_bytes);

                    if ( fmod((float)is->gcount(), datasize) != 0.f) {
                        std::cerr << "Warning: Data not aligned at finish" << std::endl;
                    }
                    count = is->gcount() / datasize;

        //            std::cout << "Read from buffer: " << count * datasize << " bytes" << std::endl;
                    if ( (is->rdstate() & std::ifstream::eofbit ) != 0 ) {
                        eof = true;
                    }

                }

                {
                    if (use_shared_acc) {
                        lock_guard<mutex> lock(shared_acc_mutex);
                        for (int i = 0; i < count; i++) {
                            acc->accumulate(&data[i * datasize]);
                        }
                    } else {
                        for (int i = 0; i < count; i++) {
                            acc->accumulate(&data[i * datasize]);
                        }
                    }
                }

                {
                    lock_guard<mutex> lock(om);
                    if (use_output_filter) {     // using write_hook
                        for (int i = 0; i < count; i++) {
                            filter->output_filter(os, &data[i * datasize]);
                        }
                    } else {
                        os->write(data, count * datasize);
                    }
                    os->flush();
        //            cout << "Wrote to buffer: " << count * datasize << " bytes" << std::endl;
                }

                if (eof) break;
            }

            return acc;
        };
};

main(int argc, char* argv[]) {
    if(argc < 3) {
        std::cout << "Please supply number of threads, and input file." << std::endl;
        return 0;
    }

    string in = argv[2];
    string out = "out.dat";
    if (argc > 3) out = argv[3];

    ifstream is(in,  ios::in  | ios::binary);
    ofstream os(out, ios::out | ios::binary | ios::trunc);

    if (!is.good()) {
        cout << "Could not open input file. Exiting." << endl;
        exit(1);
    }
    if (!os.good()) {
        cout << "Could not open output file. Exiting." << endl;
        exit(1);
    }

    int64_t filesize = 1024*1024*1024;
    filesize = filesize * 1;

    std::cout << "Reading data into buffer" << std::endl;
    vector<char> chars(filesize);                 // init vector array
    is.read(&chars[0], filesize);                 // read data into vector array
    std::string str(&chars[0], filesize);         // create string from vector array
    std::istringstream iss(str);                  // create istringstream from string
    std::ostringstream oss;                       // create empty ostringstream


    // SETUP nbuf
    int threads = stoi(argv[1]);                  // number of threads to use
    bool use_shared_acc = false;                  // use a shared accumulator
    bool use_output_filter = false;               // use an output filter
    VirtualOutputFilter *filter = (VirtualOutputFilter*) nullptr; // use an output filter
    int available_megabytes = 500;                // available megabytes on system
    int datasize = 1024;                          // bytes per calculation unit



    std::cout << "Starting nbuf" << std::endl;
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    // RUNNING nbuf
    nbuf nbuf;
    Accumulator acc = nbuf.run(iss, oss, threads, use_shared_acc, use_output_filter, filter, available_megabytes, datasize);

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Stopping nbuf" << std::endl;
    auto duration =  std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    std::cout << "Time taken: " << duration << " us" << std::endl;


//    skip writing output
//    os << oss.str();
    cout << acc << endl;

    return 0;
}