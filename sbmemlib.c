
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
# include "sbmem.h"
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>


#define MIN 5
#define LEVELS 8
#define PAGE 4096

#define MIN_SEGMENT_SIZE 32768
#define MAX_SEGMENT_SIZE 262144

#define MIN_REQUEST_SIZE 128
#define MAX_REQUEST_SIZE 4096

#define SHR_MEM_NAME "/memsegname"

#define MAX_PROCESS_COUNT 10
enum flag {
    Free, Taken
};

struct head {
    enum flag status;
    short int level;
    struct head *next;
    struct head *prev;
};

struct head *new() {
    struct head *new = (struct head *) mmap(NULL,
                                            PAGE,
                                            PROT_READ | PROT_WRITE,
                                            MAP_PRIVATE | MAP_ANONYMOUS,
                                            -1,
                                            0);
    if (new == MAP_FAILED) {
        return NULL;
    }
    assert(((long int) new & 0xfff) == 0); // 12 l a s t b i t s s h o ul d be z e r o
    new->status = Free;
    new->level = LEVELS - 1;
    //When you extend the implementation to handle larger blocks you will have
    //to change these parameters.
    return new;
}


//Given a block we want to find the buddy
// We do this by toggling the bit
//that differentiate the address of the block from its buddy. For the 32 byte
//blocks, that are on level 0, this means that we should toggle the sixth bit.
struct head *buddy(struct head *block) {
    int index = block->level;
    long int mask = 0x1 << (index + MIN);
    return (struct head *) ((long int) block ^ mask);
}

struct head *split(struct head *block) {
    int index = block->level - 1;
    int mask = 0x1 << (index + MIN);
    return (struct head *) ((long int) block | mask);
}

struct head *primary(struct head *block) {
    int index = block->level;
    long int mask = 0xffffffffffffffff << (1 + index + MIN);
    return (struct head *) ((long int) block & mask);
}

int level(int req) {
    int total = req + sizeof(struct head);
    int i = 0;
    int size = 1 << MIN;
    while (total > size) {
        size <<= 1;
        i += 1;
    }
    return i;
}

void *hide(struct head *block) {
    return (void *) (block + 1);
}

struct head *magic(void *memory) {
    return ((struct head *) memory - 1);
}

struct head *flists[LEVELS] = {NULL};

struct head *find(int index) {

    return NULL;
}

void insert(struct head *block) {

}

void *balloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    int index = level(size);
    struct head *taken = find(index);
    return hide(taken);
}

void bfree(void *memory) {
    if (memory != NULL) {
        struct head *block = magic(memory);
        insert(block);
    }
    return;
}

// Define a name for your shared memory; you can give any name that start with a slash character; it will be like a filename.

// todo: Define semaphore(s)

// Define your stuctures and variables. 
void *shm_start;
struct SegmentMetaData {
    int segmentSize;
    int processCount;
    pid_t processPid[MAX_PROCESS_COUNT];
};



int sbmem_init(int segmentsize) {

    // check if segmentsize value is valid
    if (segmentsize < MIN_SEGMENT_SIZE || segmentsize > MAX_SEGMENT_SIZE) {
        fprintf(stderr, "Error: Invalid segment size.");
        return -1;
    }

    int fd = shm_open(SHR_MEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0660);

    // check if a shared segment is already allocated
    if (fd == -1) {
        // if previously allocated, destroy the previous segment
        if (errno == EEXIST) { // works.
            fprintf(stderr, "sbmem_init, Error: %s\n", strerror(errno));
            sbmem_remove();
            fd = shm_open(SHR_MEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0660);
        } else {
            fprintf(stderr, "sbmem_init, Unknown Error: %s", strerror(errno)); // todo: remove this.
            return -1;
        }
    }

    // set this size of the memory
    ftruncate(fd, segmentsize);
    // todo: store relevant memory information (possibly in connection to the offset value)
    struct SegmentMetaData *meta = (struct SegmentMetaData *) sbmem_alloc(sizeof(struct SegmentMetaData));
    meta->segmentSize = segmentsize;
    meta->processCount = 0;

    close(fd); // we no longer need the fd
    return 0;

    // todo: store relevant memory information

    return (0);
}

int sbmem_remove() {

    return (0);
}

void *openSharedMemory() {
    int fd = shm_open(SHR_MEM_NAME, O_RDWR, 0660);

    // first, map with the minimum segment size to get the actual segment size
    void *temp_shm_start = mmap(NULL, MIN_SEGMENT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                                0);
    if (temp_shm_start == MAP_FAILED) {
        fprintf(stderr, "cannot map the shared memory: %s\n", strerror(errno));
        exit(1);
    }
    struct SegmentMetaData *metaData = (struct SegmentMetaData *) temp_shm_start;

    // check if the process can access the library
    int processCount = metaData->processCount;
    if (processCount >= MAX_PROCESS_COUNT) {
        return -1;
    }

    int actualSegmentSize = metaData->segmentSize; // todo: read the actual segment siz

    munmap(temp_shm_start, MIN_SEGMENT_SIZE);

    // map with the actual segment size
    temp_shm_start = mmap(NULL, actualSegmentSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // todo addr degisecek mi
    if (temp_shm_start == MAP_FAILED) {
        fprintf(stderr, "cannot map the shared memory: %s\n", strerror(errno));
        exit(1);
    }
    return temp_shm_start;
}

int sbmem_open() {
    // get the file descriptor to the same shared memory segment
    shm_start = openSharedMemory();
    struct SegmentMetaData *metaData = (struct SegmentMetaData *) shm_start;

    pid_t pid = getpid();
    metaData->processPid[metaData->processCount] = pid;
    metaData->processCount++;
    return 0;
}


void *sbmem_alloc(int size) {
    return (NULL);
}


void sbmem_free(void *p) {


}

int sbmem_close() {

    return (0);
}
