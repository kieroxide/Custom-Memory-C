#pragma once
#include "personalMemory.h"
#include "HeapInternal.h"
#include "Segment.h"

class Region {
  private:
    size_t size;
    Region* nextRegion;
    Segment* nextSegment;

  public:
    Region* init(size_t heapSize);
    bool freeLinks();

    Segment* findFreeSegment(size_t size);
    size_t getSize();
    Region* getNextRegion();
    void attachRegion(Region* region);
};
