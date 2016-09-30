#ifdef MACNEWAPI
/* mac.h */

#ifndef MAC_H
#define MAC_H


/* defines and structures for mac_enum_macros */
#define MAC_MAXFULLNAME     128
#define MAC_MAXSTREAMNAME   34
#define MAC_MAXDESCRIPTION  256
#define SCALPELSCANBUFFERSIZE 0x5000

#define MAC_ANY    0
#define MAC_WORD7  1
#define MAC_EXCEL7 2
#define MAC_VBA5   4

typedef dword AVTAK;

#define AVTAK_DEFAULT 0

typedef void MAC;
typedef void MACFF;

#ifdef __cplusplus
#define MACEXTDEF extern "C"
#else
#define MACEXTDEF
#endif

MACEXTDEF MAC *mac_new();
MACEXTDEF void mac_destroy(MAC *mac);
MACEXTDEF int mac_open_file(MAC *mac, char*szFileName);
MACEXTDEF int mac_close_file(MAC *mac);
//
// mac_findfirst
// Searches for the macro in question as specified by the 4-tupel:
// iType, szPath, szModule, szName
// Where
//    iType is the type of document requested
//      MAC_ANY: any document,
//      MAC_WORD7: Word <= 7.0 documents,
//      MAC_EXCEL7: Excel <= 7.0 documents
//      MAC_VBA5: Documents that use VBA5
//    szPath is the project stream path name. E.g., a Word 7.0 document's project
//      is the document itself, VBA3 project is contained in the 'dir' stream
//    szModule is the name of the module (where appropriate)
//    szMacro is the name of the macro
//
//  This function also requires the current open file handle, vmac.
//
// Returns: rmac_ff, the handle to the find macro structure needed for further
// searching. Use mac_findclose to delete this structure.
MACEXTDEF int mac_findfirst(MACFF **rmac_ff, MAC *vmac, int iType, char *szPath, char *szModule, char *szName);
MACEXTDEF int mac_findnext(MAC *, MACFF *);
MACEXTDEF int mac_findclose(MAC *, MACFF *);
MACEXTDEF int mac_read_open(MAC *vmac, MACFF *vmac_ff, void **ppBuffer, int maxlen, AVTAK read_type);
MACEXTDEF int mac_read_close(MAC *vmac, MACFF *vmac_ff, void **ppBuffer);
MACEXTDEF int mac_kill_macro(MAC *mac, MACFF *mac_ff);
MACEXTDEF int mac_type(MACFF *mac_ff);
MACEXTDEF int mac_path(MACFF *mac_ff, char *str, int len);
MACEXTDEF int mac_name(MACFF *mac_ff, char *str, int len);
MACEXTDEF int mac_macro(MACFF *mac_ff, char *str, int len);
MACEXTDEF int mac_length(MACFF *mac_ff);

/* error defines */
#define EWFW_PARSE_ERROR 0x330
#define EWFW_ENCRYPTED   0x331

#define EVBA3_GENERAL_ERROR   0x340
#define EVBA5_GENERAL_ERROR   0x350

#endif
#else
/* mac.h */

#ifndef MAC_H
#define MAC_H

#define SCALPELSCANBUFFERSIZE 0x5000

/* DC hack: these were ints being cast to pointers; no good in 16-bit */
/* They _should_ just be pointers in the first place...               */
typedef unsigned long mac_handle_type;
#define MAC_INVALID_HANDLE ( (mac_handle_type) -1 )

#include "common.h"
/* Opens the given file for access to an object in file szFileName of
 type szType.  Returns an integer handle for later calls, or 0 if the
 file doesn 't contain the wanted object, or -1 (and sets errno) in
 case of other failure.  If the file contains multiple objects, the
 first one is made active.  */
mac_handle_type mac_open(char *szFileName, char *szType);
/* Makes the next object in the file, if any, in the file referred to
 by the given handle, active.  Returns boolean.  */
int mac_next(mac_handle_type handle);
/* Reads the macro macroname into the given buffer buf, which has a
 maximum size buflen.  Returns the length read if OK, or -1 (and sets
 errno) if error.  */
int mac_read_macro(mac_handle_type handle,char *macroname,byte *buffer,int buflen);
/* Informs the mac support that we want to modify this file. This is a
    no-op.  */
int mac_write_initialize(mac_handle_type handle);
/* Deleted the given macro from the active object in the given file.
 Returns 0, if successful, or -1 (and errno) if failure.  */
