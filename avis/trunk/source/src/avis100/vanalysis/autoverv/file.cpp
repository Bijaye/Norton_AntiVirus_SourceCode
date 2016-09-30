/*##################################################################*/
/*                                                                  */
/* Program name:  File.cpp                                          */
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

#include <memory.h>   // memset
#include <sys\stat.h>
#include <sys\types.h>
#include "assert.h"

#include "file.h"
#include "section.h"

const char* const Filetype_txt[]   = {"Unknown", "EXE",  "COM",  "DEV"};
const char* const COMtype_txt[]    = {"Not COM", "E8-COM", "E9-COM", "P-COM"};
const char* const Lengthtype_txt[] = {"", "s", "l"};

//==========================================================================================


static int x2i (char c) {
	if      (c >= '0' && c <= '9') return c - '0';
	else if (c >= 'a' && c <= 'f') return c - 'a' + 0x0A;
	else if (c >= 'A' && c <= 'F') return c - 'A' + 0x0A;
   assert(0);
	return 0;
}

 
static int read (byte* pDest, int length, FILE* hFile, bool hexmode) {
   if (!hexmode) return fread (pDest, 1, length, hFile);

   char* pBfr = new char[length*2];
   if (!pBfr) return 0;

   const uInt n = fread (pBfr, 1, length*2, hFile);

   for (int i = 0; i < n/2; i++)
      pDest[i] = x2i(pBfr[2*i]) * 0x10 + x2i(pBfr[2*i+1]);

   delete[] pBfr;
   return i;
}

//------------------------------------------------------------------------------------------

File::File (const char* zFileName, bool hexmode, bool codo) {
   memset(this,0,sizeof(*this));
   if (*zFileName) load(zFileName, hexmode, codo);
}



File::~File (void) {
   for (int e = 0; e < ENCR_COUNT; e++) {
      if (Invar[e]) delete[] (Invar[e] - PAD);
   }
   delete[] MatchMap;
   delete[] CodeMap;
}



long File::anchorOffset (ANCHOR anchor) const {
   switch(anchor) {
      case START: return 0;
      case END:   return length-1;
      case ENTRY: return entry_pt;
   }
   printf ("unrecognized anchor %d", anchor);
   FailMsg ("Invalid anchor code");
   return -1;   // never gets here
}



Location File::loc (long absOffset, ANCHOR anchor) const {
   if (absOffset < 0 || absOffset >= length) FailMsg("location out of range");
   return Location (anchor, absOffset - anchorOffset(anchor)); 
}



long File::offset (Location loc) const {
   long absOffset = loc.Offset + anchorOffset(loc.Anchor);
   assert(absOffset >= 0 && absOffset < length);   //?? use isInFile first, if this is a valid possible outcome
   if (absOffset < 0 || absOffset >= length) absOffset = -1;   //?? the alternative
   return absOffset; 
}



// can be used to check if the above assertion would fire
bool File::isInFile (Location loc) const {
   long absOffset = loc.Offset + anchorOffset(loc.Anchor);
   return absOffset >= 0 && absOffset < length;
}



Keyval File::KeyvalAt (long Offset, ENCRYPTION Encr)  const {
   Keyval key = 0;
   memcpy(&key,pPlaintext(Offset),keyLength(Encr));
   return key;
}



