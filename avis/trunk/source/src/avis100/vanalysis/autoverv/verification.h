/*##################################################################*/
/*                                                                  */
/* Program name:  verification.h                                    */
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

#ifndef VERIFICATION_H
#define VERIFICATION_H

#include "section.h"

struct PAMcode;



struct VerSec {
   Location         First;
   Location         Last;
   Location         AliasedKeys;
   ENCRYPTION       Encr;
   long             Length;
   long             AdjLen;   // length adjusted down to an integer multiple of encryption cell size
   bool             isCode;
   const File*      pRefFile;
   const byte*      pRefText;
   const byte*      pRefInvar;
   const char*      zType;

   VerSec (const Section* pIconstSec, long firstOffset, long lastOffset, bool CD, const char* zClassType);

   bool operator<   (const VerSec& other) const; 
   bool shorterThan (const VerSec& other) const;
   
   bool emitVerv (vector<PAMcode*>& PAMverv) const;

   // NB: can either emit the map from the verification list or the Iconst matches. Doing the 
   // latter means that we may be able to detect the virus even if we were unable to find fixed
   // locations for verification. However, it does mean that the <MARKER>/<OFFSET> values are
   // specific to a single sample, as they will not be based on sample-independent section info.
   void emitMap (vector<char*>& MapTxt) const;

   static bool order (const VerSec* p1, const VerSec* p2);
};

dclVec(VerSec);


//-- PAM verification & repair instructions --------------------------------------------------------

enum PAM_TYPE {
   PAM_SHORT_COM = 'c',
   PAM_SHORT_EXE = 'x',
   PAM_LONG_COM  = 'B',   
   PAM_LONG_EXE  = 'E',
   PAM_COM       = 'C',  // when no length dependency, critical length = 0, so ALL eq LONG
   PAM_EXE       = 'X',
   PAM_FILE      = 'F'   // either type, com or exe
};

enum PAM_ACTION {
   PAM_NULL       =  0,
   PAM_VERIFY     = 'V',
   PAM_REPAIR     = 'R',
   PAM_SETLEN     = 'L',
   PAM_CRITLEN    = 'C'
};

enum PAM_ANCHOR {
   PAM_BEGIN = 'B',
   PAM_ENTRY = 'T',
   PAM_EOF   = 'E'
};

enum PAM_ENCR   {
   PAM_PLAIN = 'P',
   PAM_ADD1  = 'a',
   PAM_XOR1  = 'x',
   PAM_ADD2  = 'A',
   PAM_XOR2  = 'X'
};


      
struct PAMcode {
   private:
      PAM_ACTION action;
      PAM_TYPE   type;
      PAM_ENCR   encr;
      PAM_ANCHOR keyAnchor;
      long       keyOffset;
      PAM_ANCHOR startAnchor;
      long       startOffset;
      PAM_ANCHOR endAnchor;
      long       endOffset;
      PAM_ANCHOR destAnchor;      //?? not needed if repair is always done by successive appending
      long       destOffset;      //?? ditto
      uLong      CRC;

      static PAM_TYPE   convType (const char* TypeName);
      static PAM_ENCR   convEncr (ENCRYPTION e);
      static PAM_ANCHOR convAnchor (ANCHOR a);

      static const char* actionConv (PAM_ACTION action);
      static const char* typeConv (PAM_TYPE type);
      static const char* encrConv (PAM_ENCR encr);
      static const char* anchorConv (PAM_ANCHOR anchor);

   public:

      PAMcode (void);

      void setAction   (PAM_ACTION a)         {action = a;}
      void setType     (const char* TypeName) {type = convType(TypeName);}
      void setEncr     (ENCRYPTION e)         {encr = convEncr(e);}
      void setKeyLoc   (ANCHOR a, long o)     {keyAnchor = convAnchor(a);   keyOffset = o;}
      void setStartLoc (ANCHOR a, long o)     {startAnchor = convAnchor(a); startOffset = o;}
      void setEndLoc   (ANCHOR a, long o)     {endAnchor = convAnchor(a);   endOffset = o;}
      void setDestLoc  (ANCHOR a, long o)     {destAnchor = convAnchor(a);  destOffset = o; assert (action == PAM_REPAIR);}
      void setCRC      (uLong crc)            {CRC = crc; assert (action == PAM_VERIFY);}

      bool isFile (void) const {return type == PAM_FILE;}

      void emit (FILE* pFile) const;
      void emitBinary (FILE* pFile) const;   //??

      string asString (void) const;
};

#endif

