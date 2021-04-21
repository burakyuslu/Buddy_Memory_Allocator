
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <wait.h>

#include "sbmem.h"

int main() {
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

//    // todo test script 3 >> actually causes segmentation fault during random runs
//    // todo so far only happened when freeing p3
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

//    // todo test script 4
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

    // test script 5
//    pid_t idMain = getpid();
//    pid_t id1;
//    pid_t id2;
//    pid_t id3;
//    pid_t id4;
//    pid_t id5;
//    pid_t id6;
//    pid_t id7;
//    pid_t id8;
//    pid_t id9;
//    pid_t id10;
//    pid_t id11; // to test that it does not allow
    sbmem_init(1024 * 32);
    printf("test script 5 beginning \n");
    if (fork() == 0) {
        printf("pidbefore: %d\n", getpid());
        sbmem_open();
        printf("pidafter: %d\n",getpid());
        printf("pidbefore: %d\n", getpid());
        void *ptrTRY1 = sbmem_alloc(250);
        printf("pidafter: %d\n",getpid());
        printf("test script 5 beginning 5\n");
        printf("pid: %d, addr: %ld\n", getpid(), (long int) ptrTRY1);
        sbmem_free(ptrTRY1);
        sbmem_close();
        exit(0);
    } else if (fork() == 0) {
        sbmem_open();
        void *p1 = sbmem_alloc(2333);
        printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
        sbmem_free(p1);
        sbmem_close();
        exit(0);
    }
//    } else if (fork() == 0) {
//        sbmem_open();
//        void *p1 = sbmem_alloc(2333);
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//        sbmem_free(p1);
//        sbmem_close();
//        exit(0);
//    } else if (fork() == 0) {
//        sbmem_open();
//        void *p1 = sbmem_alloc(200);
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//        sbmem_free(p1);
//        sbmem_close();
//        exit(0);
//    } else if (fork() == 0) {
//        sbmem_open();
//        void *p1 = sbmem_alloc(200);
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//        sbmem_free(p1);
//        sbmem_close();
//        exit(0);
//    } else if (fork() == 0) {
//        sbmem_open();
//        void *p1 = sbmem_alloc(200);
//        printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//        sbmem_free(p1);
//        sbmem_close();
//        exit(0);
//    } else if (fork() == 0) {
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






//    if ((fork() == 0)) {
//        printf("test script 5 beginning 1\n");
//
//
//        if ((fork() == 0)) {
//            printf("test script 5 beginning 2\n");
//            if ((fork() == 0)) {
//                printf("test script 5 beginning 3\n");
//                if ((fork() == 0)) {
//                    printf("test script 5 beginning 4\n");
//                    // thread 4
////                    id4 = getpid();
//                    sbmem_open();
//                    void *ptrTRY1 = sbmem_alloc(51);
//                    printf("test script 5 beginning 5\n");
//                    printf("pid: %d, addr: %ld\n", getpid(), (long int) ptrTRY1);
//                    sbmem_free(ptrTRY1);
//                    sbmem_close();
//                } else {
//                    if ((fork() == 0)) {
//                        // process 5
////                        id5 = getpid();
//
//                        if ((fork() == 0)) {
//                            // process 6
////                            id6 = getpid();
//
//                            if ((fork() == 0)) {
//                                // process 7
////                                id7 = getpid();
//
//                                if ((fork() == 0)) {
//                                    // process 8
////                                    id8 = getpid();
//
//                                    if ((fork() == 0)) {
//                                        // process 9
////                                        id9 = getpid();
//
//                                        if ((fork() == 0)) {
//                                            // process 10
////                                            id10 = getpid();
//
//                                            sbmem_open();
//                                            void *p1 = sbmem_alloc(2333);
//                                            printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//                                            sbmem_free(p1);
//                                            sbmem_close();
//                                        }
//
//                                        // 9
//                                        sbmem_open();
//                                        void *p1 = sbmem_alloc(2333);
//                                        printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//                                        sbmem_free(p1);
//                                        sbmem_close();
//                                    }
//
//                                    // 8
//                                    sbmem_open();
//                                    void *p1 = sbmem_alloc(200);
//                                    printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//                                    sbmem_free(p1);
//                                    sbmem_close();
//                                }
//                                // 7
//                                sbmem_open();
//                                void *p1 = sbmem_alloc(200);
//                                printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//                                sbmem_free(p1);
//                                sbmem_close();
//                            }
//                            // 6
//                            sbmem_open();
//                            void *p1 = sbmem_alloc(200);
//                            printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//                            sbmem_free(p1);
//                            sbmem_close();
//                        }
//                        // 5
//                        sbmem_open();
//                        void *p1 = sbmem_alloc(200);
//                        printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//                        sbmem_free(p1);
//                        sbmem_close();
//                    }
//
//                    // thread 3
////                    id3 = getpid();
//
//                    sbmem_open();
//                    void *ptrX = sbmem_alloc(4000);
//                    printf("pid: %d, addr: %ld\n", getpid(), (long int) ptrX);
//                    void *ptrY = sbmem_alloc(2000);
//                    printf("pid: %d, addr: %ld\n", getpid(), (long int) ptrY);
//                    void *ptrZ = sbmem_alloc(997);
//                    printf("pid: %d, addr: %ld\n", getpid(), (long int) ptrZ);
//                    void *ptrXY = sbmem_alloc(91);
//                    printf("pid: %d, addr: %ld\n", getpid(), (long int) ptrXY);
//                    sbmem_free(ptrXY);
//                    sbmem_free(ptrZ);
//                    sbmem_free(ptrY);
//                    sbmem_free(ptrX);
//
//                    sbmem_close();
//                }
//            } else {
////                id2 = getpid();
//                sbmem_open();
//                void *p1 = sbmem_alloc(2333);
//                void *p2 = sbmem_alloc(1023);
//                void *p3 = sbmem_alloc(3950);
//                void *p4 = sbmem_alloc(1243);
//                printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//                printf("pid: %d, addr: %ld\n", getpid(), (long int) p2);
//                printf("pid: %d, addr: %ld\n", getpid(), (long int) p3);
//                printf("pid: %d, addr: %ld\n", getpid(), (long int) p4);
//                sbmem_free(p1);
//                sbmem_free(p2);
//                void *p5 = sbmem_alloc(1071);
//                printf("pid: %d, addr: %ld\n", getpid(), (long int) p5);
//                sbmem_free(p3);
//                sbmem_free(p4);
//                sbmem_free(p5);
//                sbmem_close();
//
//            }
//        } else {
//            // thread 1
//            sbmem_open();
//            void *p1 = sbmem_alloc(2333);
//            printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//            sbmem_free(p1);
//            sbmem_close();
//        }
//    } else {
//        // main process
//        // do nothing
//    }


    wait(NULL); // 1
    wait(NULL); // 2
    wait(NULL); // 3
    wait(NULL); // 4
    wait(NULL); // 5
    wait(NULL); // 6
    wait(NULL); // 7
    wait(NULL); // 8
    wait(NULL); // 9
    wait(NULL); // 10

//    fork();
//    fork();
//    fork();
//    sbmem_open();
//    void *p1 = sbmem_alloc(2333);
//    printf("pid: %d, addr: %ld\n", getpid(), (long int) p1);
//    void *p2 = sbmem_alloc(1023);
//    printf("pid: %d, addr: %ld\n", getpid(), (long int) p2);
//    void *p3 = sbmem_alloc(3950);
//    printf("pid: %d, addr: %ld\n", getpid(), (long int) p3);
//    void *p4 = sbmem_alloc(1243);
//    printf("pid: %d, addr: %ld\n", getpid(), (long int) p4);
//    sbmem_free(p1);
//    sbmem_free(p2);
//    void *p5 = sbmem_alloc(1071);
//    printf("pid: %d, addr: %ld\n", getpid(), (long int) p5);
//    sbmem_free(p3);
//    sbmem_free(p4);
//    sbmem_free(p5);
//    sbmem_close();
//    sbmem_remove();
    printf("test script 5 end");
    return 0;
}