#include <iostream>
#include "personalMemory.h"

using namespace std;

int main(int argc, char** args) {
    // Create free memory segment
    cout << "\nProgram Starting...\n\n";
    Heap* heap = initHeap();

    void* myString = myMalloc(heap, 10);
    char* string = static_cast<char*>(myString);
    for (int i = 0; i < 9; i++) {
        string[i] = 'A';
    }
    string[9] = '\0';
    cout << "Program Output: " << endl;
    cout << string << endl;
    cout << "Debug:" << endl;
    // printAll(heap->memorySegment);

    freeHeap(heap);
    return 0;
}
