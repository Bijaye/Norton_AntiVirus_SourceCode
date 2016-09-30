/*##################################################################*/
/*                                                                  */
/* Program name:  verification.cpp                                  */
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

#include "verification.h"
#include "polymath.h"



VerSec::VerSec (const Section* pIconstSec, long firstOffset, long lastOffset, bool CD, const char* zClassType):
   First       (pIconstSec->First + firstOffset),
   Last        (pIconstSec->First + lastOffset),
   AliasedKeys (pIconstSec->First),
   Encr        (pIconstSec->encr),
   Length      (lastOffset-firstOffset+1),
   AdjLen      (Length - Length % size(Encr)),
   isCode      (CD),
   pRefFile    (pIconstSec->Segments[0]->pFile),
   pRefText    (pRefFile->pPlaintext(First)),
   pRefInvar   (pRefFile->pInvar(Encr,First)), 
   zType       (zClassType)
{
   assert(firstOffset >= 0 && lastOffset >= 0 && lastOffset >= firstOffset);
   assert(pIconstSec->Segments[0]->first+lastOffset <= pIconstSec->Segments[0]->last);
}



bool VerSec::emitVerv (vector<PAMcode*>& PAMverv) const {
   PAMcode PAM; // PAM instructions

   if (!isCode) return false;
   //?? what's a reasonable minimum size? does CRC work for very short sections?
   if (AdjLen < 2 * size(Encr) || AdjLen < sizeof(polyn)) return false;

   PAM.setAction(PAM_VERIFY);
   PAM.setType(zType);
   PAM.setEncr(Encr);
   PAM.setKeyLoc(First.Anchor,First.Offset);    // aliased key: 1st. byte/word in section
   PAM.setStartLoc(First.Anchor,First.Offset);
   PAM.setEndLoc(Last.Anchor,Last.Offset-(Length-AdjLen)); // may be truncation to a length that is an integer multiple of the key length 


   // get the text
   byte* pText = new byte[AdjLen + sizeof(uLong)]; //?? workaround until decrypt fixed (takes src, dest void* and length)

   // (aliased) decrypt, if necessary
   if (Encr != PLAIN) {
      for (long o = 0; o < AdjLen; o += size(Encr)) {
         *(uLong*)(pText+o) = decrypt(Encr, pRefText+o, *(uLong*)pRefText);
      }
   } else {
      memcpy(pText, pRefText, AdjLen);
   }

   // verify the section
   long CRC = crc_calc(CRC_TABLE, 0L, pText, AdjLen);
   PAM.setCRC(CRC);
   PAMverv.push_back (new PAMcode(PAM));

   delete[] pText;
   return true;
}



void VerSec::emitMap (vector<char*>& MapTxt) const {
   char* line = NULL;  // A line of text to be output (dynamically allocated)

   if (AdjLen < 2 * size(Encr) || AdjLen < sizeof(polyn)) return; //?? what's a reasonable minimum size?

   line = new char[32];
   sprintf(line,"<MARKER> %s",AnchorName[First.Anchor]);
   MapTxt.push_back(line);

   line = new char[32];
   sprintf(line,"<OFFSET> %ld",First.Offset);
   MapTxt.push_back(line);

   line = new char[32];
   sprintf(line,"<LENGTH> %ld",AdjLen);
   MapTxt.push_back(line);

   line = new char[32];
   sprintf(line,"<CRYPT> %s",name(Encr));
   MapTxt.push_back(line);

   line = new char[32];
   sprintf(line,"<CODE> %s",isCode ? "YES" : "NO");
   MapTxt.push_back(line);

   line = new char[32 + 2*AdjLen];
   strcpy(line,"<BYTES> ");
   char* pHex = strrchr(line,'\0');
   for (int b = 0; b < AdjLen; b++) 
      sprintf(pHex + 2*b,"%02X",int(pRefText[b]));
   MapTxt.push_back(line);
}



bool VerSec::operator< (const VerSec& other) const {
   return First < other.First;
}


bool VerSec::shorterThan (const VerSec& other) const {
   return Length < other.Length;
}


bool VerSec::order (const VerSec* p1, const VerSec* p2) {
   if (*p1 < *p2) return true;
   if (*p2 < *p1) return false;
   return p1->shorterThan(*p2);
}


//-- PAM verification & repair instructions --------------------------------------------------------


PAM_TYPE PAMcode::convType (const char* TypeName) {
   if      (!strcmp(TypeName,"FILE"))   return PAM_FILE;
   else if (!strcmp(TypeName,"COM"))    return PAM_COM;
   else if (!strcmp(TypeName,"EXE"))    return PAM_EXE;
   else if (!strcmp(TypeName,"COMs"))    return PAM_SHORT_COM;
   else if (!strcmp(TypeName,"EXEs"))    return PAM_SHORT_EXE;
   else if (!strcmp(TypeName,"COMl"))    return PAM_LONG_COM;
   else if (!strcmp(TypeName,"EXEl"))    return PAM_LONG_EXE;
   else FailMsg ("PAMcode - invalid file type code");

   return PAM_FILE;   // to stop the compiler whining
}


PAM_ENCR PAMcode::convEncr (ENCRYPTION e) {
   switch (e) {
      case PLAIN:   return PAM_PLAIN;
      case ADD1:    return PAM_ADD1;
      case XOR1:    return PAM_XOR1;
      case ADD2:    return PAM_ADD2;
      case XOR2:    return PAM_XOR2;
      default:      FailMsg("PAMcode - invalid encryption code");
   }
   return PAM_PLAIN;   // to stop the compiler whining
}


