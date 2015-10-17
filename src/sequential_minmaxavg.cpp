#include <iostream>
#include <fstream>
#include <limits>
#include <chrono>

using namespace std;

double getAverage(double sum, double count) {
    if (count == 0) return 0;
    return sum / count;
};

int main(int argc, char* argv[]) {

    ifstream is(argv[1],  ios::in  | ios::binary);

    int filesize = 1024*1024*1024;

    std::cout << "Starting nbuf" << std::endl;
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    std::cout << "Reading data into buffer" << std::endl;
    char* chars = new char[filesize];
    is.read(chars, filesize);

    int64_t count = 0;
    int64_t sum = 0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < filesize; i++) {
        double l = (double) chars[i];
        sum += l;
        count++;
        if (l > max)
                max = l;
        if (l < min)
            min = l;
    }

    std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();
    std::cout << "Stopping nbuf" << std::endl;

    auto duration1 =  std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    std::cout << "Time taken: " << duration1 << " us" << std::endl;
    auto duration2 =  std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
    std::cout << "Time taken: " << duration2 << " us" << std::endl;


    cout << "Accumulator average is: " << getAverage(sum, count) << std::endl
        << count << " items were counted." << std::endl
        << min << " was the lowest value." << std::endl
        << max << " was the highest value." << std::endl;


    return 0;
}
