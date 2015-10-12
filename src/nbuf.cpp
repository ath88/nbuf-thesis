#include <thread>
#include <future>
#include <mutex>
#include <iostream>
#include <cmath>
#include <cstdint>

#include "nbuf.hpp"

void VirtualAccumulator::accumulate(char *data) {
    return;
};

void VirtualAccumulator::combine(const VirtualAccumulator *acc2) {
    return;
};

VirtualAccumulator* VirtualNbufConfig::accumulatorFactory() {
    return new VirtualAccumulator();
};

void VirtualNbufConfig::output_filter(std::ostream *os, char *data) {
    std::cout
        << "This is the default output filter. "
        << "You will want to overwrite this, "
        << "if you want to use the output filter. "
        << "Terminating program." << std::endl;
    exit(1);
    return;
};

VirtualAccumulator* Nbuf::run(std::istream &is, std::ostream &os, VirtualNbufConfig *config) {
    std::future<VirtualAccumulator*> futures[config->threads];
    VirtualAccumulator *acc = config->accumulatorFactory();

    for (int i = 0; i < config->threads; i++) {
        std::packaged_task<VirtualAccumulator*()> task(
            std::bind(&Nbuf::worker, &is, &os, acc, config)); // wrap the function

        std::future<VirtualAccumulator*> future = task.get_future();  // get a future
        std::thread(std::move(task)).detach(); // launch on a thread
        futures[i] = std::move(future);
    }

    for (int i = 0; i < config->threads; i++) {
        try {
            if (config->use_sequential_execution) {
                futures[i].get();
            } else {
                VirtualAccumulator *future_acc = futures[i].get();
                acc->combine(future_acc);
                delete future_acc;
            }
        }
        catch (std::exception &e) {
            std::cout << "Thread " << i << " threw an exception: " << e.what() << std::endl;;
        }
    }

    return acc;
};

VirtualAccumulator* Nbuf::worker(std::istream *is, std::ostream *os, VirtualAccumulator* shared_acc, VirtualNbufConfig *config) {
    static std::mutex im;
    static std::mutex om;
    static std::mutex shared_acc_mutex;
    VirtualAccumulator *acc = config->accumulatorFactory();

    int64_t available_bytes = config->available_megabytes * 1024 * 1024;
    int64_t bytes_per_worker = available_bytes / config->threads;
    int64_t usable_bytes = (bytes_per_worker / config->datasize) * config->datasize;

    if (usable_bytes == 0) {
        std::cout << "The memory allocated to each thread is too small to load data into. Give more memory for nbuf" << std::endl;
        exit(1);
    }

    char* data = new char[usable_bytes];

    if (config->use_sequential_execution) {
        acc = shared_acc;
    } else {
        acc = config->accumulatorFactory();
    }

    bool eof = false;
    while(!eof) {
        int count = 0;
        {
            std::lock_guard<std::mutex> lock(im);

            is->read(data, usable_bytes);

            if ( fmod((float)is->gcount(), config->datasize) != 0.f) {
                std::cerr << "Warning: Data not aligned at finish" << std::endl;
            }
            count = is->gcount() / config->datasize;

//            std::cout << "Read from buffer: " << count * datasize << " bytes" << std::endl;
            if ( (is->rdstate() & std::istream::eofbit ) != 0 ) {
                eof = true;
            }

        }

        {
            if (config->use_sequential_execution) {
                std::lock_guard<std::mutex> lock(shared_acc_mutex);
                for (int i = 0; i < count; i++) {
                    acc->accumulate(&data[i * config->datasize]);
                }
            } else {
                for (int i = 0; i < count; i++) {
                    acc->accumulate(&data[i * config->datasize]);
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(om);
            if (config->use_output_filter) {     // using write_hook
                for (int i = 0; i < count; i++) {
                    config->output_filter(os, &data[i * config->datasize]);
                }
            } else {
                os->write(data, count * config->datasize);
            }
            os->flush();
//            cout << "Wrote to buffer: " << count * datasize << " bytes" << std::endl;
        }

        if (eof) break;
    }

    return acc;
};