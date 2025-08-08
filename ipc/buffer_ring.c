#include <sys/mman.h>
#include "buffer_ring.h"
#include <stdlib.h>

struct BufferRing *init_buffer_ring(int size) {
	
	struct BufferRing *br = mmap(NULL, sizeof *br, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (br == MAP_FAILED) return NULL;
    
    br->buffer = mmap(NULL, sizeof(int) * size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    if (br->buffer == MAP_FAILED){
        munmap(br, sizeof *br);
        return NULL;
    }
    
    br->size = size;
    br->idx_reader = 0;
    br->idx_writer = 0;
    
    sem_init(&br->mutex, 1, 1);
    sem_init(&br->slots, 1, size);
    sem_init(&br->items, 1, 0);
    return br;
}

void free_buffer_ring(struct BufferRing *br) {
    
    if (!br) return;
    sem_destroy(&br->mutex);
    sem_destroy(&br->slots);
    sem_destroy(&br->items);
    munmap(br->buffer, sizeof(int) * br->size);
    munmap(br, sizeof *br);
}

int buffer_ring_read(struct BufferRing *br) {
	sem_wait(&br->items);
	sem_wait(&br->mutex);
	
	int val = br->buffer[br->idx_reader];
	br->idx_reader = (br->idx_reader +1) % br->size;
	
	sem_post(&br->mutex);
	sem_post(&br->slots);
	
	return val;
}

void buffer_ring_write(struct BufferRing *br, int value) {
	
	sem_wait(&br->slots);
	sem_wait(&br->mutex);
	
	br->buffer[br->idx_writer] = value;
	br->idx_writer = (br->idx_writer + 1) % br->size;
	
	sem_post(&br->mutex);
	sem_post(&br->items);
}
