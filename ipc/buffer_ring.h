#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>

struct BufferRing {
	int *buffer;
	int idx_reader;
	int idx_writer;
	int size;
	
	sem_t mutex;
	sem_t slots;
	sem_t items;
};

struct BufferRing *init_buffer_ring(int size);

void free_buffer_ring(struct BufferRing *br);

int buffer_ring_read(struct BufferRing *br);

void buffer_ring_write(struct BufferRing *br, int value);
