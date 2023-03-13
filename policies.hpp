#ifndef POLICIES_HPP
#define POLICIES_HPP
#include "dataStructures.hpp"
#include <string>

/* Policy functions */
void fifo(const char* traceFile, int nFrames, bool debugMode);
void lru(const char* traceFile, int nFrames, bool debugMode);
void vms(const char* traceFile, int nFrames, int p, bool debugMode);

#endif /* POLICIES_HPP */
