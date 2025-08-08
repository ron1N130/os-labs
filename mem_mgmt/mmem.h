#ifndef MMEM_H__
#define MMEM_H__

#include <stdbool.h>
#include <stdint.h>

#define NO_PAGE_FRAMES 8

// The page_frame struct contains everything that is related to a page_frame
typedef struct page_frame {
    // the virtual page that is in this page frame
    uint64_t virtual_address;
    
    // referenced bit - only needed for CLOCK
    bool referenced;
} page_frame;

// The memory struct manages all page frames of a given system
// You may organize your pages in the array of page_frames however you like
// (as long as you evict and load the right pages for the strategy of course :) )
typedef struct memory {
    struct page_frame pages[NO_PAGE_FRAMES];
    bool is_full;
    uint8_t first;
    uint8_t last;
} memory;


// Evicts a page with the virtual address virtual_address
// You do not need to implement this function, 
// you should however call this function, whenever you evict a page in your code.
void evict_page(uint64_t virtual_address);

// Loads a page with the virtual address virtual_address
// You do not need to implement this function, 
// you should however call this function, whenever you load a page in your code.
void load_page(uint64_t virtual_address);


#endif