#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <chrono>

#include "nbuf.hpp"

using namespace std;

class Accumulator : public VirtualAccumulator {
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
        };
        double getAverage() {
            if (count == 0) return 0;
            return sum / count;
        };
        friend ostream& operator<<(ostream &o, Accumulator &acc) {
            o << "Accumulator average is: " << acc.getAverage() << std::endl
                << acc.count << " items were counted." << std::endl
                << acc.min << " was the lowest value." << std::endl
                << acc.max << " was the highest value.";
            return o;
        };
        void combine(const VirtualAccumulator *vacc) {
            Accumulator *acc = (Accumulator*) vacc;
            count += acc->count;
            sum += acc->sum;
            if (max < acc->max)
                max = acc->max;

            if (min > acc->min)
                min = acc->min;
        }

        int64_t count = 0;
        double sum = 0;
        double min = std::numeric_limits<double>::max();
        double max = std::numeric_limits<double>::min();
};


class NbufConfig : public VirtualNbufConfig {
    public:
        VirtualAccumulator* accumulatorFactory() {
            return new Accumulator();
        }
};

bool stringToBool(char *c) {
    char ch = c[0];
    return ch == 't';
}

int main(int argc, char* argv[]) {
    if(argc < 3) {
        std::cout << "Please supply number of threads, memory limit and sequential execution bool" << std::endl;
        return 0;
    }

    string in =  "data/1GB.dat";
    string out = "1GB_out.dat";

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


    NbufConfig config;
    config.threads = stoi(argv[1]);
    config.available_megabytes = stoi(argv[2]);
    config.use_sequential_execution = stringToBool(argv[3]);

    std::cout << "Starting nbuf" << std::endl;
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    // RUNNING nbuf
    Nbuf nbuf;
    Accumulator *acc = (Accumulator*) nbuf.run(is, os, &config);

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Stopping nbuf" << std::endl;
    auto duration =  std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    std::cout << "Time taken: " << duration << " us" << std::endl;

    cout << *acc << endl;
    return 0;
}
