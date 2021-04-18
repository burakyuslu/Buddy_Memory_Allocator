
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


#define MIN 7
#define LEVELS 8
#define PAGE 4096

#define MIN_SEGMENT_SIZE 32768
#define MAX_SEGMENT_SIZE 262144

#define MIN_REQUEST_SIZE 128
#define MAX_REQUEST_SIZE 4096

#define SHR_MEM_NAME "/memsegname"

#define MAX_PROCESS_COUNT 10
struct Block {
    bool allocated;
    short int level;
    struct Block *next;
    struct Block *prev;
};

// Given a block we want to find the buddy
// We do this by toggling the bit
// that differentiate the address of the block from its buddy. For the 32 byte
// blocks, that are on level 0, this means that we should toggle the sixth bit.
struct Block *buddy(struct Block *block) {
    int index = block->level;
    long int mask = 0x1 << (index + MIN);
    if (index + MIN < 12)
        return (struct Block *) ((long int) block ^ mask);
    else
        return (struct Block *) ((long int) block - mask);
}

struct Block *split(struct Block *block) {
    int index = block->level - 1;
    int mask = 0x1 << (index + MIN);
    struct Block *result = (struct Block *) ((long int) block + mask);
    return result;
}

struct Block *primary(struct Block *block) {
    int index = block->level;
    long int mask = 0xffffffffffffffff << (1 + index + MIN);
    return (struct Block *) ((long int) block & mask);
}

int level(int req) {
    int total = req + (int) sizeof(short int);
    int i = 0;
    int size = 1 << MIN;
    while (total > size) {
        size <<= 1;
        i += 1;
    }
    return i;
}

void *hide(struct Block *block) {
    return (void *) (block + 1);
}

struct Block *show(void *memory) {
    return ((struct Block *) memory - 1);
}

struct Block *flists[LEVELS] = {NULL};

void insertNode(struct Block *node, struct Block *list[]) {
    int index = node->level;
    node->next = list[index];
    if (list[index])
        list[index]->prev = node; // todo: remove if unnecessary.
    list[index] = node; // bu satir su sekilde olmayacak mi: list[index]->prev = node;
}

struct Block *find(int index) {
    if (index < 0) {
        fprintf(stderr, "ERROR: find gets a negative index\n");
        exit(1);
    }
    // initializtion ???
    if (index > LEVELS - 1) { // required level cannot be provided.
        return NULL;
    } else if (flists[index] != NULL) { // the node with required level exists
        struct Block *node = flists[index];
        flists[index] = node->next; // remove the found node from the free list.
        return node;
    } else {
        struct Block *node = find(index + 1); // find the required level recursively.
        if (node != NULL) {
            do { // split the bigger blocks to smaller ones. add them to their respective free lists.
                struct Block *myBuddy = split(node);
                // todo: initialization
                node->level--;
                myBuddy->allocated = false;
                myBuddy->level = node->level;
                insertNode(myBuddy, flists);
            } while (node->level > index);
            return node;
        }
    }
    return NULL;
}

void *alloc(int size) {
    if (size == 0) {
        return NULL;
    }
    int index = level(size);
    struct Block *node = find(index);
    if (node == NULL) {
        return NULL;
    } else {
        node->allocated = true;
        return hide(node);
    }
}

void removeNode(struct Block *node, struct Block *list[]) {
    int index = node->level;
    if (list[index] == node) {
        list[index] = node->next;
        if (node->next) {
            node->next->prev = NULL;
            node->next->next = NULL;
        }
    } else {
        node->prev->next = node->next;
        if (node->next)
            node->next->prev = node->prev;
    }
    node->prev = NULL;
    node->next = NULL;
}

void freeBlock(struct Block *node) {
    struct Block *myBuddy = buddy(node);
    if (myBuddy->allocated) { // todo remove allocated boolean later.
        node->allocated = false;
        insertNode(node, flists);
    } else {
        struct Block *merged = node;
        while (!(myBuddy->allocated)) {
            // remove my buddy from the freelist.
            removeNode(myBuddy, flists);

            merged = primary(node);
            node->allocated = false;
            merged->level++;
            merged->allocated = false;

            // add larger block to the freelist.
            myBuddy = buddy(merged);
        }
        insertNode(merged, flists);
    }
}

void bfree(void *memory) {
    if (memory != NULL) {
        struct Block *block = show(memory);
        freeBlock(block);
    }
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
    void *temp_shm_start = mmap(NULL, segmentsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                                0);

    struct Block *s = (struct Block *) temp_shm_start;
    s->allocated = false;
    s->next = NULL;
    s->prev = NULL;
    s->level = LEVELS;
    flists[LEVELS - 1] = s;

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

int findMinimumRequiredLevel(int size) {
    bool isPowOf2 = false;
    int overhead = 32; // todo : update this.
    int necessarySize = size + overhead;
    int actualMemAllocSize = 0;

    // determine if the size is a power of 2
    if ((necessarySize != 0) && ((necessarySize & (necessarySize - 1)) == 0)) {
        isPowOf2 = true;
        actualMemAllocSize = necessarySize;
    }

    // if not, round up to the next power of 2
    int level = 0;
    if (!isPowOf2) {
        actualMemAllocSize = 1;
        while (actualMemAllocSize < necessarySize) {
            actualMemAllocSize = 2 * actualMemAllocSize;
        }
    }


}


void *sbmem_alloc(int size) {
    return (NULL);
}


void sbmem_free(void *p) {


}

int sbmem_close() {

    return (0);
}
