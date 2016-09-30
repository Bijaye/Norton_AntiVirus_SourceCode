/*##################################################################*/
/*                                                                  */
/* Program name:  segment.cpp                                       */
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

#include "segment.h"

#include "assert.h"


// returns change in length of segment
long Segment::adjustFirstTo (Location l) {
   const long delta = adjustFirstBy (first - pFile->offset(l));
   if (first != pFile->offset(l)) FailMsg ("segment first adjustment");
   return delta;
}


// returns change in length of segment
long Segment::adjustFirstBy (long delta) {
   long m;
   const long newFirst = first - delta;
   assert (delta <= 0);
   for (m = first; m < newFirst && pFile->MatchMap[m]; m++) pFile->MatchMap[m] = 0;
   delta = first - m;
   first = m;
   return delta;
}


// returns change in length of segment
long Segment::adjustLastTo (Location l, char MapFlag) {
   const long delta = adjustLastBy (pFile->offset(l) - last, MapFlag);
   if (last != pFile->offset(l)) 
      FailMsg("segment last adjustment");   // NB: could have happened if a padding area had matched as an Iconst area, so now this is done before Iconst matching
   return delta;
}


// returns change in length of segment
long Segment::adjustLastBy (long delta, char MapFlag) {
   long m;
   const long newLast = last + delta;
   if (delta <= 0)
      for (m = last; m > newLast && pFile->MatchMap[m]; m--) pFile->MatchMap[m] = 0;
   else
      for (m = last; m < newLast && !pFile->MatchMap[m+1]; m++) pFile->MatchMap[m+1] = MapFlag;
   delta = m - last;
   last = m;
   return delta;
}