PAM_ANCHOR PAMcode::convAnchor (ANCHOR a) {
   switch (a) {
      case START:   return PAM_BEGIN;
      case END:     return PAM_EOF;
      case ENTRY:   return PAM_ENTRY;
      default:      FailMsg("PAMcode - invalid anchor point");
   }
   return PAM_BEGIN;   // to stop the compiler whining
}


const char* PAMcode::actionConv (PAM_ACTION action) {
   switch (action) {
      case PAM_VERIFY: return "DO_VERIFY";
      case PAM_REPAIR: return "DO_REPAIR";
      case PAM_SETLEN: return "DO_SETLEN";
      default: FailMsg ("bad PAM opcode");
   }
   return "";   // to stop the compiler whining
}


const char* PAMcode::typeConv (PAM_TYPE type) {
   switch (type) {
      case PAM_COM:  return "COM";
      case PAM_EXE:  return "EXE";
      case PAM_SHORT_COM:  return "COMs";
      case PAM_SHORT_EXE:  return "EXEs";
      case PAM_LONG_COM:  return "COMl";
      case PAM_LONG_EXE:  return "EXEl";
      case PAM_FILE: return "FILE";
      default: FailMsg ("bad PAM type");
   }
   return "";   // to stop the compiler whining
}


const char* PAMcode::encrConv (PAM_ENCR encr) {
   switch(encr) {
      case PAM_PLAIN: return "PLAIN";
      case PAM_ADD1:  return "ADD1";
      case PAM_XOR1:  return "XOR1";
      case PAM_ADD2:  return "ADD2";
      case PAM_XOR2:  return "XOR2";
      default: FailMsg ("bad PAM encryption");
   }
   return "";   // to stop the compiler whining
}


const char* PAMcode::anchorConv (PAM_ANCHOR anchor) {
   switch(anchor) {
      case PAM_BEGIN: return "BEGIN";
      case PAM_EOF:  return "EOF";
      case PAM_ENTRY:  return "ENTRY";
      default: FailMsg ("bad PAM anchor");
   }
   return "";   // to stop the compiler whining
}



PAMcode::PAMcode (void): 
      action(PAM_NULL), type(PAM_FILE), encr(PAM_PLAIN),
      keyAnchor(PAM_BEGIN),  keyOffset(0),
      startAnchor(PAM_BEGIN),startOffset(0),
      endAnchor(PAM_BEGIN),  endOffset(0),
      destAnchor(PAM_BEGIN), destOffset(0),
      CRC(0)
      {}


void PAMcode::emit (FILE* pFile) const {
   if (!pFile) return;

   if (action == PAM_VERIFY) {
      fprintf(pFile,"Verify %s, %s, %s,%ld, %s,%ld, %s,%ld, %09lxh\n",
              typeConv(type),          encrConv(encr),
              anchorConv(keyAnchor),   keyOffset,
              anchorConv(startAnchor), startOffset,
              anchorConv(endAnchor),   endOffset,
              CRC);
   }else if (action == PAM_REPAIR) {
      fprintf(pFile,"Repair %s, %s, %s,%ld, %s,%ld, %s,%ld, %s,%ld\n",
              typeConv(type),          encrConv(encr),
              anchorConv(keyAnchor),   keyOffset,
              anchorConv(startAnchor), startOffset,
              anchorConv(endAnchor),   endOffset,
              anchorConv(destAnchor),  destOffset);
   }else if (action == PAM_SETLEN) {
      fprintf(pFile,"SetLength %s\n",typeConv(type));
   }else if (action == PAM_CRITLEN) {
      fprintf(pFile,"CriticalLength %s\n",typeConv(type));
   }//??else error
}


void PAMcode::emitBinary (FILE* pFile) const {   //??
   if (!pFile) return;

   fprintf(pFile,"%c",char(action));
   fprintf(pFile,"%c",char(type));
   fprintf(pFile,"%c",char(encr));
   fprintf(pFile,"%c",char(keyAnchor));
   fwrite (&keyOffset,sizeof(keyOffset),1,pFile);
   fprintf(pFile,"%c",char(startAnchor));
   fwrite (&startOffset,sizeof(startOffset),1,pFile);
   fprintf(pFile,"%c",char(endAnchor));
   fwrite (&endOffset,sizeof(endOffset),1,pFile);
   fprintf(pFile,"%c",char(destAnchor));
   if (action == PAM_VERIFY)
      fwrite (&CRC,sizeof(CRC),1,pFile);
   else
      fwrite (&destOffset,sizeof(destOffset),1,pFile);

   if ferror(pFile) FailMsg("PAM verv write");
}


string PAMcode::asString (void) const {
   string txt;

   txt += "op: ";        txt +=char(action);      txt +=char(type);
   if (action == PAM_SETLEN) return txt;

   txt += " encr: ";     txt +=char(encr);

   if (encr != PAM_PLAIN) {
      txt += " @ ";         txt +=char(keyAnchor);   txt += str("%7ld",keyOffset);
   }else {
      txt += "           ";
   }

   txt += " start ";     txt +=char(startAnchor); txt += str("%7ld",startOffset);
   txt += " end ";       txt +=char(endAnchor);   txt += str("%7ld",endOffset);

   if (action == PAM_REPAIR) {
      txt += " dest: ";     txt +=char(destAnchor);  txt += str("%7ld",destOffset);
   }else {
      txt += " CRC: ";      txt += str("%08lX",CRC);
   }

   return txt;
}
