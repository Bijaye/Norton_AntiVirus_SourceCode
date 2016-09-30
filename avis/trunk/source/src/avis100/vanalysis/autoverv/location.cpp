/*##################################################################*/
/*                                                                  */
/* Program name:  location.cpp                                      */
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

#include "location.h"

const Location Location::null;

const char* AnchorName[] = {
   "B",
   "T",
   "E"
};


bool Location::operator <  (Location other) const {
   return Anchor <  other.Anchor
       || Anchor == other.Anchor && Offset <  other.Offset
       || Anchor == other.Anchor && Offset == other.Offset && Range < other.Range;
}

bool Location::immediatelyFollows (const Location other) const {
   return Anchor == other.Anchor && Offset == other.Offset+1;
}


bool Location::precedes (const Location other) const {
   return Anchor == other.Anchor && Offset < other.Offset;
}


string Location::asString (void) const {
   string txt;
   if (Anchor != NULL_ANCHOR) {
      txt += str("%ld",Offset);
      txt += AnchorName[Anchor];
      txt += '.';
      txt += str("%ld",Range);
   }
   return txt;
}
   



