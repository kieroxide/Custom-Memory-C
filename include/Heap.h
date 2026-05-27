#pragma once
#include "HeapInternal.h"
#include "Region.h"

class alignas(HEAP_ALIGNMENT) Heap {
  private:
    size_t payloadSize;
    Region* mainRegion;
    Heap* init(size_t allocSize);
    Region* getLastRegion();
    Region* createRegion(size_t allocSize);
    Region* findFreeRegion(size_t allocSize);

  public:
    static constexpr size_t MINIMUM_REGION_SIZE = 1024 * 1000; // 1000kb Bytes
    static constexpr size_t MINIMUM_SEGMENT_SIZE = 8;

    static Heap* create(size_t allocSize = MINIMUM_REGION_SIZE);
    bool freeHeap();
    void* alloc(size_t allocSize);
    void* realloc(void* ptr, size_t newSize);
    bool dealloc(void* ptr);
    size_t getSize();

    Region* TESTING_findFreeRegion(size_t allocSize);
};
