#pragma once

const int HEAP_INIT_SIZE = 1024 * 1000; // 1000kb Bytes

enum Status { FREE, USED };

struct Segment {
    size_t size;
    Status status;
    void* memory; // pointer to first address of segment
    Segment* nextSegment;
};

struct Region {
    size_t size;
    Segment* nextSegment;
    Region* nextRegion;
};

struct Heap {
    size_t size;
    Region* mainRegion;
};

void printAll(Segment* memory);

Heap* initHeap();
bool freeHeap(Heap* heap);

void* myMalloc(Heap* heap, size_t size);

Segment* createSegment(size_t size, Status status, void* memory, Segment* nextSegment = nullptr);

void my_free(void* ptr);
void* my_calloc(size_t count, size_t size);
void* my_realloc(void* ptr, size_t newSize);