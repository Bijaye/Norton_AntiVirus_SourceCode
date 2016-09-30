#include <stdio.h>
#include <stdlib.h>

#include "autoverv.h"
  
static void remove_partial_coverage (struct phase2_match* pMatchSet,
                                     const int num_goat_bytes,
                                     const struct reconstruction_vector goat_bytes[],
                                     const int fileType,
                                     const EXErelocationTableBoundaries UniGoatRTB);



struct phase2_match *greedy_cover(int num_goat_bytes,
                          struct reconstruction_vector *goat_bytes,
                          struct phase1_match **possible_matches,
                          const int fileType,
                          EXErelocationTableBoundaries UniGoatRTB)
{

  int
    i,
    j,
    *covered_bytes;

  char
    g_anchor_type,
    vv_anchor_type;

  int
    max_coverage,
    g_offset,
    vv_offset,
    relative_offset;
  
  struct phase1_match
    *pm;

  struct phase2_match
    *best_match_summary,
    *best_match_summary_prev,
    *match_summary,
    *match_summary_prev,
    *match_summary_final_list,
    *match_summary_list;

  match_summary_list = (struct phase2_match *) NULL;

  // 6/26/97 Fred: Why allocate this here? It is useless for the moment, and causes a memory leak on its
  // reallocation a few lines later
  //covered_bytes = (int *) malloc(sizeof(int) * num_goat_bytes);
  //for(i=0;i<num_goat_bytes;i++)
  //  covered_bytes[i] = 0;

  for(i=0;i<num_goat_bytes;i++)
    {
      g_anchor_type = (goat_bytes[i].loc).anchor_type;
      g_offset = (goat_bytes[i].loc).offset;

      pm = possible_matches[i];
      while(pm)
      {
        vv_anchor_type = pm->loc.anchor_type;
        vv_offset = pm->loc.offset;
        relative_offset = vv_offset - g_offset;

        match_summary = match_summary_list;
        while(match_summary)
          {
            if((g_anchor_type == match_summary->g_anchor_type) &&
             (vv_anchor_type == match_summary->vv_anchor_type) &&
             (relative_offset == match_summary->relative_offset) &&
             (pm->encryption_info.crypt_type == match_summary->encryption_info.crypt_type) &&
                 (!pm->encryption_info.crypt_type ||
                   ((pm->encryption_info.key_location.anchor_type == match_summary->encryption_info.key_location.anchor_type) &&
               (pm->encryption_info.key_location.offset == match_summary->encryption_info.key_location.offset) &&
               (pm->encryption_info.key_correction[0] == match_summary->encryption_info.key_correction[0]))
                 ))
            {
              match_summary->coverage++;
              match_summary->covered_goat_bytes[i] = 1;
              break;
            }
            match_summary = match_summary->next;
          }
        if(!match_summary)
          {
            match_summary = (struct phase2_match *) malloc(sizeof(struct phase2_match));
            match_summary->g_anchor_type = g_anchor_type;
            match_summary->vv_anchor_type = vv_anchor_type;
            match_summary->relative_offset = relative_offset;
            match_summary->encryption_info = pm->encryption_info;
            match_summary->coverage = 1;
            match_summary->covered_goat_bytes = (int *) malloc(sizeof(int) * num_goat_bytes);
            for(j=0;j<num_goat_bytes;j++)
               match_summary->covered_goat_bytes[j]=0;
            match_summary->covered_goat_bytes[i] = 1;
            match_summary->next = match_summary_list;

            match_summary_list = match_summary;
          }
        pm = pm->next;
      }       
    }

  //AJR: if a match summary covers part of a program segment, it must cover all of it
  //     - remove partial covers
#if defined(USE_STRUCTURE)
  for (match_summary = match_summary_list; match_summary; match_summary = match_summary->next) {
     remove_partial_coverage (match_summary, num_goat_bytes, goat_bytes, fileType, UniGoatRTB);
  }
#endif

  match_summary_final_list = NULL;

  covered_bytes = (int *) malloc(sizeof(int) * num_goat_bytes);
  for(j=0;j<num_goat_bytes;j++)
    covered_bytes[j]=0;
  max_coverage = -1;

  while(max_coverage != 0)
    {
      max_coverage = 0;

      best_match_summary = NULL;
      best_match_summary_prev = NULL;
      match_summary_prev = NULL;

      match_summary = match_summary_list;
      while(match_summary)
      {
        for(j=0;j<num_goat_bytes;j++)
          {
            if(covered_bytes[j])
            {
              if(match_summary->covered_goat_bytes[j] == 1)
                {
                  match_summary->covered_goat_bytes[j]=0;
                  match_summary->coverage--;
                }
            }
          }

        if(match_summary->coverage > max_coverage)
          {
            max_coverage = match_summary->coverage;
            best_match_summary = match_summary;
            best_match_summary_prev = match_summary_prev;
		} 
        match_summary_prev = match_summary;
        match_summary = match_summary->next;
      }

      if(best_match_summary)
      {

/*      printf("Best coverage is: %d.\n",max_coverage);*/

        /* Remove best match from match summary list */
        if(best_match_summary_prev)
          best_match_summary_prev->next = best_match_summary->next;
        else
          match_summary_list = best_match_summary->next;

        best_match_summary->next = match_summary_final_list;
        match_summary_final_list = best_match_summary;
        for(j=0;j<num_goat_bytes;j++)
          covered_bytes[j] = best_match_summary->covered_goat_bytes[j];
      }
    } 

  // 6/27/97 Fred: Free every match that we didn't keep in the match_summary_final_list
  while (match_summary_list) {
	match_summary = match_summary_list->next;
	free(match_summary_list->covered_goat_bytes);
	free(match_summary_list);
	match_summary_list = match_summary;
  }

  free(covered_bytes);

  return(match_summary_final_list);
}