bool File::load (const char* zFileName, bool hexmode, bool codo) {
   FILE*        hFile = NULL;
   int          e;
   struct _stat FileStat;
   FILE* hCodo = NULL;
   char zCodeDataFileName [_MAX_PATH+1];
   char line[4096];
   char CodoDelimiters[] = " []*\t\n";   //?? what's * mean?
   long c;

   // open the file
   hFile = fopen (zFileName, "rb");
   if (!hFile) {
      puts (zFileName);
      if (errno != ENOENT) perror ("ERR: binary file open fail");
      else                 printf ("ERR: %s not found\n", zFileName); 
      return false;
   }

   // get its stats
   if (_stat(zFileName,&FileStat) == -1) {
      puts (zFileName);
      perror ("   binary file get stats fail");
      goto fail;
   }
   length = FileStat.st_size / (hexmode ? 2 : 1);

   if (hexmode && FileStat.st_size % 2) {
      printf ("%s\n   hex mode file has odd length: %lu\n",FileStat.st_size);
      goto fail;
   }
   if (length > MAXINT) {
      printf ("%s\n   too large: %lu\n",FileStat.st_size);
      goto fail;
   }
   fseek(hFile, 0, SEEK_END);
   if (ftell(hFile) != FileStat.st_size) {
      printf ("%s\n   size inconsistency: %lu %lu\n",zFileName,FileStat.st_size,ftell(hFile));
      goto fail;
   }
   fseek(hFile, 0, SEEK_SET);

   // allocate file content/invariants areas, with PAD bytes, all 0, on either side
   for (e = PLAIN; e < ENCR_COUNT; e++) {
      Invar[e] = new byte[length + 2*PAD];
      if (!Invar[e]) {
         puts(zFileName);
         puts("   unable to allocate bytes array");
         goto fail;
      }
      memset (Invar[e],0,PAD);
      Invar[e] += PAD;
      memset (Invar[e] + length,0,PAD);
   }

   MatchMap = new char[length];
   if (!MatchMap) {
      puts(zFileName);
      puts("   unable to allocate match map array");
      goto fail;
   }

   // read the file
   if (read (pPlaintext(0), length, hFile, hexmode) != length) {
      puts (zFileName);
      perror ("   binary file read fail");
      goto fail;
   }
   memset (MatchMap, '\0', length);

   // create the invariants
   for (e = PLAIN; e < ENCR_COUNT; e++) {
      memInvariant((ENCRYPTION)e,Invar[e],pPlaintext(0),length); //?? pPlaintext?
   }

   // fill in the file's info.
   setEntryPoint();
   strcpy(filename, zFileName);

   fclose (hFile);


   // now load code-data segregation

   CodeMap = new char[length];
   assert(CodeMap);
   for (c = 0; c < length; c++) CodeMap[c] = '\0';

   if (codo) {

      strcpy(zCodeDataFileName, zFileName);
      strcat(zCodeDataFileName, ".sections");
      hCodo = fopen(zCodeDataFileName, "r");

      if (hCodo) while (fgets (line, sizeof(line)-1, hCodo)) {
         long first, last;
         //?? if bad codo data, treat the whole file as code 
         //?? (or all as non-code?) 
         //?? will do recon. from code? how will match work if
         //?? the codo doesn't match up from file to file?
         const char* pFirst = strtok(line, CodoDelimiters);
         if (!pFirst || !strlen(pFirst)) {
            puts ("*** bad Codo start data");
            first = 0L;
            continue; // currently, ignore bad sections
         }else
            first = atol(pFirst);

         const char* pLast = strtok(NULL, CodoDelimiters);
         if (!pLast || !strlen(pLast)) {
            puts ("*** bad Codo end data");
            last = length-1;
            continue; // currently, ignore bad sections
         }else
            last = atol(pLast);

         if (first < 0L || first >= length) {
            puts ("*** Codo start data out of range");
            first = 0L;
            continue; // currently, ignore bad sections
         }
         if (last <= 0L || last  >= length) {
            puts ("*** Codo end data out of range");
            last  = length-1;
            continue; // currently, ignore bad sections
         }

         for (c = first; c <= last; c++) {
            CodeMap[c] = CODE_IN_THIS_FILE;
         }
      }

      if (hCodo) fclose(hCodo);
   }

   return true;

fail:
   fclose (hFile);
   return false;
}


