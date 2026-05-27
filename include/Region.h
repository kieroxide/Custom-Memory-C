#pragma once
#include "HeapInternal.h"
#include "Segment.h"

class alignas(HEAP_ALIGNMENT) Region {
  private:
    size_t payloadSize;
    Region* nextRegion;
    Segment* nextSegment;

  public:
    Region* init(size_t heapSize);
    Region* getNextRegion();
    Segment* findFreeSegment(size_t allocSize);
    bool freeLinks();
    size_t getSize();
    void attachRegion(Region* region);
};
