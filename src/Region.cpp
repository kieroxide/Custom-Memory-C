#include <iostream>
#include <windows.h>
#include "Region.h"
#include "Segment.h"

using namespace std;

Region* Region::init(size_t heapSize) {
    this->nextRegion = nullptr;
    this->payloadSize = heapSize - sizeof(Region);

    void* segmentStart = (char*)this + sizeof(Region);

    // Create first free segment for region
    Segment* segment = (Segment*)segmentStart;
    Segment* memorySegment = segment->init(this->payloadSize, nullptr);

    this->nextSegment = memorySegment;
    return this;
}

bool Region::freeLinks() {
    // Free linked regions
    bool success = true;

    Region* region = this->nextRegion;
    while (region != nullptr) {
        Region* next = region->nextRegion;
        success = success && (VirtualFree(region, WINDOWS_FREE_ALL, MEM_RELEASE) != 0);
        region = next;
    }

    return success;
}

Segment* Region::findFreeSegment(size_t allocSize) {
    Region* currentRegion = this;
    Segment* searchedSegment = nullptr;
    while (true) {
        // We traverse all regions until one is found
        searchedSegment = this->nextSegment->findFreeSegment(allocSize);
        if (searchedSegment != nullptr) {
            return searchedSegment;
        }

        if (this->nextRegion == nullptr) {
            break;
        }

        currentRegion = currentRegion->nextRegion;
    }

    return searchedSegment;
}

Region* Region::getNextRegion() {
    return this->nextRegion;
}

void Region::attachRegion(Region* region) {
    this->nextRegion = region;
}

size_t Region::getSize() {
    return this->payloadSize;
}