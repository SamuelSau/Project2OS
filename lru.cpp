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

void lru(std::string traceFile, int nFrames, int debugMode)
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
    int pageIndex = 0;

    // Process trace file
    unsigned addr;
    char rw;

    // define data structures
    std::vector<PageTableEntry> pageTable;

    while (file >> std::hex >> addr >> rw) // read the traces' address and read/write bit
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
            pageTable.push_back(newEntry); // most recently used page is at the end of the vector, therefore least recently used is at the beginning of vector
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
            if (isFound) 
            {
                PageTableEntry deletedEntry;
                deletedEntry.addr = pageTable[pageIndex].addr;
                deletedEntry.rw = pageTable[pageIndex].rw;
                pageTable.erase(std::find(pageTable.begin(), pageTable.end(), pageNum)); 
                pageTable.push_back(deletedEntry);                                       
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
                    pageIndex = i;
                    if (rw == 'W') // overwrite again
                    {
                        pageTable[i].rw = rw;
                    }
                    break;
                }
            }
            if (isFound)
            { // MOVE FROM WHEREVER FOUND TO MOST RECENTLY USED
                PageTableEntry deletedEntry;
                deletedEntry.addr = pageTable[pageIndex].addr;
                deletedEntry.rw = pageTable[pageIndex].rw;
                pageTable.erase(std::find(pageTable.begin(), pageTable.end(), pageNum)); 
                pageTable.push_back(deletedEntry);                                       
                continue;
            }
            else
            {
                
                diskReads++;
                PageTableEntry newEntry;
                newEntry.addr = pageTable[0].addr;
                newEntry.rw = pageTable[0].rw;
                pageTable.push_back(newEntry);
                pageTable.erase(pageTable.begin());

            }
        }
        // Print debug information if requested
        if (debugMode)
        {
            std::cout << "Address: " << addr << "RW: " << rw << std::endl;
        }

    } // end of while loop

    // Print final statistics
    std::cout << "Total event traces: " << totalAccesses << std::endl;
    std::cout << "Total disk reads: " << diskReads << std::endl;
    std::cout << "Total disk writes: " << diskWrites << std::endl;

    file.close();
}
