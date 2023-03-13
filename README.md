#Project 2 OS 
#By Samuel Sau

To compile all the files, use "g++ -W memsim.cpp lru.cpp fifo.cpp vms.cpp -o memsim"

Alternatively, you can do "make memsim" to compile easily

If you want to delete the executable, memsim, you can do "make clean"

To run the file use "./memsim <tracefile> <nframes> <lru|fifo> <debug|quiet>"

To run the file for vms do "./memsim <tracefile> <nframes> <vms> <p> <debug|quiet>"

Example outputs:

[samuel51@forest.usf.edu@cselx01 project2]$ ./memsim bzip.trace 64 vms 50 quiet
Total event traces: 1000000
Total disk reads: 1290
Total disk writes: 427

[samuel51@forest.usf.edu@cselx01 project2]$ ./memsim bzip.trace 64 lru quiet
Trace Count: 1000000
Read Count: 1264
Write Count: 420

[samuel51@forest.usf.edu@cselx01 project2]$ ./memsim bzip.trace 64 fifo quiet
Trace Count: 1000000
Read Count: 1467
Write Count: 514

There is no need for dataStructures.cpp since C++ STL provides all the necessary data structures needed.
That is why I decided to not compile dataStructures.cpp, because that is not contribute to the program.
However the dataStructures.hpp is used to initialize the entries for the page table.