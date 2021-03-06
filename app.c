
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>

#include "sbmem.h"

int randomNumberInRange(int low, int high) {
    return low + (rand() % (high - low));
}

int kb(int b) { return 1024 * b; }

void tester() {
    srand(time(NULL));

    const int size = 6;
    int mem_size[] = {kb(32), kb(64), kb(128), kb(256)};
    int range[] = {128, 256, 512, 1024, 2048, 4096};
    int k = 0;
    for (int memIndex = 0; memIndex < 4; memIndex++) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                sbmem_init(mem_size[memIndex]);
                sbmem_open();
                printf("%d,", mem_size[memIndex]);
                while (sbmem_alloc(randomNumberInRange(128, 4096)) !=NULL) {

//                    void *ptr = sbmem_alloc();
//                    sbmem_free(ptr);
//                    k++;
//                    if (k > 1000) {
//                        break;
//                    }
                }
                k = 0;
                sbmem_close();
                sbmem_remove();
            }
        }
    }

}

int main() {
    int i, ret;
    char *p;

    ret = sbmem_open();
    if (ret == -1)
        exit(1);

    p = sbmem_alloc(256); // allocate space to hold 1024 characters
    for (i = 0; i < 256; ++i)
        p[i] = 'a'; // init all chars to ‘a’
    sbmem_free(p);

    sbmem_close();
    return 0;
//    sbmem_init(32768);
//    void *p1 = sbmem_alloc(2000);
//    void *p2 = sbmem_alloc(2000);
//    sbmem_free(p1);
//    sbmem_free(p2);

//    sbmem_init(32768);
//    int i, ret;
//    char *p;
//
//    ret = sbmem_open();
//    if (ret == -1)
//	    exit (1);
//
//
//    p = sbmem_alloc (256); // allocate space to hold 1024 characters
//    for (i = 0; i < 256; ++i){
//        p[i] = 'a'; // init all chars to ‘a’
//    }
//    sbmem_free (p);
//
//    sbmem_close();
//
//    return (0);

//    sbmem_init( 4 * 32768);
//    sbmem_open();
//    void *p12 = sbmem_alloc(2333);
//    void *p22 = sbmem_alloc(1023);
//    void *p32 = sbmem_alloc(3950);
//    void *p42 = sbmem_alloc(1243);
//    sbmem_free(p12);
//    sbmem_free(p22);
//    void *p52 = sbmem_alloc(1071);
//    sbmem_free(p42);
//    sbmem_free(p52);
//    sbmem_free(p32);
//
//    sbmem_close();
//    sbmem_remove();
//    return (0);

//    // always gives seg-fault - only difference: free'ing order is different
//    sbmem_init(  32768);
//    sbmem_open();
//    void *p1 = sbmem_alloc(2333);
//    void *p2 = sbmem_alloc(1023);
//    void *p3 = sbmem_alloc(3950);
//    void *p4 = sbmem_alloc(1243);
//    sbmem_free(p1);
//    sbmem_free(p2);
//    void *p5 = sbmem_alloc(1071);
//    sbmem_free(p3);
//    sbmem_free(p4);
//    sbmem_free(p5);
//
//    sbmem_close();
//    sbmem_remove();
//    return (0);

    // test script 5 - never causes seg-fault
//    sbmem_init( 4 * 32768);
//    sbmem_open();
//    void *p1 = sbmem_alloc(2333);
//    sbmem_free(p1);
//    void *p2 = sbmem_alloc(1023);
//    sbmem_free(p2);
//    void *p3 = sbmem_alloc(3950);
//    sbmem_free(p3);
//    void *p4 = sbmem_alloc(1243);
//    sbmem_free(p4);
//    void *p5 = sbmem_alloc(1071);
//    sbmem_free(p5);
//
//    sbmem_close();
//    sbmem_remove();
//    return (0);

//    sbmem_init(1024 * 256);
//    printf("test script 5 beginning \n");
//    if (fork() == 0) {
//        sbmem_open();
//        void *ptrTRY1 = sbmem_alloc(250);
//        printf("test script 5 beginning pid1:%d\n", getpid());
//        sbmem_free(ptrTRY1);
//        sbmem_close();
//        exit(0);
//    } else if (fork() == 0) {
//        printf("test script 5 beginning pid:%d\n", getpid());
//        sbmem_open();
//        void *p1 = sbmem_alloc(2333);
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//        sbmem_free(p1);
//        sbmem_close();
//        exit(0);
//
//    } else if (fork() == 0) {
//        sbmem_open();
//        printf("test script 5 beginning pid:%d\n", getpid());
//        void *p1 = sbmem_alloc(2333);
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//        sbmem_free(p1);
//        sbmem_close();
//        exit(0);
//    } else if (fork() == 0) {
//        sbmem_open();
//        printf("test script 5 beginning pid:%d\n", getpid());
//        void *p1 = sbmem_alloc(200);
//        printf("test script 5 beginning pid:%d\n", getpid());
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//        sbmem_free(p1);
//        sbmem_close();
//        exit(0);
//    } else if (fork() == 0) {
//        sbmem_open();
//        printf("test script 5 beginning pid:%d\n", getpid());
//        void *p1 = sbmem_alloc(200);
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//        sbmem_free(p1);
//        sbmem_close();
//        exit(0);
//    } else if (fork() == 0) {
//        printf("test script 5 beginning pid:%d\n", getpid());
//        sbmem_open();
//        void *p1 = sbmem_alloc(200);
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//        sbmem_free(p1);
//        sbmem_close();
//        exit(0);
//    } else if (fork() == 0) {
//        printf("test script 5 beginning pid:%d\n", getpid());
//        sbmem_open();
//        void *ptrX = sbmem_alloc(4000);
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) ptrX);
//        void *ptrY = sbmem_alloc(2000);
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) ptrY);
//        void *ptrZ = sbmem_alloc(997);
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) ptrZ);
//        void *ptrXY = sbmem_alloc(91);
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) ptrXY);
//        sbmem_free(ptrXY);
//        sbmem_free(ptrZ);
//        sbmem_free(ptrY);
//        sbmem_free(ptrX);
//
//        sbmem_close();
//        exit(0);
//    } else if (fork() == 0) {
//        sbmem_open();
//        void *p1 = sbmem_alloc(2333);
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//        sbmem_free(p1);
//        sbmem_close();
//        exit(0);
//    }
//    printf("test script 5 end");
    return 0;
}