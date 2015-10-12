#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;

int main(int argc, char* argv[]) {

    ifstream is(argv[1],  ios::in  | ios::binary);
    ofstream os("1GB_out.dat", ios::out | ios::binary | ios::trunc);

    if (!is.good()) {
        cout << "Could not open input file: " << argv[1] << ". Exiting." << endl;
        exit(1);
    }

    int filesize = 1024*1024*1024;

    std::cout << "Reading data into buffer" << std::endl;
    vector<char> chars(filesize);                 // init vector array
    is.read(&chars[0], filesize);                 // read data into vector array
    std::string str(&chars[0], filesize);         // create string from vector array
    std::istringstream iss(str);                  // create istringstream from string
    char *chars_out = new char[filesize];

    std::cout << "Starting test" << std::endl;
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    iss.read(&chars_out[0], filesize);

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Stopping test" << std::endl;
    auto duration =  std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    std::cout << "Time taken: " << duration << " us" << std::endl;

    return 0;
}