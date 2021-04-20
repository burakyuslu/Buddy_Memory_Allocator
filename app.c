
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

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
//    void *p1 = sbmem_alloc(2333);
//    void *p2 = sbmem_alloc(1023);
//    void *p3 = sbmem_alloc(3950);
//    void *p4 = sbmem_alloc(1243);
//    sbmem_free(p1);
//    sbmem_free(p2);
//    void *p5 = sbmem_alloc(1071);
//    sbmem_free(p4);
//    sbmem_free(p5);
//    sbmem_free(p3);
//
//    sbmem_close();
//    sbmem_remove();
//    return (0);

//    // todo test script 4
//    // always gives seg-fault - only difference: free'ing order is different
    sbmem_init(  32768);
    sbmem_open();
    void *p1 = sbmem_alloc(2333);
    void *p2 = sbmem_alloc(1023);
    void *p3 = sbmem_alloc(3950);
    void *p4 = sbmem_alloc(1243);
    sbmem_free(p1);
    sbmem_free(p2);
    void *p5 = sbmem_alloc(1071);
    sbmem_free(p3);
    sbmem_free(p4);
    sbmem_free(p5);

    sbmem_close();
    sbmem_remove();
    return (0);

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


    return 0;
}

// lots of imports
# define SHR_MEM_NAME "/group_a_shr_mem"

void producerBasic() {

    // PRODUCER KISMI
    int fd; // fd = file descriptor
    void *shm_start; // data'ya pointer

    fd = shm_open(SHR_MEM_NAME, O_RDWR | O_CREAT, 0660); // memory'yi ac
    if (fd < 0) {
        perror("can not open shared memory\n");
        exit(1);
    }
    ftruncate(fd, 512); // memory'nin length'ini ayarla

    //mmap fonksiyonu bu sekilde kullaniliyor
    shm_start = mmap(NULL, 512, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_start < 0) {
        perror("can not map shared memory \n");
        exit(1);
    }
    // file descriptor'u kapat
    close(fd);

    // daha sonra memory'e veri yazabiliyor


}

// consumer
void consumerBasic() {
    int fd;
    void *sptr;
    struct stat sbuf; // sbuf memory segment'in size'i gibi seyleri tutuyor

    fd = shm_open("/afilename", O_RDWR, 0660); // shared memory segmentini aciyor
    if (fd < 0) {
        perror("can not open shared memory\n");
        exit(1);
    }
    fstat(fd, &sbuf); // burada sbuf'i fd'ye bagliyoruz

    // mmap usttekine benzer sekilde
    sptr = mmap(NULL, sbuf.st_size,
                PROT_READ | PROT_WRITE, MAP_SHARED,
                fd, 0);

    // fd'yi kapat
    close(fd);

    // free'nin karsiligi
    shm_unlink("/afilename");
}


// Semaphore usage.

#define SEMNAME_MUTEX       "/name_sem_mutex"

void semaphore_usage() {
    sem_t *sem_mutex;       /* explain the purpose: protects the buffer */

    // todo: kac adet semaphore a ihtiyacimiz olacak? bunu belirle.
    // todo: ayni isimdeki semaphorelari clean etmek ile etmemek arasindaki fark nedir?
    /* first clean up semaphores with same names */
    sem_unlink(SEMNAME_MUTEX);

    // create semaphore
    if (sem_mutex < 0) {
        perror("can not create semaphore\n");
        exit(1);
    }
    printf("sem %s created\n", SEMNAME_MUTEX);

    // use semaphore
    sem_wait(sem_mutex);
    // critical section begins



    // critical section ends
    sem_post(sem_mutex);

    // close semaphore
    // todo: bu fonksiyonlarin dokumantasyonunu oku
    sem_close(sem_mutex);
}