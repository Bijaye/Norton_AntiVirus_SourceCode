/*##################################################################*/
/*                                                                  */
/* Program name:  lcs.h                                             */
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

#include "match.h"

// constants --------------------------------------------------------------------------

#define min_skip_length 1000      /* sets minimum match size for which we think we won't find a */
                                  /* larger region containing a subset of the given region. */
                                  /* This is used to speed the matching process by skipping match */
                                  /* attempts from all the offsets in regions skipped */


// macros -----------------------------------------------------------------------------

#define LCS(S,E) pLCS(S,E,NULL,0,NULL)

// functions --------------------------------------------------------------------------

/* finds longest common substring of valid bytes between given files */
struct Match* lcs(File_vec& vFile, ENCRYPTION e,
                  int suggested_start[], int lowerbound,char * approxworked);

// writes the LCS' as they are calculated, or reads those saved by a previous run, 
// depending on the global flag LCS_IOmode
struct Match* fileLCS(File_vec& vFile, ENCRYPTION e,
                      int suggested_start[], int lowerbound,char * approxworked);

// function pointer -------------------------------------------------------------------

typedef struct Match* (*LCS_PF)(File_vec& vFile, ENCRYPTION e,
                      int suggested_start[], int lowerbound,char * approxworked);


extern LCS_PF pLCS;
extern char* LCS_IOmode;
extern FILE* hLCS_IO;