//------------------------------------------------------------------------------------------
//AJR
static void remove_partial_coverage (struct phase2_match* pMatchSet,  //set of infection->goat mappings
                                     const int num_goat_bytes,   //# of goat bytes needing repair
                                     const struct reconstruction_vector goat_bytes[], //those bytes 
                                     const int fileType,  //type of file being repaired: COM, EXE or BOTH
                                     const EXErelocationTableBoundaries UniGoatRTB)
{
   // goat byte iterator
   int i;
   //status of goat byte examined in previous iteration
   int prevGoatByteOffset = -1;
   int prevGoatByteIsCovered = 0;

   if (pMatchSet->g_anchor_type != 0 || fileType == COM) {   // only concerned with offset from beginning
      return;
   }

   //push back leading boundaries to section start
   for (i = 0; i < num_goat_bytes; i++) {
      int offset = goat_bytes[i].loc.offset;
      int nextSectionStart = next_ProgramSection_Beginning(offset,EXE,UniGoatRTB);   //treat BOTH as EXE
      int thisGoatByteIsCovered = pMatchSet->covered_goat_bytes[i];
      int byteBeforeThisIsCovered = prevGoatByteIsCovered && prevGoatByteOffset == offset-1;

      if (thisGoatByteIsCovered && !byteBeforeThisIsCovered) {   // at boundary...
         if (offset != nextSectionStart) {                       // ...in middle of section
            pMatchSet->covered_goat_bytes[i] = 0;
            pMatchSet->coverage--;
            thisGoatByteIsCovered = 0;
         }
      }
      prevGoatByteIsCovered = thisGoatByteIsCovered;
      prevGoatByteOffset = offset;
   }
   //pull back trailing boundaries to section end
   prevGoatByteOffset = -1;
   prevGoatByteIsCovered = 0;
   while (i--) {
      int offset = goat_bytes[i].loc.offset;
      int priorSectionEnd = prior_ProgramSection_End(offset,EXE,UniGoatRTB);   //treat BOTH as EXE
      int thisGoatByteIsCovered = pMatchSet->covered_goat_bytes[i];
      int byteAfterThisIsCovered = prevGoatByteIsCovered && prevGoatByteOffset == offset+1;

      if (thisGoatByteIsCovered && !byteAfterThisIsCovered) {   // at boundary...
         if (offset != priorSectionEnd) {                       // ...in middle of section
            pMatchSet->covered_goat_bytes[i] = 0;
            pMatchSet->coverage--;
            thisGoatByteIsCovered = 0;
         }
      }
      prevGoatByteIsCovered = thisGoatByteIsCovered;
      prevGoatByteOffset = offset;
   }
}
