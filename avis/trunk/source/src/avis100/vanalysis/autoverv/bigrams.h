/*##################################################################*/
/*                                                                  */
/* Program name:  bigrams.h                                         */
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

#include "encryption.h"
#include "File.h"

#define MAX_BIGRAM_COUNTS 25      /* Heuristic parameter used to decide when
                                     enough bigrams are enough */

//Fred
// WARNING First type of bigram is little endian on Intel machines, while second type is always big endian
#ifdef CASTSHORTBIGRAM
#   define BIGRAM(array,i) ((int) *((unsigned short *) &array[i]))
#else
#   define BIGRAM(array,i) ((int) (array[i]*256+array[i+1]))
    // The following does not work! I don't know why...   precedence of + over << - AJR 
    //#define BIGRAM(array,i) ((int) (((int)array[i])<<8+((int)array[i+1])))
#endif


/* creates an index of bigrams in file -- used for matching */

void index_bigrams(File* pFile, ENCRYPTION e, int maxCount = MAX_BIGRAM_COUNTS);

