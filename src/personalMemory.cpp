#include <iostream>
#include <windows.h>
#include "personalMemory.h"

using namespace std;

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