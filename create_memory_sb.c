

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "sbmem.h"

int main(int argc, char** argv) {

    if( argc == 2){
        int mSegSize = atoi(argv[2]);
        sbmem_init(mSegSize);
    }
    else {
        sbmem_init(32768);
    }
    return (0);
}
