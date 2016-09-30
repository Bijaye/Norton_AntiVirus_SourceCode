/*##################################################################*/
/*                                                                  */
/* Program name:  loadsamples.h                                     */
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

#ifndef LOADSAMPLES_H
#define LOADSAMPLES_H

#include "sample.h"

// The first version of this function is for use when the list file
// contains lines of the format <infected-path,goat-path>,
// which is the current usage. The second version is a deprecated
// version as use in earlier tests, in which the list file gives the
// name of infected files only, and the goats are all in a given
// subdirectory, with the same name as the infected file.

// In both cases, the code-data segregation (if used) is assumed 
// to be in the same directory as the infected file, with the
// infected file's name suffixed with '.sections'.


// NB  Currently, comma-delimited lists are used ('inherited' from 
//     samples.lst), but a comma is a valid file name character.
//     '*' would be a better choice, as it is not a valid filename 
//     character and is not likely to be so given its use as a wild
//     card character. This function accepts either; the use of a 
//     comma should be removed if we were ever to switch to something
//     else.

int loadSampleList (const char* zListName,
                    Sample_vec& Replicants, File_vec& Samples,
                    bool hexMode = false, bool useCodeDataSeg = false);

int loadSampleList (const char* zListName, const char* zHostDir, 
                    Sample_vec& Replicants, File_vec& Samples,
                    bool hexMode = false, bool useCodeDataSeg = false);

#endif
