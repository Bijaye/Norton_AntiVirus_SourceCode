/*##################################################################*/
/*                                                                  */
/* Program name:  sample.cpp                                        */
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

#include "sample.h"
#include "match.h"



Sample::Sample (const char* zHost, const char* zInf, bool hexmode, bool useCodeDataSeg)
:  Host(zHost, hexmode), Inf(zInf, hexmode, useCodeDataSeg) {
   Pair.reserve(2); Pair.push_back(&Host); Pair.push_back(&Inf);
}



Sample::~Sample (void) {
   for (Match_vec_iter m = HImatches.begin(); m != HImatches.end(); m++) delete *m;
}



void Sample::matchHI (void) {
   assert(HImatches.size() == 0);
   // find all the matches between the sample's host and infected files
   match (Pair, HImatches, min(MIN_HI,min(Host.length,Inf.length)/2), HI_MAP);
   // sort the matches by increasing offset into the infected file
   sort (HImatches.begin(), HImatches.end(), beforeInInf());
}


unsigned long Sample::lengthMatched (void) const {
   unsigned long MatchLength = 0;
   for (int m = 0; m < HImatches.size(); m++) {
      MatchLength += HImatches[m]->length;
   }
   assert (Host.length >= MatchLength);
   return MatchLength;
}
