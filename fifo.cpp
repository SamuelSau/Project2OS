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

void fifo(std::string traceFile, int nFrames, bool debugMode)
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
    bool isFound = false;

    // Process trace file
    unsigned addr;
    char rw;

    // define data structures
    std::vector<PageTableEntry> pageTable;

    while (file >> std::hex >> addr >> rw) //read the traces' address and read/write bit
    {
        totalAccesses++;

        // Extract page number and offset from address
        unsigned pageNum = addr / PAGE_SIZE;

        // Add the first entry to page table
        if (pageTable.size() == 0)
        { 
            diskReads++;
            PageTableEntry newEntry;
            newEntry.addr = pageNum;
            newEntry.rw = rw;
            pageTable.push_back(newEntry);
        }

        else if (pageTable.size() < nFrames) // populate the page table
        {
            isFound = false;
            for (int i = 0; i < pageTable.size(); i++)
            {
                if (pageTable[i].addr == pageNum) // if page is in page table
                {
                    isFound = true;

                    if (rw == 'W') // overwrite with W regardless of R or W  in page table
                    {
                        pageTable[i].rw = rw;
                    }
                    break;
                }
            }
            if (isFound) //essentially go back to the for loop since we dont need to do anything
            {
                continue;
            }
            else
            {
                diskReads++;
                PageTableEntry newEntry;
                newEntry.addr = pageNum;
                newEntry.rw = rw;
                pageTable.push_back(newEntry);
            }
        }

        else // if pageTable.size() >= nFrames, when the pagetable is full and we need to do FIFO
        {
            isFound = false;
            for (int i = 0; i < pageTable.size(); i++)
            {
                if (pageTable[i].addr == pageNum) // check again if page is in page table, and its found
                {
                    isFound = true;

                    if (rw == 'W') // overwrite again
                    {
                        pageTable[i].rw = rw;
                    }
                    break;
                }
            }
            if (isFound)
            {
                continue;
            }
            else
            {
                if (pageTable[0].rw == 'W')
                {
                    diskWrites++;
                }
                // Not in page table
                pageTable.erase(pageTable.begin()); // remove first element in page table
                PageTableEntry newEntry;
                newEntry.addr = pageNum;
                newEntry.rw = rw;
                pageTable.push_back(newEntry);
                diskReads++;
            }
        }
        // Print debug information if requested
        if (debugMode)
        {
            std::cout << "Address: " << addr << " RW: " << rw << std::endl;
        }

    } // end of while loop

    // Print final statistics
    std::cout << "Total event traces: " << totalAccesses << std::endl;
    std::cout << "Total disk reads: " << diskReads << std::endl;
    std::cout << "Total disk writes: " << diskWrites << std::endl;

    file.close();
}
