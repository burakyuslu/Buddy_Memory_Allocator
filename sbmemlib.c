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
#include <semaphore.h>

#define MIN 7
#define MAX_LEVEL 11

#define MIN_SEGMENT_SIZE 32768
#define MAX_SEGMENT_SIZE 262144

#define MIN_REQUEST_SIZE 128
#define MAX_REQUEST_SIZE 4096

#define MAX_PROCESS_COUNT 10


// Define a name for your shared memory; you can give any name that start with a slash character; it will be like a filename.
#define SHR_MEM_NAME "/memsegname"

// todo: Define semaphore(s)
// #define SEM_META_NAME "semmeta"
#define SEM_MEM_NAME "semmem"
// sem_t *semMeta;
sem_t *semMem;

// Define your structures and variables.
struct overhead {
    bool tag;
    int k;
};

struct block {
    bool tag;
    int k;
    struct block *front;
    struct block *back;
};

struct meta {
    struct block avail[MAX_LEVEL]; // change this to support all segment sizes
    short int level;
    void *start;
    int processCount;
    int segmentSize;
    pid_t processPid[10];
};

void *shm_start;
struct meta *data;

void *allocateFrom(int k, struct meta *data);

void deallocate(int max_level, struct block *l, struct meta *data);

struct block *buddy(struct block *b, void *base, int k) {
    long int x = ((long int) b);
    long int m = 1 << (MIN + k + 1);
    long int offset = 1 << (MIN + k);
    long int res = (x - (long int) base) % m;
    if (res == 0) {
        return (struct block *) ((long int) b + offset);
    } else if (res == offset) {
        return (struct block *) ((long int) b - offset);
    } else {
        fprintf(stderr, "kardes kardese boyle yapmaz\n");
        exit(1);
    }
}

struct block *show(void *ptr) {
    return (struct block *) ((long int) ptr - sizeof(struct overhead));
}

void *hide(struct block *ptr) {
    return (void *) ((long int) ptr + sizeof(struct overhead));
}

int level(int req) {
    int total = req + (int) sizeof(struct overhead);
    int i = 0;
    int size = 1 << MIN;
    while (total > size) {
        size <<= 1;
        i += 1;
    }
    return i;
}

int sbmem_init(int segmentsize) {

    // semaphores
    // clean up semaphores with the same names in case it was not done properly before
    // sem_unlink(SEM_META_NAME);
    sem_unlink(SEM_MEM_NAME);

    // create & initialize the semaphores
    semMem = sem_open(SEM_MEM_NAME, O_CREAT, S_IROTH | S_IWOTH, 1);

    sem_wait(semMem);

    // check if segmentsize value is valid
    if (segmentsize < MIN_SEGMENT_SIZE || segmentsize > MAX_SEGMENT_SIZE) {
        fprintf(stderr, "Error: Invalid segment size.");
        return -1;
    }

    int fd = shm_open(SHR_MEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0660);

    // check if a shared segment is already allocated
    if (fd == -1) {
        // if previously allocated, destroy the previous segment
        if (errno == EEXIST) { // tested - functions correctly
            shm_unlink(SHR_MEM_NAME);
            fd = shm_open(SHR_MEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0660);
        } else {
            fprintf(stderr, "sbmem_init, Unknown Error: %s", strerror(errno)); // todo: remove this.
            return -1;
        }
    }

    // set this size of the memory
    ftruncate(fd, segmentsize);
    void *temp_shm_start = mmap(NULL, segmentsize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED , fd, 0);
    struct block *base = temp_shm_start;
    int m = level(segmentsize) - 1;
    base->front = base;
    base->back = base;
    base->tag = true;
    base->k = m;
    data = (struct meta *) ((long int) base + sizeof(struct block));
    data->level = (short int) m;
    data->start = base;
    data->processCount = 0;
    data->segmentSize = segmentsize;

    for (int i = 0; i < m; i++) {
        data->avail[i].k = -1;
        data->avail[i].front = &(data->avail[i]);
        data->avail[i].back = &(data->avail[i]);
    }
    data->avail[m].tag = true;
    data->avail[m].k = -1;
    data->avail[m].front = base;
    data->avail[m].back = base;
    data->start = base;
    base->front = &(data->avail[m]);
    base->back = &(data->avail[m]);
    allocateFrom(1, data);

    close(fd); // we no longer need the fd
    munmap(temp_shm_start, segmentsize);

    sem_post(semMem);
    return 0;
}

int sbmem_remove() {
    sem_unlink(SEM_MEM_NAME);
    shm_unlink(SHR_MEM_NAME);
    return (0);
}


