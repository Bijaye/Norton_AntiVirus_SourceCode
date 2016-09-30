/*##################################################################*/
/*                                                                  */
/* Program name:  section.h                                         */
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

#ifndef SECTION_H
#define SECTION_H

#include "encryption.h"
#include "location.h"
#include "segment.h"
#include "key.h"
#include "match.h"

//-------------------------------------------------------------------------------------

struct Section {
   Location     First, Last;
   ENCRYPTION   encr;
   Segment_vec  Segments;
   long         minLength;
   const Match* pMatch;
   
   // use this constructor to make a section of segments that match each other (e.g. Iconst)
   Section (const Match& match, ANCHOR anchor = NULL_ANCHOR);  //?? used? if NULL_ANCHOR, find best
   // use this constructor when the segments do not match each other (eg H, HI)
   Section (const Segment_vec& Segs);
   // this constructs a section to fit between the predecessor and successor
   Section (const Section* pPredecessor, const Section* pSuccessor, ENCRYPTION e = PLAIN);
   // copy constructor
   Section (const Section& other);

  ~Section (void);

   void initSegments (const Segment_vec& Segs,  ANCHOR anchor = NULL_ANCHOR);  //?? used? if NULL_ANCHOR, find best

   bool appendSection (const Section& other);

   Key aliasedKey (const File_vec& Files) const {return Key(*this,Files);}   //?? make Files a member of this class?

   Location boundaryLocn (Boundary whichEnd, ANCHOR a) const;
   Location bestLocn (Boundary whichEnd) const;
   uLong worstRange (void) const;
   bool isGoodSection (bool anchorsMustMatch = true, uLong maxAcceptableRange = 0) const;
   bool precedesInAllFiles (const Section& other) const;
   bool precedesInAllFiles (Location Loc) const;

   void emitPAMmap (vector<char*>& MapTxt, const char* zClassName) const;
//?? not needed?   bool operator< (const Section& other) const {return precedesInAllFiles(other);}
   string asString (void) const; 
};

dclVector(Section)

struct beforeInFiles {
   bool operator() (const Section* pS1, const Section* pS2) const {return pS1->precedesInAllFiles(*pS2);}
};




struct HIsection {
   Section     Infected;
   Section     Host;
   const Key*  pActualKey;


   HIsection (const Segment_vec& HostSegs, const Segment_vec& InfSegs);

   void removeVariability (void);

   ENCRYPTION encr (void) const {assert(Infected.encr == Host.encr); return Infected.encr;} //?? does Host.encr mean anything? 
   long minLength  (void) const {assert(Infected.minLength == Host.minLength); return Infected.minLength;}
   uLong worstRange (void) const;
   bool isGoodSection (bool anchorsMustMatch = true, uLong maxAcceptableRange = 0) const;
   ENCRYPTION findCommonEncryption (void) const;
};

dclVector(HIsection)


struct HIbeforeInHost {
   bool operator() (const HIsection* s1, const HIsection* s2) const {
      return s1->Host.First < s2->Host.First;
   }
};

//-------------------------------------------------------------------------------------

#endif
