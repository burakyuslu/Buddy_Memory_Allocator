/* -*- linux-c -*- */
/* $Id: commondefs.h,v 1.2 2015/03/04 12:01:17 korpe Exp korpe $   */

#define BUFFER_SIZE 10
  
struct shared_data
{
  int in;
  int out;
  int buffer[BUFFER_SIZE];
};


#define SHAREDMEM_NAME "/somenamehere"
#define SHAREDMEM_SIZE 1024 /* bytes; large enough to hold the data */

#define NUM_ITEMS_TOPASS 50
