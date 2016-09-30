/*##################################################################*/
/*                                                                  */
/* Program name:  lcs.cpp                                           */
/*                                                                  */
/* Package name:  autoverv.exe                                      */
/*                                                                  */
/* Description:                                                     */
/*                                                                  */
/*                                                                  */
/* Statement:     Licensed Materials - Property of IBM              */
/*                (c) Copyright IBM Corp. 1999                      */
/*                                                                  */
/* Author:        Andy Raybould                                     */
/*                                                                  */
/*                U.S. Government Users Restricted Rights - use,    */
/*                duplication or disclosure restricted by GSA ADP   */
/*                Schedule Contract with IBM Corp.                  */
/*                                                                  */
/*                                                                  */
/*##################################################################*/

// This file contains the LCS function, and those it calls, as implemented in 
// the original Autoverv. It has been modified in the following ways:
//
//    The sub-functions have been brought in as static functions.
//
//    'new' is used to allocate the returned Match object.
//
//    A couple of additional tests have been added to avoid complaints
//    by bounds checker that it reads uninitialized data.

#include <stdio.h>
#include <stdlib.h>
#include "assert.h"

#include "lcs.h"
#include "bigrams.h"
#include "newhandler.h"

extern LCS_PF pLCS       = NULL;
extern char*  LCS_IOmode = NULL;
extern FILE*  hLCS_IO    = NULL;


/* computes the length of a match at a given pt in each of the files */
/*  ... used by lcs */
static int find_match_len(File* master, int master_pos,
                          File* slave,  int slave_pos,
                          ENCRYPTION e,
		                    int current_best_match_length);

//-------------------------------------------------------------------------------------

Match* fileLCS(File_vec& vFile, ENCRYPTION e,
                      int suggested_start[], int lowerbound,char * approxworked)
{
   if (!hLCS_IO) return lcs(vFile, e, suggested_start, lowerbound, approxworked);

   if (LCS_IOmode[0] == 'w') {
      Match* pMatch = lcs(vFile, e, suggested_start, lowerbound, approxworked);
      if (pMatch == NULL) {
         fprintf(hLCS_IO,"%d \t%u \t%d \t%d\n", 0, 0, 0, 0);
      }else {
         assert(pMatch->segments.size() == vFile.size());
         fprintf(hLCS_IO,"%d \t%u \t%d \t%d\n", pMatch->length, pMatch->segments.size(), pMatch->encr, pMatch->segments.size()); //?? seg size appears twice for now, as num has gone from Match
         for (int s = 0; s < pMatch->segments.size(); s++) {
            const Segment* pSegment = pMatch->segments[s];
            assert(pSegment->pFile == vFile[s]);
            assert(pSegment->encr == pMatch->encr);
            fprintf (hLCS_IO, "\t%ld \t%ld \t%d\n", pSegment->first, pSegment->last, pSegment->encr);
         }
      }
      fflush(hLCS_IO);
      return pMatch;
   }else {
      assert(LCS_IOmode[0] == 'r');
      Match* pMatch = new Match(vFile.size());
      int nSegments;
      fscanf(hLCS_IO,"%d \t%u \t%d \t%d\n", &pMatch->length, &nSegments, &pMatch->encr, &nSegments);
      if (pMatch->length == 0 && nSegments == 0) {
         delete pMatch;
         return NULL;
      }
      assert(nSegments == vFile.size());
      for (int s = 0; s < nSegments; s++) {
         Segment* pSegment = new Segment;
         pSegment->pFile = vFile[s];
         fscanf (hLCS_IO, "\t%ld \t%ld \t%d\n", &pSegment->first, &pSegment->last, &pSegment->encr);
         assert(pSegment->first <= pSegment->last);
         assert(pSegment->encr == pMatch->encr);
         pMatch->segments.push_back(pSegment);
      }
      return pMatch;
   }
}



//-------------------------------------------------------------------------------------

