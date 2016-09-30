#include <stdio.h>
#include <stdlib.h>
#include "autoverv.h"


void free_match(struct match_data *match)
  {
    if(match)
      {
        if(match->pos_in_sample)
          free(match->pos_in_sample);
        free(match);
      }
	match = NULL; // 6/25/97 Fred: for clarity and to keep BoundsChecker quiet
  }