int sbmem_open() {
    // get the file descriptor to the same shared memory segment
    // clean up semaphores with the same names in case it was not done properly before
    // sem_unlink(SEM_META_NAME);
    sem_unlink(SEM_MEM_NAME);

    // create & initialize the semaphores
    semMem = sem_open(SEM_MEM_NAME, O_CREAT, S_IROTH | S_IWOTH, 1);

    sem_wait(semMem);
    int fd = shm_open(SHR_MEM_NAME, O_RDWR, 0660);

    // first, map with the minimum segment size to get the actual segment size
    void *temp_shm_start = mmap(NULL, MIN_SEGMENT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED_NOREPLACE , fd, 0);
    if (temp_shm_start == MAP_FAILED) {
        fprintf(stderr, "cannot map the shared memory1: %s\n", strerror(errno));
        exit(1);
    }
    data = (struct meta *) ((long int) temp_shm_start + sizeof(struct block));

    int processCount = data->processCount;
    if (processCount >= MAX_PROCESS_COUNT) {
        return -1;
    }

    int actualSegmentSize = data->segmentSize;

    munmap(temp_shm_start, MIN_SEGMENT_SIZE);

    // map with the actual segment size
    shm_start = mmap(NULL, actualSegmentSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED_NOREPLACE , fd,
                     0);
    data = (struct meta *) ((long int) shm_start + sizeof(struct block));
    if (shm_start == MAP_FAILED) {
        fprintf(stderr, "cannot map the shared memory2: %s\n", strerror(errno));
        exit(1);
    }
    close(fd);

    pid_t pid = getpid();
    data->processPid[data->processCount] = pid;
    data->processCount++;

    sem_post(semMem);
    return 0;
}

void *allocateFrom(int k, struct meta *data) {
    // R1. Find block.
    int j;
    for (j = k; data->avail[j].front == &(data->avail[j]) && j <= data->level; j++);
    if (j > data->level) return NULL;

    // R2. Remove from list.
    struct block *avail = data->avail;
    struct block *l = avail[j].back;
    struct block *p = l->back;
    avail[j].back = p;
    p->front = &(avail[j]);
    l->tag = false;

    // R3. split required?
    if (j == k) return l; // no

    while (j > k) { // R3 and R4. Split and split required
        j--;
        p = (struct block *) ((long int) l + (1 << (MIN + j)));
        p->tag = true;
        p->k = j;
        p->front = &(avail[j]);
        p->back = &(avail[j]);
        avail[j].front = p;
        avail[j].back = p;
    }
    l->k = k;
    l->tag = false;
    l->back = NULL;
    l->front = NULL;
    return l;
}

void *sbmem_alloc(int size) {

    if (size < MIN_REQUEST_SIZE || size > MAX_REQUEST_SIZE)
        return NULL;
    if (data == NULL) {
        fprintf(stderr, "Meta data is null in sbmem_alloc\n");
    }
    if (shm_start == NULL) {
        shm_start = data->start;
    }
    sem_post(semMem);
    sem_wait(semMem);
    printf("data: %ld\n", (long int) data);
    printf("1pid: %d, requestSize: %d, allocated: %d\n", getpid(), size, 1 << (MIN + level(size)));
    void *res =  hide(allocateFrom(level(size), data));
    printf("2pid: %d, requestSize: %d, allocated: %d\n", getpid(), size, 1 << (MIN + level(size)));
    sem_post(semMem);
    return res;
}

void deallocate(int max_level, struct block *l, struct meta *data) {
    int m = max_level;
    int k = l->k;

    // S1. Is buddy available
    struct block *p = buddy(l, data->start, k);
    while (!(k == m || p->tag == false || (p->tag == true && p->k != k))) {
        // S2. Combine with buddy.
        p->back->front = p->front;
        p->front->back = p->back;
        k++;
        if (((long int) p) < ((long int) l))
            l = p;
        p = buddy(l, data->start, k);

    }

    // S3. Put on list.
    struct block *avail = data->avail;
    l->tag = true;
    p = avail[k].front;
    l->front = p;
    p->back = l;
    l->k = k;
    l->back = &(avail[k]);
    avail[k].front = l;

}

void sbmem_free(void *p) {
    sem_wait(semMem);
    if (p == NULL) {
        return;
    }
    if (data == NULL) {
        fprintf(stderr, "Meta data is null in sbmem_free\n");
    }
    deallocate(data->level, show(p), data);
    sem_post(semMem);
}

int sbmem_close() {
    sem_wait(semMem);

    // remove the process from the processes array
    // shift the processes in the array such that al processes (their id's) occupy the first processCount indices
    bool foundProcess = false;
    pid_t callerId = getpid();
    for (int i = 0; i < 10; i++) {
        if (callerId == data->processPid[i]) {
            foundProcess = true;
        }
        if (foundProcess && i != 10) {
            data->processPid[i] = data->processPid[i + 1];
        }
    }
    data->processCount--;

    munmap(shm_start, data->segmentSize);
    sem_post(semMem);
    sem_close(semMem);
    return 0;
}
