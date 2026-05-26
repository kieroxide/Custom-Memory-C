#include <iostream>
#include "Heap.h"

using namespace std;

int main(int argc, char** args) {
    // Create free memory segment
    cout << "\nProgram Starting...\n\n";
    Heap* heap = Heap::create();

    void* myString = heap->alloc(10);
    char* string = static_cast<char*>(myString);
    for (int i = 0; i < 9; i++) {
        string[i] = 'A';
    }
    string[9] = '\0';

    cout << "Program Output: " << endl;
    cout << string << endl;
    cout << "Debug:" << endl;

    // printAll(heap->memorySegment);

    void* s = heap->realloc(myString, 10);
    char* str = static_cast<char*>(s);
    cout << str[0] << endl;

    heap->freeHeap();
    return 0;
}
