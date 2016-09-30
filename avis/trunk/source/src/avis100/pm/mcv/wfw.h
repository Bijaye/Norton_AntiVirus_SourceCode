#ifdef MACNEWAPI 
/* wfw.h */

#ifndef WFW_H
#define WFW_H

//#include <stdio.h>

#ifdef WS16
#define MAXMASTER 100
#define MAXSTRINGS 1500
#else
#define MAXMASTER 1005
#define MAXSTRINGS 1500
#endif

//extern int wfw_errno;

#ifdef NNN
struct str_ll_t {
	int	    index;
	char	*szName;
	struct str_ll_t *next;
};
#endif

/* physical image of a master macro table entry */
struct mtab_master_t {
   byte    ucId;
   byte    ucKey;
   word    wSNameIndex;
   word    wPNameIndex;
   word    wDescription;
   dword   unknown08;
   INT32   lLength;
   dword   unknown16;      // if 0 then entry is deleted
   INT32   lOffset;
};

struct ToolBar_t {	// header of the tool bar structure
//	word	usUnknown01;	// usually 0, 1 or 6
//	byte	ucUnknown02;	// usually 6
  word	usSubHeaderSize;	// size of the sub headers
  word	usButtonEntrySize;	// size of the buttons
  word	usUnknownEntrySize;	// size of the unknown entries
  word	nTables;		/* number of tables to follow
				   (excluding the string table) */
};

struct ToolBarSubHeader_t {
  INT16 siType;	/* type: 0, 1, 8(?), 10 => 20 byte entries 
		   -1 => 14 byte entries */
  word  usUnknown01,
    usUnknown02,
    usUnknown03,
    usUnknown04,
    usUnknown05,
    usUnknown06,
    usUnknown07,
    usUnknown08;
  word	nEntries;
};

struct ToolBarButton_t {
  INT16 siUnknown01,
    siUnknown02,
    siUnknown03,
    siStringType,
    siIcon,
    siUnknown04,
    siUnknown05;
};

struct MenuTab_t {
  INT16	siUnknown01,
    siUnknown02,
    siMenuStrIndex,
    siUnknown03,
    siUnknown04,
    siUnknown05;
};

typedef vector<char * > CStringVector;

typedef struct MTab_t {
  INT32    iMacroTablePos;
  INT32    iMacroTableLen;
  int      nMaster;
  struct mtab_master_t *Master[MAXMASTER];
  int      nSName;
  //struct str_ll_t	   *SecStrTab;
  vector<char * > vSName;
  int      nPName;
  char    *szPName[MAXSTRINGS];
  int      nUnknown02;
  char     sUnknown02[4][MAXSTRINGS];
  int      nUnknown03;
#ifdef MCV
  char     sUnknown03[0x0e][MAXSTRINGS];
  struct MenuTab_t	pMenuTab[MAXSTRINGS];
#endif
  int      nUnknown12;
  int      nMenuTab;
  int      nToolBarTables;
} MTAB;

class WordDoc {
  DOCFILE       *df;
  OLE2STREAM    *dfstream;
  byte          key[16];
  int           wfw_errno;

  bool          ParseMacroTable(byte *, int);
  bool          ParseMTabMasterTable(byte **p, byte *pEOTable, int nKillEntry);
  bool          ParseMTabUnknown02(byte **p, byte *pEOTable);
  bool          ParseMTabUnknown03(byte **p, byte *pEOTable);
  bool          ParseMTabUnknown12( byte **p, byte *pEOTable);
  bool          ParseMenuTab(byte **p, byte *pEOTable);
  bool          ParseMTabPrimaryStrTab(byte **p, byte *pEOTable);
  bool          ParseMTabSecondaryStrTab(byte **p, byte *pEOTable);
  bool          ParseToolBarTab(byte **p, byte *pEOTable);
  bool          DeleteMTabPName(int nEntry, int bPName);
  bool          RewriteMacroTable(void);
  void          free_MTab();
  void          InitMTab();
  inline void   FreeSName(void) { for(int i = 0; i < MTab.vSName.size(); i++) free(MTab.vSName[i]); };
  int           get_master_entry(char *);

public:
  MTAB  MTab;
        WordDoc(DOCFILE *);
        ~WordDoc();
  bool  Open(DFSID);
  bool  Close();
  bool  ReadOpen(char *, void **, int *, int, AVTAK);
  bool  ReadClose(void **, AVTAK);
  bool  Overwrite(char*, char*, dword, byte);
  bool  Delete(char *);
  bool  CanRewrite();
  bool  GetTemplateByte(byte *);
  bool  SetTemplateByte(byte);
  bool  Twiddle(char *);
  bool  CheckMTabEntry(int);
};

