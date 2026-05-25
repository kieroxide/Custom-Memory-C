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
    Heap heap = Heap();

    void* p = heap.alloc(100);
    EXPECT_NE(p, nullptr);
}

TEST(memoryTests, mallocCanWriteToMemory) {
    Heap heap = Heap();

    void* p = heap.alloc(100);
    memset(p, 0xAA, 100);

    unsigned char* c = (unsigned char*)p;
    for (int i = 0; i < 100; i++) {
        EXPECT_EQ(c[i], 0xAA);
    }
}

TEST(memoryTests, mallocCanWriteStruct) {
    Heap heap = Heap();
    struct Test {
        int a;
        float b;
    };

    Test* t = (Test*)heap.alloc(sizeof(Test));
    t->a = 42;
    t->b = 3.14f;

    EXPECT_EQ(t->a, 42);
    EXPECT_EQ(t->b, 3.14f);
}

TEST(memoryTests, mallocCanAllocateTwoSegments) {
    Heap heap = Heap();

    void* p = heap.alloc(100);
    void* n = heap.alloc(64);
    EXPECT_NE(p, n);
}

TEST(memoryTests, mallocResizesHeap) {
    Heap heap = Heap();

    // Allocate two full region blocks
    void* p = heap.alloc(HEAP_INIT_SIZE);
    void* n = heap.alloc(HEAP_INIT_SIZE);

    size_t expectedSize = (HEAP_INIT_SIZE - sizeof(HeapStruct)) +
                          2 * (HEAP_INIT_SIZE + sizeof(Region) + sizeof(Segment));

    EXPECT_EQ(heap.getSize(), expectedSize);
}

TEST(memoryTests, mallocResizesAndReturnsNotNullPtr) {
    Heap heap = Heap();

    // Allocate two full region blocks
    void* p = heap.alloc(HEAP_INIT_SIZE);
    void* n = heap.alloc(HEAP_INIT_SIZE);

    EXPECT_NE(n, nullptr);
}

TEST(memoryTests, mallocResizesAndCanWriteToMemory) {
    Heap heap = Heap();

    // Allocate two full region blocks
    void* p = heap.alloc(HEAP_INIT_SIZE);
    void* n = heap.alloc(HEAP_INIT_SIZE);

    memset(n, 0xAA, 100);

    unsigned char* c = (unsigned char*)n;
    for (int i = 0; i < 100; i++) {
        EXPECT_EQ(c[i], 0xAA);
    }
}