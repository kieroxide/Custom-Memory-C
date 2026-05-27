#pragma once
#include "HeapInternal.h"

class alignas(HEAP_ALIGNMENT) Segment {
  private:
    enum Status { FREE, USED };
    Status status;
    size_t payloadSize;
    Segment* nextSegment;
    Segment* prevSegment;
    void* memory;

    Segment* mergeFreeRight(Segment* nextSegment);
    Segment* merge();

  public:
    Segment* init(size_t size, Segment* prevSegment, Status status = FREE);
    Segment* findFreeSegment(size_t size);
    Segment* reduceSize(size_t newSize, size_t minSegmentPayloadSize);
    static Segment* memoryToSegment(void* memoryPtr,
                                    size_t offsetToHeader = sizeof(Segment),
                                    bool forwards = false);
    size_t getSize();
    void* getMemory();
    void* splitAndAllocate(size_t sizeAllocated);
    bool free();
    bool attemptRightMerge(size_t newSize);

    Segment* TESTING_getPrevSegment();
    Segment* TESTING_getNextSegment();
};
