// mmem_clock.c
#include "mmem_clock.h"     // stud_clock_* prototypes :contentReference[oaicite:3]{index=3}
#include <stdlib.h>

memory* stud_clock_init_list() {
    memory* mem = malloc(sizeof(memory));
    if (!mem) return NULL;
    mem->is_full = false;
    mem->first   = 0;    // ‘hand’ / next candidate for eviction
    mem->last    = 0;    // next free slot
    for (int i = 0; i < NO_PAGE_FRAMES; i++) {
        mem->pages[i].virtual_address = 0;
        mem->pages[i].referenced      = false;
    }
    return mem;
}

void stud_clock_map_page(memory* mem, uint64_t virtual_address) {
    if (!mem || virtual_address == 0) return;

    // If already loaded, set referenced bit
    for (int i = 0; i < NO_PAGE_FRAMES; i++) {
        if (mem->pages[i].virtual_address == virtual_address) {
            mem->pages[i].referenced = true;
            return;
        }
    }

    if (!mem->is_full) {
        // still free slots: load at ‘last’
        load_page(virtual_address);                           // :contentReference[oaicite:4]{index=4}
        mem->pages[mem->last].virtual_address = virtual_address;
        mem->pages[mem->last].referenced      = true;         // difference from lecture
        mem->last = (mem->last + 1) % NO_PAGE_FRAMES;
        if (mem->last == mem->first)
            mem->is_full = true;
    } else {
        // full: find a victim via clock hand
        while (mem->pages[mem->first].referenced) {
            mem->pages[mem->first].referenced = false;
            mem->first = (mem->first + 1) % NO_PAGE_FRAMES;
        }
        // evict victim at ‘first’
        evict_page(mem->pages[mem->first].virtual_address);    // :contentReference[oaicite:5]{index=5}
        load_page(virtual_address);
        mem->pages[mem->first].virtual_address = virtual_address;
        mem->pages[mem->first].referenced      = true;
        // advance hand past the newly loaded page
        mem->first = (mem->first + 1) % NO_PAGE_FRAMES;
        mem->last  = mem->first;
    }
}

void stud_clock_access_page(memory* mem, uint64_t virtual_address) {
    if (!mem || virtual_address == 0) return;
    // hit?
    for (int i = 0; i < NO_PAGE_FRAMES; i++) {
        if (mem->pages[i].virtual_address == virtual_address) {
            mem->pages[i].referenced = true;
            return;
        }
    }
    // miss → map it
    stud_clock_map_page(mem, virtual_address);
}
