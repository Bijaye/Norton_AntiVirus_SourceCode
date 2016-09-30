/*##################################################################*/
/*                                                                  */
/* Program name:  section.cpp                                       */
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

#include "section.h"
#include "map.h"
#include "test.h"    //??testing only


const void* findchr (const unsigned char* z, int c, int l) {
   while (l) {
      if (z[--l] == c) return z;
   };
   return NULL;
}

//-----------------------------------------------------------------------------------------------

Section::Section (const Match& Match, ANCHOR anchor)  // if NULL_ANCHOR, find best
:  pMatch(&Match)
{
   initSegments (Match.segments, anchor);
}



Section::Section (const Segment_vec& Segs)
:  pMatch(NULL)
{
   initSegments (Segs);
}



void Section::initSegments (const Segment_vec& Segs, ANCHOR anchor) {  // if NULL_ANCHOR, find best
   int s;

   minLength = LONG_MAX;
   //?? is there a better way than copying? can we do without them altogether?
   for(s = 0; s < Segs.size(); s++) {
      Segments.push_back (new Segment(*Segs[s]));
      minLength = min(minLength, Segs[s]->length());
   }

   if (anchor == NULL_ANCHOR) {
      First = bestLocn (FIRST);
      Last  = bestLocn (LAST);
   }else {
      First = boundaryLocn (FIRST, anchor);
      Last  = boundaryLocn (LAST, anchor);
   }

   encr = Segs[0]->encr;
   for(s = 1; s < Segs.size(); s++) {   //?? fix up the input segments? if so, reset s=0 if a difference is found to make sure they're all the same
      encr = getSuperset (encr, Segs[s]->encr);
   }
}



// for generating a section that fits between *pPredecessor and *pSuccessor.
// If pPredecessor is NULL, start at (START,0,0); if pSuccessor is NULL, end at (END,0,0)
Section::Section (const Section* pPredecessor, const Section* pSuccessor, ENCRYPTION e)
:  encr(e), pMatch(NULL)
{
   assert ( pPredecessor ||  pSuccessor);
   assert (!pPredecessor || !pSuccessor || pPredecessor->Segments.size() == pSuccessor->Segments.size());

   First = (pPredecessor ? pPredecessor->Last + 1L : Location(START,0,0));
   Last  = (pSuccessor   ? pSuccessor->First  - 1L : Location(END,0,0));

   const Segment_vec& ReferenceSegs = (pPredecessor ? pPredecessor->Segments : pSuccessor->Segments);

   for (int s = 0; s < ReferenceSegs.size(); s++) {
      const File* pFile = ReferenceSegs[s]->pFile;
      assert(!pPredecessor || !pSuccessor || pSuccessor->Segments[s]->pFile == pFile);
      if (pFile->isInFile(First) && pFile->isInFile(Last)) {
         const long first = pFile->offset(First);
         const long last  = pFile->offset(Last);
         if (last >= first) {
            const long length = last - first + 1;
            Segments.push_back (new Segment(pFile, first, length, encr));
            if (s == 0 || length < minLength) minLength = length;
            continue;
         }
      }
      // predecessor follows successor
      Segments.push_back (new Segment());
      minLength = -1;
   }
   assert(s > 0);
}



Section::Section (const Section& other)
:  First(other.First), Last(other.Last), encr(other.encr), minLength(other.minLength), pMatch(other.pMatch)
{
   //?? is there a better way than copying? can we do without them altogether?
   for(int s = 0; s < other.Segments.size(); s++) {
      Segments.push_back (new Segment(*other.Segments[s]));
   }
}



Section::~Section (void) {
   //?? is there a better way than copying above?
   //?? copying segments is good if we modify them afterwards - e.g. truncating to get zero range
   for(int s = 0; s < Segments.size(); s++) delete Segments[s];
}



