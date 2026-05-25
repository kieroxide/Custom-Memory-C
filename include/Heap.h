#pragma once
#include "HeapInternal.h"
#include "Region.h"

class Heap {
  private:
    size_t size;
    Region* mainRegion;

  public:
    static Heap* create(size_t size = HEAP_INIT_SIZE);
    Heap* init(size_t size);
    bool free();

    size_t getSize();
    void* alloc(size_t size);
    Region* getLastRegion();
    Region* createRegion(size_t size);
    Region* findFreeRegion(size_t size);
};
