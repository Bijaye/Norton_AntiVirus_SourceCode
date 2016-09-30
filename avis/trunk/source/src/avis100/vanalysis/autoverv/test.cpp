/*##################################################################*/
/*                                                                  */
/* Program name:  test.cpp                                          */
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "test.h"

#define L2   8    // 1/2 length of max. match to print //??25 for charprinting  
#define LPAD 3    // length of padding to print before and after match

//#define prtLoc(Loc) Loc.Offset,AnchorName[Loc.Anchor],Loc.Range
//#define FLOC "%6d%s.%d"

#define prtLoc(Loc) Loc.asString().c_str()
#define FLOC "%5s"   // %4d%s.%d



void BREAK (void) {
   return;
}



void prtHex (const byte* bfr, size_t l) {
   while (l--) {
      printf ("%02X",(int)*(bfr++));
      if (l) printf(" ");
   }
}



struct MATCH_DATA {
   static const char* byteFmt; 
   static const char* charFmt;

   const byte* txt; 
   const char* fmt;
   int getAsChar (int i) const {
      return !txt[i] ? ' ' :
             txt[i] == CODE_IN_THIS_FILE ? 'C' :
             txt[i] == CODE_IN_OTHER_FILE ? 'O' :
             txt[i];
   }
   int value(int i) const {return fmt == charFmt ? getAsChar(i) : txt[i];} 
};
const char* MATCH_DATA::byteFmt = " %02X"; 
const char* MATCH_DATA::charFmt = " %c ";


void matchPrint (const Match* pMatch, char MatchType, long FileLen) {
   if (!pMatch || !pMatch->segments.size()) return;

   int l = pMatch->length;
   printf ("%s match, length %d", name(pMatch->encr), l);
   if (FileLen) printf (" (%d%%, all but %d bytes)", (l*100)/FileLen, FileLen-l);
   printf ("\n");

   for (int s = 0; s < pMatch->segments.size(); s++) {
      const File* pFile = pMatch->segments[s]->pFile;
      const char* name = pFile->filename;
      int o = pMatch->segments[s]->first;

      MATCH_DATA Data[] = {
         {pFile->Invar[0],            MATCH_DATA::byteFmt},
         {pFile->Invar[pMatch->encr], MATCH_DATA::byteFmt},
         {(byte*)pFile->MatchMap,     MATCH_DATA::charFmt},
         {(byte*)pFile->CodeMap,      MATCH_DATA::charFmt}
      };

      printf ("   %s @%8d in %s\n", (s || MatchType != HI_MAP ? "Inf.":"Host"), o, name);

      for (int t = 0; t < arraySize(Data); t++) {
         if (!Data[t].txt) {assert(t==3);continue;}
         if (t && Data[t].txt == Data[t-1].txt) {assert(pMatch->encr == PLAIN); continue;}

         printf("   ");

         for (int d = o-LPAD; d < o+l+LPAD; d++) {
            if (d == o || d == o+l) printf(" |");
            if (d == o+L2 && d < o+l-L2) printf(" _");

            if (d < o+L2 || d >= o+l-L2) {
               if (d < 0 || d >= pFile->length) {
                  printf(" . ");
               }else {
                  printf(Data[t].fmt,(int)(Data[t].value(d)));
               }
            }
         }
         printf("\n");
      }
   }
   matchSummaryPrint(pMatch,FileLen);
   printf("\n");
}


void matchSummaryPrint (const Match* pMatch, long FileLen) {
   if (!pMatch || !pMatch->segments.size()) return;

   int l = pMatch->length;
   printf ("   %s match, length %d", name(pMatch->encr), l);
   if (FileLen) printf (" (%d%%, all but %d bytes)", (l*100)/FileLen, FileLen-l);
   printf ("\n");

   for (int s = 0; s < pMatch->segments.size(); s++) {
      const Segment* pSeg = pMatch->segments[s];
      const char* name = pSeg->pFile->filename;
      if (strrchr(name, '\\')) name = strrchr(name, '\\') + 1;

      printf("%16.16s",name);
      for (int a = 0; a < ANCHOR_COUNT; a++) {
         printf ("  |  %7ld %7ld",pSeg->firstLocn(ANCHOR(a)).Offset,pSeg->lastLocn(ANCHOR(a)).Offset); 
      }
      printf("\n");
   }
}



// checks that a match is a common substring and it's not a substring of a longer one

