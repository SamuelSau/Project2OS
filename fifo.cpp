#include "policies.hpp"
#include "dataStructures.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>

#define PAGE_SIZE 4096 // 4KB -> 2^12 page size

using namespace std;

void fifo(std::string traceFile, int nFrames, int debugMode)
{
    // Open trace file
    std::ifstream file(traceFile);

    if (!file)
    {
        cout << "Error: could not open trace file" << endl;
        exit(1);
    }

    // Initialize statistics
    int totalAccesses = 0, diskReads = 0, diskWrites = 0;
    bool inPage = false;

    // Process trace file
    unsigned addr;
    char rw;

    // define data structures
    std::vector<PageTableEntry> pageTable;

    // Initialize pageTable with based on number of frames
    pageTable.resize(nFrames);

    // Initialize pageTable entries with valid = false and frame = i
    for (int i = 0; i < nFrames; i++)
    {
        pageTable[i].rw = 'n';
        pageTable[i].addr = 0;
    }

    while (file >> std::hex >> addr >> rw)
    {
        totalAccesses++;

        // Extract page number and offset from address
        unsigned pageNum = addr / PAGE_SIZE;

        // Check if page is already in page table
        for (int i = 0; i < nFrames; i++)
        {
            if (pageTable.size() < nFrames)
            {
                if (pageTable[i].addr == pageNum) // if page is in page table
                {
                    inPage = true;

                    if (rw == 'W') // overwrite regardless of R or W  in page table
                    {
                        pageTable[i].rw = rw;
                    }
                }
                // create new page table entry and add it back to page table
                PageTableEntry newEntry;
                newEntry.addr = pageNum;
                newEntry.rw = rw;
                pageTable.push_back(newEntry);
        }
            else // if page.size() >= nFrames,
            {
                for (int i = 0; i < nFrames; i++)
                {
                    if (pageTable[i].addr == pageNum) //check again if page is in page table
                    {
                        inPage = true;

                        if (rw == 'W')
                        {
                            pageTable[i].rw = rw;
                        }
                    }
                    if (inPage)
                    {
                        continue;
                    }
                    else {
                        pageTable.erase(pageTable.begin()); //remove first element in page table
                        PageTableEntry newEntry;
                        newEntry.addr = pageNum;
                        newEntry.rw = rw;
                        pageTable.push_back(newEntry);
                    }
                }
            }

            
        }

        // Print debug information if requested
    if (debugMode)
        {   
        std::cout << "Address: " << addr << "RW: "<< rw <<  std::endl;
        }
    }


    

    // Print final statistics
    std::cout << "Total event traces: " << totalAccesses << std::endl;
    std::cout << "Total disk reads: " << diskReads << std::endl;
    std::cout << "Total disk writes: " << diskWrites << std::endl;

    file.close();
}
