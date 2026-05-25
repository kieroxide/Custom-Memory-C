#include <iostream>
#include <windows.h>
#include "personalMemory.h"

using namespace std;

// Requests memory from OS for our heap (metadata for heap is in the memory too)
Heap* initHeap() {
    size_t realSizeNeeded = HEAP_INIT_SIZE + sizeof(Heap) + sizeof(Region) + sizeof(Segment);

    void* memoryBlock =
        VirtualAlloc(nullptr, realSizeNeeded, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    Heap* heap = (Heap*)memoryBlock;
    heap->size = HEAP_INIT_SIZE - sizeof(Heap);

    // Create heap region metadata in heap block
    void* afterHeap = (char*)memoryBlock + sizeof(Heap);
    Region* mainRegion = (Region*)afterHeap;
    mainRegion->nextRegion = nullptr;
    mainRegion->size = heap->size - sizeof(Region);

    void* afterRegion = (char*)afterHeap + sizeof(Region);

    // Create first free segment for region
    Segment* memorySegment = createSegment(mainRegion->size, FREE, afterRegion);

    mainRegion->nextSegment = memorySegment;
    heap->mainRegion = mainRegion;

    return heap;
}

bool freeHeap(Heap* heap) {
    const size_t FREE_ALL = 0;
    bool success = true;

    // Free linked regions
    Region* region = heap->mainRegion->nextRegion;
    while (region != nullptr) {
        Region* next = region->nextRegion;
        success = success && (VirtualFree(region, FREE_ALL, MEM_RELEASE) != 0);
        region = next;
    }

    // Free heap and first region
    success = success && (VirtualFree(heap, FREE_ALL, MEM_RELEASE) != 0);
    return success;
}

// Creates and returns pointer to memory allocation with passed size param
void* myMalloc(Heap* heap, size_t size) {
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

Segment* createSegment(size_t size, Status status, void* memory, Segment* nextSegment) {
    Segment* seg = (Segment*)memory;

    seg->size = size - sizeof(Segment);
    seg->status = status;
    seg->nextSegment = nextSegment;
    seg->memory = (char*)memory + sizeof(Segment);

    return seg;
}

void printAll(Segment* memory) {
    Segment* current = memory;
    while (current != nullptr) {
        cout << "Address: " << current;
        cout << ", Next Address: " << current->nextSegment;
        cout << ", Data Memory Address: " << current->memory;
        cout << ", Size: " << current->size;
        cout << ", Status: " << current->status << endl;
        current = current->nextSegment;
    }
}