//g++ memsim.cpp lru.cpp fifo.cpp vms.cpp -o memsim
//memsim.exe bzip.trace 64 vms 50 quiet 

#include "dataStructures.hpp"
#include "policies.hpp"
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

using namespace std;

int main(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc < 4 || argc > 6)
    {
        cout << "Usage: memsim.exe <tracefile> nFrames algo p quiet/debug" << endl;
        exit(1);
    }
    // Parse the arguments
    string traceFile = argv[1];
    int nFrames = atoi(argv[2]);
    string algo = argv[3];
    int p = 0; // represents the percentage for vms
    bool debug = false;
    
    if (algo == "vms" && argc == 6)
    {
        p = atoi(argv[4]);
    }

    else if (argc == 6) //for vms debug
    {
        if (strcmp(argv[5], "debug") == 0) {
            debug = true;
        }
    }

    else if (argc == 5){ //for fifo and lru debug
        if (strcmp(argv[4], "debug") == 0) {
            debug = true;
        }
    }

    // Calculate the secondary and primary sizes
    int s_size = (nFrames * p) / 100;
    int p_size = nFrames - s_size;

    // Run algo based on the command line arguments
    if (algo == "fifo")
    {
        fifo(traceFile, nFrames, debug);
    }
    else if (algo == "lru")
    {
        lru(traceFile, nFrames, debug);
    }

    else if (algo == "vms")
    {
        if (p == 0 || p == 1 || s_size == 0)
        { // if p is 0, then degererate to FIFO
            fifo(traceFile, nFrames, debug);
        }
        else if (p == 100 || p == 99 || p_size == 0)
        { // if p is 100, then degererate to LRU
            lru(traceFile, nFrames, debug);
        }
        else
        { // otherwise do segmented fifo algorithm
            vms(traceFile, nFrames, p, debug);
        }
    }
    else
    {
        cout << "Invalid algorithm..." << endl;
        return 1;
    }
    return 0;
}