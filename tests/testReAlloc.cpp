#include <gtest/gtest.h>
#include "Heap.h"

TEST(memoryTests, reallocCanReduceSizeAndMerge) {
    Heap* heap = Heap::create();

    void* e = heap->alloc(100);


    void* r = heap->realloc(e, 5);

    Region* re = heap->findFreeRegion(20);
    Segment* s = re->findFreeSegment(20);

    EXPECT_EQ(s->getSize(), HEAP_INIT_SIZE - 5 - sizeof(Segment));
    EXPECT_EQ(e, r);


    heap->freeHeap();
}

TEST(memoryTests, reallocSameSize) {
    Heap* heap = Heap::create();

    void* e = heap->alloc(100);
    void* r = heap->realloc(e, 100);

    EXPECT_EQ(e, r);


    heap->freeHeap();
}


TEST(memoryTests, reallocHasToCopy) {
    Heap* heap = Heap::create();

    void* e = heap->alloc(100);
    void* f = heap->alloc(100);
    void* g = heap->alloc(100);

    void* fr = heap->realloc(e, 200);
    Segment* frSeg = Segment::memoryToSegment(fr);
    
    EXPECT_NE(f, fr);
    EXPECT_EQ(frSeg->getSize(), 200);


    heap->freeHeap();
}