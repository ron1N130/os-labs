// mmem_fifo.c
#include "mmem_fifo.h"      // stud_fifo_* prototypes :contentReference[oaicite:0]{index=0}
#include <stdlib.h>

memory* stud_fifo_init_list() {
    memory* mem = malloc(sizeof(memory));
    if (!mem) return NULL;
    mem->is_full = false;
    mem->first = 0;
    mem->last  = 0;
    for (int i = 0; i < NO_PAGE_FRAMES; i++) {
        mem->pages[i].virtual_address = 0;   // empty slot
        // referenced bit unused in FIFO
    }
    return mem;
}

void stud_fifo_map_page(memory* mem, uint64_t virtual_address) {
    if (!mem || virtual_address == 0) return;

    // If already loaded, do nothing
    for (int i = 0; i < NO_PAGE_FRAMES; i++) {
        if (mem->pages[i].virtual_address == virtual_address)
            return;
    }

    if (!mem->is_full) {
        // still free slots: load at ‘last’
        load_page(virtual_address);                           // :contentReference[oaicite:1]{index=1}
        mem->pages[mem->last].virtual_address = virtual_address;
        mem->last = (mem->last + 1) % NO_PAGE_FRAMES;
        if (mem->last == mem->first)
            mem->is_full = true;
    } else {
        // full: evict at ‘first’
        evict_page(mem->pages[mem->first].virtual_address);    // :contentReference[oaicite:2]{index=2}
        load_page(virtual_address);
        mem->pages[mem->first].virtual_address = virtual_address;
        // advance both pointers to maintain circular queue
        mem->first = (mem->first + 1) % NO_PAGE_FRAMES;
        mem->last  = mem->first;
    }
}

void stud_fifo_access_page(memory* mem, uint64_t virtual_address) {
    if (!mem || virtual_address == 0) return;
    // hit?
    for (int i = 0; i < NO_PAGE_FRAMES; i++) {
        if (mem->pages[i].virtual_address == virtual_address)
            return;
    }
    // miss → map it
    stud_fifo_map_page(mem, virtual_address);
}
