#include <iostream>
#include <fstream>
#include <limits>

using namespace std;

double getAverage(double sum, double count) {
    if (count == 0) return 0;
    return sum / count;
};

int main(int argc, char* argv[]) {

    ifstream is(argv[1],  ios::in  | ios::binary);

    int filesize = 1024*1024*1024;

    std::cout << "Reading data into buffer" << std::endl;
    char* chars = new char[filesize];
    is.read(chars, filesize);

    int64_t count = 0;
    int64_t sum = 0;
    double min = std::numeric_limits<double>::max();
    double max = 0;

    for(int i = 0; i < filesize; i++) {
        double l = (double) chars[i];
        sum += l;
        count++;
        if (l > max)
                max = l;
        if (l < min)
            min = l;
    }

    cout << "Accumulator average is: " << getAverage(sum, count) << std::endl
        << count << " items were counted." << std::endl
        << min << " was the lowest value." << std::endl
        << max << " was the highest value." << std::endl;

    return 0;
}