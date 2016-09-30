/*##################################################################*/
/*                                                                  */
/* Program name:  test.h                                            */
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

#ifndef TEST_H
#define TEST_H

#include "fileclass.h"
#include "match.h"
#include "break.h"

void prtHex (const byte* bfr, size_t l);

void matchPrint (const Match* pMatch, char MatchType, long FileLen = 0L);
void matchSummaryPrint (const Match* pMatch, long FileLen = 0L);
// checks that a match is a common substring and it could not be extended
bool matchCheck (const Match* pMatch);

void prtHImatches (const FileClass& Class);
void prtIconstMatches (const FileClass& Class);
void prtHIsections (const FileClass& Class);
void prtHIsect (const HIsection* pS);
void prtIconstSections (const FileClass& Class);
void prtIvarSections (const FileClass& Class);
void prtIsections (const FileClass& Class);
void prtHsections (const FileClass& Class);
void prtActualKeys (const FileClass& Class);
void prtAliasedKeys (const FileClass& Class);
void prtPossRecons (const FileClass& Class, const char* zTitle);
void prtChosenRecons (const FileClass& Class);
void prtVerifications (const FileClass& Class);
void prtReconstruction (const FileClass& Class);
void prtClass (const FileClass& Class);

#endif
