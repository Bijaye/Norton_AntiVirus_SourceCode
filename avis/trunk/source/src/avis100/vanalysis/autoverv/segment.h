/*##################################################################*/
/*                                                                  */
/* Program name:  segment.h                                         */
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

#ifndef SEGMENT_H
#define SEGMENT_H

#include "assert.h"

#include "file.h"

//-------------------------------------------------------------------------------------

typedef enum {
   HOST = 0,
   INFECTED = 1, 
   HI,
   Iconst,
   Ivar
} section_type;

typedef enum {
   FIRST = 1,
   LAST
} Boundary;

//-------------------------------------------------------------------------------------

struct Segment {
   const File* pFile;
   long  first;
   long  last;
   ENCRYPTION encr;

   Segment (void): pFile(NULL), first(0), last(0), encr(PLAIN) {}

   Segment (const File* pF, uInt start, uInt len, ENCRYPTION e)
         :  pFile(pF), first(start), last(start+len-1), encr(e) {assert(last >= first);}

   long     adjustFirstTo  (Location l);
   long     adjustFirstBy  (long delta);
   long     adjustLastTo   (Location l, char MAP_FLAG = 0x00);
   long     adjustLastBy   (long delta, char MAP_FLAG = 0x00);

   long     length         (void)     const {return last-first+1;}
   Location firstLocn      (ANCHOR a) const {return pFile->loc(first,a);}
   Location lastLocn       (ANCHOR a) const {return pFile->loc(last,a);}
   Location locn(Boundary b,ANCHOR a) const {return b == FIRST ? firstLocn(a) : lastLocn(a);}
   bool     isInFile (Location l)     const {return pFile->isInFile(l);}
};

dclVector(Segment)

//-------------------------------------------------------------------------------------

#endif
