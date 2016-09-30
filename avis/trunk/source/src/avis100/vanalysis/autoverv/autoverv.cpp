/*##################################################################*/
/*                                                                  */
/* Program name:  autoverv.cpp                                      */
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

#include "autoverv.h"

polyn* CRC_TABLE = NULL;

int MIN_REPLICANTS = 4;
int MIN_HI = 26;
int MIN_I  = 4;


bool isNull(void* p) {return p == NULL;}

string str (const char* fmt, long n) {
   string str;
   char bfr[1024];
   sprintf(bfr,fmt,n);
   str += bfr;
   return str;
}

string str (const char* fmt, uLong n) {
   string str;
   char bfr[1024];
   sprintf(bfr,fmt,n);
   str += bfr;
   return str;
}

char* strDup (const char* original) {
   const size_t l = strlen(original)+1;
   char* zDup = new char[l];
   if (zDup) memcpy(zDup,original,l);
   return zDup;
}


