
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "sbmem.h"

int main() {

//    int i, ret;
//    char *p;
//
//    ret = sbmem_open(); 
//    if (ret == -1)
//	exit (1);
//
//    p = sbmem_alloc (256); // allocate space to hold 1024 characters
//    for (i = 0; i < 256; ++i)
//	p[i] = 'a'; // init all chars to ‘a’
//    sbmem_free (p);
//
//    sbmem_close();
//
//    return (0);
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