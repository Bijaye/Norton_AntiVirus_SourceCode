//AJR 2/27/98 Replace && with || in map-bytes calculation
#include <stdio.h>
#include <stdlib.h>

#include "autoverv.h"

void create_invariant(struct sample_data *sample, int encrypt_type,
                      unsigned char **inv_bytes, unsigned char **inv_map)
  {
    unsigned char *buffer, *map_buffer;
    int i;

    if(!(buffer = (unsigned char *)calloc(sample->num_bytes,sizeof(unsigned char))))
      mem_err();
    if(!(map_buffer = (unsigned char *)calloc(sample->num_bytes,sizeof(unsigned char))))
      mem_err();

    memcpy(map_buffer,sample->map_bytes,sample->num_bytes);

    switch(encrypt_type) {

      case 0:     /* plain-text */
        free(buffer);
        free(map_buffer);
        buffer = sample->bytes;
        map_buffer = sample->map_bytes;
        i = sample->num_bytes;
        break;

      case 1:     /* xor */
        for(i=0;i<(sample->num_bytes-key_length[1]);i++)
          {
            buffer[i]=sample->bytes[i]^sample->bytes[i+1];   /* create invariant */
            if((sample->map_bytes[i+1]==0) /*AJR|| (sample->map_bytes[i]==0)*/)
              map_buffer[i]=0;
           }
        break;

      case 2:     /* add */
        for(i=0;i<(sample->num_bytes-key_length[2]);i++)
          {
            buffer[i]=sample->bytes[i+1]-sample->bytes[i];   /* create invariant */
            if((sample->map_bytes[i+1]==0) /*AJR|| (sample->map_bytes[i]==0)*/)
              map_buffer[i]=0;
           }
        break;

      case 3:     /* xor2 */
        for(i=0;i<(sample->num_bytes-key_length[3]);i++)
          {
            buffer[i]=sample->bytes[i]^sample->bytes[i+2];   /* create invariant */
            if(!sample->map_bytes[i+2] || !sample->map_bytes[i+1] /*AJR|| !sample->map_bytes[i]*/)
              map_buffer[i]=0;
           }
        break;

    }

    /* adjust the map bytes to compensate for key length */
    for(;i<sample->num_bytes;i++)
      map_buffer[i] = 'J';

    *inv_bytes = buffer;
    *inv_map = map_buffer;
  }
