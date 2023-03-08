#ifndef POLICIES_HPP
#define POLICIES_HPP
#include "dataStructures.hpp"
#include <string>

/* Policy functions */
void fifo(std::string traceFile, int nFrames, int debugMode);
void lru(std::string traceFile, int nFrames, int debugMode);
void vms(std::string traceFile, int nFrames, int p, int debugMode);

#endif /* POLICIES_HPP */
