#include <gtest/gtest.h>
#include "personalMemory.h"
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

    heap->free();
}

TEST(memoryTests, mallocCanWriteToMemory) {
    Heap* heap = Heap::create();

    void* p = heap->alloc(100);
    memset(p, 0xAA, 100);

    unsigned char* c = (unsigned char*)p;
    for (int i = 0; i < 100; i++) {
        EXPECT_EQ(c[i], 0xAA);
    }

    heap->free();
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

    heap->free();
}

TEST(memoryTests, mallocCanAllocateTwoSegments) {
    Heap* heap = Heap::create();

    void* p = heap->alloc(100);
    void* n = heap->alloc(64);
    EXPECT_NE(p, n);

    heap->free();
}

TEST(memoryTests, mallocResizesHeap) {
    Heap* heap = Heap::create();
    size_t fullAlloc = HEAP_INIT_SIZE - sizeof(Heap) - sizeof(Region) - sizeof(Segment);

    // Allocate two full region blocks
    void* p = heap->alloc(fullAlloc);
    
    // Only first Alloc has heap header
    void* n = heap->alloc(fullAlloc + sizeof(Heap));

    size_t expectedSize = 2 * HEAP_INIT_SIZE;

    EXPECT_EQ(heap->getSize() + sizeof(Heap), expectedSize);

    heap->free();
}

TEST(memoryTests, mallocResizesAndReturnsNotNullPtr) {
    Heap* heap = Heap::create();
    size_t fullAlloc = HEAP_INIT_SIZE - sizeof(Heap) - sizeof(Region) - sizeof(Segment);

    // Allocate two full region blocks
    void* p = heap->alloc(fullAlloc);
    void* n = heap->alloc(fullAlloc);

    EXPECT_NE(n, nullptr);

    heap->free();
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

    heap->free();
}