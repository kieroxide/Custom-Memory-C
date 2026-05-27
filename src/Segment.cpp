#include <iostream>
#include <windows.h>
#include "Segment.h"

using namespace std;

bool Segment::attemptRightMerge(size_t newSize) {
    // Check if we can grow into right neighbour
    size_t sizeGrowthRequired = newSize - this->payloadSize;
    size_t rightLeeway = this->nextSegment->getSize() - sizeGrowthRequired;
    if (this->nextSegment->status == 0 && rightLeeway >= 0) {
        // If we can grow into the right segment
        Segment* nextSegment = this->nextSegment;

        this->payloadSize += sizeGrowthRequired;
        nextSegment->payloadSize -= sizeGrowthRequired;
        Segment* movedSegment = Segment::memoryToSegment(this->memory, this->getSize(), true);

        movedSegment->init(nextSegment->payloadSize, this);
        this->nextSegment = movedSegment;
        return true;
    }
    return false;
}

Segment* Segment::reduceSize(size_t newSize) {
    // Size Reduces
    size_t currentSize = this->payloadSize;
    this->payloadSize = newSize;

    // We create a new free segment if there is space
    if (currentSize - newSize >= sizeof(Segment)) {
        Segment* freeSeg = Segment::memoryToSegment(this->memory, newSize, true);
        size_t remainingSize = currentSize - newSize;
        // Link A Free and B: A -> B => A -> free -> B
        freeSeg->init(remainingSize, this);
        freeSeg->nextSegment = this->nextSegment;
        freeSeg->merge();
        this->nextSegment = freeSeg;
    } else {
        // Could merge without need for header
        // TODO
    }
    return this;
}

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

Segment* Segment::TESTING_getNextSegment() {
    return this->nextSegment;
}

Segment* Segment::TESTING_getPrevSegment() {
    return this->prevSegment;
}

void* Segment::getMemory(){
    return this->memory;
}