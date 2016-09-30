/*##################################################################*/
/*                                                                  */
/* Program name:  File.h                                            */
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

#ifndef FILE_H
#define FILE_H

#include "autoverv.h"
#include "encryption.h"
#include "location.h"


/* VERV file-type */
typedef enum {
   UNKNOWN_FILETYPE = 0,
   S_EXE,
   S_COM,
   DEV
} FILE_TYPE;

extern const char* const Filetype_txt[];


typedef enum {
   NOT_COM = 0,
   E8_COM,
   E9_COM,
   P_COM,
} COM_TYPE;

extern const char* const COMtype_txt[];


typedef enum {
   ANY_LENGTH = 0,
   SHORT,
   LONG
} FILELENGTH_TYPE;

extern const char* const Lengthtype_txt[];



//-------------------------------------------------------------------------------------

struct File {
  char  filename[_MAX_PATH+1];   /* filename of sample on disk */
  long  length;                  /* Length of sample in bytes */
  byte* Invar[ENCR_COUNT];       /* The file's bytes, under all invariants; Invar[0] is plaintext*/
  char* MatchMap;                /* maps the validity/match status of bytes */
  FILE_TYPE file_type;           /* VERV file-type - see above*/
  COM_TYPE  COM_type;            /* VERV file-type - see above*/
  uInt entry_pt;                 /* File entry point */
  char* CodeMap;                 // code/data map

  /* internal use fields */
  int bigram_counts[65536];      /* counts for each bigram */
  int *position[65536];          /* array of positions for each bigram */
  int max_match_len;             /* maximum match length for the current bigram */
  int max_match_pos;             /* position of that match */

  // methods
  explicit 
  File               (const char* zFileName, bool hexmode = false, bool codo = false);

 ~File               (void);

  byte*    pInvar       (ENCRYPTION Encr, long Offset)  const {return Invar[Encr] + Offset;}
  byte*    pInvar       (ENCRYPTION Encr, Location Loc) const {return pInvar(Encr, offset(Loc));}
  byte*    pPlaintext   (long Offset)  const {return pInvar(PLAIN,Offset);}
  byte*    pPlaintext   (Location Loc) const {return pInvar(PLAIN,Loc);}
  Keyval   KeyvalAt     (long Offset, ENCRYPTION Encr)  const;
  Keyval   KeyvalAt     (Location Loc, ENCRYPTION Encr) const {return KeyvalAt(offset(Loc),Encr);}
  Location loc          (long absOffset, ANCHOR anchor) const;
  long     offset       (Location loc) const;
  bool     isInFile     (Location loc) const;
  bool     isFieldStart (Location Loc) const;
  bool     isFieldStart (long Offset) const;
  bool     isFieldEnd   (Location Loc) const;
  bool     isFieldEnd   (long Offset) const;
  bool     isInMZheader (Location Loc) const;
  bool     isInMZheader (long Offset) const;

  bool     isCode (long  Offset) const {return CodeMap && CodeMap[Offset];}
  bool     isCode (Location Loc) const {return isCode(offset(Loc));}

  const Section* pSectionContainingOffset(long Offset, const Section_vec& Sections) const;
  const Section* pSectionContainingLocation(Location Locn, const Section_vec& Sections) const {
     return pSectionContainingOffset(offset(Locn),Sections);
  }

  const Section* LocnSectionIndex (Location Locn, const Section_vec& Sections) const;

private:
  bool load          (const char* zFileName, bool hexmode = false, bool codo = false);
  uInt setEntryPoint (void);
  long anchorOffset (ANCHOR anchor) const;

};



struct File_vec: public vector<File*> {
   Location alternativeAnchor (Location Locn, ANCHOR A) const;
};

dclVecIter(File);

//-------------------------------------------------------------------------------------

class MapType {
   public: virtual bool operator()(const File* pFile, long offset) const = 0;
};

class isUnmatched: public MapType {
   public: bool operator() (const File* pFile, long offset) const {return !pFile->MatchMap[offset];}
};

class isIsegment: public MapType {
   public: bool operator() (const File* pFile, long offset) const {return !pFile->MatchMap[offset] || pFile->MatchMap[offset] == I_MAP;}
};

//-------------------------------------------------------------------------------------

#endif
