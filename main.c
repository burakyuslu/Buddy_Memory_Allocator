#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdlib.h>

#define MIN 7

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
    struct block avail[9]; // change this to support all segment sizes
    short int level;
    void *start;
};

void *hide(struct block *ptr);

struct block *show(void *ptr);

struct block *buddy(struct block *b, void *base, int k);

void *allocateFrom(int k, struct meta *data);

void *shm_start;

void deallocate(int max_level, struct block *l, struct meta *data) {
    int m = max_level;
    int k = l->k;

    // S1. Is buddy available
    struct block *p = buddy(l, shm_start, k);
    while (!(k == m || p->tag == false || (p->tag == true && p->k != k))) {
        // S2. Combine with buddy.
        ((struct block *) ((long) shm_start + (long) p->back))->front = p->front;
        ((struct block *) ((long) shm_start + (long) p->front))->back = p->back;
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
    ((struct block *) ((long) shm_start + (long) p))->back = (struct block *) ((long) l - (long) shm_start);
    l->k = k;
    l->back = (struct block *) ((long) &(avail[k]) - (long) shm_start);
    avail[k].front = (struct block *) ((long) l - (long) shm_start);

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

int main() {
    void *mem = malloc(1024 * 32);
    struct block *base = (struct block *) mem;
    shm_start = mem;
    int m = 8;
    base->front = base;
    base->back = base;
    base->tag = true;
    base->k = m;
    struct meta *data = (struct meta *) ((long int) base + sizeof(struct block));
    data->level = m;
    printf("%d\n", 1 << 1);
    for (int i = 0; i < m; i++) {
        data->avail[i].k = -1;
        data->avail[i].front = (struct block *) ((long int) &(data->avail[i]) - (long int) shm_start);
        data->avail[i].back = (struct block *) ((long int) &(data->avail[i]) - (long int) shm_start);
    }
    data->avail[m].tag = true;
    data->avail[m].k = -1;
    data->avail[m].front = (struct block *) ((long int) base - (long int) shm_start);
    data->start = base;
    data->avail[m].back = (struct block *) ((long int) base - (long int) shm_start);
    base->front = (struct block *) ((long int) &(data->avail[m]) - (long int) shm_start);
    base->back = (struct block *) ((long int) &(data->avail[m]) - (long int) shm_start);
    struct block *allocated = allocateFrom(1, data);
    void *p1 = allocateFrom(level(2333), data);
    void *p2 = allocateFrom(level(1023), data);
    void *p3 = allocateFrom(level(3950), data);
    void *p4 = allocateFrom(level(1243), data);
    deallocate(m, p1, data);
    deallocate(m, p2, data);
    void *p5 = allocateFrom(level(1071), data);
    deallocate(m, p3, data);
    deallocate(m, p4, data);
    deallocate(m, p5, data);
    deallocate(m, allocated, data);
    return 0;
}

void *allocateFrom(int k, struct meta *data) {
    // R1. Find block.
    int j;
    for (j = k; j <= data->level; j++) {
        long int t1 = (long) shm_start + (long) data->avail[j].front;
        long int t2 = (long) &(data->avail[j]);
        if (t1 != t2) break;
    };
    if (j > data->level) return NULL;

    // R2. Remove from list.
    struct block *avail = data->avail;
    struct block *l = (struct block *) ((long) shm_start + (long) avail[j].back);
    struct block *p = (struct block *) ((long) shm_start + (long) l->back);
    avail[j].back = (struct block *) ((long) p - (long) shm_start);
    p->front = (struct block *) ((long) &(avail[j]) - (long) shm_start);
    l->tag = false;

    // R3. split required?
    if (j == k) return l; // no

    while (j > k) { // R3 and R4. Split and split required
        j--;
        p = (struct block *) ((long int) l + (1 << (MIN + j)));
        p->tag = true;
        p->k = j;
        p->front = (struct block *) ((long) &(avail[j]) - (long) shm_start);
        p->back = (struct block *) ((long) &(avail[j]) - (long) shm_start);
        avail[j].front = (struct block *) ((long) p - (long) shm_start);
        avail[j].back = (struct block *) ((long) p - (long) shm_start);
    }
    l->k = k;
    l->tag = false;
    l->back = NULL;
    l->front = NULL;
    return l;
}


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

