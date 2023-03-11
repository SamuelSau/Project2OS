#include "policies.hpp"
#include "dataStructures.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#define PAGE_SIZE 4096 // 4KB -> 2^12 page size

using namespace std;

 void vms(std::string traceFile, int nFrames, int p, bool debugMode)
 {
    // Open trace file
    std::ifstream file(traceFile);

    if (!file)
    {
        printf("Error: could not open trace file\n");
        exit(1);
    }

    // Initialize statistics
    int totalAccesses = 0, diskReads = 0, diskWrites = 0;
    bool isFound = false;
    int pageIndex = 0;

    // Calculate the secondary and primary cache sizes
    int s_size = (nFrames * p) / 100;
    int p_size = nFrames - s_size;

    // define data structures
    std::vector<PageTableEntry> pageTablePrimary(p_size);   // FIFO
    std::vector<PageTableEntry> pageTableSecondary(s_size); // LRU

    // Process trace file
    unsigned addr;
    char rw;

    while (file >> std::hex >> addr >> rw) // read the traces' address and read/write bit
    {
        totalAccesses++;

        // Extract page number and offset from address
        unsigned pageNum = addr / PAGE_SIZE;

        if (pageTablePrimary.size() == p_size) // fifo page table is full
        {
            isFound = false;
            for (auto &page : pageTablePrimary)
            {
                if (page.addr == pageNum) // if page is in page table
                {
                    isFound = true;

                    if (rw == 'W')
                    {
                        page.rw = 'W';
                    }
                    break;
                }
            }
            if(isFound){
                continue;
            }

            else if (!isFound) // if page is not in page table
            {
                if (pageTableSecondary.size() == s_size)
                {
                    isFound = false;
                    // if page is in lru, then remove from lru and push to fifo
                    for (int i = 0; i < pageTablePrimary.size(); i++)
                    {
                        if (pageTableSecondary[i].addr == pageNum) // if in lru table
                        {

                            isFound = true;
                            pageIndex = i;

                            if (rw == 'W')
                            {
                                pageTableSecondary[i].rw = rw;
                            }

                            break;
                        }
                    }
                    if (isFound)
                    {
                        PageTableEntry entryToPrimary;
                        entryToPrimary.addr = pageTableSecondary[pageIndex].addr;
                        entryToPrimary.rw = pageTableSecondary[pageIndex].rw;

                        if (rw == 'R' && pageTableSecondary[pageIndex].rw == 'W')
                        {
                            entryToPrimary.rw = 'W';
                        }
                        
                        pageTableSecondary.erase(pageTableSecondary.begin() + pageIndex);
                        pageTablePrimary.push_back(entryToPrimary);

                        // before push back to pageTablePrimary, need to push front of pageTablePrimary to pageTableSecondary
                        PageTableEntry entryToLRU;
                        entryToLRU.addr = pageTablePrimary[0].addr;
                        entryToLRU.rw = pageTablePrimary[0].rw;
                        pageTablePrimary.erase(pageTablePrimary.begin()); // delete the front of pageTablePrimary
                        pageTableSecondary.push_back(entryToLRU);         // add back to pageTableSecondary
                    }
                    // page not found in lru, then push front of fifo to lru and new push back to fifo
                    else if (!isFound)
                    {
                        if (pageTableSecondary[0].rw == 'W')
                        {
                            diskWrites++;
                        }

                        diskReads++;

                        PageTableEntry entryToLRU;
                        entryToLRU.addr = pageTablePrimary[0].addr;
                        entryToLRU.rw = pageTablePrimary[0].rw;

                        PageTableEntry newEntry;
                        newEntry.addr = pageNum;
                        newEntry.rw = rw;

                        pageTableSecondary.erase(pageTableSecondary.begin()); // delete the front of pageTablePrimary
                        pageTablePrimary.erase(pageTablePrimary.begin());
                        pageTableSecondary.push_back(entryToLRU); // add back to pageTableSecondary
                        pageTablePrimary.push_back(newEntry);
                    }
                }
            }
        }
        if (debugMode)
        {
            std::cout << "Address: " << addr << " RW: " << rw << std::endl;
        }
    } // end of while loop

    // Print final statistics
    std::cout << "Trace Count: " << totalAccesses << std::endl;
    std::cout << "Read Count: " << diskReads << std::endl;
    std::cout << "Write Count: " << diskWrites << std::endl;

    file.close();
 }
