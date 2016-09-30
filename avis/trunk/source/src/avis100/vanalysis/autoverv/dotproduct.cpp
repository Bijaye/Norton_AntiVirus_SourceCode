/*##################################################################*/
/*                                                                  */
/* Program name:  dotProduct.cpp                                    */
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

#include <math.h>
#include <stdio.h>

#include "dotProduct.h"
#include "autoverv.h"


_inline static double dot_product(int counts1[65536], int counts2[65536], double result11, double result22);


#define CLOSENESS_THRESHOLD 0.3


// returns the number of acceptable replicants remaining in Replicants.

int DotProductExclusion (Sample_vec& Replicants, FILE* hExcludeLog, int count_limit) {
   vector<double>   dp;
   vector<double>   dp_self;
   double           dp_cum = 0.0;
   int              r;                    // replicants index

   int num_replicants = Replicants.size();
   if (num_replicants <= 2) return num_replicants;

   ENCRYPTION EncrSet[] = {ADD1, XOR2};   // only do for ADD1 and XOR2

   for (int E = 0; E < arraySize(EncrSet); E++) { 
      ENCRYPTION e = EncrSet[E];    
      
      for (r = 0; r < num_replicants; r++) {
         File* pFile = &Replicants[r]->Inf;
         dp_self.push_back(0.0);          //create & initialize dp_self[r]
         
         index_bigrams (pFile, e);
         for (int bigram = 0; bigram < 65536L; bigram++) {
            if (pFile->bigram_counts[bigram] > count_limit) pFile->bigram_counts[bigram] = 0;
            dp_self[r] += (pFile->bigram_counts[bigram] * pFile->bigram_counts[bigram]);
         }
      }

      for (r = 0; r < num_replicants; r++) {
         dp.push_back (0.0);          //create & initialize dp[r]
         for (int o = 0; o < r; o++) {
            File* pFile = &Replicants[r]->Inf;
            File* pOtherFile = &Replicants[o]->Inf;
            double dotprod = dot_product (pFile->bigram_counts, pOtherFile->bigram_counts, dp_self[r], dp_self[o]);
            dp[r]  += dotprod;
            dp[o]  += dotprod;
            dp_cum += dotprod;
         }
      }

      /* clean up */
      for (r = 0; r < num_replicants; r++) {
         File* pFile = &Replicants[r]->Inf;
         for (int bigram = 0; bigram < 65536L; bigram++)
            if (pFile->position[bigram])
               free(pFile->position[bigram]);
      }
   }
   
   // now choose the acceptable replicants 
   Sample_vec accepted_replicants;
   
   /* .<- # of encryption types used in dot product calculations */
   dp_cum /= double (num_replicants * (num_replicants-1) / 2.0 * arraySize(EncrSet));
   // printf("dp_cum is %lf.\n",dp_cum);*/ 

   for (r = 0; r < num_replicants; r++) {
      
      /* .<- this 2 = # of encryption types used in dot product calculations */
      dp[r] /= (double) ((num_replicants-1) * arraySize(EncrSet));
      // printf("Average for sample %d is %lf.\n",j,dp[j]);*/ 

      if (dp[r] >= (CLOSENESS_THRESHOLD * dp_cum)) {
         accepted_replicants.push_back (Replicants[r]);
      }else {
         const char* zExcludedFile = Replicants[r]->Inf.filename;
         printf("WARN: Sample %s is strange -- excluding it.\n",zExcludedFile);

         // AJR: if left in place, unit testing will try to repair it
         fprintf (hExcludeLog, "%s\n", zExcludedFile);
         if (ferror(hExcludeLog)) {
            perror ("ERR: exclude log write");
            exit(18);
         }
         delete Replicants[r];
      }
   }
   Replicants = accepted_replicants;
   return Replicants.size();
}
 
//----------------------------------------------------------------------------------------------------------
 
_inline static double dot_product(int counts1[65536], int counts2[65536], double result11, double result22) {
   register int i;
   register int c1;
   register int c2;
   double result12;
   
   result12=counts1[0]*counts2[0];
   for(i=65536;--i;) {
     c1 = counts1[i];
     if (c1 == 0) continue;
     c2 = counts2[i];
     if (c2 == 0) continue;
     result12 += (double)(c1*c2);
   }
   
   return(result12 / sqrt(result11*result22));
}

