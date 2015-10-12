#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

int main(int argc, char* argv[]) {

    ifstream is("data/1GB.dat",  ios::in  | ios::binary);
    ofstream os(argv[1],  ios::in  | ios::binary);

    if (!os.good()) {
        cout << "Could not open output file: " << argv[1] << ". Exiting." << endl;
        exit(1);
    }

    int filesize = 1024*1024*1024;
    char *chars = new char[filesize];
    is.read(chars, filesize);

    std::cout << "Starting test" << std::endl;
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    os.write(chars, filesize);

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Stopping test" << std::endl;
    auto duration =  std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    std::cout << "Time taken: " << duration << " us" << std::endl;

    return 0;
}