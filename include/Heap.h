#pragma once

class Heap {
  private:
    struct HeapStruct* heap;

  public:
    Heap();
    ~Heap();


    size_t getSize();
    void* alloc(size_t size);
};
