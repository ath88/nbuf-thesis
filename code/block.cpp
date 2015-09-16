#include <thread>
#include <future>
#include <mutex>
#include <iostream>
#include <fstream>

using namespace std;

class Accumulator {
    public:
        void accumulate(char l) {
            sum += l;
            count++;
        };
        double getAverage() {
            if (count == 0) return 0;
            return sum / count;
        };
        friend Accumulator operator+=(Accumulator &acc1, const Accumulator &acc2) {
            acc1.count += acc2.count;
            acc1.sum += acc2.sum;
            return acc1;
        }
        friend ostream& operator<<(ostream &o, Accumulator &acc) {
            o << "Accumulator average is: " << acc.getAverage()
                << ". " << acc.count << " items were counted.";
            return o;
        };
    private:
        int count = 0;
        double sum = 0;
};

class Unit {
    public:
        void f(Accumulator &acc) {
//            data++;
            acc.accumulate(data);
        };

        friend istream& operator>>(istream &i, Unit &unit) {
            i.read((char *) &unit, sizeof(Unit));
            return i;
        };
        friend ostream& operator<<(ostream &o, const Unit &unit) {
            o.write((char *) &unit, sizeof(Unit));
            return o;
        };
    private:
        char data = 0;
//        int data2 = 0;
//        bool data3 = 0;
};

const bool shared_accumulator = false;
const int THREADS = 1;
const int megabytes = 300;

// below is boilerplate
const int bytes = megabytes * 1024 * 1024;
const int units_per_worker = bytes / sizeof(Unit) / THREADS;

static mutex im;
static mutex om;
static mutex cm;
static mutex shared_acc_mutex;

Accumulator* worker(istream *is, ostream *os, Accumulator *shared_acc) {
    Accumulator *acc;
    Unit* units = new Unit[units_per_worker];

    if (shared_accumulator) {
        acc = shared_acc;
    } else {
        acc = new Accumulator;
    }

    bool eof = false;
    while(!eof) {
        int count = 0;
        {
            lock_guard<mutex> lock(im);

            is->read((char*) units, sizeof(Unit) * units_per_worker);
            count = is->gcount() / sizeof(Unit);

            if ( (is->rdstate() & std::ifstream::eofbit ) != 0 ) {
                cout << "Done reading: " << count << endl;
                eof = true;
            }

        }

        {
            if (shared_accumulator) {
                lock_guard<mutex> lock(shared_acc_mutex);
                for (int i = 0; i < count; i++) {
                    units[i].f(*acc);
                }
            } else {
                for (int i = 0; i < count; i++) {
                    units[i].f(*acc);
                }
            }
        }

        {
            lock_guard<mutex> lock(om);
            os->write((char*) units, sizeof(Unit) * count);
        }

        if (eof) break;
    }
    return acc;
};

Accumulator run(istream &is, ostream &os) {
    array<future<Accumulator*>, THREADS> futures;

    Accumulator acc;

    for (int i = 0; i < THREADS; i++) {
        std::packaged_task<Accumulator*()> task(std::bind(worker, &is, &os, &acc)); // wrap the function
        std::future<Accumulator*> future = task.get_future();  // get a future
        std::thread(std::move(task)).detach(); // launch on a thread
        futures[i] = std::move(future);
    }

    for (int i = 0; i < THREADS; i++) {
    	try {
            if (shared_accumulator) {
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
}

main(int argc, char* argv[]) {
    string in = "in.dat";
    string out = "out.dat";

    cout << "Memory (in bytes) per unit: " << sizeof(Unit) << endl;
    cout << "Units per worker: " << units_per_worker << endl;
    cout << "Memory (in bytes) per worker: " << sizeof(Unit) * units_per_worker << endl;

    if (argc > 1) in = argv[1];
    if (argc > 2) out = argv[2];

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

    Accumulator acc = run(is, os);
    cout << acc << endl;
}