#pragma once
#include "HeapInternal.h"
#include "Segment.h"

class Region {
  private:
    size_t payloadSize;
    Region* nextRegion;
    Segment* nextSegment;

  public:
    bool freeLinks();
    Region* init(size_t heapSize);

    Segment* findFreeSegment(size_t allocSize);
    size_t getSize();
    Region* getNextRegion();
    void attachRegion(Region* region);
};
