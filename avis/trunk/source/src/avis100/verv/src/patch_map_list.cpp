//AJR
//   find out how much overlap between emulator code sections and I-const sections,
//   and how much is based on 'starred' sections.

//AJR 
//    Used in fixing the aliasing problem (map_recs may share match_list items
//    needing to be split - see the comments before patch_finnaly_andfree())
#include <vector>
#include <map>
#include <set>
using namespace std;

#include "autoverv.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <vfw.h>
/*#include <values.h>*/
#include <assert.h>

#ifndef _WIN32
#  define min(a,b) (((a)<=(b))?(a):(b))
#  define max(a,b) (((a)>=(b))?(a):(b))
#endif

/*--------------------------------------------------------------------------*/

#ifdef VERV_CODO

extern int ALL_CODE;   // AJR temp?

#define SECTIONNAME ".sections"

FILE *trace;

struct sectionlist 
{
   unsigned long         begin;
   unsigned long         end;
   bool                  starred;   //AJR
   struct   sectionlist *next;
};

//AJR: for the aliasing problem
typedef map<match_list*, set<map_rec*> > MATCH_XREF;

/* prototypes */
struct   sectionlist *build_sections(char *filename, char *dir);

int      patch_temp_boundaries(struct match_list *matchos,
                               struct sectionlist *sections,
                               int    nsample);

struct   temp_boundaries *is_in_section(int     value, 
                                        struct  temp_boundaries *bobo,
                                        int     posinsample);

struct   temp_boundaries *create_newsection(struct temp_boundaries *bobo,
                                            int    off,
                                            int    lth, 
                                            struct temp_boundaries *next);

void     cleanup_sections(struct temp_boundaries *bobo);

int      patch_finnaly_andfree(struct  map_rec *mapos, 
                               int     numsamples);

struct   match_list *create_new_matchlist(struct   match_list *pTemplate,
                                          int      numsamples);

//AJR: for the aliasing problem
static bool patchMatches(const MATCH_XREF& MatchXref, int nSamples);

static match_list* splitMatch (match_list* pMatch, int offset, int code, int nSamples);

static map_rec* splitMap (map_rec* pMap, const match_list* pNewMatch);

/*--------------------------------------------------------------------------*/
// for debugging
static void PrintMatchList (struct map_rec** MapLists); 
/*--------------------------------------------------------------------------*/




/* patches the map_list strucutres so it will include the code and data regions */

