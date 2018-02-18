#ifndef HEAP_H
#define HEAP_H

#include <cstddef>
#include <new>

#include <iostream>
using std::cout;
using std::endl;

class MemControlBlock {

public:

    /** @brief True if the MemControlBlock is before a block of available memory */
    bool available;

    /** @brief The size of the following block of memory, in bytes */
    size_t size;

    MemControlBlock * previous;
    MemControlBlock * next;

    MemControlBlock(const bool availableIn, const int sizeIn)
        : available(availableIn), size(sizeIn),
          previous(nullptr), next(nullptr)
    {
    }

};

class Heap {

private:
    char * const memory;

    /** @brief The first MemControlBlock for the heap -- the head of the list */
    MemControlBlock * startOfHeap;

public:

    ~Heap() {
        delete [] memory;
    }

    // no copy constructor, move constructor, assignment operator, ...
    Heap(const Heap &) = delete;
    Heap(Heap &&) = delete;
    Heap & operator=(const Heap &) = delete;
    Heap & operator=(Heap &&) = delete;

    Heap(const size_t sizeIn)
        : memory(new char[sizeIn]) {

        // make a MemControlBlock at the start of the reserved memory
        startOfHeap = new(memory) MemControlBlock(// true = is available
                                                  true,
                                                  // size = the size of the heap - the size of the MemControlBlock a the start of the heap
                                                  sizeIn - sizeof(MemControlBlock)
                                                 );
    }

    /** @brief Used for debugging - get the address of the start of the heap */
    char* getStartOfHeap() const {
        return memory;
    }

    /** @brief Used for debugging -- print out the details of the MemControlBlocks on the heap */
    void print() const {
        MemControlBlock * curr = startOfHeap;

        for (int i = 0; curr; ++i, curr = curr->next) {
            cout << "Block " << i << ": ";
            if (curr->available) {
                cout << " free, ";
            } else {
                cout << " in use, ";
            }
            cout << curr->size << " bytes\n";
        }

    }


    /** @brief Request a block of memory of the given size
     *
     * Uses the 'Worst Fit' algorithm to choose a suitable block of available memory
     *
     * Returns the memory address of the start of the requested memory
     * If no block is big enough, it returns nullptr.
     */
    char * allocateMemoryWorstFit(size_t requested) {
        int rounded_requested = (requested / 4) * 4;

        MemControlBlock * worstFit = worstFitAlg(rounded_requested);

        if (worstFit != nullptr) {
            worstFit->available = false;
            int spareSpace = (worstFit->size - rounded_requested) - sizeof(MemControlBlock);

            char * worstFitPos = reinterpret_cast<char*>(worstFit);
            if (spareSpace>=0) {

                MemControlBlock * memBlock = new (worstFitPos + sizeof(MemControlBlock) + rounded_requested) MemControlBlock(true, spareSpace);
                memBlock->next = worstFit->next;
                memBlock->previous = worstFit;
                worstFit->next = memBlock;
                worstFit->size = rounded_requested;

            }
            char * startAddress = sizeof(MemControlBlock) + worstFitPos;
            return startAddress;
        }

        return nullptr;

    }

    MemControlBlock * worstFitAlg(int rounded_requested) {
        MemControlBlock * curr = startOfHeap;
        MemControlBlock * worstFit = nullptr;
        do {
            if (curr->available) {
                if ((worstFit == nullptr) && (curr->size >= rounded_requested)) {
                       worstFit = curr;
                } else if (!(worstFit == nullptr) && (curr->size >= rounded_requested)) {
                    if (curr->size < worstFit->size) {
                        worstFit = curr;
                    }
                }
            }
            curr = curr->next;

        } while (curr != nullptr);
        return worstFit;
    }







    /** @brief Deallocate the memory used by the object at the given address */
    void deallocateMemory(char * toDeallocate) {
        char * address = toDeallocate - sizeof(MemControlBlock);
        MemControlBlock * block = reinterpret_cast<MemControlBlock *>(address);
        block->available = true;

        cout << block->previous << endl;

        if (block->previous) {
            if (block->previous->available) {

                block->previous->size = block->previous->size + block->size + sizeof(MemControlBlock);
                block->previous = block->next;
                //block->next = block->previous;

            }
        }

         if (block->next) {
             if (block->next->available) {

                block->next->size = block->next->size + block->size + sizeof(MemControlBlock);
                block->next = block->previous;
                //block->previous = block->next;


            }

        }

    }
};

#endif
