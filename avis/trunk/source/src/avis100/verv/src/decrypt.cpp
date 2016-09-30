#include <stdlib.h>
#include <stdio.h>

#include "autoverv.h"


unsigned char *decrypt(struct match_data *match, unsigned char *bytes)
  {
    unsigned char *buffer;
    int i;

    /* allocate buffer */
    buffer = (unsigned char *)ALLOC(unsigned char, match->length);  
    if(!buffer)
      mem_err();

    switch(match->type) {
        case 0:  /* plain */
        memcpy(buffer,bytes,match->length);
        break;
      
      case 1:  /* xor */
        for(i=0;i<match->length;i++)
          buffer[i] = bytes[i]^bytes[0];
        break;

      case 2:  /* add */
        for(i=0;i<match->length;i++)
          buffer[i] = bytes[i]-bytes[0];
        break;
       
      case 3:  /* xor2 */
        //for(i=0;i<match->length;i+=2)
        for(i=0;i< ((match->length)/2)*2 ;i+=2)
          {
            buffer[i] = bytes[i]^bytes[0];
            buffer[i+1] = bytes[i+1]^bytes[1];
          }
      }
    return(buffer);
  } 