int patch_map_list(struct map_rec **map_list, 
                   struct sample_data **samples, 
                   int    num_samples, 
                   char   *infected_dir,
                   polyn  *crc_table)
{
   int    i, j;
   struct sectionlist *sections = NULL;
   struct map_rec *mapos = NULL;
   //AJR for the aliasing problem
   MATCH_XREF MatchXref;

#ifdef DDEBUG
   trace = fopen("err.out" , "w");
#endif

   //AJR count the 'before' viral sections
   long totalVlength[3] = {0,0,0};
   assert (3 >= file_types);
   for(i=0;i<file_types;i++) {
      for(mapos=map_list[i];mapos;mapos=mapos->next) {
         if(mapos->record_type == 'V') {
            struct match_list *matchos = (struct match_list *)(mapos->record_ptr);
            totalVlength[i] += matchos->match->length;
         }
      }
      printf("V-section total length, filetype %d: %ld\n", i, totalVlength[i]); 
   }
   
   /* first clean this map_list (initialize some of it's elements */
   for(i=0;i<file_types;i++) {
PrintMatchList(map_list);
      for(mapos=map_list[i];mapos;mapos=mapos->next) {
         if(mapos->record_type == 'V')
         { 
            struct match_list *matchos = (struct match_list *)(mapos->record_ptr);
            matchos->code = ALL_CODE ? VERV_CODE : -1; 
                          //^^^^^^^^^^^^^^^^^^^^^^ AJR temp?
            matchos->tempboundaries = NULL; 
         }
      }
   }

   if (ALL_CODE) {   // AJR temp?
      #ifdef DDEBUG
         fclose(trace);
      #endif
      return SUCCESS;
   }

   // debug
   //printf ("Number of samples: %d\n", num_samples);
      
   /* now set values for the tempboundaries structure */
   for(i=0;i<num_samples;i++) {
      /* first get the corresponding sectionlist struct of this filename */
      if((sections=build_sections(samples[i]->filename, infected_dir))) {
         for(j=0;j<file_types;j++) {
            /* and for each map_list, set the temp_boundaries struct */
            for(mapos=map_list[j];mapos;mapos=mapos->next) {
               /* only proceed the Virus sections 'V' */
               if(mapos->record_type == 'V') {
                  struct match_list *matchos = (struct match_list *)(mapos->record_ptr);
                  if(patch_temp_boundaries(matchos, sections, i) == -1) {
                     return NOT_ENOUGMEM;
                  }
               }
            }
         }
         /* free this section list */
         while(sections)
         {
            struct sectionlist *temp = sections->next;
            free(sections);
            sections = temp;
         }
         sections = NULL;
      }
   }
       
   // then here really patch this mess and free the temp_boundaries structs 
   for(j=0;j<file_types;j++) {
      for(mapos=map_list[j];mapos;mapos=mapos->next) {
         if(mapos->record_type == 'V') { 
            struct match_list *matchos = (struct match_list *)mapos->record_ptr;
///***             
            if(matchos->tempboundaries == NULL) {
               if(matchos->code == -1) matchos->code = 0;
            }else {
               //AJR if(patch_finnaly_andfree(mapos, num_samples) == -1) return NOT_ENOUGMEM;
               matchos->code = VERV_DATA;   // assume all data to start with
               MatchXref[matchos].insert(mapos);
            }
//****/puts("NO SECTIONS!!!"); matchos->code = VERV_CODE;
         }
      }
   }
   //AJR
   if (!patchMatches(MatchXref, num_samples)) return NOT_ENOUGMEM;

               
   /* no update the crc values for each element in the list */
   printf("Now Updating CRC values\n");
             
   for(j=0;j<file_types;j++) {
      for(mapos=map_list[j];mapos;mapos=mapos->next) {
         if(mapos->record_type == 'V')
         { 
            struct match_list *matchos = (struct match_list *)mapos->record_ptr;
            unsigned char *tempbuffer;
            struct match_data *momo = matchos->match;
                
           /* invariant_byte[0][0] is the same as samples[0]->bytes beacause this 
            * operation below has been already done :
            *        samples[i]->bytes=invariant_bytes[i][0];
            *        samples[i]->map_bytes=invariant_map_bytes[i][0];
            * and crc_calc was used this way:
            *        tempbuffer = decrypt(best_match,&(invariant_bytes[0][0][best_match->pos_in_sample[0]]));
            *        matchos->crc = crc_calc(crc_table, 0L, tempbuffer, best_match->length);
            *        free(tempbuffer);
            */
#ifdef DDEBUG
            fprintf(trace, "old crc for (%04lX) is (%08lX) -> ", matchos->match->length, matchos->crc);
#endif
            /* decrypt (if necessary) with first byte as key */
            tempbuffer = decrypt(momo, &(samples[0]->bytes[momo->pos_in_sample[0]]));
            /* calculate crc of region */
            matchos->crc = crc_calc(crc_table, 0L, tempbuffer, momo->length);
#ifdef DDEBUG
            fprintf(trace, "(%08lX)\n", matchos->crc);
#endif
            /* release the temporary buffer */
            free(tempbuffer);
         }
      }
   }
PrintMatchList(map_list);
   
   //AJR count the 'after' viral sections
   for(i=0;i<file_types;i++) {
      long totalCodeLength = 0;
      long VlengthRemaining = totalVlength[i];
      for(mapos=map_list[i];mapos;mapos=mapos->next) {
         if(mapos->record_type == 'V') {
            struct match_list *matchos = (struct match_list *)(mapos->record_ptr);
            VlengthRemaining -= matchos->match->length;
            if (matchos->code == VERV_CODE) {
               totalCodeLength += matchos->match->length;
            }
         }
      }
      printf("V-section code total length, filetype %d: %ld\n", i, totalCodeLength); 
      if (VlengthRemaining) printf("*** missing V-match bytes: %ld\n", VlengthRemaining);
   }
   

#ifdef DDEBUG
   fclose(trace);
#endif
                      
   return SUCCESS;
}

