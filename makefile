CXXFLAGS = -W

.PHONY: all clean

all: memsim

memsim: memsim.cpp lru.cpp fifo.cpp vms.cpp
        g++ $(CXXFLAGS) $^ -o $@

clean:
        rm -f memsim