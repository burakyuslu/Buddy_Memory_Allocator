

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "sbmem.h"

// todo revision - birsey degistirmedim
int main() {
    sbmem_remove();
    printf ("Memory segment is removed from the system. System is clean now.\n");
    printf ("You can no longer run processes to allocate memory using the library\n");
    return (0);
}