/*--------------------------------------------------------------------------*/

/* builds the linked list of different sections read from *.sections filename */

struct sectionlist *build_sections(char *filename, char *dir)
{
   static char        bufferos[256];
   FILE               *sectionfile;
   unsigned long      end, begin;
   struct sectionlist *temp, 
      *prev = NULL, 
      *sections = NULL;
   bool starred;   // AJR
   
   sprintf(bufferos, "%s"SECTIONNAME, filename);
   // debug
   //printf ("Reading Sections of %s\n", filename);
   if((sectionfile=fopen(bufferos, "r")) == NULL)
   {
      fprintf(stderr, "build_sections: Can't open %s"SECTIONNAME"\n", filename);
      return NULL;
   }
   
   while(fgets(bufferos, sizeof(bufferos), sectionfile) != NULL)
   {
      /* format of line is "* [ begining end]" */
      if(*bufferos == '*') {
         sscanf(bufferos, "* [%lu %lu]", &begin, &end);
         starred = true;    // AJR
      }else {
         sscanf(bufferos, "  [%lu %lu]", &begin, &end);
         starred = false;   // AJR
      }
      
#ifdef DDEBUG
      /*fprintf(trace, "file:%s\t [%lu %lu]\n", filename, begin, end);*/
#endif
      
      temp = (struct sectionlist *)malloc(sizeof(struct sectionlist));
      if(temp == NULL) return NULL; /* not enough memory, get more RAM */
      if(sections == NULL) sections = temp;
      if(prev != NULL) prev->next = temp;
      prev = temp;
      temp->begin=begin;
      temp->end=end;
      temp->starred = starred;   // AJR
      temp->next = NULL;
   }
   
   return sections;
}

/*--------------------------------------------------------------------------*/

/* this function patches for each match_list an embeded boundary struct */

//AJR   Give meaningful names to variables, get rid of the baby-talk (variable 
//      names like bobo, tempbobo etc.)

// 'pVmatch' points to a match_list element representing a V-type match between 
// infected samples;
// 'pEmulatorCodeList' is a list of code sections within a particular infected sample,
// from the Bochs emulator;
// 'nSample' is the index of that sample.
//
// pVmatch->tempboundaries is a list of (offset, length) pairs, where 'offset' is 
// relative to the beginning of the match, giving the subsections of the match that
// have been identified as code. This function modifies that list to include any part
// of the match that has been identified as code in the current file, either by 
// extending an existing entry that is overlapped by a code section or by creating a
// new entry for code sections that do not overlap any existing entries.

