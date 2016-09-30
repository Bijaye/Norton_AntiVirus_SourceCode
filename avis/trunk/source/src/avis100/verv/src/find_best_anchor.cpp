#include <stdlib.h>
#include <stdio.h>
/*#include <values.h>*/
#include "autoverv.h"


int best_anchor(int mins[3], int maxes[3]);  /* computes best anchor type */ 

struct boundary_location  *find_best_anchor(int *offsets,
                                            int num_samples,
                                            struct sample_data **samples)
  {
    int
      i,
      startpos,
      filetype;

    int mins[file_types+1][3],maxes[file_types+1][3];
    int b,t,e/*,bestanchor[file_types+1],range[file_types+1]*/;

    struct boundary_location *boundary = NULL;


    for(filetype=0;filetype<=file_types;filetype++)
      for(i=0;i<3;i++)
        {
          mins[filetype][i] = MAXINT;     /* initial mins are big */
          maxes[filetype][i] = -MAXINT;   /* initial maxes are small */
        }

    for(i=0;i<num_samples;i++)
      if(samples[i])
        {
          startpos = offsets[i];
          /* calculate offsets from various anchors */
          b = startpos;
          t = startpos-samples[i]->entry_pt;
          e = startpos-samples[i]->num_bytes+1;

          /* check for new mins & maxes */
          filetype = get_file_type(samples[i]);  /* get number based on file type */
          if(filetype >= 0)  /* if successful */
            {
              /* set whichever mins & maxes are appropriate for this file type*/
              if (b<mins[filetype][0]) mins[filetype][0]=b;
              if (t<mins[filetype][1]) mins[filetype][1]=t;
              if (e<mins[filetype][2]) mins[filetype][2]=e;
              if (b>maxes[filetype][0]) maxes[filetype][0]=b;
              if (t>maxes[filetype][1]) maxes[filetype][1]=t;
              if (e>maxes[filetype][2]) maxes[filetype][2]=e;
            }
          if (b<mins[ALL][0]) mins[ALL][0]=b;
          if (t<mins[ALL][1]) mins[ALL][1]=t;
          if (e<mins[ALL][2]) mins[ALL][2]=e;
          if (b>maxes[ALL][0]) maxes[ALL][0]=b;
          if (t>maxes[ALL][1]) maxes[ALL][1]=t;
          if (e>maxes[ALL][2]) maxes[ALL][2]=e;

        }
    boundary = (struct boundary_location *) malloc(sizeof(struct boundary_location));

    for(filetype=0;filetype<=file_types;filetype++)
      {
        boundary->bestanchor[filetype] =
        best_anchor(mins[filetype],maxes[filetype]);   /* decide on a best anchor for this region */
        boundary->offset_min[filetype] = mins[filetype][boundary->bestanchor[filetype]];
        boundary->offset_range[filetype] = maxes[filetype][boundary->bestanchor[filetype]] -
          mins[filetype][boundary->bestanchor[filetype]];  /* compute range of offsets from this anchor */

      }

    return(boundary);
  }


int best_anchor(int mins[3], int maxes[3])
  {
    int i, min_delta=MAXINT, min_anchor;

    for(i=0;i<3;i++)
      if(maxes[i]-mins[i] < min_delta)
        {
          min_delta = maxes[i]-mins[i];
          min_anchor = i;
        }

    return(min_anchor);
  }
