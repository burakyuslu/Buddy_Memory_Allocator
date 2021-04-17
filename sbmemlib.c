
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
# include "sbmem.h"
#include <sys/stat.h>        /* For mode constants */
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include <bits/fcntl.h>
#include <errno.h>

#include <fcntl.h>

#define MIN 5
#define LEVELS 8
#define PAGE 4096

#define MIN_SEGMENT_SIZE 32768
#define MAX_SEGMENT_SIZE 262144

#define MIN_REQUEST_SIZE 128
#define MAX_REQUEST_SIZE 4096


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
    // todo: Look-up the man pages for mmap() and see what the arguments mean.
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

// Define semaphore(s)

// Define your stuctures and variables. 

// todo: add semaphore

#define SHR_MEM_NAME "/memsegname"

int sbmem_init(int segmentsize) {

    int fd;
    void *sbmem_ptr;

    // check if segmentsize value is valid
    if (segmentsize < MIN_SEGMENT_SIZE || segmentsize > MAX_SEGMENT_SIZE) {
        fprintf(STDERR_FILENO, "Error: Invalid segment size.");
        return -1;
    }

    fd = shm_open(SHR_MEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0660);
    // check if a shared segment is already allocated
    if (fd == -1) {
        // if previously allocated, destroy the previous segment
        if (errno == EEXIST) { // todo: test this code.
            sbmem_remove();
            fd = shm_open(SHR_MEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0660);
        } else {
            return -1;
        }
    }

    // adjust the new segment
    ftruncate(fd, segmentsize);

    // mmap explanation - remove later
    // addr: The starting address for the new mapping is specified in addr. If addr is NULL,
    // then the kernel chooses the (page-aligned) address at which to create the mapping
    // prot: access modifier for the selected memory; read allows read, write allows write;
    // exec allow execution; none removes all permissions
    // MAP_SHARED: changes made in the segment by one of the processes affect other processes as well. You can also set all to be private.
    // This is offset from where the file mapping started. In simple terms, the
    // mapping connects to (offset) to (offset+length-1) bytes for the file open on fd.
    sbmem_ptr = mmap(NULL, segmentsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);


    // todo: store relevant memory information

    return (0);
}

int sbmem_remove() {

    return (0);
}

int sbmem_open() {

    return (0);
}


void *sbmem_alloc(int size) {
    return (NULL);
}


void sbmem_free(void *p) {


}

int sbmem_close() {

    return (0);
}