int patch_temp_boundaries(struct match_list *pVmatch, 
                          struct sectionlist *pEmulatorCodeList,
                          int    nSample)
{
   int MatchPosn = pVmatch->match->pos_in_sample[nSample];
   int MatchLength = pVmatch->match->length;
   
   struct sectionlist     *pBochsCode = pEmulatorCodeList;
   // walk the list of code sections from the emulator
   while(pBochsCode)
   {
      // NB: subsection offsets are relative to the match position in the sample
      struct temp_boundaries
         *pVirCodeList = pVmatch->tempboundaries,
         *pVirCode,
         *pNewVirCode;
      int endoffset;
      
      /* try to find if pBochsCode->begin is found */
      if((pVirCode = is_in_section(pBochsCode->begin, pVirCodeList, MatchPosn)) != NULL)
      {
         /* merge them both , offset is still the same of course */
         endoffset = pVirCode->length + pVirCode->offset;
         pVirCode->offset = min(pVirCode->offset, pBochsCode->begin - MatchPosn);
         pVirCode->length = max(endoffset, pBochsCode->end - MatchPosn) - pVirCode->offset;
         if((pVirCode->length + pVirCode->offset) > MatchLength) 
            pVirCode->length = MatchLength - pVirCode->offset;
      }
      else if((pVirCode = is_in_section(pBochsCode->end, pVirCodeList, MatchPosn)) != NULL)
      {
         /* merge them both , knowing that the begin doesn't fit */
         endoffset = pVirCode->length + pVirCode->offset;
         pVirCode->offset = max(0, min(pVirCode->offset, pBochsCode->begin - MatchPosn));
         pVirCode->length = max(endoffset, pBochsCode->end-MatchPosn) - pVirCode->offset;
         if((pVirCode->length + pVirCode->offset) > MatchLength) { /* shouldn't happen */
            assert(0);
            pVirCode->length = MatchLength - pVirCode->offset;
         }
      }
      else
      {
         /* new section if it fits somepart, add it or die */
         int  boff = pBochsCode->begin - MatchPosn;
         int  eoff = pBochsCode->end - MatchPosn; /* + 1; */
         
         if(((boff >= 0) && (boff < MatchLength)) || ((boff < 0) && ((eoff) > 0)))
         {
            boff = max(0, boff);
            eoff = min(MatchLength, eoff);
            if(!(pNewVirCode = create_newsection(pVirCodeList, boff, eoff - boff, NULL)))
               return -1;
            /* is it the first time we create a section in it ? */
            // if not, the new section will have been linked into the list
            if(pVirCodeList == NULL) pVmatch->tempboundaries = pVirCodeList = pNewVirCode;
         }
      }
      
      /* process the next element */
      pBochsCode = pBochsCode->next;
   }
   
   /* may be we better merge collapsing sections...*/
   /*cleanup_sections(pVmatch->tempboundaries);*/
   
   return 1;
}

/*--------------------------------------------------------------------------*/

/* this function looks for 'value' in the linked list of sections */

struct temp_boundaries *is_in_section(int value, 
                                      struct temp_boundaries *bobo,
                                      int posinsample)
{
   struct temp_boundaries *tempo = NULL;
   
   if(bobo == NULL) return NULL;
   
   for(tempo=bobo;tempo;tempo=tempo->next)
   {
      if((tempo->present == 1) &&
         (value >= (tempo->offset+posinsample)) &&
         (value <= (tempo->length+tempo->offset+posinsample)))
         return tempo;
   }
   
   return NULL; /* nothing found */
}

/*--------------------------------------------------------------------------*/

/* this function create a new temp_boundary structure */

struct temp_boundaries *create_newsection(struct temp_boundaries *bobo,
                                          int off,
                                          int lth, 
                                          struct temp_boundaries *next)
{
   struct temp_boundaries *current, 
      *newone = NULL;
   
   for(current=bobo;current;current=current->next) 
      newone = current;
   
   current = newone;
   
   newone = (struct temp_boundaries *)malloc(sizeof(struct temp_boundaries));
   if(newone == NULL) return NULL;
   if(bobo == NULL) current  = newone;
   else current->next        = newone;
   newone->next              = next;
   newone->offset            = off;
   newone->length            = lth;
   newone->present           = 1;
   
   return current;
}

/*--------------------------------------------------------------------------*/

/* may be we better merge collapsing sections...*/
// not needed: the only use of the temp_boundaries list is to see if a given offset
// falls in a code section, so it doesn't matter if that offset is covered by more
// than one member of the list.

