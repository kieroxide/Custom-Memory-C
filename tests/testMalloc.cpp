#include <gtest/gtest.h>
#include "Heap.h"

/* FORMAT FOR TESTS:
TEST(TestSuiteName, TestName) {
    arrange
    act
    assert
} */

TEST(memoryTests, mallocReturnsNotNullPtr) {
    Heap* heap = Heap::create();

    void* p = heap->alloc(100);
    EXPECT_NE(p, nullptr);

    heap->freeHeap();
}

TEST(memoryTests, mallocCanWriteToMemory) {
    Heap* heap = Heap::create();

    void* p = heap->alloc(100);
    memset(p, 0xAA, 100);

    unsigned char* c = (unsigned char*)p;
    for (int i = 0; i < 100; i++) {
        EXPECT_EQ(c[i], 0xAA);
    }

    heap->freeHeap();
}

TEST(memoryTests, mallocCanWriteStruct) {
    Heap* heap = Heap::create();
    struct Test {
        int a;
        float b;
    };

    Test* t = (Test*)heap->alloc(sizeof(Test));
    t->a = 42;
    t->b = 3.14f;

    EXPECT_EQ(t->a, 42);
    EXPECT_EQ(t->b, 3.14f);

    heap->freeHeap();
}

TEST(memoryTests, mallocCanAllocateTwoSegments) {
    Heap* heap = Heap::create();

    void* p = heap->alloc(100);
    void* n = heap->alloc(64);
    EXPECT_NE(p, n);

    heap->freeHeap();
}

TEST(memoryTests, mallocResizesHeap) {
    Heap* heap = Heap::create(); // HEAP_INIT_SIZE + headers size is default
    size_t fullAlloc = HEAP_INIT_SIZE;

    // Allocate two full region blocks
    void* p = heap->alloc(fullAlloc);

    // Only first Alloc has heap header
    void* n = heap->alloc(fullAlloc + sizeof(Heap));

    size_t expectedSize = 2 * (HEAP_INIT_SIZE + sizeof(Region) + sizeof(Segment) + sizeof(Heap));

    EXPECT_EQ(heap->getSize() + sizeof(Heap), expectedSize);

    heap->freeHeap();
}

TEST(memoryTests, mallocFullAllocOneRegion) {
    Heap* heap = Heap::create(); // HEAP_INIT_SIZE + headers size is default

    // Allocate one full region block
    void* p = heap->alloc(HEAP_INIT_SIZE);

    size_t expectedSize = (HEAP_INIT_SIZE + sizeof(Heap) + sizeof(Region) + sizeof(Segment));

    EXPECT_EQ(heap->getSize() + sizeof(Heap), expectedSize);

    heap->freeHeap();
}

TEST(memoryTests, mallocResizesAndReturnsNotNullPtr) {
    Heap* heap = Heap::create();
    size_t fullAlloc = HEAP_INIT_SIZE - sizeof(Heap) - sizeof(Region) - sizeof(Segment);

    // Allocate two full region blocks
    void* p = heap->alloc(fullAlloc);
    void* n = heap->alloc(fullAlloc);

    EXPECT_NE(n, nullptr);

    heap->freeHeap();
}

TEST(memoryTests, mallocResizesAndCanWriteToMemory) {
    Heap* heap = Heap::create();
    size_t fullAlloc = HEAP_INIT_SIZE - sizeof(Heap) - sizeof(Region) - sizeof(Segment);

    // Allocate two full region blocks
    void* p = heap->alloc(fullAlloc);
    void* n = heap->alloc(fullAlloc);

    memset(n, 0xAA, 100);

    unsigned char* c = (unsigned char*)n;
    for (int i = 0; i < 100; i++) {
        EXPECT_EQ(c[i], 0xAA);
    }

    heap->freeHeap();
}

TEST(memoryTests, mallocFreeSegment) {
    Heap* heap = Heap::create();

    void* p = heap->alloc(100);
    bool response = heap->dealloc(p);
    void* n = heap->alloc(100);

    EXPECT_EQ(response, true);
    EXPECT_EQ(p, n);

    heap->freeHeap();
}

TEST(memoryTests, mallocSplitsSegmentsCorrectly) {
    Heap* heap = Heap::create();

    // We allocate 100 bytes so our next free segment
    // Should be at p + 100 bytes
    void* p = heap->alloc(100);
    Region* r = heap->findFreeRegion(100);
    Segment* s = r->findFreeSegment(100);

    void* expectedNextFreeSeg = (char*)p + 100;
    EXPECT_EQ(expectedNextFreeSeg, s);

    heap->freeHeap();
}

TEST(memoryTests, deallocFreeWillMergeContigiousFreeSegmentsPt1) {
    Heap* heap = Heap::create();

    void* d = heap->alloc(10);
    void* e = heap->alloc(10);
    void* f = heap->alloc(10);

    // d segment should merge with e segment
    bool rese = heap->dealloc(e);
    bool resd = heap->dealloc(d);

    Region* r = heap->findFreeRegion(20);
    Segment* s = r->findFreeSegment(20);

    EXPECT_EQ(rese, true);
    EXPECT_EQ(resd, true);

    EXPECT_EQ(s->memory, d);
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

    Region* r = heap->findFreeRegion(20);
    Segment* s = r->findFreeSegment(20);

    EXPECT_EQ(s->memory, e);
    EXPECT_EQ(s->getSize(), 20 + sizeof(Segment));
    EXPECT_EQ(s->prevSegment->memory, d);
    EXPECT_EQ(s->nextSegment->memory, g);

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

    Region* r = heap->findFreeRegion(20);
    Segment* s = r->findFreeSegment(20);

    EXPECT_EQ(s->memory, d);
    EXPECT_EQ(s->getSize(), 30 + (2 * sizeof(Segment)));
    EXPECT_EQ(s->prevSegment, nullptr);
    EXPECT_EQ(s->nextSegment->memory, g);

    heap->freeHeap();
}

TEST(memoryTests, deallocFreeWillMergeContigiousFreeSegmentsPt4) {
    Heap* heap = Heap::create();

    void* e = heap->alloc(10);

    // e segment should merge with free segment after it
    heap->dealloc(e);

    Region* r = heap->findFreeRegion(20);
    Segment* s = r->findFreeSegment(20);

    EXPECT_EQ(s->memory, e);
    EXPECT_EQ(s->getSize(), HEAP_INIT_SIZE);
    EXPECT_EQ(s->prevSegment, nullptr);
    EXPECT_EQ(s->nextSegment, nullptr);

    heap->freeHeap();
}