#include "policies.hpp"
#include "dataStructures.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>


#define PAGE_SIZE 4096 // 4KB -> 2^12 page size

using namespace std;

void lru(std::string traceFile, int nFrames, int debugMode)
{
    // Open trace file
    std::ifstream file(traceFile);

    if (!file)
    {
        printf("Error: could not open trace file\n");
        exit(1);
    }

    // Initialize statistics
    int totalAccesses = 0, diskReads = 0, diskWrites = 0, inPage = 0, pageCount = 0;

    // Process trace file
    // unsigned addr;
    // char rw;

    std::string line;
    while (std::getline(file, line)) {
    std::cout << line << std::endl;
    }
    // Print final statistics
    std::cout << "Total event traces: " << totalAccesses << std::endl;
    std::cout << "Total disk reads: " << diskReads << std::endl;
    std::cout << "Total disk writes: " << diskWrites << std::endl;

    file.close();
}
