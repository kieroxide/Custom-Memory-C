#include <gtest/gtest.h>
#include "personalMemory.h"

/* FORMAT FOR TESTS:
TEST(TestSuiteName, TestName) {
    arrange
    act
    assert
} */

TEST(memoryTests, mallocReturnsNotNullPtr) {
    Heap* heap = initHeap();

    void* p = myMalloc(heap, 100);
    EXPECT_NE(p, nullptr);

    freeHeap(heap);
}

TEST(memoryTests, mallocCanWriteToMemory) {
    Heap* heap = initHeap();

    void* p = myMalloc(heap, 100);
    memset(p, 0xAA, 100);

    unsigned char* c = (unsigned char*)p;
    for (int i = 0; i < 100; i++) {
        EXPECT_EQ(c[i], 0xAA);
    }

    freeHeap(heap);
}

TEST(memoryTests, mallocCanWriteStruct) {
    Heap* heap = initHeap();
    struct Test { int a; float b; };

    Test* t = (Test*)myMalloc(heap, sizeof(Test));
    t->a = 42;
    t->b = 3.14f;

    EXPECT_EQ(t->a, 42);
    EXPECT_EQ(t->b, 3.14f);

    freeHeap(heap);
}

TEST(memoryTests, mallocCanAllocateTwoSegments) {
    Heap* heap = initHeap();

    void* p = myMalloc(heap, 100);
    void* n = myMalloc(heap, 64);
    EXPECT_NE(p, n);

    freeHeap(heap);
}

TEST(memoryTests, mallocResizesHeap) {
    Heap* heap = initHeap();

    // Allocate two full region blocks
    void* p = myMalloc(heap, HEAP_INIT_SIZE);
    void* n = myMalloc(heap, HEAP_INIT_SIZE);

    size_t expectedSize =
        (HEAP_INIT_SIZE - sizeof(Heap)) + 2 * (HEAP_INIT_SIZE + sizeof(Region) + sizeof(Segment));
    EXPECT_EQ(heap->size, expectedSize);

    freeHeap(heap);
}

TEST(memoryTests, mallocResizesAndReturnsNotNullPtr) {
    Heap* heap = initHeap();

    // Allocate two full region blocks
    void* p = myMalloc(heap, HEAP_INIT_SIZE);
    void* n = myMalloc(heap, HEAP_INIT_SIZE);
    
    EXPECT_NE(n, nullptr);

    freeHeap(heap);
}

TEST(memoryTests, mallocResizesAndCanWriteToMemory) {
    Heap* heap = initHeap();

    // Allocate two full region blocks
    void* p = myMalloc(heap, HEAP_INIT_SIZE);
    void* n = myMalloc(heap, HEAP_INIT_SIZE);
    
    memset(n, 0xAA, 100);

    unsigned char* c = (unsigned char*)n;
    for (int i = 0; i < 100; i++) {
        EXPECT_EQ(c[i], 0xAA);
    }

    freeHeap(heap);
}