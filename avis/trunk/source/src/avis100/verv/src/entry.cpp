#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "autoverv.h"

void entry(struct sample_data *sample)
{
  unsigned int entrypt;
  unsigned char *buffer;

  buffer = sample->bytes;

  if ((buffer[0]=='M') && (buffer[1]=='Z'))    /* if an .EXE */
    {
      /* compute entry point from header */
      entrypt = (((long)buffer[9])<<8) + (long)buffer[8];
      entrypt = entrypt<<4;
      entrypt += (((long) buffer[0x16] + (((long)buffer[0x17])<<8))<<4);
      entrypt += (((long) buffer[0x14] + (((long)buffer[0x15])<<8)));
      if(entrypt > 0x00100000)
        entrypt -= 0x00100000;
      strcpy(sample->file_type,"S-EXE");
    }
   else if (buffer[0]==0xE9)     /* if E9-COM */
     {
       /* get jump target */
       entrypt = 3 + buffer[1] + (((unsigned int) buffer[2]) << 8);
       strcpy(sample->file_type,"E9-COM");
     }
   else if (buffer[0]==0xE8)     /* if E8-COM */
     {
       /* get jump target */
       entrypt = 3 + buffer[1] + (((unsigned int) buffer[2]) << 8);
       strcpy(sample->file_type,"E8-COM");
     }
   else if (buffer[0]==0xFF && buffer[1]==0xFF)    /* if device driver */
     {
       /* compute entry point */
       entrypt = buffer[6] + (((unsigned int)buffer[7] << 8));
       strcpy(sample->file_type,"DEV");
     }
   else     /* otherwise, it's a P-COM or ? */
     {
       entrypt=0;   /* the entry point is at the beginning */
       strcpy(sample->file_type,"P-COM");
     }

  sample->entry_pt = entrypt;

  return;
}
