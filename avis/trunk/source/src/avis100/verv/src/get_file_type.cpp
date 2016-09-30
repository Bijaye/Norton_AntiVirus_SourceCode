#include <stdlib.h>
#include <string.h>
#include "autoverv.h"


int get_file_type(struct sample_data *sample)
  {
    if(!strcmp(sample->file_type,"S-EXE"))
      return(EXE);
    if(!strcmp(sample->file_type,"E8-COM") ||
       !strcmp(sample->file_type,"E9-COM") ||
       !strcmp(sample->file_type,"P-COM") )
      return(COM);

    /* otherwise ... */
    return(-1);
  }
