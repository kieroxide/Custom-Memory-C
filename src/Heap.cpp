#include <iostream>
#include <windows.h>
#include "Heap.h"
#include "Region.h"
#include "Segment.h"

using namespace std;

void* Heap::realloc(void* ptr, size_t newSize) {
    Segment* segment = Segment::memoryToSegment(ptr);
    size_t size = segment->getSize();

    if (newSize == size) {
        return ptr;
    }

    if (newSize < size) {
        return segment->reduceSize(newSize)->getMemory();
    } else {
        if (segment->attemptRightMerge(newSize)) {
            return segment->getMemory();
        } else {
            void* newMemPtr = this->alloc(newSize);
            memcpy(newMemPtr, segment->getMemory(), segment->getSize());
            return newMemPtr;
        }
    }
}

bool Heap::dealloc(void* ptr) {
    Segment* segment = Segment::memoryToSegment(ptr);

    return segment->free();
}

Heap* Heap::create(size_t allocSize) {
    allocSize += sizeof(Heap) + sizeof(Region) + sizeof(Segment);
    if (allocSize < HEAP_INIT_SIZE / 2) {
        allocSize = HEAP_INIT_SIZE;
    }

    void* memoryBlock = VirtualAlloc(nullptr, allocSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    Heap* heap = (Heap*)memoryBlock;
    heap->init(allocSize);
    return heap;
}

Heap* Heap::init(size_t allocSize) {
    this->payloadSize = allocSize - sizeof(Heap);

    // Create heap region metadata in heap block
    void* regionStart = (char*)this + sizeof(Heap);
    Region* region = (Region*)regionStart;
    region->init(this->payloadSize);

    this->mainRegion = region;
    return this;
}

Region* Heap::createRegion(size_t allocSize) {
    size_t regionSize = allocSize + sizeof(Region) + sizeof(Segment);
    if (regionSize < HEAP_INIT_SIZE / 2) {
        regionSize = HEAP_INIT_SIZE;
    }

    void* memoryBlock = VirtualAlloc(nullptr, regionSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    // Create region metadata in new region block
    Region* newRegion = (Region*)memoryBlock;
    this->payloadSize += regionSize;
    return newRegion->init(regionSize);
}

bool Heap::freeHeap() {
    bool success = this->mainRegion->freeLinks();

    // Free heap and first region
    return success && (VirtualFree(this, WINDOWS_FREE_ALL, MEM_RELEASE) != 0);
}

void* Heap::alloc(size_t allocSize) {
    // Find contigious free space in memory
    Region* freeRegion = this->findFreeRegion(allocSize);
    if (freeRegion != nullptr) {
        Segment* freeSegment = freeRegion->findFreeSegment(allocSize);
        if (freeSegment != nullptr) {
            return freeSegment->splitAndAllocate(allocSize);
        }
    }

    // No free space found so allocate another region
    // Attach Region to last region in region list
    Region* newRegion = this->createRegion(allocSize);
    Region* lastRegion = this->getLastRegion();
    lastRegion->attachRegion(newRegion);
    return newRegion->findFreeSegment(allocSize)->getMemory();
}

size_t Heap::getSize() {
    return this->payloadSize;
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

Region* Heap::findFreeRegion(size_t allocSize) {
    Region* currentRegion = this->mainRegion;
    Segment* searchedSegment = nullptr;
    while (true) {
        // We traverse all regions until one is found
        searchedSegment = currentRegion->findFreeSegment(allocSize);
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