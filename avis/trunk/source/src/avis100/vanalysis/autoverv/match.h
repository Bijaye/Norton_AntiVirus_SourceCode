/*##################################################################*/
/*                                                                  */
/* Program name:  match.h                                           */
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

#ifndef MATCH_H
#define MATCH_H

#include "segment.h"

//-------------------------------------------------------------------------------------

struct Match {
  int  length;
  Segment_vec segments;
  ENCRYPTION encr;
  const Match* pParent;

  Match (uInt n = 0): length(0), encr(PLAIN), pParent(NULL) {segments.reserve(n);}
  Match (const Match& other);
  Match (const Match& other, const File_vec& Files);
 ~Match (void) {for (Segment_vec_iter s = segments.begin(); s != segments.end(); s++) delete *s;}

  bool adjust (void);

  // NB: can either emit the map from the verification list or the Iconst matches - see 
  // VerSec::emitMap() in verifivation.h.
  void emitMap (vector<char*>& MapTxt) const;

  Segment* seg     (section_type seg_t) const 
                   {assert(seg_t==HOST||seg_t==INFECTED);return segments[seg_t];}    //??HI only

  int  offset      (section_type seg_t) const 
                   {assert(seg_t==HOST||seg_t==INFECTED);return seg(seg_t)->first;}   //??HI only

  Segment* infSeg  (void)               const {return seg(INFECTED);}       //??HI only
  int  infOffset   (void)               const {return offset(INFECTED);}    //??HI only
  Segment* hostSeg (void)               const {return seg(HOST);}           //??HI only
  int  hostOffset  (void)               const {return offset(HOST);}        //??HI only

  bool isBeforeInInf (const Match& other) const {return infOffset() < other.infOffset();} //?? HI only
};

dclVector(Match)


/***??
struct HImatch: public Match {
  int  infOffset   (void)               const {return segments[1]->first;}
  bool beforeInInf (const HImatch& other) const {return infOffset() < other.infOffset();}
};

dclVector(HImatch)
**/

// function object: tests if HImatch m1 comes before HImatch m2 in the infected file 
struct beforeInInf {
   bool operator() (const Match* m1, const Match* m2) const {return m1->isBeforeInInf(*m2);}
};


//-------------------------------------------------------------------------------------

// Successively finds LCS in the so-far unmatched parts of the members of Sample.
// Matches will be appended to Matches in order of decreasing size.
// lMin is the shortest acceptable match length.
// cMapMarker is the value that will be put in the samples' map_bytes to indicate
//   that a byte is part of a matched section.
// return value is the number of matches found. (-1 if error??)

int match (File_vec& vFile, Match_vec& Matches, uInt lMin, char cMapMarker);

//-------------------------------------------------------------------------------------

#endif
