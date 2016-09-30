/*##################################################################*/
/*                                                                  */
/* Program name:  Sample.h                                          */
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

#ifndef SAMPLE_H
#define SAMPLE_H

#include "match.h"

//-------------------------------------------------------------------------------------

struct Sample {
   File        Host;
   File        Inf;
   File_vec    Pair;
   Match_vec   HImatches;

   Sample (const char* zHost, const char* zInf, bool hexmode = false, bool useCodeDataSeg = false);
  ~Sample (void);

  void matchHI (void);

  long VirusLength (void) const {return Inf.length - Host.length;}

  unsigned long lengthMatched   (void) const;
  unsigned long lengthUnMatched (void) const {return Host.length - lengthMatched();}
  int           percentMatched  (void) const {return (lengthMatched() * 100) / Host.length;}
};

dclVector(Sample)

//-------------------------------------------------------------------------------------

#endif
