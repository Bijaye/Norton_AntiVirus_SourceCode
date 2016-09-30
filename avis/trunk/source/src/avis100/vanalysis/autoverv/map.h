/*##################################################################*/
/*                                                                  */
/* Program name:  map.h                                             */
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

#ifndef MAP_H
#define MAP_H

#include "autoverv.h"
#include "location.h"
#include "encryption.h"

void emitMapElement (vector<char*>& MapTxt, 
                     ANCHOR anchor, long offset, long len,
                     bool isCode, ENCRYPTION encr, 
                     const byte* pTxt);

void emitPAMmapElement (vector<char*>& MapTxt, 
                        ANCHOR anchor, long offset, long len,
                        const char* zClassName, bool isCode, ENCRYPTION encr, 
                        const byte* pTxt);

#endif

