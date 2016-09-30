//AJR: when looking at the infected sample map map_list for bytes to be used in
//     reconstruction, leave out the variable regions (NB: the variable region 
//     between uni-goat segments is IN the set of bytes to be reconstructed.)

//AJR: Add adjust...boundary() to deal with insufficient variability in the 
//     samples (see below.)

#include <stdio.h>
#include <stdlib.h>
/*#include <values.h>*/
#include "autoverv.h"

static void adjust_begin_boundary (struct single_boundary* pBegin_boundary);
static void adjust_end_boundary (struct single_boundary* pEnd_boundary);

   

void make_recon_arrays(struct sample_data **samples, struct sample_data **goat_samples,
                       int num_samples, struct universal_goat_match *uni_goat_match,
                       struct map_rec *map_list, int filetype,
                       struct reconstruction_vector **goat_bytes, 
                       struct reconstruction_vector **variable_virus_bytes,
                       int num_files, int *num_goat_bytes, int *num_vv_bytes)

{
  int i, j, /*AJRmax,*/ currbyte, pos;
  int curroffset, curranchor, *sample_num;
  struct single_boundary tmp_begin, tmp_end;   //AJR, tmp_next_begin;
  struct universal_goat_match *gmatch;
  struct map_rec *mlist;
  int shortest_inf_num, shortest_inf_len=MAXINT, extra;
  int shortest_goat_num, shortest_goat_len=MAXINT;
  int gap_length; //AJR


  *num_goat_bytes = *num_vv_bytes = 0;
  *goat_bytes = *variable_virus_bytes = NULL;

  /* count number of applicable files & remember shortest */
  for(i=0;i<num_samples;i++)
    if(goat_samples[i] && (get_file_type(samples[i]) == filetype))
      {
      /* remember shortest infected sample */
      if(samples[i]->num_bytes < shortest_inf_len)
        {
          shortest_inf_num = i;
          shortest_inf_len = samples[i]->num_bytes;
        }

      /* remember shortest goat sample */
      if(goat_samples[i]->num_bytes < shortest_goat_len)
        {
          shortest_goat_num = i;
          shortest_goat_len = goat_samples[i]->num_bytes;
        }
      
      }

  // 6/26/97 Fred: Moved the following test before any memory allocation

  if(!num_files || !uni_goat_match || !map_list)  /* nothing to do, so forget it */
  return;

  /* set up a map back to the old sample #'s */
  sample_num = (int *)calloc(num_files,sizeof(int));
  for(i=0,j=0;i<num_samples;i++)
    if(goat_samples[i] && (get_file_type(samples[i]) == filetype))
      sample_num[j++] = i;

  /* figure out how many goat bytes need to be reconstructed */
  gmatch = uni_goat_match;
  curroffset = 0;
  curranchor = 0;
/*  curranchor = gmatch->g_begin.bestanchor; */
  while(gmatch)
    {
      if(gmatch->g_begin.bestanchor == curranchor)  /* if in same anchor type */
      *num_goat_bytes += gmatch->g_begin.offset_min+gmatch->g_begin.offset_range-curroffset;  /* add the missing bytes */

      curranchor = gmatch->g_end.bestanchor;
      curroffset = gmatch->g_end.offset_min+1;

      if(!gmatch->next)  /* if last section */
      { /* fill to EOF */
        *num_goat_bytes += compute_offset(2,0,goat_samples[shortest_goat_num]) - 
          compute_offset(curranchor,curroffset,goat_samples[shortest_goat_num]) + 1;
      }

      gmatch = gmatch->next;
    }
  
  /* now that we know how much space we need, allocate it */
  *goat_bytes = (struct reconstruction_vector *)calloc(*num_goat_bytes,sizeof(struct reconstruction_vector));
  for(i=0;i<*num_goat_bytes;i++)
    ((*goat_bytes)[i]).sample = (unsigned char *)calloc(num_files,sizeof(unsigned char));

  /* fill up goat array with the proper bytes */
  gmatch = uni_goat_match;
  currbyte = curroffset = 0;
  curranchor = 0;
/*  curranchor = gmatch->g_begin.bestanchor; */
  while(gmatch)
    {
      if(gmatch->g_begin.bestanchor == curranchor)   /* if same anchor type */
      {   /* add the missing bytes */
        gap_length = gmatch->g_begin.offset_min+gmatch->g_begin.offset_range-curroffset;
        for(i=0;i<gap_length;i++)
          {
            for(j=0;j<num_files;j++)
            {
              pos = compute_offset(curranchor,curroffset+i,goat_samples[sample_num[j]]);
              ((*goat_bytes)[currbyte]).sample[j] = goat_samples[sample_num[j]]->bytes[pos];
              ((*goat_bytes)[currbyte]).loc.anchor_type = curranchor;
              ((*goat_bytes)[currbyte]).loc.offset = curroffset+i;
            }
            currbyte++;
          }
      }
      curranchor = gmatch->g_end.bestanchor;
      curroffset = gmatch->g_end.offset_min+1;

      if(!gmatch->next)  /* if last section */
      { /* fill to EOF */
        extra = compute_offset(2,0,goat_samples[shortest_goat_num]) - 
          compute_offset(curranchor,curroffset,goat_samples[shortest_goat_num]) + 1;
        for(i=0;i<extra;i++)
          {
            for(j=0;j<num_files;j++)
            {
              pos = compute_offset(curranchor,curroffset+i,goat_samples[sample_num[j]]);
              ((*goat_bytes)[currbyte]).sample[j] = goat_samples[sample_num[j]]->bytes[pos];
              ((*goat_bytes)[currbyte]).loc.anchor_type = curranchor;
              ((*goat_bytes)[currbyte]).loc.offset = curroffset+i;
            }
            currbyte++;
          }
      }

      gmatch=gmatch->next;
    }
  


  /* count the number of usable variable virus bytes */
  mlist = map_list;
  curroffset = 0;
  get_begin_boundary(&tmp_begin,mlist,filetype);
  curranchor = 0;
/*  curranchor = tmp_begin.bestanchor; */

  while(mlist)
    {
      /* get begin & end boundaries */
      get_begin_boundary(&tmp_begin,mlist,filetype);
      adjust_begin_boundary (&tmp_begin);   // AJR 
      get_end_boundary(&tmp_end,mlist,filetype);
      adjust_end_boundary (&tmp_end);       // AJR 
      tmp_end.offset_min += tmp_end.offset_range;  /* actually use 'offset_max' */

      if(tmp_begin.bestanchor == curranchor)  /* if in same anchor type */
      {
        //AJR if(mlist->next)
        //AJR   get_begin_boundary(&tmp_next_begin,mlist->next,filetype);  /* get beginning of next region */

        /* if this region's end (counting range) doesn't overlap next region */
        //AJR if(!(mlist->next) || (compare_boundaries(tmp_end,tmp_next_begin) < 0)) 
        //AJR  *num_vv_bytes += tmp_begin.offset_min/*AJR+tmp_begin.offset_range*/ - curroffset;//AJR+1;  /* add the bytes */
        if ((gap_length = tmp_begin.offset_min - curroffset) > 0) {   //AJR
           *num_vv_bytes += gap_length;                               //AJR
        }                                                            //AJR
      }

      curranchor = tmp_end.bestanchor;
      curroffset = 1+tmp_end.offset_min; //AJR - tmp_end.offset_range;  'offset_min' is actually 'offset_max' (see above) /* get real offset_min (changed to offset_max above) */

      if(!mlist->next)  /* if last region, fill out to EOF or for max_extra_vv_bytes */
      {
        extra = compute_offset(2,0,samples[shortest_inf_num]) - 
          compute_offset(curranchor,curroffset,samples[shortest_inf_num]) + 1;
        if(max_extra_vv_bytes && (extra > max_extra_vv_bytes))
          extra = max_extra_vv_bytes;
        if (extra > 0) {                                         //AJR
           *num_vv_bytes += extra;
        }                                                        //AJR
      }

      mlist = mlist->next;
    }


  /* allocate space for variable virus bytes */
  *variable_virus_bytes = (struct reconstruction_vector *)calloc(*num_vv_bytes,sizeof(struct reconstruction_vector));
  for(i=0;i<*num_vv_bytes;i++)
    ((*variable_virus_bytes)[i]).sample = (unsigned char *)calloc(num_files,sizeof(unsigned char));


  /* fill up variable virus array with proper bytes */
  mlist = map_list;
  currbyte = curroffset = 0;
  get_begin_boundary(&tmp_begin,mlist,filetype);
  curranchor = 0;
/*  curranchor = tmp_begin.bestanchor; */

  while(mlist)
    {
      /* get begin & end boundaries */
      get_begin_boundary(&tmp_begin,mlist,filetype);
      adjust_begin_boundary (&tmp_begin);   // AJR 
      get_end_boundary(&tmp_end,mlist,filetype);
      adjust_end_boundary (&tmp_end);   // AJR 
      tmp_end.offset_min += tmp_end.offset_range;  /* actually use 'offset_max' */

      if(tmp_begin.bestanchor == curranchor)  /* if in same anchor type */
      {
        //AJRif(mlist->next)
        //AJR  get_begin_boundary(&tmp_next_begin,mlist->next,filetype);  /* get beginning of next region */

        /* if this region's end (counting range) doesn't overlap next region */
        //AJRif(!(mlist->next) || (compare_boundaries(tmp_end,tmp_next_begin) < 0))
        //AJR  {
            gap_length = tmp_begin.offset_min/*AJR+tmp_begin.offset_range*/-curroffset; //AJR+1;
            for(i=0;i<gap_length;i++)
            {
              for(j=0;j<num_files;j++)
                {
                  pos = compute_offset(curranchor,curroffset+i,samples[sample_num[j]]);
                  //((*variable_virus_bytes)[currbyte]).sample[j] = samples[sample_num[j]]->bytes[pos];
		   			// WARNING, the line above sometimes gives negative position
			   		// No time to do it well -> quick and dirty way, CHANGE THIS ASAP

				      if (pos>=0 && pos<samples[sample_num[j]]->num_bytes)
					      ((*variable_virus_bytes)[currbyte]).sample[j] = samples[sample_num[j]]->bytes[pos];
				      else
					      ((*variable_virus_bytes)[currbyte]).sample[j] = 0;

                  ((*variable_virus_bytes)[currbyte]).loc.anchor_type = curranchor;
                  ((*variable_virus_bytes)[currbyte]).loc.offset = curroffset+i;
                }
              currbyte++;
            }
        //AJR}
      }

      curranchor = tmp_end.bestanchor;
      curroffset = 1+tmp_end.offset_min; //AJR - tmp_end.offset_range;  /* get real offset_min (changed to offset_max above) */

      if(!mlist->next)  /* if last region, fill out to EOF or for max_extra_vv_bytes */
      {
        extra = compute_offset(2,0,samples[shortest_inf_num]) - 
          compute_offset(curranchor,curroffset,samples[shortest_inf_num]) + 1;
        if(max_extra_vv_bytes && (extra > max_extra_vv_bytes))
          extra = max_extra_vv_bytes;
        for(i=0;i<extra;i++)
          {
            for(j=0;j<num_files;j++)
            {
              pos = compute_offset(curranchor,curroffset+i,samples[sample_num[j]]);
			  // I added the following test to make sure that pos is a valid offset
			  // this is known to have caused problems for at least one virus (kycc506 (?)), Fred
			  if (pos < 0 || pos >= samples[sample_num[j]]->num_bytes)
				((*variable_virus_bytes)[currbyte]).sample[j] = 0;
			  else
				((*variable_virus_bytes)[currbyte]).sample[j] = samples[sample_num[j]]->bytes[pos];
              ((*variable_virus_bytes)[currbyte]).loc.anchor_type = curranchor;
              ((*variable_virus_bytes)[currbyte]).loc.offset = curroffset+i;
            }
            currbyte++;
          }
      }

      mlist = mlist->next;
    }
  // 6/26/97 Fred: Added the following free
  free (sample_num);

}


// AJR 
// The following two functions are quick & dirty fixes for the fact that the infected
// files sometimes have insufficient variability, so that bytes that are really Ivar
// are instead classified as Iconst or HI. This may incorrectly eliminate certain bytes
// as candidate reconstruction source bytes.

// This is particularly true of the most significant byte of EXE header words,
// which may be constant across all samples (usually 0?). This may prevent the
// immediately-preceding byte being used for repair, as the whole word is not a
// candidate.

// Quick fix 1: move the upper boundary of a non-terminal Ivar section (i.e.
// the beginning boundary of the Iconst or HI section above it) up by 1.
static void adjust_begin_boundary (struct single_boundary* pBegin_boundary) {
   pBegin_boundary->offset_min++;
}



static void adjust_end_boundary (struct single_boundary* pEnd_boundary) {
}
