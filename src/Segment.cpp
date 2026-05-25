#include <iostream>
#include <windows.h>
#include "personalMemory.h"
#include "Segment.h"

using namespace std;

Segment* Segment::init(size_t size) {
    this->payloadSize = size - sizeof(Segment);
    this->status = FREE;
    this->nextSegment = nullptr;
    this->memory = (char*)this + sizeof(Segment);

    return this;
}

Segment* Segment::findFreeSegment(size_t size) {
    Segment* currentSegment = this;
    while (currentSegment != nullptr) {
        // traverse all segments within region
        if (currentSegment->status == FREE && currentSegment->payloadSize >= size) {
            return currentSegment;
        }
        currentSegment = currentSegment->nextSegment;
    }
    return nullptr;
}

void* Segment::splitAndAllocate(size_t sizeAllocated) {
    // Create new segment header
    size_t freeSizeAfterSegmentCreation = this->payloadSize - sizeAllocated;
    this->status = USED;
    this->payloadSize = sizeAllocated;

    // Don't split if no space left
    if (freeSizeAfterSegmentCreation <= sizeof(Segment)) {
        this->nextSegment = nullptr;
        return this->memory;
    }

    // Split remaining size to free segment
    void* segmentStart = (char*)this + sizeof(Segment) + this->payloadSize;
    Segment* freeSegment = (Segment*)segmentStart;
    Segment* memorySegment = freeSegment->init(freeSizeAfterSegmentCreation);

    this->nextSegment = freeSegment;
    return this->memory;
}