struct Match* lcs(File_vec& Files, ENCRYPTION e, 
                   int suggested_start[], int lowerbound, char * approxworked)
{
  const int nFiles = Files.size();
  int i, j, bytenum,
       min_file_len,
       master_file,
       bigram,
       bigramok,
       mlen,smallestmax,
       largest_match=0,matching;
  int *last_pos, *pos;
  File* pFile;
  unsigned char ch;
  struct Match *match = NULL;

#if 0

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
	//startofs = (int *) calloc (nFiles, sizeof(int));
	//endofs = (int *) calloc (nFiles, sizeof(int));
  for (i=0;i<nFiles;i++) {
	// Just a check, very ugly...
	  if (suggested_start [i] >= Files [i]->length) suggested_start [i] = 0;
	//
	pFile = Files [i];
	suggstart = (int) suggested_start [i];
	thisstart = suggstart-10000;	//DEBUG, this value should not be hard-coded
	thisend = suggstart+10000;
	if (thisend > pFile->length-1) {
		thisend = pFile->length-1;
	}
	if (thisstart < 0) {
		thisstart = 0;
	}
	/*
	for (j=suggstart;j<=thisend;j++)
		if (pFile->MatchMap[j]) {thisend=j-1; break;}
	for (j=suggstart;j>=thisstart;j--)
		if (pFile->MatchMap[j]) {thisstart=j+1; break;}
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
		match_array [minstart+j] = Files [0]->MatchMap[suggested_start[0]+j];
	for (i=1;i<nFiles;i++)
		for (j=-minstart;j<=minend;j++) {
			if (match_array [minstart+j] |= Files [i]->MatchMap[suggested_start[i]+j]) continue;
			match_array [minstart+j] |= Files[0]->Invar[e][suggested_start[0]+j]-Files[i]->Invar[e][suggested_start[i]+j];
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

	// Perhaps this should be min_skip_length ?? Perhaps this should vary from one call to another
	// Perhaps this should be a string common to all virus parts... (try that)
	if (maxmatchlen>lowerbound) { //Fred: Once again, this value should not be hard-coded, change that when it works
      const uInt offset = suggested_start[i]-minstart+maxmatchstart;
		match = new Match(nFiles);
		if(match == NULL) mem_err();
      match->encr = e;
		match->length = maxmatchlen;
      for (i=0;i<nFiles;i++)
         match->segments.push_back(new Segment(Files[i],offset,match->length,e));
		*approxworked = 1;
		return (match);
	}

end_new_lcs:

#endif

  /* this array holds position we've last checked contiguous to max_match_pos, so we can */
  /* tell if we can ignore a region or not */
  last_pos=(int *)calloc(nFiles,sizeof(int));
  if(!last_pos)
    mem_err();
  for(i=0;i<nFiles;i++)
    last_pos[i]=-2;

  min_file_len = MAXINT;             /* assume shortest is MAXINT */
  for(i=0;i<nFiles;i++)                /* go through each file */
    {
      pFile = Files[i];
      index_bigrams(pFile,e);              /* index/count the bigrams */
      if (pFile->length < min_file_len)  /* if the current file is shorter than min */
        {
          min_file_len = pFile->length;   /* make it the new min */
          master_file = i;
        }
    }

//  for(bytenum=0;bytenum<min_file_len;bytenum++)  /* go through each byte of the master file */
  for(bytenum=0;bytenum<min_file_len-1;bytenum++)  /* go through each byte of the master file */
    {
      //bigram = (int)(Files[master_file]->Invar[e][bytenum] << 8) + Files[master_file]->Invar[e][bytenum+1];  /* compute bigram */
	  bigram = BIGRAM(Files[master_file]->Invar[e],bytenum);
      bigramok=-1;
      for(i=0;i<nFiles;i++)    /* go through each file and see if there is at least */
        {                           /* one of this bigram in it & not too many */
            if (Files[i]->bigram_counts[bigram] == 0
            ||  Files[i]->bigram_counts[bigram] >  MAX_BIGRAM_COUNTS) 
            {
               bigramok = 0;    /* note that it isn't ok */
               break;           /* and bail */
            }
        }
      if (bigramok)   /* if it was found in all */
        {
          smallestmax = MAXINT;                    /* assume smallest max is really big */
          for(i=0;i<nFiles;i++)
            {
              if (i == master_file)              /* ignore the master file */
                {
                  Files[i]->max_match_pos = bytenum; /* but, first save the position for later reference */
                  continue;
                }
              pFile = Files[i];
              pFile->max_match_len = 0;       /* initialize maximum match length */
              pFile->max_match_pos = 0;       /* & pos */
              pos = pFile->position[bigram];
              for(j=0;j<pFile->bigram_counts[bigram];j++) {  /* go through each occurence of the bigram in this slave file */
                    /* and determine the maximum match length */
                if(pos[j] == last_pos[i]+1)  /* did we just find a match in this region? */
                  last_pos[i]++;    /* increment that position */
                else
                  {
                    mlen = find_match_len(Files[master_file],bytenum,pFile,pos[j],e,pFile->max_match_len);
                    if(mlen > pFile->max_match_len)
                      {
                        pFile->max_match_len=mlen;
                        pFile->max_match_pos=pos[j];
                      }
                  }
              }

              if(pFile->max_match_len < smallestmax)  /* smaller than current smallest max? */
                smallestmax = pFile->max_match_len;   /* then make new smallest max */
            }

#if 1
          /* now look backwards to see if we match back in common bigrams */
          matching=-1;
          j=0;
          while(matching) /* while we're still ok */
            {
              if (Files[0]->max_match_pos-j >= 0)   // AJR stops boundsChecker complaining
                 ch=Files[0]->Invar[e][Files[0]->max_match_pos-j];  /* remember the char */
              for(i=0;i<nFiles;i++)
                {
                  pFile = Files[i];
                  if(pFile->max_match_pos<=0)  /* beginning of file? */
                    {
                      matching=0;    /* we're done */
                      break;
                    }
                  if(pFile->max_match_pos-j < 0 ||   // AJR stops boundsChecker complaining
                     (pFile->Invar[e][pFile->max_match_pos-j] != ch)   /* no match? */
                     || pFile->MatchMap[pFile->max_match_pos-j]  /* or invalid ? */
                     || Files[0]->MatchMap[Files[0]->max_match_pos-j] )
                    {
                      matching=0;    /* we're done */
                      j--;           /* didn't match at last pos */
                      break;
                    }
                }
              if(matching)
                j++;         /* next char back */
            }

          // At this point, smallestmax and j are the longest common match lengths forwards and
          // backwards, respectively, from each file's max_match_pos.

/*************************************************************************************************
          // AJR: codo - if any file has a code byte at its max_match_pos, then it's a code match,
          // so, for each offset from there to smallestmax, at least one file must have a code
          // byte; smallestmax will be reduced, if necessary, to the largest value for which
          // this is so.
          // Vice-versa for data matches - none may be code.
          // And equivalently backwards to an offset of j.

          bool lookingForCode = false;
          for (int f = 0; f < nFiles; f++) {
             if (Files[f]->isCode(Files[f]->max_match_pos)) lookingForCode = true;
          }

          for (int o = 0; o < smallestmax; o++) { 
             bool codeSeenHere = false;
             for (f = 0; f < nFiles; f++) {
                if (Files[f]->isCode(Files[f]->max_match_pos + o)) codeSeenHere = true;
             }
             if (codeSeenHere != lookingForCode) break;
          }
          for (f = 0; f < nFiles; f++) 
             Files[f]->max_match_len = min(Files[f]->max_match_len,o);
          smallestmax = o;

          for (o = 0; o < j; o++) { 
             bool codeSeenHere = false;
             for (f = 0; f < nFiles; f++) {
                if (Files[f]->isCode(Files[f]->max_match_pos - o)) codeSeenHere = true;
             }
             if (codeSeenHere != lookingForCode) break;
          }
          j = o;

          // AJR end
*************************************************************************************************/

          /* now fix up values if necessary */
          if(j)
            {
              for(i=0;i<nFiles;i++)
                {
                  Files[i]->max_match_pos -= j;
                  Files[i]->max_match_len += j;
                }
              smallestmax += j;
            }
#endif

          if(smallestmax > largest_match)     /* is this the biggest match yet? */
            {
              largest_match = smallestmax;
              if(match) delete match;
              match = new Match(nFiles);    
              /* save match data */
              match->encr = e;
              match->length = largest_match;        /* assign length */
              if(match->length > min_skip_length) { /* big enough? */
                 //??printf ("?? skipped %ld from %ld\n",match->length,bytenum);
                 bytenum += match->length;   /* then skip matching attempts for rest of Invar[e] */
              }                              /* in th region */
              for(i=0;i<nFiles;i++) {        /* go through each file and save the match pos */
                last_pos[i] = Files[i]->max_match_pos;
                match->segments.push_back(
                   new Segment(Files[i],Files[i]->max_match_pos,match->length,e));
              }
             }
        } /* end "if not too many of this bigram" */
    } /* end for loop which goes through each byte of master */

    /* clean up our mess */
    free(last_pos);

    for(i=0;i<nFiles;i++)
      for(j=0;j<65536;j++)
        if(Files[i]->bigram_counts[j])
          free(Files[i]->position[j]);
    return(match);
}


//-------------------------------------------------------------------------------------


static int find_match_len(File* master, int master_pos,
                          File* slave,  int slave_pos,
                          ENCRYPTION e,
                          int current_best_match_length)
  {
     int master_max,
     slave_max,
     m_curr_pos,
     s_curr_pos;

     master_max = master->length;
     slave_max = slave->length;

#if 1 /* Quick test to see if we can't possibly do better than previous best match. If so, kick out now. */

     m_curr_pos=master_pos+current_best_match_length;
     s_curr_pos=slave_pos+current_best_match_length;

     if((m_curr_pos >= master_max) || (s_curr_pos >= slave_max) ||   /* Past end */
      master->MatchMap[m_curr_pos] ||  slave->MatchMap[s_curr_pos] ||  /* Invalid Invar[e]? */
      (master->Invar[e][m_curr_pos] != slave->Invar[e][s_curr_pos]))  /* a mismatch */
       return 0;
#endif

     m_curr_pos=master_pos;
     s_curr_pos=slave_pos;


     while((m_curr_pos < master_max) && (s_curr_pos < slave_max) &&   /* Not past end */
         !master->MatchMap[m_curr_pos] && !slave->MatchMap[s_curr_pos] &&  /* while still valid */
           (master->Invar[e][m_curr_pos] == slave->Invar[e][s_curr_pos]))  /* and not a mismatch */
       {
         m_curr_pos++;    /* go to next byte in each file */
         s_curr_pos++;
       }

    return(m_curr_pos-master_pos);    /* compute length and return */
  }
