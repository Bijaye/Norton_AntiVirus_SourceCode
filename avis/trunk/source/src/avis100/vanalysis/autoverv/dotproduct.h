/*##################################################################*/
/*                                                                  */
/* Program name:  dotProduct.h                                      */
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

// now go through the (painful) exercise of deciding if certain 
// samples should be excluded from consideration

#include "sample.h"
#include "bigrams.h"

int DotProductExclusion (Sample_vec& Replicants, FILE* fExcludeLog, int count_limit = MAX_BIGRAM_COUNTS);
