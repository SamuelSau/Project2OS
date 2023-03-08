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

int main(int argc, char* argv[]){

    // Check for correct number of arguments
    if(argc < 4 || argc > 6) {
        cout << "Usage: ./memsim <tracefile> nFrames algo p quiet/debug" << endl;
        return 1;
    }
    // Parse the arguments
    string traceFile = argv[1];
    int nFrames = atoi(argv[2]);
    string algo = argv[3];
    int p = 0; //represents the percentage for vms
    bool debug = false;

    //Calculate the secondary and primary sizes
    int s_size = (nFrames * p) / 100; 
    int p_size = nFrames - s_size;
    
    if(algo == "vms" && argc == 6) {
        p = atoi(argv[4]);
    } 
    else if(argc == 6) {
        debug = (strcmp(argv[5], "debug") == 0); 
    }
    
    //Run algo based on the command line arguments
    if(algo == "fifo") {
        fifo(traceFile, nFrames, debug);

    } 
    else if(algo == "lru") {
        lru(traceFile, nFrames, debug);
    } 
    
    else if(algo == "vms") {
        if ( p == 0 || s_size == 0) {
            fifo(traceFile, nFrames, debug);
        } 
        else if (p == 100  || p_size == 0){
            lru(traceFile, nFrames, debug);
        } 
        else{
            vms(traceFile, nFrames, debug, p); 
            }
    }
    else {
        cout << "Not valid algorithm!" << endl;
        return 1;
    }
    return 0;
}