void cleanup_sections(struct temp_boundaries *bobo)
{
/* for each element in bobo, check if there is some collapsing value
* somewhere else, like we care.
   */
   
   struct temp_boundaries *tempo, 
      *local1;
   
   for(tempo=bobo;tempo;tempo=tempo->next)
   {
      for(local1=bobo;local1;local1=local1->next)
      {
         if((local1 == tempo) || (local1->present != 1)) continue;
         
         if((tempo->offset >= local1->offset) &&
            (tempo->offset <= (local1->offset+local1->length)))
         {
            /* ok, we found two overlapping windowz. One of them has to die */
            local1->present = 0;
            tempo->offset   = min(tempo->offset, local1->offset);
            tempo->length   = max(tempo->offset+tempo->length, 
               local1->offset+local1->length);
            tempo->length  -= tempo->offset;
         }
         else if(((tempo->offset+tempo->length) >= local1->offset) &&
            ((tempo->offset+tempo->length) <= (local1->offset+local1->length)))
         {
            /* ok, we found two overlapping windowz. One of them has to die */
            local1->present = 0;
            tempo->offset   = min(tempo->offset, local1->offset);
            tempo->length   = max(tempo->offset+tempo->length, 
               local1->offset+local1->length);
            tempo->length  -= tempo->offset;
         }
      } /* for() */
   } /* for() */
   
   return;
}

/*--------------------------------------------------------------------------*/

/* this function uses the results in temp_boundaries strcuture to 
* split the Vmatch item
*/
// It creates a new map_rec / match_list combo for each contiguous
// code and data subsection of the original match, except for the 
// first such subsection, which re-uses the existing map_rec and
// match_list items.

// This doesn't work because a given match_list item may be pointed
// to by more than one map_rec, and if so, the other map_recs will
// be left pointing to the first subsection, losing track of the
// other subsections.

//int patch_finnaly_andfree(struct map_rec *pVmap, int numSamples)
//{
//   int    state = -1; /* state = 0: data */
//   /* state = 1: code */
//   int    MatchOffset = 0;
//   struct match_list *pVmatch = (struct match_list *)pVmap->record_ptr;
//   int    MatchLength = pVmatch->match->length;
//   
//   struct match_list 
//      *pMatchTemplate = NULL, 
//      *pCurrentMatch = NULL, 
//      *pPreviousMatch;
//   struct map_rec 
//      *tempmap, 
//      *pPreviousMap = pVmap;
//   
//   struct temp_boundaries *pCodeList = pVmatch->tempboundaries;
//   pVmatch->tempboundaries = NULL;
//   
//   /* create a template of the pCurrentMatch match_list */
//   pMatchTemplate = create_new_matchlist(pVmatch, numSamples);
//   
//   for(MatchOffset=0;MatchOffset<MatchLength;MatchOffset++)
//   {
//      int Code_or_Data = -1;
//      int i;
//      
//      pPreviousMatch = pCurrentMatch;
//      
//    if(is_in_section(MatchOffset, pCodeList, 0))
//      {
//         if(state != VERV_CODE) /* including state = -1 */
//         {
//            /* we go to state = 1 */
//            state = VERV_CODE;
//            /* create new section of type CODE */
//            Code_or_Data = VERV_CODE;
//         }
//         else { /* just expend the current element */ }
//      }
//      else
//      {
//         if(state != VERV_DATA) /* including state = -1 */
//         {
//            /* we go to state = 0 */
//            state = VERV_DATA;
//            /* create new section of Code_or_Data DATA */
//            Code_or_Data = VERV_DATA;
//         }
//         else { /* just expend the current list */ }
//      }
//      
//      if((Code_or_Data == -1) && (pCurrentMatch != NULL))
//      {
//         /* let's expand the current list */
//         if(pCurrentMatch->code != state) 
//            fprintf(stderr, "Big Huge error in patch_map_list\n");
//         pCurrentMatch->match->length++;
//      }
//      else
//      {
//         /* create a new one, damnit */
//         if(pCurrentMatch != NULL)
//         {
//            pCurrentMatch = create_new_matchlist(pMatchTemplate, numSamples);
//            tempmap = (struct map_rec *)malloc(sizeof(struct map_rec));
//         }
//         else
//         {
//            pCurrentMatch = pVmatch; /* otherwise, we use the existing one */
//            tempmap = pVmap;   /* same story */
//         }
//         /* now, set the values */
//         if(pCurrentMatch == NULL) return -1;
//         if(pPreviousMatch != NULL) pPreviousMatch->next = pCurrentMatch;
//         /* pCurrentMatch->next is already set from the template */
//         pCurrentMatch->match->length = 1; /* new section */
//         for(i=0;i<=file_types;i++) pCurrentMatch->begin.offset_min[i] += MatchOffset;
//         for(i=0;i<numSamples;i++)  pCurrentMatch->match->pos_in_sample[i] += MatchOffset;
//         pCurrentMatch->code = state;
//         pCurrentMatch->tempboundaries = NULL; /* so we won't step in the external loop */
//         /* now, update the linked list in map_rec pVmap */
//         /* if we are using the existing one, now prob */
//         if(tempmap == NULL) return -1;
//         tempmap->record_type = 'V';
//         tempmap->record_ptr = (void *)pCurrentMatch;
//         /* if it's the same, tempmap->next is set already */
//         if(pPreviousMap != tempmap)
//         {
//            tempmap->next = pPreviousMap->next;
//            pPreviousMap->next = tempmap;
//            pPreviousMap = tempmap;
//         }
//      }
//   }
//   
//   /* free those useless boundaries */
//   while(pCodeList)
//   {
//      struct temp_boundaries *temp = pCodeList->next;
//      free(pCodeList);
//      pCodeList = temp;
//   }
//   
//   /* and free this useless template */
//   free(pMatchTemplate->match->pos_in_sample);
//   free(pMatchTemplate->match);
//   free(pMatchTemplate);
//   
//   return 1;
//}