bool Section::appendSection (const Section& other) {
   if (other.encr != encr || other.First != Last+1L) return false;
   Last = other.Last;
   for (int s = 0; s < Segments.size(); s++) {
      const File* pFile = Segments[s]->pFile;
      const long first  = pFile->offset(First);
      const long last   = pFile->offset(Last);
      const uInt length = last - first + 1;
      *Segments[s] = Segment(pFile, first, length, encr);
      if (s == 0 || length < minLength) minLength = length;
   }
   return true;
}



uLong Section::worstRange (void) const {
   return max(First.Range, Last.Range);
}


bool Section::isGoodSection (bool anchorsMustMatch, uLong maxAcceptableRange) const {
   //?? check end not before start in all files?
   if (worstRange() > maxAcceptableRange) return false;
   if (encr == NULL_ENCRYPTION) return false;
   if (anchorsMustMatch && First.Anchor != Last.Anchor) return false;
   return true;
}



bool Section::precedesInAllFiles (const Section& other) const {
   assert(Segments.size() == other.Segments.size());

   for (int s = 0; s < Segments.size(); s++) {
      assert(Segments[s]->pFile == other.Segments[s]->pFile);
      if (Segments[s]->first >= other.Segments[s]->first) return false;
   }
   return true;
}



bool Section::precedesInAllFiles (Location Loc) const {
   for (int s = 0; s < Segments.size(); s++) {
      if (Segments[s]->last >= Segments[s]->pFile->offset(Loc)) return false;
   }
   return true;
}



string Section::asString (void) const {
   string txt = str("%6d",First.Offset) + string(AnchorName[First.Anchor]) + "." + str("%d",First.Range) + ","
              + str("%6d",Last.Offset)  + string(AnchorName[Last.Anchor])  + "." + str("%d",Last.Range)  + ","
              + string(name(encr)) + "\n";
   return txt;
}

//-----------------------------------------------------------------------------------------------

HIsection::HIsection (const Segment_vec& HostSegs, const Segment_vec& InfSegs)
:  Infected(InfSegs), Host(HostSegs), pActualKey(NULL)
{
   if (Host.encr == NULL_ENCRYPTION) {   //?? so will the infected encr, due to findSuperset
      Infected.encr = Host.encr = findCommonEncryption();   //?? other action here or later?
   }
   removeVariability();
}


void HIsection::removeVariability (void) {
   int s,h;

   assert (Host.Segments.size() == Infected.Segments.size());

   // special case: padding after the HI section containing the end of the host (e.g. to some 
   // power of 2 boundary) can cause variability in its best end location in the infected file,
   // if that location is relative to some anchor beyond it (i.e. the location offset is -ve,
   // implying an anchor type of END or ENTRY). In this case, we can extend the shorter HI
   // sections until the range is zero - the reconstruction of the extended files will be
   // followed by some junk from the padding, but that doesn't matter.

   if(Host.Last == Location(END,0,0) 
   && Infected.Last.Offset < 0 
   && Infected.Last.Range > 0 
   && Infected.Last.Range <= MAX_PADDING
   ){
      //?? extend up to next Iconst section or EOF? - can't be sure that we've seen the max range.
      //?? (NB: must extend at least as far ar minOffset+range)
      Infected.Last = Location(Infected.Last.Anchor,Infected.Last.Offset+Infected.Last.Range,0);
      for (s = 0; s < Infected.Segments.size(); s++) {
         Infected.Segments[s]->adjustLastTo(Infected.Last,P_MAP);
      }
   }


   if (Infected.First.Range != Host.First.Range || Infected.Last.Range != Host.Last.Range)
      goto fail;
   if (Infected.First.Range == 0 && Infected.Last.Range == 0) return;

   Infected.First = Location(Infected.First.Anchor, Infected.First.Offset + Infected.First.Range, 0);
   Host.First     = Location(Host.First.Anchor, Host.First.Offset + Host.First.Range, 0);
   Infected.Last  = Location(Infected.Last.Anchor, Infected.Last.Offset, 0);
   Host.Last      = Location(Host.Last.Anchor, Host.Last.Offset, 0);

   for (h = 0; h < Host.Segments.size(); h++) {
      if (!Host.Segments[h]->isInFile(Host.First) || !Host.Segments[h]->isInFile(Host.Last))
         goto fail;
   }
   
   for (s = 0; s < Infected.Segments.size(); s++) {
      if (!Infected.Segments[s]->isInFile(Infected.First) || !Infected.Segments[s]->isInFile(Infected.Last))
         goto fail;
      if(Infected.Segments[s]->adjustFirstTo(Infected.First) != Host.Segments[s]->adjustFirstTo(Host.First)) 
         goto fail;
      if(Infected.Segments[s]->adjustLastTo(Infected.Last)!= Host.Segments[s]->adjustLastTo(Host.Last))
         goto fail;
      if (Infected.Segments[s]->length() <= 0 || Host.Segments[s]->length() <= 0) 
         goto fail;
      assert(Infected.Segments[s]->length() == Host.Segments[s]->length() 
          || Host.Last == Location(END,0,0));
   }
   return;

fail:
   puts("--+ host section eliminated"); //?? other action here?
   Infected.encr = Host.encr = NULL_ENCRYPTION;
   return;
}


