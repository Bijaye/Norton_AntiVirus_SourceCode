////////////////////////////////////
// DocProp.c
// hacked from Morton's code
// objective: retrieve some properties from documents: type, character set, ...
////////////////////////////////////

#include <assert.h>
#include <string.h>

#include "common.h"
#include "df.h"
#include "mac.h"
#include "wfw.h"
#include "ll.h"
#include "DocProp.h"

#ifdef XXX
#define FERROR(no) df_tracef("%s (%i): Fatal Error #%i\n", __FILE__, __LINE__, no);
#else
#define FERROR(no)
#endif

#ifdef TRACING
#define TRACEERROR(s) {df_tracef("%s (%i): Fatal Error '%s'\n", __FILE__, __LINE__, (s)); wfw_errno = -1;}
#else
#define TRACEERROR(s)
#endif




int wfw_errno;



//number of ole2 streams of the given type in the file
int CountDocNumForType(char *szFileName, char *szType, int* docnum)
{
  int  h;
  MAC_ENUM_T *theMacro;

  *docnum = 0;
  theMacro = malloc(sizeof(MAC_ENUM_T));
  if (theMacro != NULL) {
    /* use mac_* interface instead */
//    TRACEF0(1, "mac_open(szFileName, 'WORD7')\n");
    h = mac_open(szFileName, szType);
//    DPrintInt(1, df_errno);
    if (h > -1) {
		do {
          ++*docnum;
		} while ((*docnum<100) && mac_next(h)); /* enddo */
//        TRACEF0(1, "mac_close(h)\n");
        mac_close(h);
//        DPrintInt(1, df_errno);
        free(theMacro);
        return TRUE;
    } else {
      free(theMacro);
      return FALSE;
    } /* endif */
  } else {
    printf("%s (%i): Memory allocation error\n", __FILE__, __LINE__);
	return FALSE;
  } /* endif */
}


//number of ole2 streams in the file
int CountDocNum(char *szFileName)
{
  int wfwnum, xlnum, vba5num;

  CountDocNumForType(szFileName, "WORD7", &wfwnum);
  CountDocNumForType(szFileName, "EXCEL7", &xlnum);
  CountDocNumForType(szFileName, "VBA5", &vba5num);
  return wfwnum +xlnum +vba5num;
}


int IsWfw(char *szFileName)
{
struct df_t *olefile;
int rc;
struct WordDoc_t *worddoc;

  olefile = df_OpenFile(szFileName);
  if (NULL == olefile) {
    printf("Error %d ole-opening %s.\n",df_errno,szFileName);
    return 0;
  }

  worddoc = wfw_OpenDoc(olefile,"WordDocument");
  if (worddoc == NULL) {
    rc = 0;
  }
  else {
    rc = 1;
    wfw_CloseDoc(worddoc);
  }
  df_CloseFile(olefile);
  return rc;
}


int IsVBA5(char *szFileName)
{
struct df_t *olefile;
int rc;
SFF    *Sff;
STRPAT pat;

	olefile = df_OpenFile(szFileName);
	if (NULL == olefile) {
		printf("Error %d ole-opening %s.\n",df_errno,szFileName);
		return 0;
	}

	/* just find the first relevant stream */
	strcpy(pat.str, "\\VBA\\");
	pat.type = STRPAT_RIGHTJUST;
	Sff = df_StreamFindFirst(olefile, &pat);
	if (Sff == NULL)
		rc = 0;
	else
		rc = 1;
	df_StreamFindClose(Sff);

	df_CloseFile(olefile);
	return rc;
}


int IsXl(char *szFileName)
{
struct df_t *olefile;
int rc;
SFF    *Sff;
STRPAT pat;

	olefile = df_OpenFile(szFileName);
	if (NULL == olefile) {
		printf("Error %d ole-opening %s.\n", df_errno, szFileName);
		return 0;
	}

	/* just find the first relevant stream */
	strcpy(pat.str, "\\Book");
	pat.type = STRPAT_RIGHTJUST;
	Sff = df_StreamFindFirst(olefile, &pat);
	if (Sff == NULL)
		rc = 0;
	else
		rc = 1;
	df_StreamFindClose(Sff);

	df_CloseFile(olefile);
	return rc;
}


