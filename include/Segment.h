#pragma once
#include "HeapInternal.h"

class Segment {
  private:
    enum Status { FREE, USED };
    Status status;
    size_t payloadSize;
    Segment* nextSegment;
    Segment* prevSegment;
    void* memory;

  public:
    Segment* init(size_t size, Segment* prevSegment, Status status = FREE);
    Segment* findFreeSegment(size_t size);
    void* splitAndAllocate(size_t sizeAllocated);
    Segment* reduceSize(size_t newSize);
    bool attemptRightMerge(size_t newSize);
    bool free();

    Segment* mergeFreeRight(Segment* nextSegment);
    Segment* merge();

    size_t getSize();
    void* getMemory();
    static Segment* memoryToSegment(void* memoryPtr,
                                    size_t offsetToHeader = sizeof(Segment),
                                    bool forwards = false);

    Segment* TESTING_getPrevSegment();
    Segment* TESTING_getNextSegment();
    
};
