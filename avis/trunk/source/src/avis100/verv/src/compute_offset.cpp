#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "autoverv.h"

int compute_offset(int anchor, int offset, struct sample_data *sample)
  {
    assert (anchor >=0 && anchor <= 2);

    switch(anchor) {
      case 0:
        return(offset);
      case 1:
        return(sample->entry_pt+offset);
      case 2:
      return(sample->num_bytes+offset-1);
      }
    return -(sample->num_bytes+1);
  }



int compute_boundary_minoffset (const struct single_boundary* pBoundary,
                                struct sample_data *pSample)
{
   return compute_offset(pBoundary->bestanchor, pBoundary->offset_min, pSample);
}