uLong HIsection::worstRange (void) const {
   return max(Infected.worstRange(), Host.worstRange());
}



bool HIsection::isGoodSection (bool anchorsMustMatch, uLong maxAcceptableRange) const {
   if (!Infected.isGoodSection(anchorsMustMatch, maxAcceptableRange)) return false;
   if (!Host.isGoodSection(anchorsMustMatch, maxAcceptableRange)) return false;
   if (Infected.encr != Host.encr) return false;
   // if variable length in one file, must be variable length in the other
   if (Infected.First.Anchor == Infected.Last.Anchor && Host.First.Anchor != Host.Last.Anchor) return false;
   if (Infected.First.Anchor != Infected.Last.Anchor && Host.First.Anchor == Host.Last.Anchor) return false;
   return true;
}



// Host
ENCRYPTION HIsection::findCommonEncryption (void) const {
   assert (Infected.Segments.size() == Host.Segments.size());

   for (ENCRYPTION e = PLAIN; e < ENCR_COUNT; e = ENCRYPTION(e+1)) {
      bool encrOK = true;
      for (int f = 0; f < Host.Segments.size(); f++) {
         const File* pHfile = Host.Segments[f]->pFile;
         const File* pIfile = Infected.Segments[f]->pFile;
         const long Hoffset = Host.Segments[f]->first;
         const long Ioffset = Infected.Segments[f]->first;
         const long length  = Host.Segments[f]->last - Hoffset + 1;
         assert (Infected.Segments[f]->last - Ioffset + 1 == length);
         const int adjust = keyLength(e);   // invariants may not match on the first key-length bytes

         if (memcmp (pHfile->Invar[e] + Hoffset + adjust, 
                     pIfile->Invar[e] + Ioffset + adjust, 
                     length - adjust))
         {
            encrOK = false;
            break;
         }
      }
      if (encrOK) return e;
   }
   return NULL_ENCRYPTION;
}


// Extracts PAM signatures from plaintext Iconst sections that are within the
// 512 bytes following the entry point.

// A PAM signature is between 4 and 24 bytes long, contains no wildcards, and
// must begin with one of 16 specified start bytes.

// For each start byte within that part of the section (if any) that is
// in the 512-byte region specified above, emit the longest possible PAM 
// signature that begins at that byte and is wholly within the Iconst section
// (it may extend beyond the 512-byte region).

// The signatures are emitted in VIDES2 format, for input to Autosig.

// A signature is only considered to be 'code' if every byte is code.

// If a signature starts as code, both the longest possible code and the 
// longest possible non-code signature are emitted, if they are different.

