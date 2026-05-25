#include <iostream>
#include <windows.h>
#include "Heap.h"
#include "personalMemory.h"

using namespace std;

Heap::Heap() {
    size_t realSizeNeeded = HEAP_INIT_SIZE + sizeof(HeapStruct) + sizeof(Region) + sizeof(Segment);

    void* memoryBlock =
        VirtualAlloc(nullptr, realSizeNeeded, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    HeapStruct* heap = (HeapStruct*)memoryBlock;
    heap->size = HEAP_INIT_SIZE - sizeof(HeapStruct);

    // Create heap region metadata in heap block
    void* afterHeap = (char*)memoryBlock + sizeof(HeapStruct);
    Region* mainRegion = (Region*)afterHeap;
    mainRegion->nextRegion = nullptr;
    mainRegion->size = heap->size - sizeof(Region);

    void* afterRegion = (char*)afterHeap + sizeof(Region);

    // Create first free segment for region
    Segment* memorySegment = createSegment(mainRegion->size, FREE, afterRegion);

    mainRegion->nextSegment = memorySegment;
    heap->mainRegion = mainRegion;

    this->heap = heap;
}

Heap::~Heap() {
    const size_t FREE_ALL = 0;

    // Free linked regions
    Region* region = heap->mainRegion->nextRegion;
    while (region != nullptr) {
        Region* next = region->nextRegion;
        VirtualFree(region, FREE_ALL, MEM_RELEASE);
        region = next;
    }

    // Free heap and first region
    VirtualFree(heap, FREE_ALL, MEM_RELEASE);
}

void* Heap::alloc(size_t size) {
    // Find contigious free space in memory
    Region* currentRegion = heap->mainRegion;
    Segment* currentSegment = currentRegion->nextSegment;
    while (true) {
        // We traverse all regions
        while (currentSegment != nullptr) {
            // traverse all segments within region
            if (currentSegment->status == FREE && currentSegment->size >= size + sizeof(Segment)) {
                // Create new segment header
                size_t freeSizeAfterSegmentCreation = currentSegment->size - sizeof(Segment) - size;
                currentSegment->status = USED;
                currentSegment->size = size;
                Segment* freeSegment =
                    createSegment(freeSizeAfterSegmentCreation, FREE, currentSegment->memory);
                currentSegment->nextSegment = freeSegment;
                return currentSegment->memory;
            }
            currentSegment = currentSegment->nextSegment;
        }

        if (currentRegion->nextRegion == nullptr) {
            break;
        }

        currentRegion = currentRegion->nextRegion;
    }

    // No free space found so allocate another region
    size_t regionSize = size + sizeof(Region) + sizeof(Segment);
    if (regionSize < HEAP_INIT_SIZE / 2) {
        regionSize = HEAP_INIT_SIZE;
    }

    void* memoryBlock = VirtualAlloc(nullptr, regionSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    // Create region metadata in new region block
    Region* newRegion = (Region*)memoryBlock;

    // Attach Region to last region in region list
    currentRegion->nextRegion = newRegion;
    heap->size += regionSize;
    newRegion->nextRegion = nullptr;
    newRegion->size = regionSize - sizeof(Region);

    void* afterRegion = (char*)newRegion + sizeof(Region);

    // Create first free segment for region
    Segment* memorySegment = createSegment(newRegion->size, FREE, afterRegion);

    newRegion->nextSegment = memorySegment;
    return newRegion->nextSegment->memory;
}

size_t Heap::getSize() {
    return heap->size;
}