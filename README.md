<h1 align="center"> Project 2 OS </h1> 

<h3 align="center">Simulation of Traces for Page Replacement Algorithms</h3>

This implementation uses the vector C++ STL that acts as the page table for each respective policy: FIFO, LRU, and VMS (Segmented FIFO). Each trace file, named bzip.trace and sixpack.trace, contains the hexadecimal virtual address space and read/write operation. Just like real traces, they access memory frequently and needs to be cached so that it does not need to recompute the mapping from virtual to physical memory. For this simulation, we do not have to access the memory itself, but simply implement the algorithms for the page table. We can do this by dividing the hexadecimal virtual address space and page size to obtain the page number, which is the index to the page table. We can calculate the disk reads when we get a page fault and the disk writes when we need to evict and it has a 'W' operation.  

To compile all the files, use ` g++ -W memsim.cpp lru.cpp fifo.cpp vms.cpp -o memsim `

Alternatively, you can do ` make memsim ` to compile easily

If you want to delete the executable, memsim, you can do ` make clean `

Debug mode prints all event traces.
Quiet mode runs the program without print statements. 

To run the file use ` ./memsim <tracefile> <nframes> <lru|fifo> <debug|quiet> `

To run the file for vms do ` ./memsim <tracefile> <nframes> <vms> <p> <debug|quiet> `

Example outputs:

[samuel51@forest.usf.edu@cselx01 project2]$ ` ./memsim bzip.trace 64 vms 50 quiet `
Total event traces: 1000000
Total disk reads: 1290
Total disk writes: 427

[samuel51@forest.usf.edu@cselx01 project2]$ ` ./memsim bzip.trace 64 lru quiet `
Trace Count: 1000000
Read Count: 1264
Write Count: 420

[samuel51@forest.usf.edu@cselx01 project2]$ ` ./memsim bzip.trace 64 fifo quiet `
Trace Count: 1000000
Read Count: 1467
Write Count: 514

There is no need for dataStructures.cpp since C++ STL provides all the necessary data structures needed.
That is why I decided to not compile dataStructures.cpp, because that is not contribute to the program.
However the dataStructures.hpp is used to initialize the entries for the page table.
