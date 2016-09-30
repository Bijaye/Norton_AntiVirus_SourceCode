#include <stdlib.h>
#include <stdio.h>
#include "autoverv.h"


void reconstruct_status(struct phase2_match *recon_summary,
                  struct reconstruction_vector *goat_bytes,
                  int num_goat_bytes, int filetype,
                  int *num_missing, int *calc_mz)

  {
    int i, offset;
    int *covered;
    struct phase2_match *r_summary;

    covered = (int *)calloc(num_goat_bytes,sizeof(int));

    /* figure out what we covered & didn't cover */
    r_summary = recon_summary;
    while(r_summary)
      {
      for(i=0;i<num_goat_bytes;i++)
        if(r_summary->covered_goat_bytes[i])
          covered[i] = 1;
        r_summary = r_summary->next;
      }


    *num_missing = *calc_mz = 0;

    if(filetype == EXE)
      {
      for(i=0;i<num_goat_bytes;i++)
        if(!covered[i])
          {
            if(goat_bytes[i].loc.anchor_type != 0)  /* if not from BOF */
            (*num_missing)++;
            else  /* offset from beginning of file */
            {
              offset = goat_bytes[i].loc.offset;
              if (offset < 6)  /* if in first 6 bytes, CALC_MZ_LENGTH will fix */
                *calc_mz = 1;
              else if((offset != 0x12) && (offset != 0x13))
                (*num_missing)++;
            }
          }
      }
    else  /* for COMs, must reconstruct all bytes */
      for(i=0;i<num_goat_bytes;i++)
      if(!covered[i])
        (*num_missing)++;

	// 6/26/97 Fred: added the following line
	free (covered);

  }

