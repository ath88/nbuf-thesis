#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

int main(int argc, char* argv[]) {

    ifstream is(argv[1],  ios::in  | ios::binary);

    if (!is.good()) {
        cout << "Could not open input file: " << argv[1] << ". Exiting." << endl;
        exit(1);
    }

    int filesize = 1024*1024*1024;
    char *chars = new char[filesize];

    std::cout << "Starting test" << std::endl;
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    is.read(chars, filesize);

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Stopping test" << std::endl;
    auto duration =  std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    std::cout << "Time taken: " << duration << " us" << std::endl;

    return 0;
}