class WFWFindMacro {
  int            nThisMacro;
public:
                WFWFindMacro(WordDoc *w) {WordDoc = w; };
  WordDoc       *WordDoc;
  char          *szPName;
  char          *szSName;
  char          *szDescription;
  INT32         lLength;
  bool          FindFirst();
  bool          FindNext();
  bool          FindClose(); // a no-op at the moment
};

#define LINELENGTH 16
#define PRIMARYSECTORSIZE 512
#define PRIMARYSECTORSIZE_BITS 9
#define SECONDARYSECTORSIZE 64
#define SECONDARYSECTORSIZE_BITS 6
/*#define OLE2_ARBITRARY_HINUM 128*/

#endif
 

#else 
/* wfw.h */

#ifndef WFW_H
#define WFW_H

//#include <stdio.h>
#include "df.h"

#ifdef WS16
#define MAXMASTER 100
#define MAXSTRINGS 1500
#else
#define MAXMASTER 1005
#define MAXSTRINGS 1500
#endif

extern int wfw_errno;

struct str_ll_t {
	int	    index;
	char	*szName;
	struct str_ll_t *next;
};

/* physical image of a master macro table entry */
struct mtab_master_t {
   byte    ucId;
   byte    ucKey;
   word    wSNameIndex;
   word    wPNameIndex;
   word    wDescription;
   dword   unknown08;
   INT32   lLength;
   dword   unknown16;      // if 0 then entry is deleted
   INT32   lOffset;
};

struct ToolBar_t {	// header of the tool bar structure
//	word	usUnknown01;	// usually 0, 1 or 6
//	byte	ucUnknown02;	// usually 6
  word	usSubHeaderSize;	// size of the sub headers
  word	usButtonEntrySize;	// size of the buttons
  word	usUnknownEntrySize;	// size of the unknown entries
  word	nTables;		/* number of tables to follow
				   (excluding the string table) */
};

struct ToolBarSubHeader_t {
  INT16 siType;	/* type: 0, 1, 8(?), 10 => 20 byte entries 
		   -1 => 14 byte entries */
  word  usUnknown01,
    usUnknown02,
    usUnknown03,
    usUnknown04,
    usUnknown05,
    usUnknown06,
    usUnknown07,
    usUnknown08;
  word	nEntries;
};

struct ToolBarButton_t {
  INT16 siUnknown01,
    siUnknown02,
    siUnknown03,
    siStringType,
    siIcon,
    siUnknown04,
    siUnknown05;
};

struct MenuTab_t {
  INT16	siUnknown01,
    siUnknown02,
    siMenuStrIndex,
    siUnknown03,
    siUnknown04,
    siUnknown05;
};

typedef struct MTab_t {
  INT32    iMacroTablePos;
  INT32    iMacroTableLen;
  int      nMaster;
  struct mtab_master_t *Master[MAXMASTER];
  int      nSName;
  struct str_ll_t	   *SecStrTab;
  int      nPName;
  char    *szPName[MAXSTRINGS];
  int      nUnknown02;
  char     sUnknown02[4][MAXSTRINGS];
  int      nUnknown03;
#ifdef MCV
  char     sUnknown03[0x0e][MAXSTRINGS];
  struct MenuTab_t	pMenuTab[MAXSTRINGS];
#endif
  int      nUnknown12;
  int      nMenuTab;
  int      nToolBarTables;
} MTAB;

typedef struct WordDoc_t {
  DOCFILE   *dffile;
  OLE2STREAM *dfstream;
  MTAB        MTab;
  byte       key[16];
} WORDDOC;

struct ffmacro_t {
  int            nThisMacro;
  WORDDOC       *WordDoc;
  char          *szPName;
  char          *szSName;
  char          *szDescription;
  dword          ulLength;
};

WORDDOC *wfw_OpenDoc(DOCFILE *, char *);
int wfw_CloseDoc(WORDDOC *WordDoc);
int wfw_GetMacro(WORDDOC *, char *, char **, dword *, int);
int wfw_EnumFirstMacro(WORDDOC *, struct ffmacro_t *);
int wfw_EnumNextMacro(struct ffmacro_t *);
int wfw_OverwriteMacro(WORDDOC *, char*, char*, dword, byte);
int wfw_DeleteMacro(WORDDOC *,char *);
int wfw_CanRewrite(WORDDOC *);
int wfw_GetTemplateByte(WORDDOC *worddoc, byte *value);
int wfw_SetTemplateByte(WORDDOC *worddoc, byte value);
int wfw_TwiddleMacro(WORDDOC *worddoc, char *szName);


#ifdef __cplusplus
  #define INLINE inline
#else
  #define INLINE
#endif

#define LINELENGTH 16
#define PRIMARYSECTORSIZE 512
#define PRIMARYSECTORSIZE_BITS 9
#define SECONDARYSECTORSIZE 64
#define SECONDARYSECTORSIZE_BITS 6
/*#define OLE2_ARBITRARY_HINUM 128*/

#endif
 

#endif 
