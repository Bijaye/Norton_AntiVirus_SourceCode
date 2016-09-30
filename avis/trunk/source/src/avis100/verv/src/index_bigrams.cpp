#include <stdio.h>
#include <stdlib.h>

#include "autoverv.h"

void index_bigrams(struct sample_data *sample)

{
	register int i;
	register int bigram;
	int * currnum;
	int * bigrpos;

	/* initialize bigram counters and position pointers */
	memset(sample->position,0,65536L*sizeof(int *));
	memset(sample->bigram_counts,0,65536L*sizeof(int));

	for(i=sample->num_bytes;--i;)      /* look at each byte */
	if(sample->map_bytes[i-1] == 0 && sample->map_bytes[i] == 0)  /* if not an invalid bigram */
      {
		bigram = BIGRAM(sample->bytes,i-1);
		//bigram = sample->bytes[i+1] * 256 + sample->bytes[i];
		sample->bigram_counts[bigram]++;    /* add one to the count */
      }

	for(bigram=0;bigram<65536L;bigram++)  /* allocate memory for arrays */
    if(sample->bigram_counts[bigram] && (sample->bigram_counts[bigram] <= max_bigram_counts))  /* if any of this bigram & not too many */
      if((sample->position[bigram]=(int *)malloc(sample->bigram_counts[bigram]*sizeof(int))) == NULL) /* allocate array */
        mem_err();

	if(!(currnum=(int *)calloc(65536L,sizeof(int))))  /* allocate temp counter array */
		mem_err();

	for(i=sample->num_bytes;--i;)      /* look at each byte */
		if(sample->map_bytes[i-1] == 0 && sample->map_bytes[i] == 0)  /* if not an invalid bigram */
		{
			bigram = BIGRAM(sample->bytes,i-1);
			bigrpos = sample->position[bigram];
			//if(sample->bigram_counts[bigram] <= max_bigram_counts)  /* if not too many */
			if(bigrpos) bigrpos[currnum[bigram]++]=i-1;   /* save position */
		}

	free(currnum);  /* clean up */
}
