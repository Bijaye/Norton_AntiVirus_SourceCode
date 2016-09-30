/*##################################################################*/
/*                                                                  */
/* Program name:  match.cpp                                         */
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

#include "assert.h"

#include "lcs.h"
#include "map.h"
#include "match.h"

// generally speaking, don't fail here - bad matches can arise by coincidence, and it's
// quite possible that they would be rejected anyway.

int match (File_vec& Files, Match_vec& Matches, uInt lMin, char cMapMarker) {
   int n = 0;   // # of matches found
   int e, f, o;
   const int F = Files.size();
   Match* pBestMatch;

   assert(cMapMarker);

   for (;;) {
      // find the best available match in any invariant
      pBestMatch = NULL;
      for (e = PLAIN; e < ENCR_COUNT; e++) {
         Match* pMatch = LCS(Files,ENCRYPTION(e));
         if (!pMatch) continue;
         if (pMatch->adjust())
            if (!pBestMatch || pMatch->length > pBestMatch->length) 
               swap(pMatch, pBestMatch); //assumes invariants in preferred-first order ?? create a Match comparison op?
         delete pMatch;
      }

   if (!pBestMatch || pBestMatch->length < lMin + keyLength(pBestMatch->encr)) break; // finished - no LCS or it's too short
      
      //?? if encrypted, see if it can be extended at either end (HI only?)

      // save this match...
      Matches.push_back(pBestMatch);
      n++;

      // ...and mark it in the files' maps
      char** apMatchFlag = new char*[F];
      char** apCodeFlag  = new char*[F];
      assert (apMatchFlag && apCodeFlag);
      for (f = 0; f < F; f++) {
         apMatchFlag[f] = Files[f]->MatchMap + pBestMatch->segments[f]->first;
         apCodeFlag[f]  = Files[f]->CodeMap ? Files[f]->CodeMap + pBestMatch->segments[f]->first 
                                            : NULL;
      }
      for (o = 0; o < pBestMatch->length; o++) {
         bool isCodeByte = false;
         for (f = 0; f < F; f++) {
            if (apCodeFlag[f] && *apCodeFlag[f]) {
               isCodeByte = true;
               break;
            }
         }
         for (f = 0; f < F; f++) {
            const int B = pBestMatch->segments[f]->first;
            *(apMatchFlag[f]++) = cMapMarker;
            if (apCodeFlag[f]) {
               if (isCodeByte && !*apCodeFlag[f]) 
                  *apCodeFlag[f] = CODE_IN_OTHER_FILE;
               apCodeFlag[f]++;
            }
         }
      }
      delete[] apMatchFlag;
      delete[] apCodeFlag;
   }
   delete pBestMatch;
   return n;
}



