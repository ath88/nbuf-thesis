#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

int main(int argc, char* argv[]) {

    ifstream is(argv[1],  ios::in  | ios::binary);
    ofstream os(argv[2],  ios::in  | ios::binary  | ios::trunc);

    if (!is.good()) {
        cout << "Could not open input file: " << argv[1] << ". Exiting." << endl;
        exit(1);
    }
    if (!os.good()) {
        cout << "Could not open output file: " << argv[2] << ". Exiting." << endl;
        exit(1);
    }

    int64_t filesize = 1024*1024*1024;
    filesize *= 1;
    char *chars = new char[filesize];

    // input-test
    std::cout << "Starting input-test" << std::endl;
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    is.read(chars, filesize);

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Stopping input-test" << std::endl;

    // output-test
    std::cout << "Starting output-test" << std::endl;
    std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();

    os.write(chars, filesize);

    std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();
    std::cout << "Stopping output-test" << std::endl;

    auto duration1 =  std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    std::cout << "Input-test time taken: " << duration1 << " us" << std::endl;
    auto duration2=  std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
    std::cout << "Output-test time taken: " << duration2 << " us" << std::endl;

    return 0;
}