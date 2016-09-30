/*##################################################################*/
/*                                                                  */
/* Program name:  map.cpp                                           */
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

#include "map.h"
#include "assert.h"

void emitMapElement (vector<char*>& MapTxt, 
                     ANCHOR anchor, long offset, long len,
                     bool isCode, ENCRYPTION encr, 
                     const byte* pTxt)
{
   assert(len > 0); //?? >=?
   if (len == 0) return;

   char* line = new char[32];
   sprintf(line,"<MARKER> %s",AnchorName[anchor]);
   MapTxt.push_back(line);

   line = new char[32];
   sprintf(line,"<OFFSET> %ld",offset);
   MapTxt.push_back(line);

   line = new char[32];
   sprintf(line,"<LENGTH> %ld",len);
   MapTxt.push_back(line);

   line = new char[32];
   sprintf(line,"<CRYPT> %s",name(encr));
   MapTxt.push_back(line);

   line = new char[32];
   sprintf(line,"<CODE> %s",isCode ? "YES" : "NO");
   MapTxt.push_back(line);

   line = new char[32 + 2*len];
   strcpy(line,"<BYTES> ");
   char* pHex = strrchr(line,'\0');
   for (int b = 0; b < len; b++) 
      sprintf(pHex + 2*b,"%02X",int(pTxt[offset+b]));
   MapTxt.push_back(line);
}


void emitPAMmapElement (vector<char*>& MapTxt, 
                        ANCHOR anchor, long offset, long len,
                        const char* zClassName, bool isCode, ENCRYPTION encr, 
                        const byte* pTxt)
{
   long nInstr = 0;
   char ops[256];
   memset (ops, '1', 256);

   assert(len > 0); //?? >=?
   if (len == 0) return;

   char* line = new char[512];

   for (int b = 0; b < len; b++) 
      sprintf(line + 2*b,"%02X",int(pTxt[offset+b]));

   for (b = 2*b; b < 50; b++) 
      line[b] = ' ';

   sprintf (line+b, "%s %s %s %s %ld %ld %s %ld", 
      zClassName, name(encr), isCode ? "CODE   " : "NOTCODE", AnchorName[anchor], offset, len, ops, nInstr);
   
   MapTxt.push_back(line);
}