//?? enumerate the file_type
uInt File::setEntryPoint (void) {
  if ((*pPlaintext(0)=='M' && *pPlaintext(1)=='Z')
  ||  (*pPlaintext(0)=='Z' && *pPlaintext(1)=='M'))    /* if an .EXE */
    {
       /* compute entry point from header */
       entry_pt = (((long)*pPlaintext(9))<<8) + (long)*pPlaintext(8);
       entry_pt = entry_pt<<4;
       entry_pt += (((long)*pPlaintext(0x16) + (((long)*pPlaintext(0x17))<<8))<<4);
       entry_pt += (((long)*pPlaintext(0x14) + (((long)*pPlaintext(0x15))<<8)));
       if(entry_pt > 0x00100000)
          entry_pt -= 0x00100000;
       file_type = S_EXE;
       COM_type  = NOT_COM;
    }
   else if (*pPlaintext(0)==0xE9)     /* if E9-COM */
     {
       /* get jump target */
       entry_pt  = 3 + *pPlaintext(1) + (((uInt)*pPlaintext(2)) << 8);
       file_type = S_COM;
       COM_type  = E9_COM;
     }
   else if (*pPlaintext(0)==0xE8)     /* if E8-COM */
     {
       /* get jump target */
       entry_pt  = 3 + *pPlaintext(1) + (((uInt)*pPlaintext(2)) << 8);
       file_type = S_COM;
       COM_type  = E8_COM;
     }
   else if (*pPlaintext(0)==0xFF && *pPlaintext(1)==0xFF)    /* if device driver */
     {
       /* compute entry point */
       entry_pt  = *pPlaintext(6) + (((uInt)*pPlaintext(7) << 8));
       file_type = DEV;
       COM_type  = NOT_COM;
     }
   else     /* otherwise, it's a P-COM or ? */
     {
       entry_pt  = 0;   /* the entry point is at the beginning */
       file_type = S_COM;
       COM_type  = P_COM;
     }
  return entry_pt;
}



bool File::isFieldStart (Location Loc) const {
   return isFieldStart(offset(Loc));
}


// determines whether the given offset is the first byte in a field
// NB: Whenever the field length is 1 byte, every byte is the first and only byte of its field
bool File::isFieldStart (long Offset) const {
//?? sometimes interferes with a good result
//!!   if (file_type == S_COM) {
//      return !(Offset == 2 && (COM_type == E8_COM || COM_type == E9_COM));   // is it the 2nd byte of an initial jump?
//   }

   if (file_type != S_EXE) return true;

   if (isInMZheader(Offset)) return Offset % 2 == 0;   //?? 4-byte fields, eg length (2-5)

   //?? PE format, etc.? only if/when we have PE goats

   // find the relocation table
   long RTstart = 0, RTend = 0;
   const word relocation_table_offset = *(word*)pPlaintext(0x18);
   const word relocation_table_length = *(word*)pPlaintext(0x06);
   if (relocation_table_length) {
      RTstart = relocation_table_offset;
      RTend   = relocation_table_offset + relocation_table_length * 4 - 1;
   }

   // is Offset in the relocation table?
   if (Offset >= RTstart && Offset <= RTend) {
      const int offset_in_table = Offset - RTstart;
      return offset_in_table % 4 == 0;
   }
   return true;
}



bool File::isFieldEnd (Location Loc) const {
   return isFieldEnd(offset(Loc));
}



bool File::isFieldEnd (long offset) const {
   return offset+1 == length || isFieldStart(offset+1);
}



bool File::isInMZheader (Location Loc) const {
   return isInMZheader(offset(Loc));
}



bool File::isInMZheader (long Offset) const {
   if (file_type != S_EXE) return false;   // only EXE files have MZ hdr
   return (Offset < 0x1C);
}


const Section* File::pSectionContainingOffset(long Offset, const Section_vec& Sections) const {
   for (int s = 0; s < Sections.size(); s++)
      if (Offset >= offset(Sections[s]->First) && Offset <= offset(Sections[s]->Last))
         return Sections[s];

   return NULL;
}


//-----------------------------------------------------------------------------

Location File_vec::alternativeAnchor (Location Locn, ANCHOR A) const {
   if (!size()) return Location::null;
   if (Locn.Anchor == A) return Locn;

   long minOffset = LONG_MAX, maxOffset = 0L;

   for (const_File_vec_iter f = begin(); f != end(); f++) {
      const long absOffset = (*f)->offset(Locn);
      const long newOffset = (*f)->loc(absOffset,A).Offset;
      minOffset = min(minOffset,newOffset);
      maxOffset = max(maxOffset,newOffset);
   }
   return Location(A,minOffset,maxOffset-minOffset);
}
