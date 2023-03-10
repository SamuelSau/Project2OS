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

void vms(std::string traceFile, int nFrames, int p, int debugMode)
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

    // Calculate the secondary and primary sizes
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

        // Add the first entry to page table
        if (pageTablePrimary.size() == 0)
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

            for (int i = 0; i < pageTablePrimary.size(); i++)
            {
                if (pageTablePrimary[i].addr == pageNum) // if page is in page table
                {
                    isFound = true;

                    if (rw == 'W')
                    {
                        pageTablePrimary[i].rw = rw;
                    }

                    break;
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

            for (int i = 0; i < pageTablePrimary.size(); i++)
            {
                if (pageTablePrimary[i].addr == pageNum) // if page is in page table
                {
                    isFound = true;
                    if (rw == 'W')
                    {
                        pageTablePrimary[i].rw = rw;
                    }
                    break;
                }
            }

            if (isFound == false) // if page is not in page table
            {
                if (pageTableSecondary.size() == 0) // if fifo is full and nothing in lru, then push front of vector to back of lru
                {
                    if (pageTablePrimary[0].rw == 'W')
                    {
                        diskWrites++;
                    }

                    // push the front of pageTablePrimary to pageTableSecondary
                    PageTableEntry entryToLRU;
                    entryToLRU.addr = pageTablePrimary[0].addr;
                    entryToLRU.rw = pageTablePrimary[0].rw;
                    pageTablePrimary.erase(pageTablePrimary.begin()); // delete the front of pageTablePrimary
                    pageTableSecondary.push_back(entryToLRU);         // add back to pageTableSecondary
                    // push the new page to pageTablePrimary
                    PageTableEntry newEntry;
                    newEntry.addr = pageNum;
                    newEntry.rw = rw;
                    pageTablePrimary.push_back(newEntry);
                    diskReads++;
                }

                else if (pageTableSecondary.size() < s_size) // if lru is not full and fifo is full
                {
                    // if page is in lru, then remove from lru and push to fifo
                    for (int i = 0; i < pageTableSecondary.size(); i++)
                    {
                        if (pageTableSecondary[i].addr == pageNum)
                        {
                            PageTableEntry entryToPrimary;
                            entryToPrimary.addr = pageTableSecondary[i].addr;

                            if (rw == 'R' && pageTableSecondary[i].rw == 'W')
                            {
                                rw = 'W';
                            }

                            entryToPrimary.rw = pageTableSecondary[i].rw;
                            pageTableSecondary.erase(pageTableSecondary.begin() + i);
                            pageTablePrimary.push_back(entryToPrimary);
                            isFound = true;
                            break;
                        }
                    }
                    // page not found in lru, then push front of fifo to lru and new push back to fifo
                    if (isFound == false)
                    {
                        if (pageTablePrimary[0].rw == 'W')
                        {
                            diskWrites++;
                            
                        }

                        PageTableEntry entryToLRU;
                        entryToLRU.addr = pageTablePrimary[0].addr;
                        entryToLRU.rw = pageTablePrimary[0].rw;
                        pageTablePrimary.erase(pageTablePrimary.begin()); // delete the front of pageTablePrimary
                        pageTableSecondary.push_back(entryToLRU);         // add back to pageTableSecondary
                        // push the new page to pageTablePrimary
                        PageTableEntry newEntry;
                        newEntry.addr = pageNum;
                        newEntry.rw = rw;
                        pageTablePrimary.push_back(newEntry);
                        diskReads++;
                    }
                }
                else // pageTableSecondary.size() >= s_size
                {
                    // if page is in lru, then remove from lru and push to fifo
                    for (int i = 0; i < pageTableSecondary.size(); i++)
                    {
                        if (pageTableSecondary[i].addr == pageNum)
                        {
                            PageTableEntry entryToPrimary;
                            entryToPrimary.addr = pageTableSecondary[i].addr;

                            if (rw == 'R' && pageTableSecondary[i].rw == 'W')
                            {
                                rw = 'W';
                            }

                            entryToPrimary.rw = pageTableSecondary[i].rw;
                            pageTableSecondary.erase(pageTableSecondary.begin() + i);
                            
                            //before push back to pageTablePrimary, need to push front of pageTablePrimary to pageTableSecondary
                            PageTableEntry entryToLRU;
                            entryToLRU.addr = pageTablePrimary[0].addr;
                            entryToLRU.rw = pageTablePrimary[0].rw;
                            pageTablePrimary.erase(pageTablePrimary.begin()); // delete the front of pageTablePrimary
                            pageTableSecondary.push_back(entryToLRU);         // add back to pageTableSecondary
                            
                            pageTablePrimary.push_back(entryToPrimary);
                            isFound = true;
                            break;
                        }
                    }
                    // page not found in lru, then push front of fifo to lru and new push back to fifo
                    if (isFound == false)
                    {
                        // if (pageTablePrimary[0].rw == 'W')
                        // {
                        //     diskWrites++;
                        // }

                        // if(pageTableSecondary[0].rw == 'W'){
                        //     diskWrites++;
                        // }

                        PageTableEntry entryToLRU;
                        entryToLRU.addr = pageTablePrimary[0].addr;
                        entryToLRU.rw = pageTablePrimary[0].rw;
                        pageTableSecondary.erase(pageTableSecondary.begin()); // delete the front of pageTablePrimary
                        // push the new page to pageTablePrimary
                        pageTablePrimary.erase(pageTablePrimary.begin());
                        pageTableSecondary.push_back(entryToLRU); // add back to pageTableSecondary
                        PageTableEntry newEntry;
                        newEntry.addr = pageNum;
                        newEntry.rw = rw;
                        pageTablePrimary.push_back(newEntry);
                        diskReads++;
                    }
                }
            }
        }

    } // end of while loop

    /*

    Notes:
    *When fifo (primary) full, push to lru (secondary)
    *When lru (secondary) full, then you have to evict a page from lru (secondary) (literally front of secondary)
    *If fifo is full and you found page in LRU, then remove and push back to fifo
    *If 'w', overwrite the page either in fifo or lru
    *if you find page in fifo, do nothing
    * if the operation of page is 'r' and the page operation is 'w', then make sure to change the operation to 'w'

    */

    // Print final statistics
    std::cout << "Total event traces: " << totalAccesses << std::endl;
    std::cout << "Total disk reads: " << diskReads + 1 << std::endl;
    std::cout << "Total disk writes: " << diskWrites << std::endl;

    file.close();
}
