#include "duMalloc.h"   // Include the header file for the custom memory management system
#include <stdio.h>       // Standard I/O for printing memory dumps
#include <string.h>      // For memory manipulation functions, e.g., memset

// Define the structure for memory blocks. Each block has a header with a size and a pointer to the next block.
typedef struct memoryBlockHeader {
    int size;                    // Size of the block excluding the header
    struct memoryBlockHeader* next;  // Pointer to the next block in the free list
} memoryBlockHeader;

// Define the size of the heap (128 * 8 bytes = 1024 bytes)
#define HEAP_SIZE (128 * 8)

// Define the heap as a global array of unsigned chars
unsigned char heap[HEAP_SIZE];  // Simulated heap memory, represented as raw bytes

// Define a global pointer to the head of the free list (the list of available memory blocks)
memoryBlockHeader* freeListHead = NULL;

// Initialize the memory allocator by setting up the heap and the free list
void duInitMalloc() {
    // Zero out the heap memory for debugging purposes
    memset(heap, 0, HEAP_SIZE);

    // Create the first block header at the start of the heap (the entire heap is initially free)
    memoryBlockHeader* currentBlock = (memoryBlockHeader*)heap;

    // Set up the free block: the entire heap is initially free, so the size is the total heap size minus the header size
    currentBlock->size = HEAP_SIZE - sizeof(memoryBlockHeader);  // Remaining space after accounting for the header size
    currentBlock->next = NULL;  // No next block since this is the only free block

    // Set the free list head to point to this block (this is the first free block)
    freeListHead = currentBlock;
}

// Function to print the current memory layout, particularly the free list
void duMemoryDump() {
    printf("MEMORY DUMP\n");
    printf("Free List\n");

    memoryBlockHeader* current = freeListHead;  // Start at the head of the free list
    while (current != NULL) {
        // Print the address of the current block, its offset from the heap start, and its size
        printf("Block at %p (offset: %ld), size %d\n", current, (unsigned char*)current - heap, current->size);
        current = current->next;  // Move to the next block in the free list
    }
}

// Function to allocate memory from the heap
void* duMalloc(int size) {
    // Round the requested size to the next multiple of 8 bytes (for memory alignment)
    size = (size + 7) & ~7;

    // Add space for the memory block header, which is part of the block
    size += sizeof(memoryBlockHeader);

    memoryBlockHeader* current = freeListHead;  // Start from the head of the free list
    memoryBlockHeader* prev = NULL;  // Pointer to the previous block, used for maintaining the list

    // Search for a block that is large enough to accommodate the requested size
    while (current != NULL) {
        if (current->size >= size) {  // If the current block is large enough
            // If the block is larger than needed, split it to create a new free block after the allocated block
            if (current->size > size + sizeof(memoryBlockHeader)) {
                // Create a new free block after the allocated block (split the current block)
                memoryBlockHeader* newFreeBlock = (memoryBlockHeader*)((unsigned char*)current + size);
                newFreeBlock->size = current->size - size;  // Set the size of the new free block
                newFreeBlock->next = current->next;  // Point the new free block to the next block in the list

                // Update the current block's size and adjust the free list
                current->size = size - sizeof(memoryBlockHeader);  // Adjust current block size to allocated size
                current->next = newFreeBlock;  // Set the next pointer of the allocated block to the new free block

                // If the current block is the head of the free list, update the free list head
                if (prev == NULL) {
                    freeListHead = current->next;
                } else {
                    prev->next = current->next;  // Update the previous block's next pointer
                }
            }

            // Return the memory just past the block header (the actual allocated memory)
            return (unsigned char*)current + sizeof(memoryBlockHeader);
        }

        prev = current;  // Move to the next block in the free list
        current = current->next;
    }

    // If no suitable block was found, return NULL
    return NULL;
}

// Function to free allocated memory and return it to the free list
void duFree(void* ptr) {
    // Cast the pointer back to a memoryBlockHeader by subtracting the header size
    memoryBlockHeader* blockToFree = (memoryBlockHeader*)((unsigned char*)ptr - sizeof(memoryBlockHeader));

    // Insert the freed block back into the free list in memory order
    memoryBlockHeader* current = freeListHead;
    memoryBlockHeader* prev = NULL;

    // Search for the appropriate position in the free list to insert the freed block
    while (current != NULL && (unsigned char*)current < (unsigned char*)blockToFree) {
        prev = current;
        current = current->next;
    }

    // Insert the freed block into the free list
    blockToFree->next = current;  // Point the freed block to the next block in the free list
    if (prev == NULL) {
        freeListHead = blockToFree;  // If the block is the first in the free list, update the head
    } else {
        prev->next = blockToFree;  // Otherwise, insert it between prev and current
    }
}