bool matchCheck (const Match* pMatch) {
   if (!pMatch) {puts("matchCheck: no match"); return false;}
   if (pMatch->segments.size() <= 1) {puts("matchCheck: no files"); return false;}

   int beforeMatch = 0, afterMatch = 0;

   int l = pMatch->length;
   const File* pFile0 = pMatch->segments[0]->pFile;

   // reference file is Files[0]
   int pos0 = pMatch->segments[0]->first;
   int end0 = pFile0->length - pos0;
   byte* byte0 = pFile0->Invar[pMatch->encr] + pos0;
   char* map0 = pFile0->MatchMap + pos0;

   if (end0 < l) {
      printf("matchCheck: too long for file 0 %d\n", l);
      return false;
   }

   // compare each other to reference file
   for (int s = 1; s < pMatch->segments.size(); s++) {
      int pos = pMatch->segments[s]->first;
      const File* pFile = pMatch->segments[s]->pFile;
      int end = pFile->length - pos;
      byte* byte = pFile->Invar[pMatch->encr] + pos;
      char* map = pFile->MatchMap + pos;

      if (end < l) {
         printf("matchCheck: too long for file %d %d\n", s, l);
         return false;
      }
      for (int b = 1; b <= min(pos,pos0); b++) {
         if (byte[-b] != byte0[-b]) break;
         if (map[-b] || map0[-b]) break;
         if (pFile->isFieldStart(pos-b) && pFile0->isFieldStart(pos0-b)) {
            beforeMatch++;
            break;
         }
      }
      for (int o = keyLength(pMatch->encr); o < l; o++) {
         if (byte0[o] != byte[o]) { 
            printf("matchCheck: file %d mismatch @ %d\n", s, o);
            return false;
         }
         if (map0[o] != map[o]) { 
            printf("matchCheck: file %d map mismatch @ %d\n", s, o);
            return false;
         }
      }
      for (; o < min(end,end0); o++) {
         if (byte[o] != byte0[o]) break;
         if (map[o] || map0[o]) break;
         if (pFile0->isFieldStart(o) && pFile->isFieldStart(o)) {
            afterMatch++;
            break;
         }
      }
   }
   if (beforeMatch == pMatch->segments.size()-1) {
      printf("matchCheck: could extend before\n");
      return false;
   }
   if (afterMatch == pMatch->segments.size()-1) {
      printf("matchCheck: could extend after\n");
      return false;
   }
   return true;
}


//-------------------------------------------------------------------------------------



void prtHImatches (const FileClass& Class) {
   for (int s = 0; s < Class.Samples.size(); s++) {
      printf ("------\nsample (ep %d len %d): %s\n  host (ep %d len %d)\n------\n",
         Class.Samples[s]->Inf.entry_pt,Class.Samples[s]->Inf.length,Class.Samples[s]->Inf.filename,Class.Samples[s]->Host.entry_pt,Class.Samples[s]->Host.length); 
      for (Match_vec_iter m = Class.Samples[s]->HImatches.begin(); m != Class.Samples[s]->HImatches.end(); m++) {
         matchCheck(*m);
         matchPrint(*m,HI_MAP,Class.Samples[s]->Host.length);
      }
   }
   puts("");
}



void prtIconstMatches (const FileClass& Class) {
   printf ("------------\nclass Iconst\n------------\n"); 
   for (int m = 0; m < Class.IconstMatches.size(); m++) {
      matchCheck(Class.IconstMatches[m]);
      matchPrint(Class.IconstMatches[m],I_MAP);
   }
   puts("");
}


void prtHIsections (const FileClass& Class) {
   for (int HI = 0; HI < Class.HIsections.size(); HI++) {
      prtHIsect(Class.HIsections[HI]);
   }
   puts("");
}


void prtHIsect (const HIsection* pS) {
   if (pS) { 
      printf ("HI section %5s:   H %6d%s.%d,%6d%s.%d   I %6d%s.%d,%6d%s.%d   L %6ld\n",
      name(pS->encr()),
      pS->Host.First.Offset,AnchorName[pS->Host.First.Anchor],pS->Host.First.Range,
      pS->Host.Last.Offset, AnchorName[pS->Host.Last.Anchor], pS->Host.Last.Range,
      pS->Infected.First.Offset,AnchorName[pS->Infected.First.Anchor],pS->Infected.First.Range,
      pS->Infected.Last.Offset,AnchorName[pS->Infected.Last.Anchor],pS->Infected.Last.Range,
      pS->minLength());
   }else puts ("   HI section: null");
}


void prtIconstSections (const FileClass& Class) {
   for (int Ic = 0; Ic <  Class.IconstSections.size(); Ic++) {
      printf ("Iconst section %5s:    %6d%s.%d,%6d%s.%d   Lmin %6ld\n",
              name(Class.IconstSections[Ic]->encr),
              Class.IconstSections[Ic]->First.Offset,    AnchorName[Class.IconstSections[Ic]->First.Anchor],    Class.IconstSections[Ic]->First.Range,
              Class.IconstSections[Ic]->Last.Offset,     AnchorName[Class.IconstSections[Ic]->Last.Anchor],     Class.IconstSections[Ic]->Last.Range,
              Class.IconstSections[Ic]->minLength);
   }
   puts("");
}



void prtIvarSections (const FileClass& Class) {
   for (int Iv = 0; Iv < Class.IvarSections.size(); Iv++) {
      printf ("Ivar section %5s:      %6d%s.%d,%6d%s.%d   Lmin %6ld\n", 
              name(Class.IvarSections[Iv]->encr),
              Class.IvarSections[Iv]->First.Offset,    AnchorName[Class.IvarSections[Iv]->First.Anchor],    Class.IvarSections[Iv]->First.Range,
              Class.IvarSections[Iv]->Last.Offset,     AnchorName[Class.IvarSections[Iv]->Last.Anchor],     Class.IvarSections[Iv]->Last.Range,
              Class.IvarSections[Iv]->minLength);
   }
   puts("");
}



