#include <iostream>
#include <windows.h>
#include "personalMemory.h"

using namespace std;

// void printAll(SegmentHeader* memory) {
//     SegmentHeader* current = memory;
//     while (current != nullptr) {
//         cout << "Address: " << current;
//         cout << ", Next Address: " << current->nextSegment;
//         cout << ", Data Memory Address: " << current->memory;
//         cout << ", Size: " << current->size;
//         cout << ", Status: " << current->status << endl;
//         current = current->nextSegment;
//     }
// }