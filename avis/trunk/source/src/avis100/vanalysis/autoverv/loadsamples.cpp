/*##################################################################*/
/*                                                                  */
/* Program name:  loadsamples.cpp                                   */
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

#include <stdio.h>
#include <string.h>
#include "assert.h"

#include "loadsamples.h"

// The first version of this function is for use when the list file
// contains lines of the format <infected-path,goat-path>,
// which is the current usage. The second version is a deprecated
// version as use in earlier tests, in which the list file gives the
// name of infected files only, and the goats are all in a given
// subdirectory, with the same name as the infected file.

// In both cases, the code-data segregation (if used) is assumed 
// to be in the same directory as the infected file, with the
// infected file's name suffixed with '.sections'.


// On completion, Replicants contains only infected files for which
// we have goats, and Samples contains only infected files without
// goats (i.e. original samples). Returned is the no. of replicants.

int loadSampleList (const char* zListName,
                    Sample_vec& Replicants, File_vec& Samples,
                    bool hexMode, bool useCodeDataSeg)
{
   FILE* hList;
   Sample* pReplicant;
   char  LineBuffer [_MAX_PATH*2 + 64];
   char* zInfName;
   char* zHostName;
   int   nSamples = 0;
   int   nOriginalSamples = 0;
   int   err = 0;

   hList = fopen(zListName, "rt");
   if (!hList) {
      fprintf (stderr,"%s",zListName),perror ("   list file open");
      return -1;
   }

   while (fgets(LineBuffer, sizeof(LineBuffer), hList)) {
      nSamples++;
      zInfName = strtok(LineBuffer,",*\n");
      zHostName = strtok(NULL,"\n");

      if (!(zInfName && strlen(zInfName))) {
         fprintf (stderr,"%s is incorrectly formatted at line %d\n",zListName,nSamples);
         fclose(hList); return -1;
      }

      if (!zHostName) zHostName = "";

      pReplicant = new Sample(zHostName,zInfName,hexMode,useCodeDataSeg);

      if (!pReplicant || !pReplicant->Inf.length) {
         delete pReplicant;
         fclose(hList); return -1;
      }
      if (pReplicant->Host.length) {
         Replicants.push_back (pReplicant);
      }else {   // an original sample without corresponding goat
         delete pReplicant;
         Samples.push_back(new File(zInfName, hexMode, useCodeDataSeg));
         if (nOriginalSamples++) {
            fprintf (stderr,"%s has extra original sample at line %d\n",zListName,nSamples);
            fclose(hList); return -1;
         }
      }
   }
   assert (nOriginalSamples <= 1);

   fclose (hList);
   return Replicants.size();
}

// In the list file, lines beginning with '*' are either comments or
// are cammands (of which there is currently only one; '*HEXDUMP' turns
// on the hex file mode). Otherwise, each line in the list file is the
// path to an infected file. The corresponding goat is the file with the 
// same name, but in the host directory.


int loadSampleList (const char* zListName, const char* zHostDir,
                    Sample_vec& Replicants, File_vec& Samples,
                    bool hexMode, bool useCodeDataSeg)
{
   FILE* hList;
   Sample* pReplicant;
   char zFileName [_MAX_PATH+1];
   char* pInfFileName;
   char zHostName [_MAX_PATH+1];
   char* pHostFileName;

   // set up host name buffer, with pHostFileName pointing to end of path
   strcpy(zHostName, zHostDir);
   pHostFileName = strrchr(zHostName, '\0');
   assert(pHostFileName);
   *(pHostFileName++) = DIR_DELIMITER;

   hList = fopen(zListName, "rt");
   if (!hList) {
      puts (zListName);
      perror ("   list file open");
      return -1;
   }

   while (fgets(zFileName, sizeof(zFileName), hList)) {
      if (char* p = strrchr(zFileName,'\n')) *p = '\0';
      while(zFileName[strlen(zFileName)-1] == ' ') zFileName[strlen(zFileName)-1] = '\0';

      if (zFileName[0] == '*') {   // it's a comment or command
         if (strcmp(zFileName+1, "HEXDUMP") == 0) hexMode = true;
         continue;
      }

      // append the infected file's name to the host directory path
      pInfFileName = (char * )max((unsigned long )max((unsigned long )strrchr(zFileName,DIR_DELIMITER)+1,(unsigned long )strrchr(zFileName,DRV_DELIMITER)+1),(unsigned long )zFileName);
      assert(pHostFileName + strlen(pInfFileName) < zHostName + sizeof(zHostName));
      strcpy(pHostFileName, pInfFileName);

      pReplicant = new Sample (zHostName, zFileName, hexMode, useCodeDataSeg);
      if (!pReplicant || !pReplicant->Inf.length) {
         delete pReplicant;
         fclose (hList);
         return -1;
      }
      if (pReplicant->Host.length) {
         Replicants.push_back (pReplicant);
      }else {   // an original sample without corresponding goat
         delete pReplicant;
         Samples.push_back(new File(zFileName, hexMode, useCodeDataSeg));
      }
   }

   fclose (hList);
   return Replicants.size();
}
