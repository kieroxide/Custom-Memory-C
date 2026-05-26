#pragma once
#include "HeapInternal.h"

class Segment {
  private:
    enum Status { FREE, USED };
    Status status;

  public:
    size_t payloadSize;
    Segment* nextSegment;
    Segment* prevSegment;
    void* memory;
    Segment* init(size_t size, Segment* prevSegment, Status status = FREE);
    Segment* findFreeSegment(size_t size);
    void* splitAndAllocate(size_t sizeAllocated);
    bool free();

    Segment* mergeFreeRight(Segment* nextSegment);
    Segment* merge();

    size_t getSize();
};
