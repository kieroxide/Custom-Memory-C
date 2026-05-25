#include <iostream>
#include <windows.h>
#include "Heap.h"
#include "Region.h"
#include "personalMemory.h"

using namespace std;

Heap* Heap::create(size_t size){
    size_t realSizeNeeded = size + sizeof(Heap) + sizeof(Region) + sizeof(Segment);
    if (realSizeNeeded < HEAP_INIT_SIZE / 2) {
        size = HEAP_INIT_SIZE;
    }

    void* memoryBlock =
        VirtualAlloc(nullptr, realSizeNeeded, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    Heap* heap = (Heap*)memoryBlock;
    heap->init(realSizeNeeded);
    return heap;
}

Heap* Heap::init(size_t size) {
    this->size = HEAP_INIT_SIZE - sizeof(Heap);

    // Create heap region metadata in heap block
    void* regionStart = (char*)this + sizeof(Heap);
    Region* region = (Region*)regionStart;
    region->init(this->size);

    this->mainRegion = region;
    return this;
}

Region* Heap::createRegion(size_t size) {
    size_t regionSize = size + sizeof(Region) + sizeof(Segment);
    if (regionSize < HEAP_INIT_SIZE / 2) {
        regionSize = HEAP_INIT_SIZE;
    }

    void* memoryBlock = VirtualAlloc(nullptr, regionSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    // Create region metadata in new region block
    Region* newRegion = (Region*)memoryBlock;
    this->size += regionSize;
    return newRegion->init(regionSize);
}

bool Heap::free() {
    bool success = this->mainRegion->freeLinks();

    // Free heap and first region
    return success && (VirtualFree(this, FREE_ALL, MEM_RELEASE) != 0);
}

void* Heap::alloc(size_t size) {
    // Find contigious free space in memory
    Region* freeRegion = this->findFreeRegion(size);
    if (freeRegion != nullptr) {
        Segment* freeSegment = freeRegion->findFreeSegment(size);
        if (freeSegment != nullptr) {
            return freeSegment->splitAndAllocate(size);
        }
    }

    // No free space found so allocate another region
    // Attach Region to last region in region list
    Region* newRegion = this->createRegion(size);
    Region* lastRegion = this->getLastRegion();
    lastRegion->attachRegion(newRegion);
    return newRegion->findFreeSegment(size)->memory;
}

size_t Heap::getSize() {
    return this->size;
}

Region* Heap::getLastRegion() {
    Region* current = this->mainRegion;
    while (true) {
        if (current->getNextRegion() == nullptr) {
            break;
        }
        current = current->getNextRegion();
    }
    return current;
}

Region* Heap::findFreeRegion(size_t size) {
    Region* currentRegion = this->mainRegion;
    Segment* searchedSegment = nullptr;
    while (true) {
        // We traverse all regions until one is found
        searchedSegment = currentRegion->findFreeSegment(size);
        if (searchedSegment != nullptr) {
            return currentRegion;
        }

        if (currentRegion->getNextRegion() == nullptr) {
            break;
        }

        currentRegion = currentRegion->getNextRegion();
    }

    return nullptr;
}