/*--------------------------------------------------------------------------*/

/* small function to do small things in a small world */
//idiot
struct match_list *create_new_matchlist(struct match_list *pTemplate,
                                        int numsamples)
{
   struct match_list *newone;
   struct match_data *macoumba;
   int    *posinfiles;
   
   newone = (struct match_list *)malloc(sizeof(struct match_list));
   if(newone == NULL) return NULL;
   memcpy(newone, pTemplate, sizeof(struct match_list));
   
   macoumba = (struct match_data *)malloc(sizeof(struct match_data));
   if(macoumba == NULL) return NULL;
   newone->match = macoumba;
   memcpy(macoumba, pTemplate->match, sizeof(struct match_data));
   
   posinfiles = (int *)malloc(sizeof(int)*numsamples);
   if(posinfiles == NULL) return NULL;
   memcpy(posinfiles, pTemplate->match->pos_in_sample, sizeof(int)*numsamples);
   macoumba->pos_in_sample = posinfiles;
   
   return newone;
}


/*--------------------------------------------------------------------------*/
// AJR: For the alias problem.

// If a match contains both code and data sections (as determined previously from 
// the .sections files produced by the emulator), it must be split into a sequence of
// interleaved code and data matches. MatchXref contains each match that may need
// splitting, and for each match there's the set of the map_recs that reference it.
// Whenever a match is split, all the map_recs referencing it are also split.

// this may not work if the map_recs are referenced elsewhere...

static bool patchMatches(const MATCH_XREF& MatchXref, int nSamples) {
         
   // iterate over all the matches that may need splitting into code and data sections
   for (MATCH_XREF::const_iterator match = MatchXref.begin(); match != MatchXref.end(); match++) {

      match_list* pMatch = (*match).first;
      vector<map_rec*> vMap;
      copy ((*match).second.begin(),(*match).second.end(),back_inserter(vMap));

      {printf("match @ %08X: maps @",pMatch);for(vector<map_rec*>::const_iterator m = vMap.begin();m != vMap.end(); m++)printf(" %08X",*m);puts("");}

      // detach the emulator code-section list from the match
      temp_boundaries* pCodeList = pMatch->tempboundaries;
      pMatch->tempboundaries = NULL;

      // iterate over the current match
      for (int MatchOffset = 0; MatchOffset < pMatch->match->length; MatchOffset++) {
  
         // is the current offset in a code or data section?
         const int code = is_in_section(MatchOffset,pCodeList,0) ? VERV_CODE : VERV_DATA;
                       
         // if that differs from the current match's type, split off a new match 
         if (code != pMatch->code) {

            // split the current match at the current offset, continue with the new one
            pMatch = splitMatch(pMatch, MatchOffset, code, nSamples);
            if (!pMatch) return false;
            MatchOffset = 0;   // re-align the current offset relative to the new match

            // split each of the map_recs that referenced the split match, continue with the new ones
            for(vector<map_rec*>::iterator map = vMap.begin();map != vMap.end(); map++) {
               *map = splitMap (*map, pMatch);
               if (!*map) return false;
            }
         }
      }
      //free the emulator code list
      while(pCodeList){
         temp_boundaries* pNext = pCodeList->next;
         free(pCodeList);
         pCodeList = pNext;
      }
   }
   return true;
}



