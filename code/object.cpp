#include <thread>
#include <future>
#include <mutex>
#include <iostream>
#include <fstream>

using namespace std;

class Accumulator {
    public:
        void accumulate(long l) {
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
            data++;
            acc.accumulate(data);
        };
        const int getData () { return data; };
        friend istream& operator>>(istream &i, Unit &unit) {
            i.read((char *) &unit.data, 2);
            return i;
        };
        friend ostream& operator<<(ostream &o, const Unit &unit) {
            o.write((char *) &unit.data, 2);
            return o;
        };
    private:
        int data = 0;
};

const int THREADS = 3;
const int megabytes = 20;



// below is boilerplate
const int bytes = megabytes * 1024 * 1024;
const int units_per_worker = bytes / sizeof(Unit) / 3;

static mutex im;
static mutex om;
static mutex cm;

Accumulator worker(istream *is, ostream *os) {
    Accumulator acc;
    array<Unit, units_per_worker> units;

    bool eof = false;
    while(!eof) {
        int count = 0;
        {
            lock_guard<mutex> il(im);
            for (Unit &u: units) {
                *is >> u;
                count++;
                if (is->eof()) {
                    eof = true;
                    count--;
                    break;
                }
            }
//            lock_guard<mutex> cl(cm);
//            for (int i = 0; i < count; i++) cout << "Before: " << units[i] << endl;
        }

        {
            for (int i = 0; i < count; i++) units[i].f(acc);
        }

        {
//            lock_guard<mutex> cl(cm);
//            for (int i = 0; i < count; i++) cout << "After: " << units[i] << endl;
            lock_guard<mutex> ol(om);
        	for (int i = 0; i < count; i++) *os << units[i];
        }

        if (eof) break;
    }

    return acc;
};

Accumulator interface(istream &is, ostream &os) {
    array<future<Accumulator>, THREADS> futures;

    for (int i = 0; i < THREADS; i++) {
        std::packaged_task<Accumulator()> task(std::bind(worker, &is, &os)); // wrap the function
        std::future<Accumulator> future = task.get_future();  // get a future
        std::thread(std::move(task)).detach(); // launch on a thread
        futures[i] = std::move(future);
    }

    Accumulator acc;
    for (int i = 0; i < THREADS; i++) {
    	try {
        	acc += futures[i].get();
    	}
    	catch (exception &e) {
    		cout << "Thread " << i << " threw an exception: " << e.what() << endl;;
    	}
    }

    return acc;
}

main() {
    ifstream is("data.in",  ios::in  | ios::binary);
    ofstream os("data.out", ios::out | ios::binary | ios::trunc);

    if (!is.is_open()) {
        exit(1);
    }
    if (!os.is_open()) {
        exit(1);
    }

    Accumulator acc = interface(is, os);
    cout << acc << endl;;
}