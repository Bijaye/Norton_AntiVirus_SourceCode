#include <stdio.h>


#if defined(_cplusplus) || defined(__cplusplus)
   extern "C" {
#endif

long int filesize(FILE *stream)
  {
    long int currpos, result;

    currpos = ftell(stream);         /* save old file pos */
    fseek(stream, 0L, SEEK_END);     /* go end */
    result = ftell(stream);        /* get size */
    fseek(stream,currpos,SEEK_SET);  /* return to original pos */
    return(result);
  }

#if defined(_cplusplus) || defined(__cplusplus)
   }   // extern "C" {
#endif
