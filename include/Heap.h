#pragma once
#include "HeapInternal.h"
#include "Region.h"

class Heap {
  private:
    size_t payloadSize;
    Region* mainRegion;

  public:
    static Heap* create(size_t allocSize = HEAP_INIT_SIZE);
    Heap* init(size_t allocSize);
    bool free();

    size_t getSize();
    void* alloc(size_t allocSize);
    Region* getLastRegion();
    Region* createRegion(size_t allocSize);
    Region* findFreeRegion(size_t allocSize);
};
