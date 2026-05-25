#pragma once
#include "HeapInternal.h"
#include "Region.h"

class Heap {
  private:
    size_t payloadSize;
    Region* mainRegion;
    Heap* init(size_t allocSize);

  public:
    static Heap* create(size_t allocSize = HEAP_INIT_SIZE);
    bool freeHeap();
    
    void* alloc(size_t allocSize);
    bool dealloc(void* ptr);

    size_t getSize();
    Region* getLastRegion();
    Region* createRegion(size_t allocSize);
    Region* findFreeRegion(size_t allocSize);
};
