#pragma once
#include "HeapInternal.h"
#include "Segment.h"

class Region {
  private:
    size_t payloadSize;
    Region* nextRegion;
    Segment* nextSegment;

  public:
    Region* init(size_t heapSize);
    bool freeLinks();

    Segment* findFreeSegment(size_t allocSize);
    size_t getSize();
    Region* getNextRegion();
    void attachRegion(Region* region);
};