int mac_kill_macro(mac_handle_type handle, char *macroname);
/* Rename the macro given by oldname to newname. Presently a no op.  */
int mac_rename_macro(mac_handle_type handle, char *oldname, char *newname);
/* Make the object stop being a template, iff it contains no undeleted
    macros.  Returns -1 and errno if failure (including there being
    macros left), and 0 if worked.  */
int mac_detemplatize(mac_handle_type handle);
#ifdef MCV
int mac_templatize(mac_handle_type handle);
#endif
/* Close the file.  A mac_next cannot follow this. If
    mac_write_initialize() has been working with a backup copy of the
    file, the original file is securly erased, and the backup slipped
    in in its place.  Present implementation: should be working to
    spec, except we dont work with backups */
int mac_close(mac_handle_type handle);
/* Like mac_close(), except that a temporary working file is
 abandoned, and the original existing file left unchanged.  Present
 Implementation: calls mac_close because we dont make backups.  */
int mac_abend(mac_handle_type handle);

/* defines and structures for mac_enum_macros */
#define MAXFULLNAME     128
#define MAXSTREAMNAME   34
#define MAXDESCRIPTION  256
typedef struct mac_enum_s {
   char     szName[MAXFULLNAME];             /* macro name in module.macro format */
   char     szModuleStream[MAXSTREAMNAME];   /* name of module stream */
   char     szMacroName[MAXSTREAMNAME];      /* name of macro */
   char     szMacroAltName[MAXSTREAMNAME];   /* alternative macro name */
   char     szMacroDescription[MAXDESCRIPTION]; /* Macro description, if any */
   int      lLength;
} MAC_ENUM_T;

/* Enumerate the macros in the object referenced by handle. This
    function is */
/* is to be used like: */
/* for (i=0; mac_enum_macros(handle, i, flags, &theMacro); i++) {
      ... do something ...  }
*/
/* Currently, flags are ignored. the theMacro structure must be
    allocated and */
/* destroyed by the client. Returns boolean.  */
int mac_enum_macros(mac_handle_type handle, int index, int flags, MAC_ENUM_T *theMacro);
/* Return number of macros in object */
int mac_count_macros(mac_handle_type handle);

/* provides macro information on the given file for checkup.  */
int mac_macro_info(char *szFileName, unsigned char *buffer, int count);
/* like mac_macro_info() but takes a handle instead of a file name.  */
int mac_h_macro_info(int hnd, unsigned char *buffer, int count);
/* Scalpel scans all macros in a file.  */
/* NEW for '98: detailed error reporting is done via the int pointer errorno */
char *mac_ScanMacros(unsigned long handle,
                     char *buffer,
                     int buflen,
                     void *somestructure,
                     int overbite,
                     char *(Scanner)(char *,int, void*),
                     int *errorno);

/* return handle. Internal Use Only */
void *mac_hdl(mac_handle_type handle);

/* Remove Excel4 macro sheet(s) */
int mac_kill_excel4_sheet(char *filename, char *sheetname);

/* Destroy all macro-related stuff in a VBA5 document */
int mac_zap_vba5(char *szFileName);

/* error defines */
#define VS_ERROR_BASE 0

#define EWFW_PARSE_ERROR (VS_ERROR_BASE + 0x330)
#define EWFW_ENCRYPTED   (VS_ERROR_BASE + 0x330)
//#define EWFW_INVALID_WORDDOC                          (VS_ERROR_BASE + 0x330)
//#define EWFW_MALLOC_FAILED             (VS_ERROR_BASE + 0x331)
//#define EWFW_INVALID_MACROTABLE        (VS_ERROR_BASE + 0x332)
//#define EWFW_INVALID_SUBTABLE          (VS_ERROR_BASE + 0x333)
//#define EWFW_SUBTABLE_TOO_BIG          (VS_ERROR_BASE + 0x334)
//#define EWFW_SUBTABLE_ENTRY_INVALID    (VS_ERROR_BASE + 0x335)
//#define EWFW_NON_ENGLISH_DOC           (VS_ERROR_BASE + 0x336)
//#define EWFW_GENERAL_ERROR             (VS_ERROR_BASE + 0x337)
//#define EWFW_WORDDOC_NOT_FOUND         (VS_ERROR_BASE + 0x338)

#define EVBA3_GENERAL_ERROR   (VS_ERROR_BASE + 0x340)
#define EVBA5_GENERAL_ERROR   (VS_ERROR_BASE + 0x350)

#endif
#endif
