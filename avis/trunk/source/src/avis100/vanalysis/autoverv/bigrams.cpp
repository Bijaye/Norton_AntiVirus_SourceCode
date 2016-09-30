#include "bigrams.h"
#include "newhandler.h"

//-------------------------------------------------------------------------------------


void index_bigrams(File* pFile, ENCRYPTION e, int maxCount) {
	register int i;
	register int bigram;
	int * currnum;
	int * bigrpos;

	/* initialize bigram counters and position pointers */
	memset(pFile->position,0,65536L*sizeof(int *));
	memset(pFile->bigram_counts,0,65536L*sizeof(int));

	for(i=pFile->length;--i;)      /* look at each byte */
	if(pFile->MatchMap[i-1] == 0 && pFile->MatchMap[i] == 0)  /* if not an invalid bigram */
      {
		bigram = BIGRAM(pFile->Invar[e],i-1);
		//bigram = pFile->Invar[e][i+1] * 256 + pFile->Invar[e][i];
		pFile->bigram_counts[bigram]++;    /* add one to the count */
      }

	for(bigram=0;bigram<65536L;bigram++)  /* allocate memory for arrays */
    if(pFile->bigram_counts[bigram] && (pFile->bigram_counts[bigram] <= maxCount))  /* if any of this bigram & not too many */
      if((pFile->position[bigram]=(int*)malloc(pFile->bigram_counts[bigram]*sizeof(int))) == NULL) /* allocate array */
        mem_err();

	if(!(currnum=(int *)calloc(65536L,sizeof(int))))  /* allocate temp counter array */
		mem_err();

	for(i=pFile->length;--i;)      /* look at each byte */
		if(pFile->MatchMap[i-1] == 0 && pFile->MatchMap[i] == 0)  /* if not an invalid bigram */
		{
			bigram = BIGRAM(pFile->Invar[e],i-1);
			bigrpos = pFile->position[bigram];
			//if(pFile->bigram_counts[bigram] <= maxCount)  /* if not too many */
			if(bigrpos) bigrpos[currnum[bigram]++]=i-1;   /* save position */
		}

	free(currnum);  /* clean up */
}
