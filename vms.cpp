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

    //Calculate the secondary and primary sizes
    int s_size = (nFrames * p) / 100; 
    int p_size = nFrames - s_size;

    // define data structures
    std::vector<PageTableEntry> pageTablePrimary(p_size); //FIFO
    std::vector<PageTableEntry> pageTableSecondary(s_size); //LRU

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

        else if (pageTablePrimary.size() < nFrames) // populate the page table
        {
            isFound = false;

            for (int i = 0; i < pageTablePrimary.size(); i++)
            {
                if (pageTablePrimary[i].addr == pageNum) // if page is in page table
                {
                    isFound = true;
                    break;
                }
            }

            if (!isFound) // if page is not in page table
            {
                diskReads++;
                PageTableEntry newEntry;
                newEntry.addr = pageNum;
                newEntry.rw = rw;
                pageTablePrimary.push_back(newEntry);
            }
        }

        else // page table is full
        {
            isFound = false;

            for (int i = 0; i < pageTablePrimary.size(); i++)
            {
                if (pageTablePrimary[i].addr == pageNum) // if page is in page table
                {
                    isFound = true;
                    break;
                }
            }

            if (!isFound) // if page is not in page table
            {
                diskReads++;
                PageTableEntry newEntry;
                newEntry.addr = pageNum;
                newEntry.rw = rw;
                pageTablePrimary.push_back(newEntry);
                pageTablePrimary.erase(pageTablePrimary.begin()); // remove the least recently used page
            }
        }

        if(pageTableSecondary.size() == 0){

        }
    }//end of while loop

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
    std::cout << "Total disk reads: " << diskReads << std::endl;
    std::cout << "Total disk writes: " << diskWrites << std::endl;
    std::cout << "Primary page table size: " << pageTablePrimary.size() << std::endl;
    std::cout << "Secondary page table size: " << pageTableSecondary.size() << std::endl;

    file.close();
}


