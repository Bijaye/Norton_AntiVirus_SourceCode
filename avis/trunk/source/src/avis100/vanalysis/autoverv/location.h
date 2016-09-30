/*##################################################################*/
/*                                                                  */
/* Program name:  location.h                                        */
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

#ifndef LOCATION_H
#define LOCATION_H

#include "autoverv.h"


typedef enum {
   NULL_ANCHOR = -1,
   START = 0,
   ENTRY,
   END,
   // add any more here
   ANCHOR_COUNT
} ANCHOR;


extern const char* AnchorName[];


struct Location {
   static const Location null;   // as returned by the default constructor

   ANCHOR Anchor;
   long   Offset;
   uLong  Range;

   Location (void): Anchor(NULL_ANCHOR), Offset(0), Range(UINT_MAX) {}
   Location (ANCHOR A, long O): Anchor(A), Offset(O), Range(UINT_MAX) {}
   Location (ANCHOR A, long O, uLong R): Anchor(A), Offset(O), Range(R) {}

   operator bool (void) const {return Anchor != NULL_ANCHOR;}

   Location operator+ (long delta) const {return Location(Anchor,Offset+delta,Range);}
  
   Location operator- (long delta) const {return Location(Anchor,Offset-delta,Range);}
  
   bool immediatelyFollows (const Location other) const;
   
   bool precedes (const Location other) const;

   string asString (void) const;
   
   bool operator <  (Location other) const;

   dclComparisons(Location)
};

#endif