int GetWfwCs(char *szFileName)
{
struct df_t *olefile;
struct WordDoc_t *worddoc;
int cs = 0;
byte *buf;

  olefile = df_OpenFile(szFileName);
  if (NULL == olefile) {
    printf("Error %d ole-opening %s.\n",df_errno,szFileName);
    return cs;
  }

  worddoc = wfw_OpenDoc(olefile,"WordDocument");
  if (worddoc == NULL) {
    return cs;
  }
  else {
	/* initialize pointers */
	buf = NULL;
	wfw_errno = 0;

	/* Create a general purpose buffer */
	buf = (byte*)malloc(512);
	if(buf == NULL) {
		goto error2;
	}
	/* Load the header of the WordDocument stream */
	if(!df_SeekStream(worddoc->dfstream, 0L)) {
		goto error3;
	}
	if(!df_ReadStream(worddoc->dfstream, (char*)buf, 512)) {
		goto error3;
	}
	/* Check for standard Word 2.x-7.x (SBCS and DBCS) and for 8.0
	 documents that we can deal with. */
	if (*(word*)buf == 0xa5dc) cs = 1; /* standard Word */
	else if (*(word*)buf == 0xa5ec) cs = 0; /* Word 8.0 */
	else if ((*(word*)buf == 0xa697) /* DBCS Word6, Japan */
	 || (*(word*)buf == 0xa698) /* DBCS Word6, Korea */
	 || (*(word*)buf == 0xa699) /* DBCS Word6-7 Taiwan, Word6 China? */
	 || (*(word*)buf == 0x8097) /* DBCS Word7, Japan */
	 || (*(word*)buf == 0x8098) /* DBCS Word7, Korea */
	 || (*(word*)buf == 0x8099) /* DBCS Word7, China */ )
	 cs = 2;
	else cs = 0;

error3:
	free(buf);
error2:
	buf = NULL;
//	df_CloseStream(dfstream);

	wfw_CloseDoc(worddoc);
	}
	df_CloseFile(olefile);
	return cs;
}



int GetWfwDBCSCountryCode(char *szFileName)
{
struct df_t *olefile;
struct WordDoc_t *worddoc;
int cs = 0;
byte *buf;

  olefile = df_OpenFile(szFileName);
  if (NULL == olefile) {
    printf("Error %d ole-opening %s.\n",df_errno,szFileName);
    return cs;
  }

  worddoc = wfw_OpenDoc(olefile,"WordDocument");
  if (worddoc == NULL) {
    return cs;
  }
  else {
	/* initialize pointers */
	buf = NULL;
	wfw_errno = 0;

	/* Create a general purpose buffer */
	buf = (byte*)malloc(512);
	if(buf == NULL) {
		goto error2;
	}
	/* Load the header of the WordDocument stream */
	if(!df_SeekStream(worddoc->dfstream, 0L)) {
		goto error3;
	}
	if(!df_ReadStream(worddoc->dfstream, (char*)buf, 512)) {
		goto error3;
	}
	/* Check for standard Word 2.x-7.x (SBCS and DBCS) and for 8.0
	 documents that we can deal with. */
	if (*(word*)buf == 0xa697) cs = JP; /* DBCS Word6, Japan */
	else if (*(word*)buf == 0xa698) cs = KR; /* DBCS Word6, Korea */
	else if (*(word*)buf == 0xa699) cs = TW; /* DBCS Word6-7 Taiwan, Word6 China? */
	else if (*(word*)buf == 0x8097) cs = JP; /* DBCS Word7, Japan */
	else if (*(word*)buf == 0x8098) cs = KR; /* DBCS Word7, Korea */
	else if (*(word*)buf == 0x8099) cs = CH; /* DBCS Word7, China */
	else cs = 0;

error3:
	free(buf);
error2:
	buf = NULL;
//	df_CloseStream(dfstream);

	wfw_CloseDoc(worddoc);
	}
	df_CloseFile(olefile);
	return cs;
}



int GetWordDocVersion(char *szFileName)
{
struct df_t *olefile;
//struct WordDoc_t *worddoc;
int version = 0;
byte *buf;
SFF* Sff;
STRPAT pat;
OLE2STREAM *dfstream;

  olefile = df_OpenFile(szFileName);
  if (NULL == olefile) {
    printf("Error %d ole-opening %s.\n",df_errno,szFileName);
    return version;
  }

  strcpy(pat.str, "WordDocument");
  pat.type = STRPAT_RIGHTJUST;

  Sff = df_StreamFindFirst(olefile, &pat);

  if (Sff == NULL)
    return version;
  else {
    dfstream = df_OpenStream(olefile, pat.str);
	if (dfstream ==NULL) return 0;

	/* initialize pointers */
	buf = NULL;
	wfw_errno = 0;

	/* Create a general purpose buffer */
	buf = (byte*)malloc(512);
	if(buf == NULL)
		goto error2;

	/* Load the header of the WordDocument stream */
	if(!df_SeekStream(dfstream, 0L))
		goto error3;

	if(!df_ReadStream(dfstream, (char*)buf, 512))
		goto error3;

	/* Check for standard Word 2.x-7.x (SBCS and DBCS) and for 8.0
	 documents that we can deal with. */
	if (*(word*)buf == 0xa5dc) version = 7; /* standard Word */
	else if (*(word*)buf == 0xa5ec) version = 8; /* Word 8.0 */
	else if ((*(word*)buf == 0xa697) /* DBCS Word6, Japan */
	 || (*(word*)buf == 0xa698) /* DBCS Word6, Korea */ ) version = 6;
	else if ((*(word*)buf == 0xa699) /* DBCS Word6-7 Taiwan, Word6 China? */
	 || (*(word*)buf == 0x8097) /* DBCS Word7, Japan */
	 || (*(word*)buf == 0x8098) /* DBCS Word7, Korea */
	 || (*(word*)buf == 0x8099) /* DBCS Word7, China */ ) version = 7;
	else version = 0;

error3:
	free(buf);
error2:
	buf = NULL;
//	df_CloseStream(dfstream);

	//wfw_CloseDoc(worddoc);

	df_CloseStream(dfstream);
	df_StreamFindClose(Sff);
	}
	df_CloseFile(olefile);
	return version;
}

