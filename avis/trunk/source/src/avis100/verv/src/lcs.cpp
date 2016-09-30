#include <stdio.h>
#include <stdlib.h>
/*#include <values.h>*/

#include "autoverv.h"


struct match_data *lcs(struct sample_data *samples[],
                       int num_samples, unsigned suggested_start[], int lowerbound, char * approxworked)
{
  int i, j, bytenum,
      min_sample_len,
      master_sample,
      bigram,
      bigramok,
      mlen,smallestmax,
      largest_match=0,matching;
  int *last_pos, *pos;
  struct sample_data *sample;
  unsigned char ch;
  struct match_data *match = NULL;

#if 1

  //Fred: We should try to use the entry point (or smthg similar) as an indication for our lcs search

	//int * startofs;
	//int * endofs;
	int thisstart;
	int thisend;
	int minstart=10000;
	int minend=10000;
	int suggstart;
	unsigned char * match_array;
	int matchstart;
	int matchlen;
	int imax;
	int maxmatchstart=0;
	int maxmatchlen=-1;

	if (suggested_start == NULL) goto end_new_lcs;
	*approxworked = 0;
	//startofs = (int *) calloc (num_samples, sizeof(int));
	//endofs = (int *) calloc (num_samples, sizeof(int));
  for (i=0;i<num_samples;i++) {
	// Just a check, very ugly...
	  if (suggested_start [i] >= samples [i]->num_bytes) suggested_start [i] = 0;
	//
	sample = samples [i];
	suggstart = (int) suggested_start [i];
	thisstart = suggstart-10000;	//DEBUG, this value should not be hard-coded
	thisend = suggstart+10000;
	if (thisend > sample->num_bytes-1) {
		thisend = sample->num_bytes-1;
	}
	if (thisstart < 0) {
		thisstart = 0;
	}
	/*
	for (j=suggstart;j<=thisend;j++)
		if (sample->map_bytes[j]) {thisend=j-1; break;}
	for (j=suggstart;j>=thisstart;j--)
		if (sample->map_bytes[j]) {thisstart=j+1; break;}
	*/
	//startofs[i] = thisstart;
	//endofs[i] = thisend;
	if (suggstart-thisstart<minstart) minstart = suggstart-thisstart;
	if (thisend-suggstart<minend) minend = thisend-suggstart;
  }
  //printf ("MinStart: %d\tMinEnd: %d\n", minstart, minend);
  //free(startEPofs);
  //free(endEPofs);

	match_array = (unsigned char *) malloc ((minend+minstart+1)*sizeof(unsigned char));
	if (match_array == NULL) mem_err();
	//memset (match_array,0,minend+minstart+1);

	for (j=-minstart;j<minend;j++)
		match_array [minstart+j] = samples [0]->map_bytes[suggested_start[0]+j];
	for (i=1;i<num_samples;i++)
		for (j=-minstart;j<=minend;j++) {
			if (match_array [minstart+j] |= samples [i]->map_bytes[suggested_start[i]+j]) continue;
			match_array [minstart+j] |= samples[0]->bytes[suggested_start[0]+j]-samples[i]->bytes[suggested_start[i]+j];
		}

	i=0;
	imax = minstart+minend+1;
	while (i<imax) {
		while (match_array[i] != 0 && i<imax) i++;
		matchstart = i;
		while (match_array[i] == 0 && i<imax) i++;
		matchlen = i-matchstart;
		//printf ("Matchstart: %d\tMatchlen: %d\n", matchstart, matchlen);
		if (matchlen>maxmatchlen) {
			maxmatchlen = matchlen;
			maxmatchstart = matchstart;
		}
	}
	free(match_array);

	// Perhaps this should be min_skip_length ??? Perhaps this should vary from one call to another
	// Perhaps this should be a string common to all virus parts... (try that)
	if (maxmatchlen>lowerbound) { //Fred: Once again, this value should not be hard-coded, change that when it works
		match=(struct match_data *)malloc(sizeof(struct match_data));
		if(match == NULL) mem_err();
		match->pos_in_sample = (int *)calloc(num_samples,sizeof(int));
		if(match->pos_in_sample == NULL) mem_err();
		match->length = maxmatchlen;
		for (i=0;i<num_samples;i++)
			match->pos_in_sample[i] = suggested_start[i]-minstart+maxmatchstart;
		//printf ("New");
		*approxworked = 1;
		return (match);
	}

end_new_lcs:

#endif

  /* this array holds position we've last checked contiguous to max_match_pos, so we can */
  /* tell if we can ignore a region or not */
  last_pos=(int *)calloc(num_samples,sizeof(int));
  if(!last_pos)
    mem_err();
  for(i=0;i<num_samples;i++)
    last_pos[i]=-2;

  min_sample_len = MAXINT;             /* assume shortest is MAXINT */
  for(i=0;i<num_samples;i++)           /* go through each sample */
    {
      sample = samples[i];
      index_bigrams(sample);       /* index/count the bigrams */
      if (sample->num_bytes < min_sample_len)  /* if the current sample is shorter than min */
        {
          min_sample_len = sample->num_bytes;   /* make it the new min */
          master_sample = i;
        }
    }

//  for(bytenum=0;bytenum<min_sample_len;bytenum++)  /* go through each byte of the master sample */
  for(bytenum=0;bytenum<min_sample_len-1;bytenum++)  /* go through each byte of the master sample */
    {
      //bigram = (int)(samples[master_sample]->bytes[bytenum] << 8) + samples[master_sample]->bytes[bytenum+1];  /* compute bigram */
	  bigram = BIGRAM(samples[master_sample]->bytes,bytenum);
      bigramok=-1;
      for(i=0;i<num_samples;i++)    /* go through each sample and see if there is at least */
        {                           /* one of this bigram in it & not too many */
          if(!(samples[i]->bigram_counts[bigram]) ||
             (samples[i]->bigram_counts[bigram] > max_bigram_counts) )
            {
              bigramok = 0;    /* note that it isn't ok */
              break;           /* and bail */
            }
        }
      if (bigramok)   /* if it was found in all */
        {
          smallestmax = MAXINT;                    /* assume smallest max is really big */
          for(i=0;i<num_samples;i++)
            {
              if (i == master_sample)              /* ignore the master sample */
                {
                  samples[i]->max_match_pos = bytenum; /* but, first save the position for later reference */
                  continue;
                }
              sample = samples[i];
              sample->max_match_len = 0;       /* initialize maximum match length */
              sample->max_match_pos = 0;       /* & pos */
              pos = sample->position[bigram];
              for(j=0;j<sample->bigram_counts[bigram];j++) {  /* go through each occurence of the bigram in this slave sample */
                    /* and determine the maximum match length */
                if(pos[j] == last_pos[i]+1)  /* did we just find a match in this region? */
                  last_pos[i]++;    /* increment that position */
                else
                  {
                    mlen = find_match_len(samples[master_sample],bytenum,sample,pos[j],sample->max_match_len);
                    if(mlen > sample->max_match_len)
                      {
                        sample->max_match_len=mlen;
                        sample->max_match_pos=pos[j];
                      }
                  }
              }

              if(sample->max_match_len < smallestmax)  /* smaller than current smallest max? */
                smallestmax = sample->max_match_len;   /* then make new smallest max */
            }

#if 1
          /* now look backwards to see if we match back in common bigrams */
          matching=-1;
          j=0;
          while(matching) /* while we're still ok */
            {
              ch=samples[0]->bytes[samples[0]->max_match_pos-j];  /* remember the char */
              for(i=0;i<num_samples;i++)
                {
                  sample = samples[i];
                  if(sample->max_match_pos<=0)  /* beginning of sample? */
                    {
                      matching=0;    /* we're done */
                      break;
                    }
                  if((sample->bytes[sample->max_match_pos-j] != ch)   /* no match? */
                     || sample->map_bytes[sample->max_match_pos-j]  /* or invalid ? */
                     || samples[0]->map_bytes[samples[0]->max_match_pos-j] )
                    {
                      matching=0;    /* we're done */
                      j--;           /* didn't match at last pos */
                      break;
                    }
                }
              if(matching)
                j++;         /* next char back */
            }

          /* now fix up values if necessary */
          if(j)
            {
              for(i=0;i<num_samples;i++)
                {
                  samples[i]->max_match_pos -= j;
                  samples[i]->max_match_len += j;
                }
              smallestmax += j;
            }
#endif

          if(smallestmax > largest_match)     /* is this the biggest match yet? */
            {
              largest_match = smallestmax;
              if(!match)   /* if no match allocated yet */
                {
                  match=(struct match_data *)malloc(sizeof(struct match_data));  /* allocate mem */
                  if(!match) mem_err();
                  match->pos_in_sample = (int *)calloc(num_samples,sizeof(int)); /* & for array */
                  if(!match->pos_in_sample) mem_err();
                }
              /* save match data */
              match->length = largest_match;        /* assign length */
              if(match->length > min_skip_length)   /* big enough? */
                bytenum += match->length;    /* then skip matching attempts for rest of bytes */
                                             /* in th region */
              for(i=0;i<num_samples;i++)            /* go through each sample and save the match pos */
                last_pos[i] = match->pos_in_sample[i] = samples[i]->max_match_pos;
             }
        } /* end "if not too many of this bigram" */
    } /* end for loop which goes through each byte of master */

    /* clean up our mess */
    free(last_pos);

    for(i=0;i<num_samples;i++)
      for(j=0;j<65536;j++)
        if(samples[i]->bigram_counts[j])
          free(samples[i]->position[j]);
    return(match);
}
