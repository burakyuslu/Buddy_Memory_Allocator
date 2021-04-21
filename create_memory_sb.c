

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "sbmem.h"

//todo revision
int main(int argc, char** argv) {
    if(argc != 2 && argc != 1){
        printf("Error: create_memory_sb: Argument error!\n");
        printf("Usage 1 (Allocates the default minimum size): ./create_memory_sb\n");
        printf("Usage 2 (Specify the allocation size): ./create_memory_sb memorySegmentSize\n");
        printf("Exiting...\n");
        exit (1);
    }
    if( argc == 2){
        int mSegSize = atoi(argv[2]);
        sbmem_init(mSegSize);
        printf("Desired memory initialized, with the given size value.\n");
    }
    else {
        sbmem_init(32768);
        printf("Desired memory initialized, with the default minimum value.\n");
    }
    return (0);
}
