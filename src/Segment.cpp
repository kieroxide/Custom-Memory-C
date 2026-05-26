#include <iostream>
#include <windows.h>
#include "Segment.h"

using namespace std;

Segment* Segment::memoryToSegment(void* memoryPtr, size_t offsetToHeader, bool forwards) {
    if (forwards) {
        return (Segment*)((char*)memoryPtr + offsetToHeader);
    }

    return (Segment*)((char*)memoryPtr - sizeof(Segment));
}

bool Segment::free() {
    if (this->status != USED) {
        return false;
    }
    this->status = FREE;

    this->merge();

    return true;
}

Segment* Segment::merge() {
    Segment* current = this;

    // Segment to left is free
    if (current->prevSegment != nullptr && current->prevSegment->status == FREE) {
        // Left <- Right
        current = current->prevSegment->mergeFreeRight(current);
    }

    // Segment to right is free
    if (current->nextSegment != nullptr && current->nextSegment->status == FREE) {
        // Left -> Right
        current->mergeFreeRight(current->nextSegment);
    }

    return current;
}

Segment* Segment::mergeFreeRight(Segment* nextSegment) {
    if (nextSegment->nextSegment != nullptr) {
        nextSegment->nextSegment->prevSegment = this;
    }

    size_t totalSegSize = nextSegment->getSize() + sizeof(Segment);
    this->payloadSize += totalSegSize;
    this->nextSegment = nextSegment->nextSegment;
    return this;
}

Segment* Segment::init(size_t size, Segment* prevSegment, Status status) {
    this->payloadSize = size - sizeof(Segment);
    this->status = status;
    this->nextSegment = nullptr;
    this->prevSegment = prevSegment;
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
    Segment* freeSegment =
        Segment::memoryToSegment(this, sizeof(Segment) + this->payloadSize, true);
    Segment* memorySegment = freeSegment->init(freeSizeAfterSegmentCreation, this);

    this->nextSegment = freeSegment;
    return this->memory;
}

size_t Segment::getSize() {
    return this->payloadSize;
}