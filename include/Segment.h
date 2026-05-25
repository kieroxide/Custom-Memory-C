#pragma once
#include "HeapInternal.h"

class Segment {
  private:
    enum Status { FREE, USED };
    size_t payloadSize;
    Status status;

  public:
    Segment* nextSegment;
    Segment* prevSegment;
    void* memory;
    Segment* init(size_t size, Segment* prevSegment);
    Segment* findFreeSegment(size_t size);
    void* splitAndAllocate(size_t sizeAllocated);
    bool free();

    Segment* mergeFreeRight(Segment* nextSegment);

    size_t getSize();
};
