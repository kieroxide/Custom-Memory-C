#include <iostream>
#include <windows.h>

using namespace std;

const int TOTAL_MEM_SIZE = 1000 * 64; // 1000kb Bytes

enum Status { FREE, USED };

struct segment {
    size_t size;
    Status status;
    void* memory; // pointer to first address of segment
    segment* nextSegment;
};

void printAll(segment* memory);
void* my_malloc(segment* memory, size_t size);
segment* createSegment(size_t size, Status status, void* memory, segment* nextSegment = nullptr);

void my_free(void* ptr);
void* my_calloc(size_t count, size_t size);
void* my_realloc(void* ptr, size_t newSize);


int main(int argc, char** args) {
    // Create free memory segment
    cout << "\nProgram Starting...\n\n";
    void* mem = VirtualAlloc(NULL, TOTAL_MEM_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    segment* memorySegment = createSegment(TOTAL_MEM_SIZE - sizeof(segment), FREE, mem);

    void* myString = my_malloc(memorySegment, 10);
    char* string = static_cast<char*>(myString);
    for(int i = 0; i < 9; i++){
        string[i] = 'A';
    }
    string[9] = '\0';
    cout << "Program Output: " << endl;
    cout << string << endl;
    cout << "Debug:" << endl;
    printAll(memorySegment);

    VirtualFree(mem, TOTAL_MEM_SIZE, MEM_RELEASE);
    return 0;
}

// Creates and returns pointer to memory allocation with passed size param
void* my_malloc(segment* memory, size_t size) {
    // Find contigious free space in memory
    segment* currentSegment = memory;
    while (currentSegment != nullptr) {
        if(currentSegment->status == FREE) {
            // Create new segment header
            size_t freeSizeAfterSegmentCreation = currentSegment->size - size - sizeof(segment);
            currentSegment->status = USED;
            currentSegment->size = size;
            segment* freeSegment = createSegment(freeSizeAfterSegmentCreation, FREE, (char*)memory + sizeof(segment) + size);
            currentSegment->nextSegment = freeSegment;
            return currentSegment->memory;
        }
        currentSegment = currentSegment->nextSegment;
    }
    // Create segment in the free space
    return nullptr;
}

segment* createSegment(size_t size, Status status, void* memory, segment* nextSegment) {
    segment* seg = (segment*)memory;

    seg->size = size;
    seg->status = status;
    seg->memory = (char*)memory + sizeof(segment);
    seg->nextSegment = nextSegment;

    return seg;
}


void printAll(segment* memory){
    segment* current = memory;
    while(current != nullptr){
        cout << "Address: " << current;
        cout << ", Next Address: " << current->nextSegment;
        cout << ", Data Memory Address: " << current->memory;
        cout << ", Size: " << current->size;
        cout << ", Status: " << current->status << endl;
        current = current->nextSegment;
    }
}