void Section::emitPAMmap (vector<char*>& MapTxt, const char* zClassName) const {
   const long PAM_SCAN_RANGE     = 512;
   const long PAM_MIN_SIG_LENGTH =  4;
   const long PAM_MAX_SIG_LENGTH = 24;

   const byte SigStartChars[]   = {0x2E, 0x3D, 0x50, 0x74, 0x75, 0x8B, 0xB4, 0xB8,
                                   0xB9, 0xBA, 0xBB, 0xBE, 0xBF, 0xCD, 0xE8, 0xE9};

   long lCode = 0, lMaxCode = 0, nCode = 0, lAll = 0, lMaxAll = 0, nAll = 0;

   if (encr != PLAIN) return;

   assert(pMatch);

   const Location lFirst = boundaryLocn (FIRST,ENTRY);
   const long oFirst     = long(lFirst.Offset + lFirst.Range);
   const long oLast      = long(oFirst + pMatch->length);
   const long oBegin     = max (0L,oFirst);
   const long oEnd       = min (PAM_SCAN_RANGE + PAM_MAX_SIG_LENGTH, oLast);
   const long oStartRangeEnd = min (PAM_SCAN_RANGE, oLast - PAM_MIN_SIG_LENGTH);

   const long  EP   = Segments[0]->pFile->entry_pt;
   const byte* pTxt = Segments[0]->pFile->Invar[PLAIN] + EP;
   const char* pCD  = Segments[0]->pFile->CodeMap + EP;

   for (long o = oBegin; o < oStartRangeEnd; o++) {

   // signature must begin with a member of the start byte set
   if (!findchr (SigStartChars,pTxt[o],arraySize(SigStartChars))) continue;

      // emit longest code signature possible at this offset
      for (long l = 0; l < PAM_MAX_SIG_LENGTH && o+l < oEnd && IS_CODE(pCD[o+l]); l++);

      if (l >= PAM_MIN_SIG_LENGTH) {
         emitPAMmapElement (MapTxt,ENTRY,o,l,zClassName,true,PLAIN,pTxt);

         lAll+=l; lMaxAll = max(lMaxAll,l); nAll++; lCode+=l; lMaxCode = max(lMaxCode,l); nCode++;
         printf("PAM signature (code) @ %ldT, length %d\n\t",o,l);prtHex(pTxt+o,l);puts("");
      }

   // if there's a longer non-code signature at the same offset, emit it also
   if (l == PAM_MAX_SIG_LENGTH || l+o == oEnd) continue;

      l = min(PAM_MAX_SIG_LENGTH, oEnd-o);   // must be >= min length, as o+minLength < oEnd
      
      emitPAMmapElement (MapTxt,ENTRY,o,l,zClassName,false,PLAIN,pTxt);
      
      lAll+=l; lMaxAll = max(lMaxAll,l); nAll++;
      printf("PAM signature (noncode) @ %ldT, length %d\n\t",o,l);prtHex(pTxt+o,l);puts("");
   }

   printf ("PAM signatures:        %ld, total length %ld, max length %ld\n",nAll, lAll, lMaxAll);
   printf ("PAM signatures (code): %ld, total length %ld, max length %ld\n",nCode,lCode,lMaxCode);
   return;
}



Location Section::bestLocn (Boundary whichEnd) const {
   Location minRangeLocn;   // default constr. creates 'null' locn. with max. possible range

   for (int a = 0; a < ANCHOR_COUNT; a++) {
      const Location l = boundaryLocn (whichEnd,ANCHOR(a));
      if (l.Range <= minRangeLocn.Range) { //?? <= implies prefer late anchors (used to have prefer_END_0_0 flag)
         minRangeLocn = l;
      }
   }
   return minRangeLocn;
}



Location Section::boundaryLocn (Boundary whichEnd, ANCHOR A) const {
   long minRelOff = LONG_MAX;
   long maxRelOff = LONG_MIN;

   for (int s = 0; s != Segments.size(); s++) {
      const Location loc = Segments[s]->locn(whichEnd,A);
      if (loc.Offset < minRelOff) minRelOff = loc.Offset;
      if (loc.Offset > maxRelOff) maxRelOff = loc.Offset;
   }
   return Location (A, minRelOff, maxRelOff - minRelOff);
}
