# A memory allocator

In this coursework you will be implementing a heap memory model based on memory control blocks, as discussed in the lecture.  It will use the 'Worst Fit' allocation strategy.  This work is due at 12:00 on Monday the 19th of February.

# Preamble

The file `heap.h` contains the `MemControlBlock` class seen in the lecture.  Objects of this type are placed before used/available blocks of memory on the Heap, and are doubly-linked: each `MemControlBlock` has a pointer to the next and preceding blocks.  (For the head of the list, previous is `nullptr`; for the tail of the list, next is `nullptr`.)

The file `heap.h` also contain an incomplete implementation of a class `Heap`.  This mimics the Heap memory -- it reserves a block of memory by making an array of chars (a.k.a. bytes); then places a `MemControlBlock` at the start of this block.  It uses *placement new* for this, as you will have seen in the lecture:

`startOfHeap = new(memory) MemControlBlock(true, sizeIn - sizeof(MemControlBlock))`  

To help you, there is also a `print()` function that will loop over the `MemControlBlock`s, beginning at `startOfHeap`, and print out their sizes and whether they are available.  (This also will remind you how to loop through a linked list, one node at a time.)

# a) Allocation [60% of the marks]

Complete the implementation of the function:

`char * allocateMemoryWorstFit(size_t requested)`  

This function is used to allocate blocks of memory for storing objects.  It takes a memory allocation request (a number of bytes -- `size_t` is another name for an unsigned integer) and returns a pointer to where the object should be created.  To use the Heap class, and this function we can write e.g:

`Heap heap(1048576);`  
`char * addrOne = heap.allocateMemoryWorstFit(sizeof(TestObject));`  
`TestObject * objOne = new(addrOne) TestObject();`  

Your implementation of the function should do the following:

- Round the amount `requested` up to a multiple of 4 bytes (to ensure alignment)
- Use the worst-fit-first algorithm to choose the appropriate block of memory to use for the allocation request; if no suitable block of memory is available, return nullptr..
- Mark the chosen block as no longer being available
- Calculate the amount of spare space in the chosen available block (by subtracting the requested amount)
- If this is large enough to accomodate another `MemControlBlock`:
    * Make a new MemControlBlock at the appropriate memory address: the address of the chosen block; plus the size of a MemControlBlock; plus the requested amount.  Set its size to be the amount of free memory after this new MemControlBlock, and before the next MemControlBlock.
    * Update the next/previous pointers: the new MemControlBlock should come after the chosen block, and before what used to be after the chosen block
    * Edit the size of the chosen block to the amount requested (as the new MemControlBlock took the rest of the space in the hole)
- Return the address of where the allocated memory starts: immediately after the MemControlBlock block (i.e. adding the size of a MemControlBlock onto the address of the chosen block)

To test your code, a test harness `TestHeap.cc` has been provided.  To compile this, and ensure your compiler is producing 32-bit code, run:

`g++ -std=c++11 -m32 -g -o TestHeap TestHeap.cc`

(The `-m32` argument sets it to 32-bits.)

After finishing part (a), everything in this should work up to the tests on deallocation.

# b) Deallocation [40% of the marks]

Complete the implementation of the function:

`void deallocateMemory(char * toDeallocate)`

This should:
- Take the memory address `toDeallocate`
- Subtract the size of a MemControlBlock off it, to get the address of the MemControlBlock that sits before the memory being deallocated
- Mark it as being available
- If the previous MemControlBlock is available, merge them together:
    * increase the size of the previous MemControlBlock to cover the amount of memory being deallocated, plus the size of a MemControlBlock
    * update the previous/next pointers so the MemControlBlock in front of the memory being deallocated is no longer in the list
- If the next MemControlBlock is available, merge with it, too, in a similar way.

The `TestHeap` file will test deallocation too -- there should be no errors when running the code.