// print msg, invalidate segments and return false if there's a problem?
bool Match::adjust (void) {
   // adjust for encryption type: the invariant of an encrypted matching
   // segment will not match on the first <keylength> bytes, because that
   // part of the invariant has been created by combining its own matching
   // bytes with non-matching bytes from before the matching segment.
   if (encr != PLAIN) {
      int lAdj;
      for (int s = 0; s < segments.size(); s++) {
         // The match may have extended up to <keylength>-1 bytes into the
         // initial region by coincidence, so make sure that the adjustment 
         // is <keylength>-aligned.
         int segAdj = segments[s]->first % keyLength(encr);
         if (!segAdj) segAdj = keyLength(encr);
         if (s && lAdj != segAdj)     // segments aren't aligned
            return false;
         lAdj = segAdj;
         // check that there is room for the segment to be adjusted - i.e. it is not
         // right at the start of the file or immediately after a matched region (this could 
         // occur by coincidence) ?? other reasons?
         for (int p = 1; p <= lAdj; p++) {
            const long offset = segments[s]->first-p;
            if (offset < 0 || segments[s]->pFile->MatchMap[offset]) 
               return false;   // indicates accidental match?
         }
         segments[s]->first -= lAdj;
      }
      length += lAdj;
      //??printf ("segments adjusted by %d for initial keylength mismatch effect\n",lAdj);
   }

   // adjust for structure boundary - if any byte of a match is in a field, all of that field's
   // bytes must be in the match; if not, shorten the match so it does not occupy the field.
   //?? HI matches - consider host side only?
   //?? reconstructions - must perform equivalent test for recon groups.

   // segment beginnings...
   for (;;) {
      // find the least adjustment needed
      long maxAdjLen = 0;
      for (int s = 0; s < segments.size(); s++) {
         for (long adjLen = 0; !segments[s]->pFile->isFieldStart(segments[s]->first+adjLen); adjLen++);
         maxAdjLen = max(adjLen, maxAdjLen);
      }
   if (!maxAdjLen) break;
      // make the adjustment
      //??printf ("adjusting segments by %d for program structure\n",maxAdjLen);
      for (s = 0; s < segments.size(); s++) {
         segments[s]->first += maxAdjLen;
         if (segments[s]->first >= segments[s]->last) {
            printf ("*** segment disappears in match adjustment\n"); //?? FailMsg?
            return false;
         }
      }
      length -= maxAdjLen;
   }
  
   // ... and ends
   for (;;) {
      // find the least adjustment needed
      long maxAdjLen = 0;
      for (int s = 0; s < segments.size(); s++) {
         for (long adjLen = 0; !segments[s]->pFile->isFieldEnd(segments[s]->last-adjLen); adjLen++);
         maxAdjLen = max(adjLen, maxAdjLen);
      }
   if (!maxAdjLen) break;
      // make the adjustment
      for (s = 0; s < segments.size(); s++) {
         segments[s]->last -= maxAdjLen;
         if (segments[s]->last <= segments[s]->first) return false; //??FailMsg("*** segment disappears in match adjustment");
      }
      length -= maxAdjLen;
   }

   return true;
}


//-----------------------------------------------------------------------------------------------

Match::Match (const Match& other)
:  length(other.length), encr(other.encr), pParent(other.pParent ? other.pParent : &other)
{
   segments.reserve(other.segments.size());
   for (int s = 0; s < other.segments.size(); s++) {
      segments.push_back (new Segment(*other.segments[s]));
   }
}



Match::Match(const Match& other, const File_vec& Files)
:  length(other.length), encr(other.encr), pParent(other.pParent ? other.pParent : &other)
{
   assert (other.segments.size() >= Files.size());
   segments.reserve(Files.size());
   for (int f = 0; f < Files.size(); f++) {
      for (int s = 0; s < other.segments.size(); s++) {
         if (Files[f] == other.segments[s]->pFile) {
            segments.push_back (new Segment(*other.segments[s]));
            break;
         }
      }
      assert(s < other.segments.size()); // no segment for file f
   }
}



void Match::emitMap (vector<char*>& MapTxt) const {
   if (length < 2 * size(encr) || length < sizeof(polyn)) return; //?? what's a reasonable minimum size?

   char* line = NULL;  // A line of text to be output (dynamically allocated)

   const Segment* pRefSeg = segments[0];   //?? use the original sample file?
   assert (length == pRefSeg->length());
   long StartOffset = pRefSeg->first;
   const byte* pTxt = pRefSeg->pFile->Invar[PLAIN];
   const char* pCD  = pRefSeg->pFile->CodeMap + StartOffset;
   bool isCode = IS_CODE(*pCD);

   for (long o = StartOffset; o <= pRefSeg->last; (o++,pCD++)) {
      if (IS_CODE(*pCD) != isCode) {
         emitMapElement (MapTxt,START,StartOffset,o-StartOffset,isCode,encr,pTxt);
         StartOffset = o;
         isCode = IS_CODE(*pCD);
      }
   }
   emitMapElement (MapTxt,START,StartOffset,o-StartOffset,isCode,encr,pTxt);
}
