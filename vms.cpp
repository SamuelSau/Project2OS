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

    // Calculate the secondary and primary cache sizes
    int s_size = (nFrames * p) / 100;
    int p_size = nFrames - s_size;

    // define data structures
    std::vector<PageTableEntry> pageTablePrimary;   // FIFO
    std::vector<PageTableEntry> pageTableSecondary; // LRU

    pageTablePrimary.reserve(p_size);
    pageTableSecondary.reserve(s_size);

    // Process trace file
    unsigned addr;
    char rw;

    while (file >> std::hex >> addr >> rw) // read the traces' address and read/write bit
    {
        totalAccesses++;
        
        // Extract page number and offset from address
        unsigned pageNum = addr / PAGE_SIZE;
        
        // Add the first entry to page table
        if (pageTablePrimary.empty())
        {   
            diskReads++;
            PageTableEntry newEntry;
            newEntry.addr = pageNum;
            newEntry.rw = rw;
            pageTablePrimary.push_back(newEntry); // most recently used page is at the end of the vector, therefore least recently used is at the beginning of vector
        }

        else if (pageTablePrimary.size() < p_size) // populate the page table
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
                    
                }
            }

            if (isFound == false) // if page is not in page table
            {
                diskReads++;
                PageTableEntry newEntry;
                newEntry.addr = pageNum;
                newEntry.rw = rw;
                pageTablePrimary.push_back(newEntry);
            }
        }

        else // fifo page table is full
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

            if (isFound == false) // if page is not in page table
            {
                
                if (pageTableSecondary.empty()) // if fifo is full and nothing in lru, then push front of vector to back of lru
                {
                    // push the front of pageTablePrimary to pageTableSecondary
                    diskReads++;
                    PageTableEntry entryToLRU;
                    auto itPrimary = pageTablePrimary.begin();
                    entryToLRU.addr = itPrimary->addr;
                    entryToLRU.rw = itPrimary->rw;
                    pageTablePrimary.erase(itPrimary); // delete the front of pageTablePrimary
                    pageTableSecondary.push_back(entryToLRU);         // add back to pageTableSecondary
                    // push the new page to pageTablePrimary
                    PageTableEntry newEntry;
                    newEntry.addr = pageNum;
                    newEntry.rw = rw;
                    pageTablePrimary.push_back(newEntry);
                }

                else if (pageTableSecondary.size() < s_size) // if lru is not full and fifo is full
                {
                    isFound = false;
                    // if page is in lru, then remove from lru and push back to fifo
                    for (int i = 0; i < s_size; i++)
                    {
                        if (pageTableSecondary[i].addr == pageNum)
                        {
                            PageTableEntry entryToPrimary;
                            auto itSecondary = pageTableSecondary.begin();
                            entryToPrimary.addr = pageTableSecondary[i].addr;

                            if (rw == 'R' && pageTableSecondary[i].rw == 'W') // cannot overwrite a 'W' with a 'R', so change it
                            {
                                rw = 'W';
                            }

                            entryToPrimary.rw = pageTableSecondary[i].rw;

                            if (rw == 'W')
                            {
                                entryToPrimary.rw = rw;
                                
                            }
                            
                            pageTableSecondary.erase(itSecondary + i);
                            pageTablePrimary.push_back(entryToPrimary);
                            isFound = true;
                            break;
                        }
                    }
                    // page not found in lru, then push front of fifo to lru and new push back to fifo
                    if (isFound == false)
                    {
                        
                        diskReads++;
                        PageTableEntry entryToLRU;
                        auto itPrimary = pageTablePrimary.begin();
                        entryToLRU.addr = itPrimary->addr;
                        entryToLRU.rw = itPrimary->rw;
                        pageTablePrimary.erase(itPrimary); // delete the front of pageTablePrimary
                        pageTableSecondary.push_back(entryToLRU);         // add back to pageTableSecondary
                        // push the new page to pageTablePrimary
                        PageTableEntry newEntry;
                        newEntry.addr = pageNum;
                        newEntry.rw = rw;
                        pageTablePrimary.push_back(newEntry);
                    }
                }
                else // pageTableSecondary.size() >= s_size
                {
                    isFound = false;
                    // if page is in lru, then remove from lru and push to fifo
                    for (int i = 0; i < pageTableSecondary.size(); i++)
                    {
                        if (pageTableSecondary[i].addr == pageNum) // if in lru table
                        {
                            PageTableEntry entryToPrimary;
                            auto itSecondary = pageTableSecondary.begin();
                            entryToPrimary.addr = pageTableSecondary[i].addr;

                            if (rw == 'R' && pageTableSecondary[i].rw == 'W')
                            {
                                rw = 'W';
                            }

                            entryToPrimary.rw = pageTableSecondary[i].rw;
                            
                            if (rw == 'W')
                            {
                                entryToPrimary.rw = rw;
                            }

                            pageTableSecondary.erase(itSecondary + i);
                            pageTablePrimary.push_back(entryToPrimary);

                            // before push back to pageTablePrimary, need to push front of pageTablePrimary to pageTableSecondary
                            PageTableEntry entryToLRU;
                            auto itPrimary = pageTablePrimary.begin();
                            entryToLRU.addr = itPrimary->addr;
                            entryToLRU.rw = itPrimary->rw;
                            pageTablePrimary.erase(itPrimary); // delete the front of pageTablePrimary
                            pageTableSecondary.push_back(entryToLRU);         // add back to pageTableSecondary
  
                            isFound = true;
                            break;
                        }
                    }
                    // page not found in lru, then push front of fifo to lru and new push back to fifo
                    if (isFound == false)
                    {
                        if (pageTableSecondary[0].rw == 'W')
                        {
                            diskWrites++;
                        }
                        
                        diskReads++;
                        PageTableEntry entryToLRU;
                        auto itPrimary = pageTablePrimary.begin();
                        auto itSecondary = pageTableSecondary.begin();
                        entryToLRU.addr = itPrimary->addr;
                        entryToLRU.rw = itPrimary->rw;
                        pageTableSecondary.erase(itSecondary); // delete the front of pageTablePrimary
                        // push the new page to pageTablePrimary
                        pageTablePrimary.erase(itPrimary);
                        pageTableSecondary.push_back(entryToLRU); // add back to pageTableSecondary
                        PageTableEntry newEntry;
                        newEntry.addr = pageNum;
                        newEntry.rw = rw;
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
    std::cout << "Total event traces: " << totalAccesses << std::endl;
    std::cout << "Total disk reads: " << diskReads<< std::endl;
    std::cout << "Total disk writes: " << diskWrites<< std::endl;

    file.close();
}