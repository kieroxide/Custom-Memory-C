#include <gtest/gtest.h>
#include "Heap.h"

TEST(memoryTests, deallocFreeWillMergeContigiousFreeSegmentsPt1) {
    Heap* heap = Heap::create();

    void* d = heap->alloc(10);
    void* e = heap->alloc(10);
    void* f = heap->alloc(10);

    // d segment should merge with e segment
    bool rese = heap->dealloc(e);
    bool resd = heap->dealloc(d);

    Region* r = heap->TESTING_findFreeRegion(20);
    Segment* s = r->findFreeSegment(20);

    EXPECT_EQ(rese, true);
    EXPECT_EQ(resd, true);

    EXPECT_EQ(s->getMemory(), d);
    EXPECT_EQ(s->getSize(), 20 + sizeof(Segment));

    heap->freeHeap();
}

TEST(memoryTests, deallocFreeWillMergeContigiousFreeSegmentsPt2) {
    Heap* heap = Heap::create();

    void* d = heap->alloc(10);
    void* e = heap->alloc(10);
    void* f = heap->alloc(10);
    void* g = heap->alloc(10);

    // e segment should merge with f segment
    heap->dealloc(e);
    heap->dealloc(f);

    Region* r = heap->TESTING_findFreeRegion(20);
    Segment* s = r->findFreeSegment(20);

    EXPECT_EQ(s->getMemory(), e);
    EXPECT_EQ(s->getSize(), 20 + sizeof(Segment));
    EXPECT_EQ(s->TESTING_getPrevSegment()->getMemory(), d);
    EXPECT_EQ(s->TESTING_getNextSegment()->getMemory(), g);

    heap->freeHeap();
}

TEST(memoryTests, deallocFreeWillMergeContigiousFreeSegmentsPt3) {
    Heap* heap = Heap::create();

    void* d = heap->alloc(10);
    void* e = heap->alloc(10);
    void* f = heap->alloc(10);
    void* g = heap->alloc(10);

    // e segment should merge with d segment then merge with f
    heap->dealloc(e);
    heap->dealloc(d);
    heap->dealloc(f);

    Region* r = heap->TESTING_findFreeRegion(20);
    Segment* s = r->findFreeSegment(20);

    EXPECT_EQ(s->getMemory(), d);
    EXPECT_EQ(s->getSize(), 30 + (2 * sizeof(Segment)));
    EXPECT_EQ(s->TESTING_getPrevSegment(), nullptr);
    EXPECT_EQ(s->TESTING_getNextSegment()->getMemory(), g);

    heap->freeHeap();
}

TEST(memoryTests, deallocFreeWillMergeContigiousFreeSegmentsPt4) {
    Heap* heap = Heap::create();

    void* e = heap->alloc(10);

    // e segment should merge with free segment after it
    heap->dealloc(e);

    Region* r = heap->TESTING_findFreeRegion(20);
    Segment* s = r->findFreeSegment(20);

    EXPECT_EQ(s->getMemory(), e);
    EXPECT_EQ(s->getSize(), Heap::MINIMUM_REGION_SIZE);
    EXPECT_EQ(s->TESTING_getPrevSegment(), nullptr);
    EXPECT_EQ(s->TESTING_getNextSegment(), nullptr);

    heap->freeHeap();
}