static match_list* splitMatch (match_list* pMatch, int offset, int code, int nSamples) {

   if (offset == 0) {        // this can happen when the first byte of a match is code
      pMatch->code = code;   // - no need to split it; simply set it's type correctly.
      return pMatch;
   }

   // allocate new structures & make a DEEP copy of the old...

   match_list* pNewMatch = (match_list*)malloc(sizeof(match_list));
   if (!pNewMatch) return NULL;
   memcpy (pNewMatch, pMatch, sizeof(match_list));

   pNewMatch->match = (match_data*)malloc(sizeof(match_data));
   if (!pNewMatch->match) return NULL;
   memcpy (pNewMatch->match, pMatch->match, sizeof(match_data));

   pNewMatch->match->pos_in_sample = (int*)malloc(sizeof(int)*nSamples);
   if (!pNewMatch->match->pos_in_sample) return NULL;
   memcpy (pNewMatch->match->pos_in_sample, pMatch->match->pos_in_sample, sizeof(int)*nSamples);

   // ...adjusted in code/data type, starting offset & location, and length 
   pNewMatch->code = code;
   for (int b = 0; b < file_types; b++) {
      pNewMatch->begin.offset_min[b] += offset;
   }
   for(int s = 0; s < nSamples; s++) {
      pNewMatch->match->pos_in_sample[s] += offset;
   }
   pNewMatch->match->length -= offset;
   assert(pNewMatch->match->length > 0);

   // set the original match length
   pMatch->match->length = offset;

   //?? Could redo the CRCs (for both matches!) here, but they'll be done for all matches later...
   //   Can we assume they were initially all correct? if so, it's better to do them here.

   // link the new match to the previous one
   pMatch->next = pNewMatch;

   return pNewMatch;
}



static map_rec* splitMap (map_rec* pMap, const match_list* pNewMatch) {
   
   if (pMap->record_ptr == (void*)pNewMatch) {   // if the original match wasn't split
      return pMap;
   }

   map_rec* pNewMap = (map_rec*)malloc(sizeof(map_rec));
   if (!pNewMap) return NULL;

   memcpy(pNewMap, pMap, sizeof(map_rec));
   pNewMap->record_ptr = (void*)pNewMatch;
   pMap->next = pNewMap;

   return pNewMap;
}

 

// AJR for debugging

static void PrintMatchList (struct map_rec** MapLists) {
   const struct map_rec* pMap;
   struct match_list* pMatch;
   int i;

   for(i=0;i<file_types;i++) {
      printf ("Matches for file type %d\n", i);
      for(pMap = MapLists[i]; pMap; pMap = pMap->next) {
         printf ("   map @ %08X (%c) ", pMap, pMap->record_type);
         if(pMap->record_type == 'V') {
            pMatch = (struct match_list*)(pMap->record_ptr);
            printf("match @ %08X: type %d (%c), section @ %08X: length %6d @ %d,%d,%d ", pMatch, 
               pMatch->code, pMatch->code == VERV_DATA ? 'D' : pMatch->code == VERV_CODE ? 'C' : '?',
               pMatch->match, pMatch->match->length,
               pMatch->begin.bestanchor[i],pMatch->begin.offset_min[i],pMatch->begin.offset_range[i]);
         }
         puts("");
      }
   }
}

#endif
