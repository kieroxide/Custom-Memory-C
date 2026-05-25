#pragma once
#include "HeapInternal.h"

class Segment {
  private:
    enum Status { FREE, USED };
    size_t payloadSize;
    Status status;
    Segment* nextSegment;

  public:
    void* memory;
    Segment* init(size_t size);
    Segment* findFreeSegment(size_t size);
    void* splitAndAllocate(size_t sizeAllocated);

    size_t getSize();
};
