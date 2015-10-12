#include <iostream>
#include <cstdint>

class VirtualAccumulator {
    public:
        virtual void accumulate(char *data);
        virtual void combine(const VirtualAccumulator *acc2);
};

class VirtualNbufConfig {
    public:
        virtual VirtualAccumulator* accumulatorFactory();
        virtual void output_filter(std::ostream *os, char *data);

        int     threads = 3;                        // How many threads to use
        int64_t available_megabytes = 100;          // How much available RAM to use
        int     datasize = 1024;                    // Size of the chunks used by the accumulator-method
        bool    use_sequential_execution = false;   // Use sequential execution
        bool    use_output_filter = false;          // Use the output filter
};

class Nbuf {
    public:
        static VirtualAccumulator* run(std::istream &is, std::ostream &os, VirtualNbufConfig *config);
        static VirtualAccumulator* worker(std::istream *is, std::ostream *os, VirtualAccumulator* shared_acc, VirtualNbufConfig *config);
};