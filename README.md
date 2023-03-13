To compile all the files, use g++ memsim.cpp lru.cpp fifo.cpp vms.cpp -o memsim

Alternatively, use the make command to compile that uses a makefile

To run the file use "memsim.exe <tracefile> <nframes> <lru|fifo> <debug|quiet>"

To run the file for vms do "memsim.exe <tracefile> <nframes> <vms> <p> <debug|quiet>"

Example output:

C:\Users\samdr\Project2OS>memsim.exe bzip.trace 64 vms 50 quiet    
Total event traces: 1000000
Total disk reads: 1290
Total disk writes: 427