void prtIsections (const FileClass& Class) {
   for (int I = 0; I < Class.Isections.size(); I++) {
      printf ("I section %5s:      %6d%s.%d,%6d%s.%d   Lmin %6ld\n",
              name(Class.Isections[I]->encr),
              Class.Isections[I]->First.Offset,    AnchorName[Class.Isections[I]->First.Anchor],    Class.Isections[I]->First.Range,
              Class.Isections[I]->Last.Offset,     AnchorName[Class.Isections[I]->Last.Anchor],     Class.Isections[I]->Last.Range,
              Class.Isections[I]->minLength);
   }
   puts("");
}



void prtHsections (const FileClass& Class) {
   puts("");
   for (int H = 0; H < Class.Hsections.size(); H++) {
      printf ("Host section:      %6d%s.%d,%6d%s.%d\n",
              Class.Hsections[H]->First.Offset,     AnchorName[Class.Hsections[H]->First.Anchor],     Class.Hsections[H]->First.Range,
              Class.Hsections[H]->Last.Offset,      AnchorName[Class.Hsections[H]->Last.Anchor],      Class.Hsections[H]->Last.Range);
   }
   puts("");
}



void prtActualKeys (const FileClass& Class) {
   for (int k = 0; k < Class.ActualKeys.size(); k++) {
      printf ("Actual key:  %s\n",Class.ActualKeys[k]->asString().c_str());
   }
   puts("");
}


void prtAliasedKeys (const FileClass& Class) {
   for (int k = 0; k < Class.AliasedKeys.size(); k++) {
      printf ("Aliased key: %s\n",Class.AliasedKeys[k]->asString().c_str());
   }
   puts("");
}


void prtPossRecons (const FileClass& Class, const char* zTitle) {
   int p = 0;
   for (int r = 0; r < Class.PossibleRecons.size(); r++) {
      printf ("%s %s,"FLOC"\thost "FLOC" \tsrc "FLOC" \tlen %7ld \trelOff %7d\n",
         zTitle,
         //p++,
         name(Class.PossibleRecons[r]->pKey->Encr),
         prtLoc(Class.PossibleRecons[r]->pKey->location()),
         prtLoc(Class.PossibleRecons[r]->HostLoc),
         prtLoc(Class.PossibleRecons[r]->SrcLoc),
         Class.PossibleRecons[r]->Length,
         Class.PossibleRecons[r]->relativeOffset());
   }
   puts("");
}


void prtChosenRecons (const FileClass& Class) {
   for (int r = 0; r < Class.ChosenReconList.size(); r++) {
      printf("Chosen recon: key %s,"FLOC" \trelative offset %3d\n\t "FLOC" \t"FLOC"\n",
         name(Class.ChosenReconList[r]->pKey->Encr),prtLoc(Class.ChosenReconList[r]->pKey->location()),Class.ChosenReconList[r]->relativeOffset(), 
         prtLoc(Class.ChosenReconList[r]->HostLoc), prtLoc(Class.ChosenReconList[r]->SrcLoc));
      if (Class.ChosenReconList[r]->HostLast != Class.ChosenReconList[r]->HostLoc || Class.ChosenReconList[r]->SrcLast != Class.ChosenReconList[r]->SrcLoc) {
            printf("\t "FLOC" \t"FLOC"\n",prtLoc(Class.ChosenReconList[r]->HostLast),prtLoc(Class.ChosenReconList[r]->SrcLast));
      }
   }
   puts("");
}



void prtVerifications (const FileClass& Class) {
   int v;
   puts("");
   for (v = 0; v < Class.Verifications.size(); v++) {
      printf("verification: %s length %ld, encr. %s",
         (Class.Verifications[v]->isCode ? "code" : "data"),
         Class.Verifications[v]->Length,
         name(Class.Verifications[v]->Encr));
      if (Class.Verifications[v]->Encr != PLAIN) {
         printf("\nkey:");
         printf("    \t%s",Class.Verifications[v]->AliasedKeys.asString().c_str());
      }
      printf("\nfrom:");
      printf("    \t%s",Class.Verifications[v]->First.asString().c_str());
      printf("\n  to:");
      printf("    \t%s",Class.Verifications[v]->Last.asString().c_str());
      printf("\n");
   }
}



void prtReconstruction (const FileClass& Class) {
   if (Class.Samples.size() == 0) return;
   prtChosenRecons (Class);
}



void prtClass (const FileClass& Class) {
    prtHImatches (Class);
    prtIconstMatches (Class);
    prtHIsections (Class);
    prtIconstSections (Class);
    prtIvarSections (Class);
    prtIsections (Class);
    prtHsections (Class);
    prtActualKeys (Class);
    prtAliasedKeys (Class);
    prtPossRecons (Class,"PR");
    prtChosenRecons (Class);
}