int GetXlDocVersion(char *szFileName)
{
struct df_t *olefile;
int version = 0;
SFF* Sff;
STRPAT pat;

  olefile = df_OpenFile(szFileName);
  if (NULL == olefile) {
    printf("Error %d ole-opening %s.\n",df_errno,szFileName);
    return version;
  }

  strcpy(pat.str, "Book");
  pat.type = STRPAT_RIGHTJUST;
  Sff = df_StreamFindFirst(olefile, &pat);
  if (Sff != NULL) {
	df_StreamFindClose(Sff);
    version = 7;
  }
  else {
    strcpy(pat.str, "Workbook");
    pat.type = STRPAT_RIGHTJUST;
    Sff = df_StreamFindFirst(olefile, &pat);
    if (Sff != NULL) {
	  df_StreamFindClose(Sff);
      version = 8;
	}
  }
  df_CloseFile(olefile);
  return version;
}


int IsXlDoc(char *szFileName)
{
struct df_t *olefile;
int rc = 0;
SFF* Sff;
STRPAT pat;

  olefile = df_OpenFile(szFileName);
  if (NULL == olefile) {
    printf("Error %d ole-opening %s.\n",df_errno,szFileName);
    return rc;
  }

  pat.type = STRPAT_RIGHTJUST;

  strcpy(pat.str, "Book");
  Sff = df_StreamFindFirst(olefile, &pat);
  if (Sff != NULL) {
	df_StreamFindClose(Sff);
    rc = 1;
  }
  else {
    strcpy(pat.str, "Workbook");
    Sff = df_StreamFindFirst(olefile, &pat);
    if (Sff != NULL) {
	  df_StreamFindClose(Sff);
      rc = 1;
	}
  }
  df_CloseFile(olefile);
  return rc;
}


int IsWordDoc(char *szFileName)
{
struct df_t *olefile;
int rc = 0;
SFF* Sff;
STRPAT pat;

  olefile = df_OpenFile(szFileName);
  if (olefile == NULL) {
    printf("Error %d ole-opening %s.\n", df_errno, szFileName);
    return rc;
  }

  strcpy(pat.str, "WordDocument");
  pat.type = STRPAT_RIGHTJUST;

  Sff = df_StreamFindFirst(olefile, &pat);
  if (Sff != NULL) {
	rc = 1;
	df_StreamFindClose(Sff);
  }

  df_CloseFile(olefile);
  return rc;
}


int WfwGetTemplateByte(char *szFileName)
{
struct df_t *olefile;
struct WordDoc_t *worddoc;
int rc = -1;
byte value = 0;
SFF* Sff;
STRPAT pat;
OLE2STREAM *dfstream;

  olefile = df_OpenFile(szFileName);
  if (NULL == olefile) {
    printf("Error %d ole-opening %s.\n",df_errno,szFileName);
    return rc;
  }

  strcpy(pat.str, "WordDocument");
  pat.type = STRPAT_RIGHTJUST;

  Sff = df_StreamFindFirst(olefile, &pat);
  if (Sff != NULL) {
      dfstream = df_OpenStream(olefile, pat.str);
	  if (dfstream != NULL) {
		  if (df_SeekStream(dfstream, 0xAL) && (df_ReadStream(dfstream, (char*) &value, 1) == 1)) 
		  rc = (int) 0x01 & value;
	  }
	  df_StreamFindClose(Sff);
  }

  df_CloseFile(olefile);
  return rc;
}


int WfwGetTemplateByte2(char *szFileName)
{
struct df_t *olefile;
struct WordDoc_t *worddoc;
int rc = -1;
byte value = 0;

  olefile = df_OpenFile(szFileName);
  if (NULL == olefile) {
    printf("Error %d ole-opening %s.\n",df_errno,szFileName);
    return rc;
  }

  worddoc = wfw_OpenDoc(olefile,"WordDocument");
  if (worddoc != NULL) {
    if (wfw_GetTemplateByte(worddoc, &value))
		rc = (int) 0x01 & value;
	else {
	    printf("wfw_GetTemplateByte failed.\n");
		rc = -1;
	}
	wfw_CloseDoc(worddoc);
  } else {
    printf("wfw_OpenDoc failed.\n");
  }
  df_CloseFile(olefile);
  return rc;
}


int WfwSetTemplateByte(char *szFileName, int value)
{
struct df_t *olefile;
struct WordDoc_t *worddoc;
int rc = -1;

  olefile = df_OpenFile(szFileName);
  if (NULL == olefile) {
    printf("Error %d ole-opening %s.\n",df_errno,szFileName);
    return 0;
  }

  worddoc = wfw_OpenDoc(olefile,"WordDocument");
  if (worddoc != NULL) {
    rc = wfw_SetTemplateByte(worddoc, (byte) value);
  }
  df_CloseFile(olefile);
  return rc;
}
