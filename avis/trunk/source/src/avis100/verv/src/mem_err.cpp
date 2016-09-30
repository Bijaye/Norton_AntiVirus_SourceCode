#include <stdio.h>
#include <stdlib.h>
/*#include <values.h>*/


#if defined(_cplusplus) || defined(__cplusplus)
   extern "C" {
#endif

void mem_err(void)

{
  printf("Memory allocation error!\n\n");
  abort();
}

#if defined(_cplusplus) || defined(__cplusplus)
   }   // extern "C" {
#endif
