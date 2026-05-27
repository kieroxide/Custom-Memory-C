#include <gtest/gtest.h>
#include "Heap.h"

TEST(memoryTests, reallocCanReduceSizeAndMerge) {
    Heap* heap = Heap::create();

    void* e = heap->alloc(100);

    void* r = heap->realloc(e, 10);

    Region* re = heap->TESTING_findFreeRegion(20);
    Segment* s = re->findFreeSegment(20);

    EXPECT_EQ(s->getSize(),
              Heap::MINIMUM_REGION_SIZE - 10 - sizeof(Segment));
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

TEST(memoryTests, reallocCanGrowInSizeWithNoCopy) {
    Heap* heap = Heap::create();

    void* a = heap->alloc(100);
    void* b = heap->alloc(100);
    void* c = heap->alloc(100);
    void* d = heap->alloc(100);
    void* e = heap->alloc(100);

    heap->dealloc(d);

    void* cRe = heap->realloc(c, 200);
    Segment* seg = Segment::memoryToSegment(cRe);

    EXPECT_EQ(c, cRe);
    EXPECT_EQ(seg->getSize(), 200);

    heap->freeHeap();
}

TEST(memoryTests, reallocWhenCopiedOldSegmentIsFree) {
    Heap* heap = Heap::create();

    void* a = heap->alloc(100);
    void* b = heap->alloc(100);

    void* aRealloc = heap->realloc(a, 200);
    Segment* newSeg = Segment::memoryToSegment(aRealloc);
    Segment* oldSeg = heap->TESTING_findFreeRegion(100)->findFreeSegment(100);

    EXPECT_NE(a, aRealloc);
    EXPECT_EQ(oldSeg->getMemory(), a);

    heap->freeHeap();
}