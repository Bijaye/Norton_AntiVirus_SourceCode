//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/o97api.cpv   1.22   11 Nov 1998 16:23:38   DCHI  $
//
// Description:
//  Core Office 97 access implementation.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/o97api.cpv  $
// 
//    Rev 1.22   11 Nov 1998 16:23:38   DCHI
// 
//    Rev 1.21   11 Nov 1998 16:19:24   DCHI
// Fixed warnings with int/DWORD castings.
// 
//    Rev 1.20   11 Nov 1998 16:10:14   DCHI
// - Modified O97PROJECTwmRemoveModule() to return TRUE even if the name
//   is not found.
// - Modified O97FindVBAStreams() to not return error if the dir stream
//   is not found.
// - Modified O97DeleteModule() to not call O97dirRemoveModule() if
//   the dir stream was not found.
// 
//    Rev 1.19   09 Nov 1998 13:42:30   DCHI
// Added #pragma data_seg("FAR_DATA_0") for global FAR data.
// 
//    Rev 1.18   12 Oct 1998 13:11:20   DCHI
// Added NLM relinquish control call to O97dirRemoveModule().
// 
//    Rev 1.17   04 Aug 1998 13:45:32   DCHI
// - Limited module count to 16K in O97GetModuleCount().
// - Added circular link checking to O97CountNonEmptyModules().
// 
//    Rev 1.16   27 Jul 1998 13:31:50   DCHI
// Added additional comments and minor mods to UpdateForRecompilation().
// 
//    Rev 1.15   10 Mar 1998 11:38:58   RELNITI
// ADD new CRC method: O97ModuleSourceCRC2() and O97ModuleSourceCRCContinue2()
// 
//    Rev 1.14   26 Nov 1997 15:15:32   DCHI
// Removed unreferenced local variable.
// 
//    Rev 1.13   26 Nov 1997 15:07:42   DCHI
// Added O97VBAFindAssociatedStreams() and O97VBACheckMacrosDeletable().
// 
//    Rev 1.12   28 Aug 1997 14:52:26   DCHI
// Added VBA5_GENERIC support to O97OpenVBAStorage().
// 
//    Rev 1.11   22 Aug 1997 17:07:12   DCHI
// Added code to O97ModuleGetDesc() to set the description to the empty
// string if a description was not found.
// 
//    Rev 1.10   18 Aug 1997 15:01:46   DCHI
// Added counting of CRCed bytes to CRC functions.
// 
//    Rev 1.9   14 Aug 1997 18:14:16   DCHI
// Added O97OpenVBA5ModuleULCB() and modified O97GetModuleNameAtIndex()
// to use it.
// 
//    Rev 1.8   11 Aug 1997 15:06:10   DCHI
// Removed unreferenced variables.
// 
//    Rev 1.7   11 Aug 1997 14:23:30   DCHI
// Added O97GetModuleCount(), O97GetModuleNameAtIndex(),
// O97OpenModuleStreamByName(), and O97ModuleGetDesc().
// 
//    Rev 1.6   27 Jun 1997 19:33:42   DCHI
// Rearranged order of some lines in StreamAttributeOnly() to fix
// compiler bug on WIN platform dealing with SS/DS usage.
// 
//    Rev 1.5   23 Jun 1997 17:01:50   DCHI
// Removed error return upon failure in deleting Module=MODULENAME line
// from project stream.
// 
//    Rev 1.4   13 Jun 1997 12:47:02   DCHI
// Added CRC function.
// 
//    Rev 1.3   15 May 1997 12:04:08   DCHI
// Add code to enable deletion of Class modules.  Corrected SRP_0
// callback function to return OPEN rather than RETURN.
// 
//    Rev 1.2   08 May 1997 13:13:22   DCHI
// Added root to enum storage callback; corrected check on SS return values.
// 
//    Rev 1.1   08 May 1997 12:57:46   DCHI
// Added VCS headers.
// 
//************************************************************************

#include "o97api.h"
#include "olestrnm.h"
#include "crc32.h"

///////////////////////////////////////////////////////////////////////////
//
// The following section contains information for parsing the dir stream.
//
///////////////////////////////////////////////////////////////////////////

/**************************************************************************

Here is the design of the intermediate file format for use to load projects.

There will be multiple streams -- a single directory stream, followed by
multiple streams for each code module.


Code module streams:
====================
The code module streams are pure ascii code, saved exactly in the same format
as the current VBA would do a "save as text" (including attributes associated
with each function, etc).  These streams are compressed to save space.

The streams names are in general the same name as the module.
Note that it really doesn't matter what this name is.  The only 
restrictions are that each module's stream be unique and that 
it be a valid stream name in the docfile implementation

The directory stream:
=====================
The name of the directory stream is defined with the SZ_DIRECTORY_STREAM
constant defined in this file.

The directory stream consists of multiple variable-length records:
16-bit tag
32-bit byte count (# of bytes that follow -- 0 if none)
<data>

Following is a description of the records which may appear in the
directory stream.  When loading the directory stream, records with
unrecognized tags should be ignored.  In describing the data portion
of the record, all numbers are stored in Intel (not Motorola) format.
Anything called a BSTR is stored as a 32-bit length (in bytes) followed
by the string data.  Note that we do not store all strings as BSTRs because
the data record length may imply the length of the string.  All strings are
ANSI (DBCS).

BIN_PROJ_SYSKIND     syskind (32 bit)
BIN_PROJ_LCID        lcid (32 bit)
BIN_PROJ_CODEPAGE    codepage (16 bit)
BIN_PROJ_LCIDINVOKE  LCID to be used for Invoke calls (typically English).
BIN_PROJ_NAME        The programmatic name of the project.
BIN_PROJ_DOCSTRING   The project's doc string
BIN_PROJ_HELPFILE    The project's helpfile name
BIN_PROJ_HELPCONTEXT The project's help context id (32 bit)
BIN_PROJ_LIBFLAGS    wLibFlags (32 bit, strip off unknown ones upon load
                     of cannonical format)
BIN_PROJ_VERSION     m_ulgtliboleMajorId (32 bit)
                     m_ugtliboleMinorId (16 bit)
BIN_PROJ_TYPELIB_VERSION     wVerMajor (16 bit)
                             wVerMinor (16 bit)
BIN_PROJ_GUID        project's guid (16 byte)
BIN_PROJ_PROPERTIES  for each property:
                       BSTR for property name
                       BSTR for property value
                       four byte zero to mark end of list
BIN_PROJ_CONSTANTS   String of conditional compilation constants.
BIN_PROJ_COMPAT_EXE  Compatible exe information
		     fMakeCompatible (16-bit)
		     fShowEditDlg (16-bit)
		     BSTR for exe name

BIN_PROJ_COMMAND_LINE  The Command Line arguments (BSTR)
BIN_PROJ_CONDCOMP      The Conditional Compilation arguments (BSTR)

The following two tags are used to represent the references the project
has to other projects and typelibs.  They must appear in the order that
the references should appear in the project.  The "default" references
should not be in this list.

BIN_PROJ_LIBID_REGISTERED
                     BSTR for registered or foreignregistered libid
                     major (32 bit) and minor (16 bit) id of referenced
                     project/typelib (ulgtliboleMajorId/ugtliboleMinorId).
BIN_PROJ_LIBID_PROJ  BSTR for project libid with absolute path
                     BSTR for libid with relative path
                     major (32 bit) and minor (16 bit) timestamp id of
                     referenced project (ulgtliboleMajorId/ugtliboleMinorId).

BIN_PROJ_MODULECOUNT number of modules (16 bit)

For each module, you get the following tags.  The BIN_MOD_NAME
tag must appear first.

BIN_MOD_NAME         the module's name
BIN_MOD_STREAM       the corresponding module stream's name
BIN_MOD_DOCSTRING    the module's doc string
BIN_MOD_HELPFILE     the module's helpfile name
BIN_MOD_HELPCONTEXT  the module's help context id (32 bit)
BIN_MOD_PROPERTIES   the module's property count (16 bit), then for each property:
                        BSTR for property name
                        BSTR for property value

The filtflag bits.  No data associated with any of these.
If the tag is present, the bit is 1, else 0:
BIN_MOD_FBASMOD_StdMods
BIN_MOD_FBASMOD_Classes
BIN_MOD_FBASMOD_Creatable
BIN_MOD_FBASMOD_NoDisplay
BIN_MOD_FBASMOD_NoEdit
BIN_MOD_FBASMOD_RefLibs
BIN_MOD_FBASMOD_NonBasic
BIN_MOD_FBASMOD_Private
BIN_MOD_FBASMOD_Internal
BIN_MOD_END 	     last record in a given module's collection of records

BIN_PROJ_EOF 	     last record in the directory stream

Each module stream contains the module's Basic code interspersed
with Attribute statements.  VBA93 generates attribute statements
for each function that provide the helpid and doc string (only
generated if there is a helpid and/or a doc string).

**************************************************************/

#define BIN_SZ_DIRECTORY_STREAM (OLESTR("dir"))

// bxlat - The header structure for records in the directory stream.
typedef struct tagBINXLATRECORD
{
    unsigned short usTag;       // One of the BIN_* constants.
    unsigned long  cbData;      // The number of bytes of data in rgbData.
} BINXLATRECORD;

#define BIN_PROJ_SYSKIND                 1
#define BIN_PROJ_LCID                    2
#define BIN_PROJ_CODEPAGE                3
#define BIN_PROJ_NAME                    4
#define BIN_PROJ_DOCSTRING               5
#define BIN_PROJ_HELPFILE                6
#define BIN_PROJ_HELPCONTEXT             7
#define BIN_PROJ_LIBFLAGS                8
#define BIN_PROJ_VERSION                 9
#define BIN_PROJ_GUID                   10
#define BIN_PROJ_PROPERTIES             11
#define BIN_PROJ_CONSTANTS              12

#define BIN_PROJ_LIBID_REGISTERED       13
#define BIN_PROJ_LIBID_PROJ             14

#define BIN_PROJ_MODULECOUNT            15
#define BIN_PROJ_EOF                    16
#define BIN_PROJ_TYPELIB_VERSION        17
#define BIN_PROJ_COMPAT_EXE             18
#define BIN_PROJ_COOKIE                 19
#define BIN_PROJ_LCIDINVOKE             20
#define BIN_PROJ_COMMAND_LINE           21
#define BIN_PROJ_REFNAME_PROJ           22

#define BIN_MOD_NAME                    25
#define BIN_MOD_STREAM                  26
#define BIN_MOD_DOCSTRING               28
#define BIN_MOD_HELPFILE                29
#define BIN_MOD_HELPCONTEXT             30
#define BIN_MOD_PROPERTIES              32
#define BIN_MOD_TEXTOFFSET              49

#define BIN_MOD_FBASMOD_StdMods         33
#define BIN_MOD_FBASMOD_Classes         34
#define BIN_MOD_FBASMOD_Creatable       35
#define BIN_MOD_FBASMOD_NoDisplay       36
#define BIN_MOD_FBASMOD_NoEdit          37
#define BIN_MOD_FBASMOD_RefLibs         38
#define BIN_MOD_FBASMOD_NonBasic        39
#define BIN_MOD_FBASMOD_Private         40
#define BIN_MOD_FBASMOD_Internal        41
#define BIN_MOD_FBASMOD_AllModTypes     42
#define BIN_MOD_END                     43
#define BIN_MOD_COOKIETYPE              44
#define BIN_MOD_BASECLASSNULL           45
#define BIN_MOD_BASECLASS               46

#define BIN_PROJ_LIBID_TWIDDLED         47
#define BIN_PROJ_LIBID_EXTENDED         48

#define BIN_PROJ_UNICODE_CONSTANTS      60
#define BIN_PROJ_UNICODE_HELPFILE       61
#define BIN_PROJ_UNICODE_REFNAME_PROJ   62
#define BIN_PROJ_UNICODE_COMMAND_LINE   63
#define BIN_PROJ_UNICODE_DOCSTRING      64

#define BIN_MOD_UNICODE_STREAM          50
#define BIN_MOD_UNICODE_NAME            71
#define BIN_MOD_UNICODE_DOCSTRING       72
#define BIN_MOD_UNICODE_HELPFILE        73

///////////////////////////////////////////////////////////////////////////
//
// The above section contains information for parsing the dir stream.
//
///////////////////////////////////////////////////////////////////////////


//********************************************************************
//
// Function:
//  int O97OpenVBAStorageCB()
//
// Description:
//  Checks the entry to see if it is the VBA storage.
//  If so then the function sets the following fields of the VBA5
//  structure:
//      dwVBAEntry
//      dwVBAChildEntry
//
// Returns:
//  ENUM_SS_CB_STATUS_RETURN        If the entry is a VBA storage
//  ENUM_SS_CB_STATUS_CONTINUE      If the entry is not
//
//********************************************************************

int O97OpenVBAStorageCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;

    if (lpstEntry->byMSE == STGTY_STORAGE)
    {
        // Is it the VBA storage?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszVBA,
                       SS_MAX_NAME_LEN) == 0)
        {
            LPO97_SCAN      lpstO97Scan = (LPO97_SCAN)lpvCookie;

            // Set the dwVBAEntry and dwVBAChildEntry fields

            lpstO97Scan->dwVBAEntry = dwIndex;
            lpstO97Scan->dwVBAChildEntry =
                DWENDIAN(lpstEntry->dwSIDChild);

            return(SS_ENUM_CB_STATUS_RETURN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL O97OpenVBAStorage()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  lpstO97Scan         Ptr to O97 scan structure
//
// Description:
//  For WD8 the dwMacrosChildEntry must indicate a valid storage.
//  For XL97 the dw_VBA_PROJECT_CURChildEntry must indicate a valid
//  storage.
//
//  The <VBA> storage is assumed to be in the given storage.
//
//  Upon successful exit, dwVBAEntry and dwVBAChildEntry are set
//  to the ID of the <VBA> storage and its child, respectively.
//
// Returns:
//  TRUE                If a VBA storage was found
//  FALSE               If a VBA storage was not found
//
//********************************************************************

BOOL O97OpenVBAStorage
(
    LPSS_ROOT           lpstRoot,
    LPO97_SCAN          lpstO97Scan
)
{
    LPSS_ENUM_SIBS  lpstSibs;

    // Allocate a sibiling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    switch (lpstO97Scan->wType)
    {
        case VBA5_WD8:
            SSInitEnumSibsStruct(lpstSibs,
                                 lpstO97Scan->u.stWD8.
                                     dwMacrosChildEntry);
            break;

        case VBA5_XL97:
            SSInitEnumSibsStruct(lpstSibs,
                                 lpstO97Scan->u.stXL97.
                                     dw_VBA_PROJECT_CURChildEntry);
            break;

        case VBA5_GENERIC:
            SSInitEnumSibsStruct(lpstSibs,
                                 lpstO97Scan->u.stGeneric.
                                     dwVBAParentChildEntry);
            break;

        default:
            // This should never happen

            SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
            return(FALSE);
    }

    if (SSEnumSiblingEntriesCB(lpstRoot,
                               O97OpenVBAStorageCB,
                               lpstO97Scan,
                               lpstSibs,
                               NULL) == SS_STATUS_OK)
    {
        SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
        return(TRUE);
    }

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
    return(FALSE);
}


//********************************************************************
//
// Function:
//  int O97OpenVBA5ModuleCB()
//
// Description:
//  If the name of the entry is not "dir" and it does not begin
//  with an underscore, and the name is not the empty string,
//  then the function returns success, indicating a candidate.
//
//  If lpvCookie is non-NULL, it should point to a buffer to
//  store the uppercased name of the module stream.
//
// Returns:
//  SS_ENUM_CB_STATUS_OPEN          If the entry is a candidate
//  SS_ENUM_CB_STATUS_CONTINUE      If the entry is not
//
//********************************************************************

int O97OpenVBA5ModuleCB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
)
{
    int                 i;
    WORD                wTemp;

    (void)dwIndex;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // If the name begins with an underscore or is "dir"
        //  or is empty, just return

        if (WENDIAN(lpstEntry->uszName[0]) == '_' ||
            lpstEntry->uszName[0] == 0 ||
            SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszdir,
                       SS_MAX_NAME_LEN) == 0)
        {
            return(SS_ENUM_CB_STATUS_CONTINUE);
        }

        if (lpvCookie != NULL)
        {
            // Copy the name and uppercase it

            for (i=0;i<SS_MAX_NAME_LEN;i++)
            {
                wTemp = WENDIAN(lpstEntry->uszName[i]);
                if (wTemp == 0)
                    break;

                if ('a' <= wTemp && wTemp <= 'z')
                    ((LPBYTE)lpvCookie)[i] = (BYTE)(wTemp - 'a' + 'A');
                else
                    ((LPBYTE)lpvCookie)[i] = (BYTE)wTemp;
            }

            // Zero terminate it

            ((LPBYTE)lpvCookie)[i] = 0;
        }

        // Try it

        return(SS_ENUM_CB_STATUS_OPEN);
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  int O97OpenVBA5ModuleULCB()
//
// Description:
//  If the name of the entry is not "dir" and it does not begin
//  with an underscore, and the name is not the empty string,
//  then the function returns success, indicating a candidate.
//
//  If lpvCookie is non-NULL, it should point to a buffer to
//  store the mixed-cased name of the module stream.
//
// Returns:
//  SS_ENUM_CB_STATUS_OPEN          If the entry is a candidate
//  SS_ENUM_CB_STATUS_CONTINUE      If the entry is not
//
//********************************************************************

int O97OpenVBA5ModuleULCB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
)
{
    int                 i;
    WORD                wTemp;

    (void)dwIndex;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // If the name begins with an underscore or is "dir"
        //  or is empty, just return

        if (WENDIAN(lpstEntry->uszName[0]) == '_' ||
            lpstEntry->uszName[0] == 0 ||
            SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszdir,
                       SS_MAX_NAME_LEN) == 0)
        {
            return(SS_ENUM_CB_STATUS_CONTINUE);
        }

        if (lpvCookie != NULL)
        {
            // Copy the name and uppercase it

            for (i=0;i<SS_MAX_NAME_LEN;i++)
            {
                wTemp = WENDIAN(lpstEntry->uszName[i]);
                if (wTemp == 0)
                    break;

                ((LPBYTE)lpvCookie)[i] = (BYTE)wTemp;
            }

            // Zero terminate it

            ((LPBYTE)lpvCookie)[i] = 0;
        }

        // Try it

        return(SS_ENUM_CB_STATUS_OPEN);
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL O97GetModuleCount()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  dwVBAChildEntry     Entry number of VBA storage child
//  lpdwCount           Ptr to DWORD for module count
//
// Description:
//  The function enumerates through the siblings of the VBA child
//  and counts the number of VBA modules.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL O97GetModuleCount
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwVBAChildEntry,
    LPDWORD             lpdwCount
)
{
    LPSS_ENUM_SIBS      lpstSibs;
    DWORD               dwCount;

    // Allocate a sibiling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    SSInitEnumSibsStruct(lpstSibs,
                         dwVBAChildEntry);

    dwCount = 0;
    while (SSEnumSiblingEntriesCB(lpstRoot,
                                  O97OpenVBA5ModuleCB,
                                  NULL,
                                  lpstSibs,
                                  NULL) == SS_STATUS_OK)
    {
        ++dwCount;

        if (dwCount > 16384)
            break;
    }

    if (SSFreeEnumSibsStruct(lpstRoot,lpstSibs) !=
        SS_STATUS_OK)
        return(FALSE);

    *lpdwCount = dwCount;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97GetModuleNameAtIndex()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  dwVBAChildEntry     Entry number of VBA storage child
//  dwIndex             Index of module info to look up
//  lpbyName            Buffer for name (>= 256 bytes)
//
// Description:
//  The function looks up the name of the stream of the module
//  at the given index.  The index is zero based.  If the index
//  surpasses the number of modules, the function returns error.
//
//  The string is returned as a zero-terminated string.  Since the
//  string is a Unicode string, only the first byte of each of the
//  first 255 characters are returned.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL O97GetModuleNameAtIndex
(
    LPSS_ROOT           lpstRoot,
    DWORD               dwVBAChildEntry,
    DWORD               dwIndex,
    LPBYTE              lpbyName
)
{
    LPSS_ENUM_SIBS      lpstSibs;
    DWORD               dwCurIndex;

    // Allocate a sibiling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    SSInitEnumSibsStruct(lpstSibs,
                         dwVBAChildEntry);

    dwCurIndex = 0;
    while (SSEnumSiblingEntriesCB(lpstRoot,
                                  O97OpenVBA5ModuleULCB,
                                  lpbyName,
                                  lpstSibs,
                                  NULL) == SS_STATUS_OK)
    {
        if (dwCurIndex == dwIndex)
        {
            if (SSFreeEnumSibsStruct(lpstRoot,
                                     lpstSibs) != SS_STATUS_OK)
                return(FALSE);

            return(TRUE);
        }

        if (++dwCurIndex > dwIndex)
            break;
    }

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL O97MemICmp()
//
// Description:
//  Performs a case insensitive compare of two memory buffers
//  up to n bytes.
//
// Returns:
//  TRUE        If the buffers are both the same
//  FALSE       If the buffers are different
//
//********************************************************************

BOOL O97MemICmp
(
    LPBYTE      lpbyMem0,
    LPBYTE      lpbyMem1,
    int         n
)
{
    BYTE        by0, by1;

    while (n-- > 0)
    {
        by0 = *lpbyMem0++;
        by1 = *lpbyMem1++;

        if ('a' <= by0 && by0 <= 'z')
            by0 = by0 - 'a' + 'A';

        if ('a' <= by1 && by1 <= 'z')
            by1 = by1 - 'a' + 'A';

        if (by0 != by1)
            return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97LZNTStreamAttributeOnly()
//
// Description:
//  Checks the given LZNT compressed stream to see whether it
//  contains lines that begin with something other than
//  "Attribute VB_".
//
// Returns:
//  TRUE        If the stream was successfully analyzed as
//              containing only "Attribute VB_" lines
//  FALSE       Otherwise, including error
//
//********************************************************************

BOOL O97LZNTStreamAttributeOnly
(
    LPLZNT          lpstLZNT
)
{
    DWORD           dwOffset;
    int             nNumBytes;
    int             nNextI;
    int             i;
    BYTE            abyWorkBuffer[256];

    // Assume lines are no longer than 256 bytes

    dwOffset = 0;
    nNumBytes = 256;
    while (dwOffset < lpstLZNT->dwSize)
    {
        if (dwOffset + 256 > lpstLZNT->dwSize)
            nNumBytes = (int)(lpstLZNT->dwSize - dwOffset);

        if (LZNTGetBytes(lpstLZNT,
                         dwOffset,
                         nNumBytes,
                         abyWorkBuffer) != nNumBytes)
        {
            return(FALSE);
        }

        // Iterate through lines while the line begins with
        //  "Attribute VB_"

        i = 0;
        while (i < nNumBytes)
        {
            for (nNextI=i;nNextI<nNumBytes;nNextI++)
            {
                if (abyWorkBuffer[nNextI] == 0x0D)
                    break;
            }

            // Was the end of the line found?

            if (nNextI < nNumBytes)
            {
                // Found the end of a line

                // Terminate it

                if (abyWorkBuffer[i] != 0x0D &&
                    O97MemICmp((LPBYTE)"Attribute VB_",
                               abyWorkBuffer+i,
                               sizeof("Attribute VB_") - 1) == FALSE)
                {
                    // Non attribute line found

                    return(FALSE);
                }

                i = nNextI + 2;
            }
            else
            {
                break;
            }
        }

        if (i == 0)
        {
            // The end of the line was not found, meaning that
            //  the line was not terminated.  Special condition!?

            return(FALSE);
        }

        dwOffset += i;
    }

    // Only attribute lines found

    return(TRUE);
}


//********************************************************************
//
// Function:
//  int O97FindProjectStreamsCB()
//
// Description:
//  Searches for the streams PROJECT and PROJECTwm.
//
//  Upon finding each, the function sets the following fields
//  respectively of the VBA5 structure:
//      dwPROJECTEntry
//      dwPROJECTwmEntry
//
// Returns:
//  SS_ENUM_CB_STATUS_CONTINUE      Always
//
//********************************************************************

int O97FindProjectStreamsCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Is it the PROJECT stream

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszPROJECT,
                       SS_MAX_NAME_LEN) == 0)
        {
            ((LPO97_SCAN)lpvCookie)->dwPROJECTEntry = dwIndex;
        }
        else

        // Is it the PROJECTwm stream

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszPROJECTwm,
                       SS_MAX_NAME_LEN) == 0)
        {
            ((LPO97_SCAN)lpvCookie)->dwPROJECTwmEntry = dwIndex;
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  int O97FindVBAStreamsCB()
//
// Description:
//  Searches for the streams _VBA_PROJECT and dir.
//
//  Upon finding each, the function sets the following fields
//  respectively of the VBA5 structure:
//      dw_VBA_PROJECTEntry
//      dwdirEntry
//
// Returns:
//  SS_ENUM_CB_STATUS_CONTINUE      Always
//
//********************************************************************

int O97FindVBAStreamsCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Is it the _VBA_PROJECT stream?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywsz_VBA_PROJECT,
                       SS_MAX_NAME_LEN) == 0)
        {
            ((LPO97_SCAN)lpvCookie)->dw_VBA_PROJECTEntry = dwIndex;
        }
        else

        // Is it the dir stream?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszdir,
                       SS_MAX_NAME_LEN) == 0)
        {
            ((LPO97_SCAN)lpvCookie)->dwdirEntry = dwIndex;
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL O97FindVBAStreams
//
// Description:
//  On entering, the enumeration structure should have been
//  initialized with the root sibling of the project streams.
//
//  Finds the following streams of a VBA 5 project:
//      PROJECT
//      PROJECTwm
//      _VBA_PROJECT
//      dir
//
//  Upon finding each, the function sets the following fields
//  respectively of the VBA5 structure:
//      dwPROJECTEntry
//      dwPROJECTwmEntry
//      dw_VBA_PROJECTEntry
//      dwdirEntry
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97FindVBAStreams
(
    LPSS_ROOT           lpstRoot,
    LPSS_ENUM_SIBS      lpstEnumSibs,
    LPO97_SCAN          lpstO97Scan
)
{
    /////////////////////////////////////////////////////////////
    // Find the PROJECT and PROJECTwm streams
    /////////////////////////////////////////////////////////////

    lpstO97Scan->dwPROJECTEntry = 0xFFFFFFFF;
    lpstO97Scan->dwPROJECTwmEntry = 0xFFFFFFFF;
    SSEnumSiblingEntriesCB(lpstRoot,
                           O97FindProjectStreamsCB,
                           lpstO97Scan,
                           lpstEnumSibs,
                           NULL);

    if (lpstO97Scan->dwPROJECTEntry == 0xFFFFFFFF)
    {
        // Failed to find the PROJECT stream

        return(FALSE);
    }


    /////////////////////////////////////////////////////////////
    // Find the _VBA_PROJECT and dir streams
    /////////////////////////////////////////////////////////////

    // Initialize sibling structure

    SSInitEnumSibsStruct(lpstEnumSibs,
                         lpstO97Scan->dwVBAChildEntry);

    // Search for desired streams

    lpstO97Scan->dw_VBA_PROJECTEntry = 0xFFFFFFFF;
    lpstO97Scan->dwdirEntry = 0xFFFFFFFF;
    SSEnumSiblingEntriesCB(lpstRoot,
                           O97FindVBAStreamsCB,
                           lpstO97Scan,
                           lpstEnumSibs,
                           NULL);

    if (lpstO97Scan->dw_VBA_PROJECTEntry == 0xFFFFFFFF)
    {
        // Failed to find either or both of the _VBA_PROJECT
        //  and the dir stream

        return(FALSE);
    }

    // Found all the streams

    return(TRUE);
}


//********************************************************************
//
// Function:
//  int O97EnumStorageEntriesCB()
//
// Description:
//  Searches for a storage and stores the child of the storage at
//  the DWORD pointed to by lpvCookie.
//
//  The root counts as a storage.
//
// Returns:
//  SS_ENUM_CB_STATUS_RETURN        If it is a storage
//  SS_ENUM_CB_STATUS_CONTINUE      If it is not a storage
//
//********************************************************************

int O97EnumStorageEntriesCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;

    if (lpstEntry->byMSE == STGTY_STORAGE ||
        lpstEntry->byMSE == STGTY_ROOT)
    {
        // Found a storage

        // Store the child

        *(LPDWORD)lpvCookie = DWENDIAN(lpstEntry->dwSIDChild);
        return(SS_ENUM_CB_STATUS_RETURN);
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL O97RefTableCmp()
//
// Description:
//  Performs a comparison of a single byte buffer against a single
//  byte buffer/double byte buffer.
//
// Returns:
//  TRUE        If the buffers are equal
//  FALSE       If the buffers are different
//
//********************************************************************

BOOL O97RefTableCmpI
(
    LPBYTE      lpbyMem0,
    int         nMem0Len,
    LPBYTE      lpbyMem1,
    int         nMem1Len
)
{
    BYTE        by0, by1;

    // Perform the single byte comparison

    if (nMem0Len == nMem1Len)
    {
        int         n;

        for (n=0;n<nMem0Len;n++)
            if (lpbyMem0[n] != lpbyMem1[n])
                return(FALSE);

        // Single byte comparison succeeded

        return(TRUE);
    }

    // Perform the double byte comparison

    if (nMem0Len + nMem0Len == nMem1Len)
    {
        while (nMem0Len-- != 0)
        {
            by0 = *lpbyMem0++;
            by1 = *lpbyMem1++;

            if ('a' <= by0 && by0 <= 'z')
                by0 = by0 - 'a' + 'A';

            if ('a' <= by1 && by1 <= 'z')
                by1 = by1 - 'a' + 'A';

            // The second byte must be zero

            if (by0 != by1)
                return(FALSE);

            if (*lpbyMem1++ != 0)
                return(FALSE);
        }

        return(TRUE);
    }

    // No match

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL O97PROJECTRemoveModule()
//
// Description:
//  Opens the PROJECT stream and removes the lines containing
//  Module=MODULENAME and MODULENAME={coordinates} from it.
//  The function returns error if the lines containing
//  MODULENAME are not found.  The MODULENAME comparison is
//  a case insensitive search.
//
//  The function also zeroes out the left over bytes of the
//  stream and truncates the stream if lines were deleted.
//
//  The function assumes that lines will never be greater than
//  1024 bytes and that a 0x0D 0x0A ends a line.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL O97PROJECTRemoveModule
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPO97_SCAN          lpstO97Scan,        // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    DWORD               dwOffset;
    DWORD               dwStreamLen;
    DWORD               dwModuleEqualStartOffset;
    DWORD               dwModuleEqualEndOffset;
    DWORD               dwMODULENAMEStartOffset;
    DWORD               dwMODULENAMEEndOffset;
    int                 nDesiredLineLen;
    int                 nDesiredLineLen2;
    int                 nModuleNameLen;
    int                 i, nNextI;
    BOOL                bFound;
    int                 nNumBytes;
    DWORD               dwByteCount;

    // Open the PROJECT stream

    if (SSOpenStreamAtIndex(lpstStream,
                            lpstO97Scan->dwPROJECTEntry) != SS_STATUS_OK)
    {
        // Failed to open PROJECT stream

        return(FALSE);
    }

    dwOffset = 0;
    dwStreamLen = SSStreamLen(lpstStream);

    // Determine the length of the module name

    nModuleNameLen = 0;
    while (abyModuleName[nModuleNameLen])
        ++nModuleNameLen;


    /////////////////////////////////////////////////////////////
    // Search for "Module=" or "BaseClass="
    /////////////////////////////////////////////////////////////

    bFound = FALSE;
    nDesiredLineLen = 7 + nModuleNameLen; // 7 = strlen("Module=")
    nDesiredLineLen2 = 10 + nModuleNameLen; // 10 = strlen("BaseClass=")
    while (bFound == FALSE && dwOffset < dwStreamLen)
    {
        if (dwStreamLen - dwOffset < 512)
            nNumBytes = (int)(dwStreamLen - dwOffset);
        else
            nNumBytes = 512;

        // This read should always occur at the beginning of a line

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpbyWorkBuffer,
                       (DWORD)nNumBytes,
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != (DWORD)nNumBytes)
        {
            return(FALSE);
        }

        // Iterate through lines

        i = 0;
        while (bFound == FALSE)
        {
            // Search for the end of the line

            for (nNextI=i;nNextI<nNumBytes;nNextI++)
            {
                if (lpbyWorkBuffer[nNextI] == 0x0D)
                    break;
            }

            // Was the end of the line found?

            if (nNextI < nNumBytes)
            {
                // Found the end of a line

                // Is the line the right length?

                if (nNextI - i == nDesiredLineLen)
                {
                    // Is it a "Module=MODULENAME" line?

                    if (O97MemICmp((LPBYTE)"Module=",
                                   lpbyWorkBuffer+i,
                                   7) == TRUE &&
                        O97MemICmp(abyModuleName,
                                   lpbyWorkBuffer+i+7,
                                   nModuleNameLen) == TRUE)
                    {
                        // Bingo!

                        bFound = TRUE;
                        dwModuleEqualStartOffset = dwOffset + i;
                        dwModuleEqualEndOffset = dwOffset + nNextI + 2;
                    }
                }
                else
                if (nNextI - i == nDesiredLineLen - 1)
                {
                    // Is it a "Class=MODULENAME" line?

                    if (O97MemICmp((LPBYTE)"Class=",
                                   lpbyWorkBuffer+i,
                                   6) == TRUE &&
                        O97MemICmp(abyModuleName,
                                   lpbyWorkBuffer+i+6,
                                   nModuleNameLen) == TRUE)
                    {
                        // Bingo!

                        bFound = TRUE;
                        dwModuleEqualStartOffset = dwOffset + i;
                        dwModuleEqualEndOffset = dwOffset + nNextI + 2;
                    }
                }
                else
                if (nNextI - i == nDesiredLineLen2)
                {
                    // Is it a "BaseClass=MODULENAME" line?

                    if (O97MemICmp((LPBYTE)"BaseClass=",
                                   lpbyWorkBuffer+i,
                                   10) == TRUE &&
                        O97MemICmp(abyModuleName,
                                   lpbyWorkBuffer+i+10,
                                   nModuleNameLen) == TRUE)
                    {
                        // Bingo!

                        bFound = TRUE;
                        dwModuleEqualStartOffset = dwOffset + i;
                        dwModuleEqualEndOffset = dwOffset + nNextI + 2;
                    }
                }
                else
                {
                    // Is it a "Document=MODULENAME" line?

                    if (O97MemICmp((LPBYTE)"Document=",
                                   lpbyWorkBuffer+i,
                                   9) == TRUE &&
                        O97MemICmp(abyModuleName,
                                   lpbyWorkBuffer+i+9,
                                   nModuleNameLen) == TRUE &&
                        lpbyWorkBuffer[i+9+nModuleNameLen] == '/')
                    {
                        // Bingo!

                        bFound = TRUE;
                        dwModuleEqualStartOffset = dwOffset + i;
                        dwModuleEqualEndOffset = dwOffset + nNextI + 2;
                    }
                }

                i = nNextI + 2;
            }
            else
            {
                // Begin a new read

                if (i > 0)
                    dwOffset += i;
                else
                {
                    // Line > 512 bytes?

                    dwOffset = dwStreamLen;
                }

                break;
            }
        }
    }

    if (bFound == FALSE)
    {
        // Failed to find "Module=MODULENAME" line

        dwModuleEqualEndOffset = dwModuleEqualStartOffset = 0;
    }


    /////////////////////////////////////////////////////////////
    // Search for [Workspace]
    /////////////////////////////////////////////////////////////

    bFound = FALSE;
    nDesiredLineLen = 11; // 11 = strlen("[Workspace]")
    dwOffset = dwModuleEqualEndOffset;
    while (bFound == FALSE && dwOffset < dwStreamLen)
    {
        if (dwStreamLen - dwOffset < 512)
            nNumBytes = (int)(dwStreamLen - dwOffset);
        else
            nNumBytes = 512;

        // This read should always occur at the beginning of a line

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpbyWorkBuffer,
                       (DWORD)nNumBytes,
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != (DWORD)nNumBytes)
        {
            return(FALSE);
        }

        // Iterate through lines

        i = 0;
        while (bFound == FALSE)
        {
            // Search for the end of the line

            for (nNextI=i;nNextI<nNumBytes;nNextI++)
            {
                if (lpbyWorkBuffer[nNextI] == 0x0D)
                    break;
            }

            // Was the end of the line found?

            if (nNextI < nNumBytes)
            {
                // Found the end of a line

                // Is the line the right length?

                if (nNextI - i == nDesiredLineLen)
                {
                    // Is it the "[Workspace]" line?

                    if (O97MemICmp((LPBYTE)"[Workspace]",
                                   lpbyWorkBuffer+i,
                                   11) == TRUE)
                    {
                        // Bingo!

                        bFound = TRUE;
                        dwOffset += i + nDesiredLineLen + 2;
                    }
                }

                i = nNextI + 2;
            }
            else
            {
                // Begin a new read

                if (i > 0)
                    dwOffset += i;
                else
                {
                    // Line > 512 bytes?

                    dwOffset = dwStreamLen;
                }

                break;
            }
        }
    }

    if (bFound == TRUE)
    {
        /////////////////////////////////////////////////////////////
        // Search for MODULENAME=
        /////////////////////////////////////////////////////////////

        // dwOffset should be pointing to the first line after the
        //  line containing "[Workspace]"

        bFound = FALSE;
        nDesiredLineLen = nModuleNameLen + 1; // 1 = strlen("=")
        while (bFound == FALSE && dwOffset < dwStreamLen)
        {
            if (dwStreamLen - dwOffset < 512)
                nNumBytes = (int)(dwStreamLen - dwOffset);
            else
                nNumBytes = 512;

            // This read should always occur at the beginning of a line

            if (SSSeekRead(lpstStream,
                           dwOffset,
                           lpbyWorkBuffer,
                           (DWORD)nNumBytes,
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != (DWORD)nNumBytes)
            {
                return(FALSE);
            }

            // Iterate through lines

            i = 0;
            while (bFound == FALSE)
            {
                // Search for the end of the line

                for (nNextI=i;nNextI<nNumBytes;nNextI++)
                {
                    if (lpbyWorkBuffer[nNextI] == 0x0D)
                        break;
                }

                // Was the end of the line found?

                if (nNextI < nNumBytes)
                {
                    // Found the end of a line

                    // Is the line the right length?

                    if (nNextI - i >= nDesiredLineLen)
                    {
                        // Is it the "MODULENAME=" line?

                        if (O97MemICmp(abyModuleName,
                                       lpbyWorkBuffer+i,
                                       nModuleNameLen) == TRUE &&
                            lpbyWorkBuffer[i+nModuleNameLen] == '=')
                        {
                            // Bingo!

                            bFound = TRUE;
                            dwMODULENAMEStartOffset = dwOffset + i;
                            dwMODULENAMEEndOffset = dwOffset + nNextI + 2;
                        }
                    }

                    i = nNextI + 2;
                }
                else
                {
                    // Begin a new read

                    if (i > 0)
                        dwOffset += i;
                    else
                    {
                        // Line > 512 bytes?

                        dwOffset = dwStreamLen;
                    }

                    break;
                }
            }
        }
    }

    if (bFound == FALSE)
    {
        // No MODULENAME= line

        dwMODULENAMEStartOffset = dwModuleEqualEndOffset;
        dwMODULENAMEEndOffset = dwModuleEqualEndOffset;
    }

    /////////////////////////////////////////////////////////////
    // Delete the found lines
    /////////////////////////////////////////////////////////////

    // Delete the "Module=MODULENAME" line

    if (dwModuleEqualEndOffset != dwModuleEqualStartOffset)
    {
        if (SSCopyBytes(lpstStream,
                        dwModuleEqualEndOffset,    // src
                        dwModuleEqualStartOffset,  // dst
                        dwMODULENAMEStartOffset - dwModuleEqualEndOffset) !=
            SS_STATUS_OK)
        {
            // Failed!

            return(FALSE);
        }
    }

    dwOffset = dwMODULENAMEStartOffset -
        (dwModuleEqualEndOffset - dwModuleEqualStartOffset);

    // Delete the "MODULENAME=" line

    if (SSCopyBytes(lpstStream,
                    dwMODULENAMEEndOffset,     // src
                    dwOffset,                  // dst
                    dwStreamLen - dwMODULENAMEEndOffset) != SS_STATUS_OK)
    {
        // Failed!

        return(FALSE);
    }

    // Calculate the new length

    dwOffset = dwStreamLen -
        (dwModuleEqualEndOffset - dwModuleEqualStartOffset) -
        (dwMODULENAMEEndOffset - dwMODULENAMEStartOffset);

    // Zero out the remaining bytes

    if (SSWriteZeroes(lpstStream,
                      dwOffset,
                      dwStreamLen - dwOffset) != SS_STATUS_OK)
    {
        // Failed!

        return(FALSE);
    }

    // Truncate the file

    if (SSSetStreamLen(lpstRoot,
                       SSStreamID(lpstStream),
                       dwOffset) != SS_STATUS_OK)
    {
        // Failed!

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97PROJECTwmRemoveModule()
//
// Description:
//  Opens the PROJECTwm stream and removes the lines single
//  byte and double byte strings of the MODULENAME.
//
//  The function also zeroes out the left over bytes of the
//  stream and truncates the stream if lines were deleted.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL O97PROJECTwmRemoveModule
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPO97_SCAN          lpstO97Scan,        // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    DWORD               dwOffset;
    DWORD               dwStreamLen;
    DWORD               dwStartOffset;
    DWORD               dwEndOffset;
    int                 nSingleByteStrLen;
    int                 nModuleNameLen;
    int                 i, nNextI;
    BOOL                bFound;
    int                 nNumBytes;
    DWORD               dwByteCount;

    // If the PROJECTwm stream doesn't exist, just ignore it

    if (lpstO97Scan->dwPROJECTwmEntry == 0xFFFFFFFF)
        return(TRUE);

    // Open the PROJECTwm stream

    if (SSOpenStreamAtIndex(lpstStream,
                            lpstO97Scan->dwPROJECTwmEntry) != SS_STATUS_OK)
    {
        // Failed to open PROJECTwm stream

        return(FALSE);
    }

    dwOffset = 0;
    dwStreamLen = SSStreamLen(lpstStream);

    // Determine the length of the module name

    nModuleNameLen = 0;
    while (abyModuleName[nModuleNameLen])
        ++nModuleNameLen;


    /////////////////////////////////////////////////////////////
    // Search for the single byte version of the string
    /////////////////////////////////////////////////////////////

    bFound = FALSE;
    while (bFound == FALSE && dwOffset < dwStreamLen)
    {
        if (dwStreamLen - dwOffset < 512)
            nNumBytes = (int)(dwStreamLen - dwOffset);
        else
            nNumBytes = 512;

        // This read should always occur at the beginning of a
        //  single byte string

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpbyWorkBuffer,
                       (DWORD)nNumBytes,
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != (DWORD)nNumBytes)
        {
            return(FALSE);
        }

        // Iterate through lines

        i = 0;
        while (bFound == FALSE)
        {
            // Search for the end of the single byte string

            for (nNextI=i;nNextI<nNumBytes;nNextI++)
            {
                if (lpbyWorkBuffer[nNextI] == 0)
                    break;
            }

            if (nNextI < nNumBytes)
            {
                nSingleByteStrLen = nNextI - i;
                ++nNextI;

                // Search for the end of the double byte string

                while (nNextI < nNumBytes)
                {
                    if (nNextI < nNumBytes)
                    {
                        if (lpbyWorkBuffer[nNextI++] == 0)
                        {
                            if (nNextI < nNumBytes)
                            {
                                if (lpbyWorkBuffer[nNextI] == 0)
                                    break;
                            }
                        }

                        ++nNextI;
                    }
                }

                if (nNextI < nNumBytes)
                {
                    ++nNextI;

                    // Found the end of the double byte string

                    // See if this is the set of strings

                    if (nSingleByteStrLen == nModuleNameLen &&
                        O97MemICmp(abyModuleName,
                                   lpbyWorkBuffer+i,
                                   nModuleNameLen) == TRUE)
                    {
                        // Bingo!

                        bFound = TRUE;
                        dwStartOffset = dwOffset + i;
                        dwEndOffset = dwOffset + nNextI;
                    }
                }
            }

            if (nNextI >= nNumBytes)
            {
                // Begin a new read

                if (i > 0)
                    dwOffset += i;
                else
                {
                    // String > 512 bytes?

                    dwOffset = dwStreamLen;
                }

                break;
            }

            i = nNextI;
        }
    }

    if (bFound == FALSE)
    {
        // Failed to find string

        return(TRUE);
    }


    /////////////////////////////////////////////////////////////
    // Delete the found strings
    /////////////////////////////////////////////////////////////

    if (SSCopyBytes(lpstStream,
                    dwEndOffset,       // src
                    dwStartOffset,     // dst
                    dwStreamLen - dwEndOffset) != SS_STATUS_OK)
    {
        // Failed!

        return(FALSE);
    }

    // Calculate the new length

    dwOffset = dwStreamLen - (dwEndOffset - dwStartOffset);

    // Zero out the remaining bytes

    if (SSWriteZeroes(lpstStream,
                      dwOffset,
                      dwStreamLen - dwOffset) != SS_STATUS_OK)
    {
        // Failed!

        return(FALSE);
    }

    // Truncate the file

    if (SSSetStreamLen(lpstRoot,
                       SSStreamID(lpstStream),
                       dwOffset) != SS_STATUS_OK)
    {
        // Failed!

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97dirRemoveModule()
//
// Description:
//  Opens the dir stream and removes the records pertaining
//  to the given module.
//
//  The function also zeroes out the left over bytes of the
//  stream and truncates the stream if lines were deleted.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL O97dirRemoveModule
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPO97_SCAN          lpstO97Scan,        // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    DWORD               dwOrigFileSize;
    DWORD               dwNewDirOffset;
    DWORD               dwOldDirOffset;

    BOOL                bEOF;
    WORD                wType;
    DWORD               dwDataSize;

    WORD                wModuleCount;

    BOOL                bSkippingModuleRecords;
    int                 nModuleNameLen;

    DWORD               dwTemp;
    WORD                wTemp;

    DWORD               dwCompressedStreamSize;
    DWORD               dwByteCount;

    // Open the dir stream

    if (SSOpenStreamAtIndex(lpstStream,
                            lpstO97Scan->dwdirEntry) != SS_STATUS_OK)
    {
        // Failed to open dir stream

        return(FALSE);
    }

    // Initialize LZNT structure for reading of the old dir stream

    if (VBA5LZNTInit(lpstStream,
                     lpstLZNT) == FALSE)
    {
        // Failed to initialize for reading of compressed dir stream

        return(FALSE);
    }

    // Get the original size of the file for truncation at the end

    if (SSFileSeek(lpstRoot->lpvRootCookie,
                   lpstRoot->lpvFile,
                   0,
                   SS_SEEK_END,
                   (LPLONG)&dwOrigFileSize) != SS_STATUS_OK)
    {
        // Failed to get file size

        return(FALSE);
    }

    if (dwOrigFileSize == (DWORD)-1)
    {
        // Error getting original file size

        return(FALSE);
    }

    // Determine the length of the module name

    nModuleNameLen = 0;
    while (abyModuleName[nModuleNameLen])
        ++nModuleNameLen;


    /////////////////////////////////////////////////////////////
    // Start writing the new uncompressed dir copy at the end
    /////////////////////////////////////////////////////////////

    dwNewDirOffset = dwOrigFileSize;

    bEOF = FALSE;
    dwOldDirOffset = 0;
    bSkippingModuleRecords = FALSE;
    while (bEOF == FALSE)
    {
        // Get the record type

        if (LZNTGetBytes(lpstLZNT,
                         dwOldDirOffset,
                         sizeof(WORD),
                         (LPBYTE)&wType) != sizeof(WORD))
        {
            // Failed getting type of record

            return(FALSE);
        }

        dwOldDirOffset += sizeof(WORD);

        // Get the record data size

        if (LZNTGetBytes(lpstLZNT,
                         dwOldDirOffset,
                         sizeof(DWORD),
                         (LPBYTE)&dwDataSize) != sizeof(DWORD))
        {
            // Failed getting type of record

            return(FALSE);
        }

        dwOldDirOffset += sizeof(DWORD);

        // Endianize

        wType = WENDIAN(wType);
        dwDataSize = DWENDIAN(dwDataSize);

        switch (wType)
        {
            case BIN_PROJ_MODULECOUNT:
            {
                /////////////////////////////////////////////////
                // Get the module count and decrement it
                /////////////////////////////////////////////////

                if (dwDataSize != sizeof(WORD))
                {
                    // Error

                    return(FALSE);
                }

                // Read the module count

                if (LZNTGetBytes(lpstLZNT,
                                 dwOldDirOffset,
                                 sizeof(WORD),
                                 (LPBYTE)&wModuleCount) != sizeof(WORD))
                {
                    // Failed getting type of record

                    return(FALSE);
                }

                dwOldDirOffset += sizeof(WORD);

                // Decrement the module count

                if (wModuleCount == 0)
                {
                    // Fatal error

                    return(FALSE);
                }

                wModuleCount = WENDIAN(wModuleCount);
                --wModuleCount;
                wModuleCount = WENDIAN(wModuleCount);

                break;
            }

            case BIN_MOD_NAME:
            {
#ifdef SYM_NLM
                SSProgress(lpstRoot->lpvRootCookie);
#endif // #ifdef SYM_NLM

                if (dwDataSize == (DWORD)nModuleNameLen)
                {
                    if (LZNTGetBytes(lpstLZNT,
                                     dwOldDirOffset,
                                     (WORD)nModuleNameLen,
                                     lpbyWorkBuffer) != (WORD)nModuleNameLen)
                    {
                        // Failed reading module name

                        return(FALSE);
                    }

                    // This could be the module

                    if (O97MemICmp(abyModuleName,
                                   lpbyWorkBuffer,
                                   nModuleNameLen) == TRUE)
                    {
                        // Bingo!

                        bSkippingModuleRecords = TRUE;
                    }
                }

                break;
            }

            case BIN_PROJ_EOF:
                bEOF = TRUE;
                break;

            default:
                break;
        }

        /////////////////////////////////////////////////////////
        // Skip record if part of module records to delete
        /////////////////////////////////////////////////////////

        if (bSkippingModuleRecords == FALSE)
        {
            if (SSFileSeek(lpstRoot->lpvRootCookie,
                           lpstRoot->lpvFile,
                           dwNewDirOffset,
                           SS_SEEK_SET,
                           NULL) != SS_STATUS_OK)
            {
                return(FALSE);
            }

            // Write type

            wTemp = WENDIAN(wType);
            if (SSFileWrite(lpstRoot->lpvRootCookie,
                            lpstRoot->lpvFile,
                            (LPVOID)&wTemp,
                            sizeof(WORD),
                            &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(WORD))
            {
                return(FALSE);
            }

            // Write data size

            dwTemp = DWENDIAN(dwDataSize);
            if (SSFileWrite(lpstRoot->lpvRootCookie,
                            lpstRoot->lpvFile,
                            (LPVOID)&dwTemp,
                            sizeof(DWORD),
                            &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(DWORD))
            {
                return(FALSE);
            }

            // Update offset in new dir stream

            dwNewDirOffset += sizeof(WORD) + sizeof(DWORD);

            // Write data

            if (wType == BIN_PROJ_MODULECOUNT)
            {
                // Write updated module count which should have
                //  already been endianized

                if (SSFileSeek(lpstRoot->lpvRootCookie,
                               lpstRoot->lpvFile,
                               dwNewDirOffset,
                               SS_SEEK_SET,
                               NULL) != SS_STATUS_OK)
                {
                    return(FALSE);
                }

                // Write type

                if (SSFileWrite(lpstRoot->lpvRootCookie,
                                lpstRoot->lpvFile,
                                (LPVOID)&wModuleCount,
                                sizeof(WORD),
                                &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(WORD))
                {
                    return(FALSE);
                }

                dwNewDirOffset += sizeof(WORD);
            }
            else
            {
                if (wType == BIN_PROJ_VERSION)
                    dwDataSize += sizeof(WORD);

                // Write in 512 byte chunks

                wTemp = 512;
                while (dwDataSize != 0)
                {
                    if (dwDataSize < 512)
                        wTemp = (WORD)dwDataSize;

                    // Read chunk

                    if (LZNTGetBytes(lpstLZNT,
                                     dwOldDirOffset,
                                     (int)wTemp,
                                     lpbyWorkBuffer) != (int)wTemp)
                    {
                        // Failed reading chunk

                        return(FALSE);
                    }

                    // Write chunk

                    if (SSFileSeek(lpstRoot->lpvRootCookie,
                                   lpstRoot->lpvFile,
                                   dwNewDirOffset,
                                   SS_SEEK_SET,
                                   NULL) != SS_STATUS_OK)
                    {
                        return(FALSE);
                    }

                    if (SSFileWrite(lpstRoot->lpvRootCookie,
                                    lpstRoot->lpvFile,
                                    (LPVOID)lpbyWorkBuffer,
                                    wTemp,
                                    &dwByteCount) != SS_STATUS_OK ||
                        dwByteCount != wTemp)
                    {
                        return(FALSE);
                    }

                    dwOldDirOffset += wTemp;
                    dwNewDirOffset += wTemp;
                    dwDataSize -= wTemp;
                }
            }
        }
        else
        {
            if (wType == BIN_MOD_END)
                bSkippingModuleRecords = FALSE;

            if (wType == BIN_PROJ_VERSION)
                dwDataSize += sizeof(WORD);

            dwOldDirOffset += dwDataSize;
        }
    }


    /////////////////////////////////////////////////////////////
    // Compress new dir stream into old dir stream
    /////////////////////////////////////////////////////////////

    if (LZNTCopyCompress(lpstLZNT,
                         dwOrigFileSize,    // Start offset
                         dwNewDirOffset-dwOrigFileSize,
                         &dwCompressedStreamSize) == FALSE)
    {
        // Failed to compress and write new dir stream

        return(FALSE);
    }

    // Set size of dir stream

    if (SSSetStreamLen(lpstRoot,
                       SSStreamID(lpstStream),
                       dwCompressedStreamSize) != SS_STATUS_OK)
    {
        // Failed!

        return(FALSE);
    }

    // Truncate

    if (SSFileSeek(lpstRoot->lpvRootCookie,
                   lpstRoot->lpvFile,
                   dwOrigFileSize,
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
    {
        // Failed to seek to end of original file

        return(FALSE);
    }

    if (SSFileTruncate(lpstRoot->lpvRootCookie,
                       lpstRoot->lpvFile) != SS_STATUS_OK)
    {
        // Failed to truncate

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97DeleteModule()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  lpstStream          Ptr to the module stream
//  lpstLZNT            Ptr to LZNT structure
//  abyModuleName       Ptr to zero-terminated module name
//  lpstO97Scan         Ptr to VBA5 project stream info
//  lpbyWorkBuffer      Ptr to work buffer >= 512 bytes
//
// Description:
//  Deletes a module from a VBA document by doing the following:
//      1. Zeroes out the stream.
//      2. Unlinks the stream's entry from the directory.
//      3. Removes references to the module from the PROJECT
//          stream based on the passed in module name.
//      4. Removes references to the module from the PROJECTwm
//          stream based on the passed in module name.
//      5. Removes references to the module from the dir
//          stream based on the passed in module name.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL O97DeleteModule
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              abyModuleName,
    LPO97_SCAN          lpstO97Scan,
    LPBYTE              lpbyWorkBuffer
)
{
    // Zero out the module's stream

    if (SSWriteZeroes(lpstStream,
                      0,
                      SSStreamLen(lpstStream)) != SS_STATUS_OK)
    {
        // Failed to zero out stream

        return(FALSE);
    }

    // Delete the module's stream

    if (SSUnlinkEntry(lpstRoot,
                      lpstO97Scan->dwVBAEntry,
                      SSStreamID(lpstStream)) != SS_STATUS_OK)
    {
        // Failed to unlink entry

        return(FALSE);
    }

    // Remove lines from the PROJECT stream

    if (O97PROJECTRemoveModule(lpstRoot,
                               lpstStream,
                               abyModuleName,
                               lpstO97Scan,
                               lpbyWorkBuffer) == FALSE)
    {
        // Failed to update PROJECT stream

        return(FALSE);
    }

    // Remove strings from PROJECTwm stream

    if (O97PROJECTwmRemoveModule(lpstRoot,
                                 lpstStream,
                                 abyModuleName,
                                 lpstO97Scan,
                                 lpbyWorkBuffer) == FALSE)
    {
        // Failed to update PROJECTwm stream

        return(FALSE);
    }

    // Remove module info from dir stream

    if (lpstO97Scan->dwdirEntry != 0xFFFFFFFF)
    {
        if (O97dirRemoveModule(lpstRoot,
                               lpstStream,
                               lpstLZNT,
                               abyModuleName,
                               lpstO97Scan,
                               lpbyWorkBuffer) == FALSE)
        {
            // Failed to update dir stream

            return(FALSE);
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  int O97Open__SRP_CB()
//
// Description:
//  Checks the entry to see if it is a stream with a name
//  that begins with __SRP_.  If so then the function returns
//  SS_ENUM_CB_STATUS_RETURN.
//
// Returns:
//  SS_ENUM_CB_STATUS_RETURN        If the entry's name is __SRP_*
//  SS_ENUM_CB_STATUS_CONTINUE      If the entry is not
//
//********************************************************************

int O97Open__SRP_CB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;
    (void)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Does the stream name begin with __SRP_?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywsz__SRP_,
                       6) == 0)
        {
            // Found a stream whose name begins with __SRP_

            return(SS_ENUM_CB_STATUS_RETURN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  int O97Open__SRP_0CB()
//
// Description:
//  Checks the entry to see if it is a stream with the name
//  __SRP_0.  If so then the function returns
//  SS_ENUM_CB_STATUS_OPEN.
//
// Returns:
//  SS_ENUM_CB_STATUS_OPEN          If the entry's name is __SRP_0
//  SS_ENUM_CB_STATUS_CONTINUE      If the entry is not
//
//********************************************************************

int O97Open__SRP_0CB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;
    (void)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Is it the __SRP_0 stream?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywsz__SRP_0,
                       SS_MAX_NAME_LEN) == 0)
        {
            // Found a stream whose name begins with __SRP_0

            return(SS_ENUM_CB_STATUS_OPEN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL O97UpdateForRecompilation()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  lpstStream          Ptr to allocated stream structure to use
//  lpstO97Scan         Ptr to initialized O97_SCAN_T structure
//
// Description:
//  The function does the following:
//      1. Zeroes out the __SRP_0 stream
//      2. Deletes the __SRP_0 stream
//      3. Increments the second USHORT of the _VBA_PROJECT stream
//
//  The function calls O97Get_VBA_PROJECTEndian() to determine the
//  endianness of the _VBA_PROJECT stream in order to determine
//  how to increment the second USHORT.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL O97UpdateForRecompilation
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPO97_SCAN          lpstO97Scan
)
{
    BYTE                abySecondUSHORT[2];
    LPSS_ENUM_SIBS      lpstSibs;
    DWORD               dwByteCount;

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Delete __SRP_0 stream
    /////////////////////////////////////////////////////////////

    // Get updated child of VBA storage

    SSInitEnumSibsStruct(lpstSibs,
                         lpstO97Scan->dwVBAChildEntry);

    if (SSEnumSiblingEntriesCB(lpstRoot,
                               O97Open__SRP_0CB,
                               NULL,
                               lpstSibs,
                               lpstStream) == SS_STATUS_OK)
    {
        // Zero out stream

        if (SSWriteZeroes(lpstStream,
                          0,
                          SSStreamLen(lpstStream)) != SS_STATUS_OK)
        {
            // Failed to zero out stream

            SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
            return(FALSE);
        }

        // Unlink stream

        if (SSUnlinkEntry(lpstRoot,
                          lpstO97Scan->dwVBAEntry,
                          SSStreamID(lpstStream)) != SS_STATUS_OK)
        {
            // Failed to unlink entry

            SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
            return(FALSE);
        }

        // Update the VBA child entry number

        if (SSGetChildOfStorage(lpstRoot,
                                lpstO97Scan->dwVBAEntry,
                                &lpstO97Scan->dwVBAChildEntry) != SS_STATUS_OK)
        {
            // Error getting the child of the VBA storage

            SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
            return(FALSE);
        }
    }

    // Free sibling enumeration structure

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);


    /////////////////////////////////////////////////////////////
    // Increment second ushort of _VBA_PROJECT stream
    /////////////////////////////////////////////////////////////

    // Open the _VBA_PROJECT stream

    if (SSOpenStreamAtIndex(lpstStream,
                            lpstO97Scan->dw_VBA_PROJECTEntry) !=
        SS_STATUS_OK)
    {
        // Failed to open _VBA_PROJECT stream

        return(FALSE);
    }

    // Read the second ushort

    if (SSSeekRead(lpstStream,
                   2,
                   (LPBYTE)abySecondUSHORT,
                   2 * sizeof(BYTE),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != 2 * sizeof(BYTE))
    {
        // Failed to read ushort

        return(FALSE);
    }

    if (abySecondUSHORT[0]++ == 0xFF)
        abySecondUSHORT[1]++;

    // Write back the incremented second ushort

    if (SSSeekWrite(lpstStream,
                    2,
                    (LPBYTE)&abySecondUSHORT,
                    2 * sizeof(BYTE),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != 2 * sizeof(BYTE))
    {
        // Failed to write ushort

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97CountNonEmptyModules()
//
// Description:
//  Iterates through each module in the given VBA substorage
//  and increments the module count if it contains lines other
//  than Attribute lines.
//
// Returns:
//  TRUE        On success
//  FALSE       Otherwise, including error
//
//********************************************************************

BOOL O97CountNonEmptyModules
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    DWORD               dwVBAChildEntry,    // Entry number of VBA child
    LPWORD              lpwModuleCount      // Ptr to WORD for count
)
{
    LPSS_ENUM_SIBS      lpstSibs;
    DWORD               dwFirstStreamID;

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    // Count number of non-empty modules

    dwFirstStreamID = 0;
    *lpwModuleCount = 0;
    SSInitEnumSibsStruct(lpstSibs,dwVBAChildEntry);
    while (SSEnumSiblingEntriesCB(lpstRoot,
                                  O97OpenVBA5ModuleCB,
                                  NULL,
                                  lpstSibs,
                                  lpstStream) == SS_STATUS_OK)
    {
        if (dwFirstStreamID == 0)
            dwFirstStreamID = SSStreamID(lpstStream);
        else
        if (SSStreamID(lpstStream) == dwFirstStreamID)
            break;

        // Determine whether the stream consists of only
        //  Attribute lines

        if (VBA5LZNTModuleInit(lpstStream,
                               lpstLZNT) == FALSE)
        {
            // Failed to initialize for reading source text string

            SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
            return(FALSE);
        }

        if (O97LZNTStreamAttributeOnly(lpstLZNT) == FALSE)
        {
            // The stream contains non-attribute lines

            if (*lpwModuleCount < 0xFFFF)
                ++*lpwModuleCount;
        }
    }

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
    return(TRUE);
}


//********************************************************************
//
// Function:
//  void O97ModuleSourceCRCInit()
//
// Parameters:
//  lpstState       Ptr to current CRC state structure
//
// Description:
//  Initializes the CRC state structure.
//
// Returns:
//  Nothing
//
//********************************************************************

void O97ModuleSourceCRCInit
(
    LPO97_CRC_STATE     lpstState
)
{
    lpstState->eState = eO97_CRC_STATE_BOL;
    lpstState->dwSubState = 0;
    CRC32Init(lpstState->dwCRC);
    lpstState->dwByteCount = 0;
}


//********************************************************************
//
// Function:
//  void O97ModuleSourceCRCContinue()
//
// Parameters:
//  lpstState       Ptr to current CRC state structure
//  byCurByte       Current byte to add to the CRC
//
// Description:
//  Continues the CRC32 using byCurByte.  Lines beginning
//  with attribute and blank lines consisting of only spaces are
//  skipped.  All characters 'A' through 'Z' are lowercased before
//  being CRCed.
//
// Returns:
//  Nothing
//
//********************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyO97attribute[10] =
{
    'a', 't', 't', 'r', 'i', 'b', 'u', 't', 'e', ' '
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

void O97ModuleSourceCRCContinue
(
    LPO97_CRC_STATE     lpstState,
    BYTE                byCurByte
)
{
    DWORD               dw;

    if (byCurByte <= 'Z' && byCurByte >= 'A')
        byCurByte = byCurByte + ('a' - 'A');

    switch (lpstState->eState)
    {
        case eO97_CRC_STATE_BOL:
            if (byCurByte == gabyO97attribute[0])
            {
                // Could it be an attribute line

                lpstState->eState = eO97_CRC_STATE_ATTRIBUTE;
                lpstState->dwSubState = 1;
            }
            else
            if (byCurByte == ' ')
            {
                // Could it be a line of spaces?

                lpstState->eState = eO97_CRC_STATE_SPACES;
                lpstState->dwSubState = 1;
            }
            else
            if (byCurByte == 0x0D)
            {
                lpstState->eState = eO97_CRC_STATE_SPACES;
                lpstState->dwSubState = 0;
            }
            else
            {
                // It is neither a line of spaces nor an
                //  attribute line

                lpstState->eState = eO97_CRC_STATE_NORMAL;

                lpstState->dwByteCount++;
                CRC32Continue(lpstState->dwCRC,byCurByte);
            }
            break;

        case eO97_CRC_STATE_ATTRIBUTE:
            if (byCurByte == gabyO97attribute[lpstState->dwSubState])
            {
                if (++(lpstState->dwSubState) == sizeof(gabyO97attribute))
                {
                    // It is an attribute line

                    lpstState->eState = eO97_CRC_STATE_SKIP_LINE;
                }
            }
            else
            {
                // CRC the attribute bytes seen thus far
                //  and then continue normally

                lpstState->dwByteCount += lpstState->dwSubState + 1;

                for (dw=0;dw<lpstState->dwSubState;dw++)
                    CRC32Continue(lpstState->dwCRC,gabyO97attribute[dw]);

                CRC32Continue(lpstState->dwCRC,byCurByte);

                lpstState->eState = eO97_CRC_STATE_NORMAL;
            }
            break;

        case eO97_CRC_STATE_SPACES:
            switch (byCurByte)
            {
                case 0x20:
                    if (lpstState->dwSubState)
                        lpstState->dwSubState++;
                    break;

                case 0x0D:
                    // Assumption is that 0x0D always
                    //  precedes 0x0A
                    break;

                case 0x0A:
                    // Saw a line consisting only of spaces

                    // Reset to start state

                    lpstState->eState = eO97_CRC_STATE_BOL;
                    break;

                default:
                    // Found a non-space character in a supposed
                    //  space line

                    lpstState->dwByteCount += lpstState->dwSubState + 1;

                    // CRC all the spaces and return to normal

                    while (lpstState->dwSubState-- != 0)
                        CRC32Continue(lpstState->dwCRC,0x20);

                    CRC32Continue(lpstState->dwCRC,byCurByte);

                    lpstState->eState = eO97_CRC_STATE_NORMAL;
                    break;
            }
            break;

        case eO97_CRC_STATE_NORMAL:
            // CRC the byte

            lpstState->dwByteCount++;
            CRC32Continue(lpstState->dwCRC,byCurByte);

            if (byCurByte == 0x0A)
            {
                // Assumption is that 0x0A always ends a line

                lpstState->eState = eO97_CRC_STATE_BOL;
            }
            break;

        case eO97_CRC_STATE_SKIP_LINE:
            // Skip the byte

            if (byCurByte == 0x0A)
            {
                // Assumption is that 0x0A always ends a line

                lpstState->eState = eO97_CRC_STATE_BOL;
            }
            break;

        default:
            // This should never happen

            break;
    }
}



//********************************************************************
//
// Function:
//  BOOL O97ModuleSourceCRC()
//
// Parameters:
//  lpstStream      Ptr to open module stream
//  lpstLZNT        Ptr to created LZNT structure
//  lpdwCRC         Ptr to DWORD to store CRC (NULL if not wanted)
//  lpdwByteCount   Ptr to DWORD for byte count (NULL if not wanted)
//
// Description:
//  Takes the CRC32 of the module source text.  Lines beginning
//  with attribute and blank lines consisting of only spaces are
//  skipped.  All characters 'A' through 'Z' are lowercased before
//  being CRCed.
//
//  The byte count is the number of bytes CRCed, not including bytes
//  that were skipped.
//
// Returns:
//  TRUE        On success
//  FALSE       Otherwise, including error
//
//********************************************************************

BOOL O97ModuleSourceCRC
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwByteCount
)
{
    BYTE                abyWorkBuffer[512];
    int                 i, nNumBytesToRead;
    DWORD               dwOffset;
    O97_CRC_STATE_T     stCRCState;

    // Initialize the module for LZNT access

    if (VBA5LZNTModuleInit(lpstStream,
                           lpstLZNT) == FALSE)
        return(FALSE);

    // Initialize the CRC

    O97ModuleSourceCRCInit(&stCRCState);

    dwOffset = 0;
    nNumBytesToRead = 512;
    while (dwOffset < lpstLZNT->dwSize)
    {
        if (lpstLZNT->dwSize - dwOffset < (DWORD)nNumBytesToRead)
            nNumBytesToRead = (int) (lpstLZNT->dwSize - dwOffset);

        // Get a buffer of bytes

        if (LZNTGetBytes(lpstLZNT,
                         dwOffset,
                         nNumBytesToRead,
                         abyWorkBuffer) != nNumBytesToRead)
            return(FALSE);

        // CRC the bytes

        for (i=0;i<nNumBytesToRead;i++)
            O97ModuleSourceCRCContinue(&stCRCState,abyWorkBuffer[i]);

        dwOffset += nNumBytesToRead;
    }

    if (lpdwCRC != NULL)
        *lpdwCRC = stCRCState.dwCRC;

    if (lpdwByteCount != NULL)
        *lpdwByteCount = stCRCState.dwByteCount;

    return(TRUE);
}

//********************************************************************
//
// Function:
//  void O97ModuleSourceCRCContinue2()
//
// Parameters:
//  lpstState       Ptr to current CRC state structure
//  byCurByte       Current byte to add to the CRC
//
// Description:
//  Continues the CRC32 using byCurByte.
//  byCurByte is mapped to bytoCRC using gabyO97CRCCharMap array
//
//  The following items are not CRCed:
//  - Lines beginning with ATTRIBUTE
//  - Lines beginning with OPTION EXPLICIT
//  - white spaces
//  - line-continuation mark
//  - any byCurByte that's mapped into '?'
//
// Returns:
//  Nothing
//
//********************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyO97CRCCharMap[256] =
{
    0x3F,   // 0x00: Convert to '?'
    0x3F,   // 0x01: Convert to '?'
    0x3F,   // 0x02: Convert to '?'
    0x3F,   // 0x03: Convert to '?'
    0x3F,   // 0x04: Convert to '?'
    0x3F,   // 0x05: Convert to '?'
    0x3F,   // 0x06: Convert to '?'
    0x3F,   // 0x07: Convert to '?'
    0x3F,   // 0x08: Convert to '?'
    0x3F,   // 0x09: Convert to '?'
    0x3F,   // 0x0A: Convert to '?'
    0x3F,   // 0x0B: Convert to '?'
    0x3F,   // 0x0C: Convert to '?'
    0x3F,   // 0x0D: Convert to '?'
    0x3F,   // 0x0E: Convert to '?'
    0x3F,   // 0x0F: Convert to '?'
    0x3F,   // 0x10: Convert to '?'
    0x3F,   // 0x11: Convert to '?'
    0x3F,   // 0x12: Convert to '?'
    0x3F,   // 0x13: Convert to '?'
    0x3F,   // 0x14: Convert to '?'
    0x3F,   // 0x15: Convert to '?'
    0x3F,   // 0x16: Convert to '?'
    0x3F,   // 0x17: Convert to '?'
    0x3F,   // 0x18: Convert to '?'
    0x3F,   // 0x19: Convert to '?'
    0x3F,   // 0x1A: Convert to '?'
    0x3F,   // 0x1B: Convert to '?'
    0x3F,   // 0x1C: Convert to '?'
    0x3F,   // 0x1D: Convert to '?'
    0x3F,   // 0x1E: Convert to '?'
    0x3F,   // 0x1F: Convert to '?'
    0x20,   // 0x20: ' ' Space
    0x21,   // 0x21: '!' Exclamation point
    0x22,   // 0x22: '"' Quotation mark
    0x23,   // 0x23: '#' Number sign
    0x24,   // 0x24: '$' Dollar sign
    0x25,   // 0x25: '%' Percent sign
    0x26,   // 0x26: '&' Ampersand
    0x27,   // 0x27: ''' Apostrophe
    0x28,   // 0x28: '(' Opening parenthesis
    0x29,   // 0x29: ')' Closing parenthesis
    0x2A,   // 0x2A: '*' Asterisk
    0x2B,   // 0x2B: '+' Plus sign
    0x2C,   // 0x2C: ',' Comma
    0x2D,   // 0x2D: '-' Hyphen or minus sign
    0x2E,   // 0x2E: '.' Period
    0x2F,   // 0x2F: '/' Slash
    0x30,   // 0x30: '0'
    0x31,   // 0x31: '1'
    0x32,   // 0x32: '2'
    0x33,   // 0x33: '3'
    0x34,   // 0x34: '4'
    0x35,   // 0x35: '5'
    0x36,   // 0x36: '6'
    0x37,   // 0x37: '7'
    0x38,   // 0x38: '8'
    0x39,   // 0x39: '9'
    0x3A,   // 0x3A: ':' Colon
    0x3B,   // 0x3B: ';' Semicolon
    0x3C,   // 0x3C: '<' Less than sign
    0x3D,   // 0x3D: '=' Equal sign
    0x3E,   // 0x3E: '>' Greater than sign
    0x3F,   // 0x3F: '?' Question mark
    0x40,   // 0x40: '@' At sign
    0x61,   // 0x41: Convert from 'A' to 'a'
    0x62,   // 0x42: Convert from 'B' to 'b'
    0x63,   // 0x43: Convert from 'C' to 'c'
    0x64,   // 0x44: Convert from 'D' to 'd'
    0x65,   // 0x45: Convert from 'E' to 'e'
    0x66,   // 0x46: Convert from 'F' to 'f'
    0x67,   // 0x47: Convert from 'G' to 'g'
    0x68,   // 0x48: Convert from 'H' to 'h'
    0x69,   // 0x49: Convert from 'I' to 'i'
    0x6A,   // 0x4A: Convert from 'J' to 'j'
    0x6B,   // 0x4B: Convert from 'K' to 'k'
    0x6C,   // 0x4C: Convert from 'L' to 'l'
    0x6D,   // 0x4D: Convert from 'M' to 'm'
    0x6E,   // 0x4E: Convert from 'N' to 'n'
    0x6F,   // 0x4F: Convert from 'O' to 'o'
    0x70,   // 0x50: Convert from 'P' to 'p'
    0x71,   // 0x51: Convert from 'Q' to 'q'
    0x72,   // 0x52: Convert from 'R' to 'r'
    0x73,   // 0x53: Convert from 'S' to 's'
    0x74,   // 0x54: Convert from 'T' to 't'
    0x75,   // 0x55: Convert from 'U' to 'u'
    0x76,   // 0x56: Convert from 'V' to 'v'
    0x77,   // 0x57: Convert from 'W' to 'w'
    0x78,   // 0x58: Convert from 'X' to 'x'
    0x79,   // 0x59: Convert from 'Y' to 'y'
    0x7A,   // 0x5A: Convert from 'Z' to 'z'
    0x5B,   // 0x5B: Opening bracket '['
    0x5C,   // 0x5C: '\' Backward slash
    0x5D,   // 0x5D: ']' Closing bracket
    0x5E,   // 0x5E: '^' Caret
    0x5F,   // 0x5F: '_' Underscore
    0x60,   // 0x60: '`' Grave
    0x61,   // 0x61: 'a'
    0x62,   // 0x62: 'b'
    0x63,   // 0x63: 'c'
    0x64,   // 0x64: 'd'
    0x65,   // 0x65: 'e'
    0x66,   // 0x66: 'f'
    0x67,   // 0x67: 'g'
    0x68,   // 0x68: 'h'
    0x69,   // 0x69: 'i'
    0x6A,   // 0x6A: 'j'
    0x6B,   // 0x6B: 'k'
    0x6C,   // 0x6C: 'l'
    0x6D,   // 0x6D: 'm'
    0x6E,   // 0x6E: 'n'
    0x6F,   // 0x6F: 'o'
    0x70,   // 0x70: 'p'
    0x71,   // 0x71: 'q'
    0x72,   // 0x72: 'r'
    0x73,   // 0x73: 's'
    0x74,   // 0x74: 't'
    0x75,   // 0x75: 'u'
    0x76,   // 0x76: 'v'
    0x77,   // 0x77: 'w'
    0x78,   // 0x78: 'x'
    0x79,   // 0x79: 'y'
    0x7A,   // 0x7A: 'z'
    0x7B,   // 0x7B: '{' Opening brace
    0x7C,   // 0x7C: '|' Vertical line
    0x7D,   // 0x7D: '}' Closing brace
    0x7E,   // 0x7E: '~' Tilde
    0x3F,   // 0x7F: Convert to '?'
    0x3F,   // 0x80: Convert to '?'
    0x3F,   // 0x81: Convert to '?'
    0x3F,   // 0x82: Convert to '?'
    0x3F,   // 0x83: Convert to '?'
    0x3F,   // 0x84: Convert to '?'
    0x3F,   // 0x85: Convert to '?'
    0x3F,   // 0x86: Convert to '?'
    0x3F,   // 0x87: Convert to '?'
    0x3F,   // 0x88: Convert to '?'
    0x3F,   // 0x89: Convert to '?'
    0x3F,   // 0x8A: Convert to '?'
    0x3F,   // 0x8B: Convert to '?'
    0x3F,   // 0x8C: Convert to '?'
    0x3F,   // 0x8D: Convert to '?'
    0x3F,   // 0x8E: Convert to '?'
    0x3F,   // 0x8F: Convert to '?'
    0x3F,   // 0x90: Convert to '?'
    0x3F,   // 0x91: Convert to '?'
    0x3F,   // 0x92: Convert to '?'
    0x3F,   // 0x93: Convert to '?'
    0x3F,   // 0x94: Convert to '?'
    0x3F,   // 0x95: Convert to '?'
    0x3F,   // 0x96: Convert to '?'
    0x3F,   // 0x97: Convert to '?'
    0x3F,   // 0x98: Convert to '?'
    0x3F,   // 0x99: Convert to '?'
    0x3F,   // 0x9A: Convert to '?'
    0x3F,   // 0x9B: Convert to '?'
    0x3F,   // 0x9C: Convert to '?'
    0x3F,   // 0x9D: Convert to '?'
    0x3F,   // 0x9E: Convert to '?'
    0x3F,   // 0x9F: Convert to '?'
    0x3F,   // 0xA0: Convert to '?'
    0x3F,   // 0xA1: Convert to '?'
    0x3F,   // 0xA2: Convert to '?'
    0x3F,   // 0xA3: Convert to '?'
    0x3F,   // 0xA4: Convert to '?'
    0x3F,   // 0xA5: Convert to '?'
    0x3F,   // 0xA6: Convert to '?'
    0x3F,   // 0xA7: Convert to '?'
    0x3F,   // 0xA8: Convert to '?'
    0x3F,   // 0xA9: Convert to '?'
    0x3F,   // 0xAA: Convert to '?'
    0x3F,   // 0xAB: Convert to '?'
    0x3F,   // 0xAC: Convert to '?'
    0x3F,   // 0xAD: Convert to '?'
    0x3F,   // 0xAE: Convert to '?'
    0x3F,   // 0xAF: Convert to '?'
    0x3F,   // 0xB0: Convert to '?'
    0x3F,   // 0xB1: Convert to '?'
    0x3F,   // 0xB2: Convert to '?'
    0x3F,   // 0xB3: Convert to '?'
    0x3F,   // 0xB4: Convert to '?'
    0x3F,   // 0xB5: Convert to '?'
    0x3F,   // 0xB6: Convert to '?'
    0x3F,   // 0xB7: Convert to '?'
    0x3F,   // 0xB8: Convert to '?'
    0x3F,   // 0xB9: Convert to '?'
    0x3F,   // 0xBA: Convert to '?'
    0x3F,   // 0xBB: Convert to '?'
    0x3F,   // 0xBC: Convert to '?'
    0x3F,   // 0xBD: Convert to '?'
    0x3F,   // 0xBE: Convert to '?'
    0x3F,   // 0xBF: Convert to '?'
    0x3F,   // 0xC0: Convert to '?'
    0x3F,   // 0xC1: Convert to '?'
    0x3F,   // 0xC2: Convert to '?'
    0x3F,   // 0xC3: Convert to '?'
    0x3F,   // 0xC4: Convert to '?'
    0x3F,   // 0xC5: Convert to '?'
    0x3F,   // 0xC6: Convert to '?'
    0x3F,   // 0xC7: Convert to '?'
    0x3F,   // 0xC8: Convert to '?'
    0x3F,   // 0xC9: Convert to '?'
    0x3F,   // 0xCA: Convert to '?'
    0x3F,   // 0xCB: Convert to '?'
    0x3F,   // 0xCC: Convert to '?'
    0x3F,   // 0xCD: Convert to '?'
    0x3F,   // 0xCE: Convert to '?'
    0x3F,   // 0xCF: Convert to '?'
    0x3F,   // 0xD0: Convert to '?'
    0x3F,   // 0xD1: Convert to '?'
    0x3F,   // 0xD2: Convert to '?'
    0x3F,   // 0xD3: Convert to '?'
    0x3F,   // 0xD4: Convert to '?'
    0x3F,   // 0xD5: Convert to '?'
    0x3F,   // 0xD6: Convert to '?'
    0x3F,   // 0xD7: Convert to '?'
    0x3F,   // 0xD8: Convert to '?'
    0x3F,   // 0xD9: Convert to '?'
    0x3F,   // 0xDA: Convert to '?'
    0x3F,   // 0xDB: Convert to '?'
    0x3F,   // 0xDC: Convert to '?'
    0x3F,   // 0xDD: Convert to '?'
    0x3F,   // 0xDE: Convert to '?'
    0x3F,   // 0xDF: Convert to '?'
    0x3F,   // 0xE0: Convert to '?'
    0x3F,   // 0xE1: Convert to '?'
    0x3F,   // 0xE2: Convert to '?'
    0x3F,   // 0xE3: Convert to '?'
    0x3F,   // 0xE4: Convert to '?'
    0x3F,   // 0xE5: Convert to '?'
    0x3F,   // 0xE6: Convert to '?'
    0x3F,   // 0xE7: Convert to '?'
    0x3F,   // 0xE8: Convert to '?'
    0x3F,   // 0xE9: Convert to '?'
    0x3F,   // 0xEA: Convert to '?'
    0x3F,   // 0xEB: Convert to '?'
    0x3F,   // 0xEC: Convert to '?'
    0x3F,   // 0xED: Convert to '?'
    0x3F,   // 0xEE: Convert to '?'
    0x3F,   // 0xEF: Convert to '?'
    0x3F,   // 0xF0: Convert to '?'
    0x3F,   // 0xF1: Convert to '?'
    0x3F,   // 0xF2: Convert to '?'
    0x3F,   // 0xF3: Convert to '?'
    0x3F,   // 0xF4: Convert to '?'
    0x3F,   // 0xF5: Convert to '?'
    0x3F,   // 0xF6: Convert to '?'
    0x3F,   // 0xF7: Convert to '?'
    0x3F,   // 0xF8: Convert to '?'
    0x3F,   // 0xF9: Convert to '?'
    0x3F,   // 0xFA: Convert to '?'
    0x3F,   // 0xFB: Convert to '?'
    0x3F,   // 0xFC: Convert to '?'
    0x3F,   // 0xFD: Convert to '?'
    0x3F,   // 0xFE: Convert to '?'
    0x3F    // 0xFF: Convert to '?'
};

BYTE FAR gabyO97optionexplicit[15] =
{
    'o', 'p', 't', 'i', 'o', 'n', ' ', 'e', 'x', 'p', 'l', 'i', 'c', 'i', 't'
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

void O97ModuleSourceCRCContinue2
(
    LPO97_CRC_STATE     lpstState,
    BYTE                byCurByte
)
{
    DWORD               dw;
    BOOL                bOKtoCRC;
    BYTE                bytoCRC;

    bytoCRC = gabyO97CRCCharMap[byCurByte];

    bOKtoCRC = TRUE;
    if ((bytoCRC == 0x3F) || (bytoCRC == 0x20))
        bOKtoCRC = FALSE; //anything marked as 0x3F is not CRC'ed.


    switch (lpstState->eState)
    {
        case eO97_CRC_STATE_BOL:
            if (bytoCRC == gabyO97attribute[0])
            {
                // Could it be an attribute line?

                lpstState->eState = eO97_CRC_STATE_ATTRIBUTE;
                lpstState->dwSubState = 1;
            }
            else
            if (bytoCRC == gabyO97optionexplicit[0])
            {
                // Could it be an OPTION EXPLICIT line?

                lpstState->eState = eO97_CRC_STATE_OPTIONEXPLICIT;
                lpstState->dwSubState = 1;
            }
            else
            if (byCurByte == ' ')
            {
                // Could it be a beginning of " _" ?

                lpstState->eState = eO97_CRC_STATE_LINECONTINUE;
                lpstState->dwSubState = 1;
            }
            else
            if ((byCurByte == 0x0D) || (byCurByte == 0x0A))
            {
                // Could it be a blank line?

                lpstState->eState = eO97_CRC_STATE_BOL;
            }
            else
            {
                // It is not a " _" nor
                //  attribute line nor
                //  option explicit line nor
                //  blank lines,
                // so continue normally and CRC the byte

                lpstState->eState = eO97_CRC_STATE_NORMAL;
                if (bOKtoCRC)
                {
                    lpstState->dwByteCount++;
                    CRC32Continue(lpstState->dwCRC,bytoCRC);
                }
            }
            break;

        case eO97_CRC_STATE_ATTRIBUTE:
            if (bytoCRC == gabyO97attribute[lpstState->dwSubState])
            {
                // still matching attribute line.

                if (++(lpstState->dwSubState) == sizeof(gabyO97attribute))
                {
                    // It is an attribute line

                    lpstState->eState = eO97_CRC_STATE_SKIP_LINE;
                }
            }
            else
            {
                // CRC the attribute bytes seen thus far

                lpstState->dwByteCount += lpstState->dwSubState;

                for (dw=0;dw<lpstState->dwSubState;dw++)
                {
                    CRC32Continue(lpstState->dwCRC,gabyO97attribute[dw]);
                }

                // CRC the current byte if not white space
                //  and then continue normally

                lpstState->eState = eO97_CRC_STATE_NORMAL;
                if(bOKtoCRC)
                {
                    lpstState->dwByteCount++;
                    CRC32Continue(lpstState->dwCRC,bytoCRC);
                }
                else
                if (byCurByte == ' ')
                // Could it be a beginning of " _" ???
                {
                    lpstState->eState = eO97_CRC_STATE_LINECONTINUE;
                    lpstState->dwSubState = 1;
                }
                else
                if (byCurByte == 0x0D)
                // It's end of line
                {
                    lpstState->eState = eO97_CRC_STATE_BOL;
                }

            }
            break;

        case eO97_CRC_STATE_OPTIONEXPLICIT:
            if (bytoCRC == gabyO97optionexplicit[lpstState->dwSubState])
            {
                // still matching option explicit

                if (++(lpstState->dwSubState) == sizeof(gabyO97optionexplicit))
                {
                    // It is an OPTION EXPLICIT line

                    lpstState->eState = eO97_CRC_STATE_SKIP_LINE;
                }
            }
            else
            {
                // CRC the optionexplicit bytes seen thus far

                lpstState->dwByteCount += lpstState->dwSubState;

                for (dw=0;dw<lpstState->dwSubState;dw++)
                {
                    if (dw != 6) // the sixth one is ' ' so don't CRC it
                    {
                       CRC32Continue(lpstState->dwCRC,gabyO97optionexplicit[dw]);
                    }
                    else
                    {
                        lpstState->dwByteCount --;
                    }
                }

                // CRC the current byte if not white space
                //  and then continue normally

                lpstState->eState = eO97_CRC_STATE_NORMAL;
                if(bOKtoCRC)
                {
                    lpstState->dwByteCount++;
                    CRC32Continue(lpstState->dwCRC,bytoCRC);
                }
                else
                if (byCurByte == ' ')
                // Could it be a beginning of " _" ???
                {
                    lpstState->eState = eO97_CRC_STATE_LINECONTINUE;
                    lpstState->dwSubState = 1;
                }
                else
                if (byCurByte == 0x0D)
                // it's end of line
                {
                    lpstState->eState = eO97_CRC_STATE_BOL;
                }

            }
            break;

        case eO97_CRC_STATE_NORMAL:

            // CRC the byte if not white space

            if(bOKtoCRC)
            {
                lpstState->dwByteCount++;
                CRC32Continue(lpstState->dwCRC,bytoCRC);
            }
            else
            // Could it be a beginning of " _" ???
            if (byCurByte == ' ')
            {
                lpstState->eState = eO97_CRC_STATE_LINECONTINUE;
                lpstState->dwSubState = 1;
            }
            else
            // see if the line ends
            if (byCurByte == 0x0D)
            {
                // it's end of line
                lpstState->eState = eO97_CRC_STATE_BOL;
            }
            break;

        case eO97_CRC_STATE_LINECONTINUE:
            switch (byCurByte)
            {
                case '_':
                    if (lpstState->dwSubState == 1)
                        lpstState->dwSubState++;
                    break;

                case 0x0D:
                    // we've either found " _" or " " at the end of a line
                    // either way, we don't want to CRC it,
                    // so go to new line

                    lpstState->dwSubState = 0;
                    lpstState->eState = eO97_CRC_STATE_BOL;
                    break;

                default:
                    // CRC the previously seen _ if needed

                    if (lpstState->dwSubState == 2)
                    {
                        lpstState->dwByteCount++;
                        CRC32Continue(lpstState->dwCRC,'_');
                    }

                    // CRC the current byte if not whitespace

                    if (bOKtoCRC)
                    {
                        lpstState->dwByteCount ++;
                        CRC32Continue(lpstState->dwCRC,bytoCRC);
                    }

                    //Continue normally

                    lpstState->dwSubState = 0;
                    lpstState->eState = eO97_CRC_STATE_NORMAL;
                    break;
            }
            break;

        case eO97_CRC_STATE_SKIP_LINE:
            // Skip the byte

            if (byCurByte == 0x0D)
            {
                // it's either EOL or 0x0A follows right after this.

                lpstState->eState = eO97_CRC_STATE_BOL;
            }
            break;

        default:
            // This should never happen

            break;
    }
}

//********************************************************************
//
// Function:
//  BOOL O97ModuleSourceCRC2()
//
// Parameters:
//  lpstStream      Ptr to open module stream
//  lpstLZNT        Ptr to created LZNT structure
//  lpdwCRC         Ptr to DWORD to store CRC (NULL if not wanted)
//  lpdwByteCount   Ptr to DWORD for byte count (NULL if not wanted)
//
// Description:
//  Takes the CRC32 of the module source text.
//
//  The byte count is the number of bytes CRCed, not including bytes
//  that are skipped. See O97ModuleSourceCRCContinue2() above for
//  details on bytes that are skipped.
//
// Returns:
//  TRUE        On success
//  FALSE       Otherwise, including error
//
//********************************************************************

BOOL O97ModuleSourceCRC2
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwByteCount
)
{
    BYTE                abyWorkBuffer[512];
    int                 i, nNumBytesToRead;
    DWORD               dwOffset;
    O97_CRC_STATE_T     stCRCState;

    // Initialize the module for LZNT access

    if (VBA5LZNTModuleInit(lpstStream,
                           lpstLZNT) == FALSE)
        return(FALSE);

    // Initialize the CRC

    O97ModuleSourceCRCInit(&stCRCState);

    dwOffset = 0;
    nNumBytesToRead = 512;
    while (dwOffset < lpstLZNT->dwSize)
    {
        if (lpstLZNT->dwSize - dwOffset < (DWORD)nNumBytesToRead)
            nNumBytesToRead = (int) (lpstLZNT->dwSize - dwOffset);

        // Get a buffer of bytes

        if (LZNTGetBytes(lpstLZNT,
                         dwOffset,
                         nNumBytesToRead,
                         abyWorkBuffer) != nNumBytesToRead)
            return(FALSE);

        // CRC the bytes

        for (i=0;i<nNumBytesToRead;i++)
            O97ModuleSourceCRCContinue2(&stCRCState,abyWorkBuffer[i]);

        dwOffset += nNumBytesToRead;
    }

    if (lpdwCRC != NULL)
        *lpdwCRC = stCRCState.dwCRC;

    if (lpdwByteCount != NULL)
        *lpdwByteCount = stCRCState.dwByteCount;

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97OpenModuleStreamByName()
//
// Parameters:
//  lpstRoot        Ptr to root structure
//  lpstStream      Stream structure to use
//  dwVBAChildEntry Entry of first VBA child
//  lpbyName        Name to search for
//
// Description:
//  The function performs a case-insensitive search for the given
//  module in the VBA tree.  Only the first byte is checked for
//  each wide character byte in each directory entry checked.
//
// Returns:
//  TRUE        On success and found
//  FALSE       On error or not found
//
//*************************************************************************

int O97OpenModuleStreamByNameCB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
)
{
    (void)dwIndex;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        int     i;
        BYTE    byChar0, byChar1;

        // Is it the desired stream?

        for (i=0;i<SS_MAX_NAME_LEN && lpstEntry->uszName[i] &&
            i < *(LPBYTE)lpvCookie;i++)
        {
            byChar0 = ((LPBYTE)lpvCookie)[i+1];
            if (byChar0 <= 'Z' && byChar0 >= 'A')
                byChar0 = byChar0 + ('a' - 'A');

            byChar1 = *(LPBYTE)(lpstEntry->uszName + i);
            if (byChar1 <= 'Z' && byChar1 >= 'A')
                byChar1 = byChar1 + ('a' - 'A');

            if (byChar0 != byChar1)
                break;
        }

        if (i == *(LPBYTE)lpvCookie && i < SS_MAX_NAME_LEN &&
            lpstEntry->uszName[i] == 0)
            return(SS_ENUM_CB_STATUS_OPEN);
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}

BOOL O97OpenModuleStreamByName
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    DWORD               dwVBAChildEntry,
    LPBYTE              lpbyName
)
{
    LPSS_ENUM_SIBS      lpstSibs;
    BOOL                bFound;

    // Allocate a sibiling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    SSInitEnumSibsStruct(lpstSibs,
                         dwVBAChildEntry);

    if (SSEnumSiblingEntriesCB(lpstRoot,
                               O97OpenModuleStreamByNameCB,
                               lpbyName,
                               lpstSibs,
                               lpstStream) == SS_STATUS_OK)
        bFound = TRUE;
    else
        bFound = FALSE;

    if (SSFreeEnumSibsStruct(lpstRoot,lpstSibs) !=
        SS_STATUS_OK)
        return(FALSE);

    return(bFound);

}


//*************************************************************************
//
// BOOL O97ModuleGetDesc()
//
// Parameters:
//  lpstStream      Module stream
//  lpstLZNT        Ptr to the LZNT state structure
//  lpbyMacroName   Macro name of which to get the description
//  lpbyDesc        Buffer for description
//  nDescBufSize    Size of description buffer
//
// Description:
//  lpstStream is assumed to point to a module stream.  If lpstLZNT
//  is NULL, the function allocates an LZNT structure.  The structure
//  is initialized for access to the module stream.
//
//  The function searches through the compressed source for
//  "Attribute [lpbyMacroName].VB_Description = "[lpbyDesc]".
//  Where lpbyMacroName is an input parameter and lpbyDesc is
//  filled with the value within quotes up to nBufDescSize - 1.
//  Both lpbyMacroName and lpbyDesc are interpreted as pascal
//  strings.
//
//  If there is no description, lpbyDesc returns with the empty
//  string.
//
//  Note 0: lpbyDesc must not be NULL and nDescBufSize must be >= 1
//  Note 1: The macro name should be in lower case.
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//
//*************************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyO97vb_description[] =
{
    'v','b','_','d','e','s','c','r','i','p','t','i','o','n',' ','=',' ','\"'
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

#define O97_DESC_STATE_BOL          0
#define O97_DESC_STATE_ATTRIBUTE    1
#define O97_DESC_STATE_MATCH_NAME   2
#define O97_DESC_STATE_FIND_DESC    3
#define O97_DESC_STATE_GET_DESC     4
#define O97_DESC_STATE_SKIP_LINE    5
#define O97_DESC_STATE_DONE         6

BOOL O97ModuleGetDesc
(
    LPSS_STREAM     lpstStream,
    LPLZNT          lpstLZNT,
    LPBYTE          lpbyMacroName,
    LPBYTE          lpbyDesc,
    int             nDescBufSize
)
{
    BOOL            bResult = TRUE;
    LPLZNT          lpstLZNTtoUse;

    if (nDescBufSize-- < 1)
        return(FALSE);

    if (lpstLZNT == NULL)
    {
        if (LZNTAllocStruct(lpstStream->lpstRoot->lpvRootCookie,
                            &lpstLZNTtoUse) == FALSE)
            return(FALSE);
    }
    else
        lpstLZNTtoUse = lpstLZNT;

    // Initialize description to empty string

    lpbyDesc[0] = 0;

    if (VBA5LZNTModuleInit(lpstStream,
                           lpstLZNTtoUse) == FALSE)
    {
        // Failed to initialize access to the module source

        bResult = FALSE;
    }
    else
    {
        BYTE        abyWorkBuffer[512];
        DWORD       dwOffset;
        int         nNumBytesToRead, nBufIndex, nState, nSubState;
        BYTE        byCurByte;

        nState = O97_DESC_STATE_BOL;
        nSubState = 0;

        dwOffset = 0;
        nNumBytesToRead = nBufIndex = 512;
        while (nState != O97_DESC_STATE_DONE)
        {
            if (++nBufIndex >= nNumBytesToRead)
            {
                if (dwOffset >= lpstLZNTtoUse->dwSize)
                {
                    if (nState == O97_DESC_STATE_GET_DESC)
                    {
                        // Store the length of the description so far

                        lpbyDesc[0] = (BYTE)nSubState;
                    }
                    else
                    {
                        // Could not find the description

                        bResult = FALSE;
                    }
                    break;
                }

                if (lpstLZNTtoUse->dwSize - dwOffset < (DWORD)nNumBytesToRead)
                    nNumBytesToRead = (int) (lpstLZNTtoUse->dwSize - dwOffset);

                // Get a buffer of bytes

                if (LZNTGetBytes(lpstLZNTtoUse,
                                 dwOffset,
                                 nNumBytesToRead,
                                 abyWorkBuffer) != nNumBytesToRead)
                {
                    bResult = FALSE;
                    break;
                }

                nBufIndex = 0;
                dwOffset += nNumBytesToRead;
            }

            byCurByte = abyWorkBuffer[nBufIndex];

            if (byCurByte <= 'Z' && byCurByte >= 'A')
                byCurByte = byCurByte + ('a' - 'A');

            switch (nState)
            {
                case O97_DESC_STATE_BOL:
                    if (byCurByte == gabyO97attribute[0])
                    {
                        // Could it be an attribute line

                        nState = O97_DESC_STATE_ATTRIBUTE;
                        nSubState = 1;
                    }
                    else
                    {
                        // It is neither a line of spaces nor an
                        //  attribute line

                        nState = O97_DESC_STATE_SKIP_LINE;
                    }
                    break;

                case O97_DESC_STATE_ATTRIBUTE:
                    if (byCurByte == gabyO97attribute[nSubState])
                    {
                        if (++nSubState == sizeof(gabyO97attribute))
                        {
                            // It is an attribute line

                            nState = O97_DESC_STATE_MATCH_NAME;
                            nSubState = 0;
                        }
                    }
                    else
                    if (byCurByte == 0x0A)
                    {
                        nState = O97_DESC_STATE_BOL;
                    }
                    else
                    {
                        // Did not match

                        nState = O97_DESC_STATE_SKIP_LINE;
                    }
                    break;

                case O97_DESC_STATE_MATCH_NAME:
                    if (nSubState == lpbyMacroName[0])
                    {
                        if (byCurByte == '.')
                        {
                            // Found so check for "VB_Description = "

                            nState = O97_DESC_STATE_FIND_DESC;
                            nSubState = 0;
                        }
                    }
                    else
                    if (byCurByte != lpbyMacroName[++nSubState])
                    {
                        // The macro name did not match

                        if (byCurByte == 0x0A)
                            nState = O97_DESC_STATE_BOL;
                        else
                            nState = O97_DESC_STATE_SKIP_LINE;
                    }
                    break;

                case O97_DESC_STATE_FIND_DESC:
                    if (byCurByte == gabyO97vb_description[nSubState])
                    {
                        if (++nSubState == sizeof(gabyO97vb_description))
                        {
                            // The description comes next

                            nState = O97_DESC_STATE_GET_DESC;
                            nSubState = 0;
                        }
                    }
                    else
                    if (byCurByte == 0x0A)
                    {
                        nState = O97_DESC_STATE_BOL;
                    }
                    else
                    {
                        // Did not match

                        nState = O97_DESC_STATE_SKIP_LINE;
                    }
                    break;

                case O97_DESC_STATE_GET_DESC:
                    if (byCurByte == '\"' || byCurByte == 0x0A ||
                        nSubState > nDescBufSize || nSubState >= 255)
                    {
                        // Got the description

                        lpbyDesc[0] = (BYTE)nSubState;
                        nState = O97_DESC_STATE_DONE;
                        break;
                    }
                    else
                    {
                        // Store the character of the description

                        lpbyDesc[++nSubState] = abyWorkBuffer[nBufIndex];
                    }
                    break;

                case O97_DESC_STATE_SKIP_LINE:
                    if (byCurByte == 0x0A)
                    {
                        // Assumption is that 0x0A always ends a line

                        nState = O97_DESC_STATE_BOL;
                    }
                    break;

                default:
                    // This should never happen

                    break;
            }
        }
    }

    // Free the allocated LZNT structure if necessary

    if (lpstLZNT == NULL)
    {
        if (LZNTFreeStruct(lpstStream->lpstRoot->lpvRootCookie,
                           lpstLZNTtoUse) == FALSE)
            bResult = FALSE;
    }

    return(bResult);
}


//********************************************************************
//
// Function:
//  BOOL O97VBAFindAssociatedStreams()
//
// Description:
//  The following fields of lpstO97Scan should be valid before
//  entering this function:
//
//          dwVBAParentEntry
//          dwVBAParentChildEntry
//          dwVBAEntry
//          dwVBAChildEntry
//
//  The function will fill in the following fields by calling
//  the shared function to do so:
//
//          dwPROJECTEntry
//          dwPROJECTwmEntry
//          dw_VBA_PROJECTEntry
//          dwdirEntry
//
// Returns:
//  TRUE        If all of the associated streams were found
//  FALSE       If any of the streams could not be found
//
//********************************************************************

BOOL O97VBAFindAssociatedStreams
(
    LPSS_ROOT           lpstRoot,
    LPO97_SCAN          lpstO97Scan
)
{
    LPSS_ENUM_SIBS      lpstSibs;

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    // Find VBA 5 specific streams

    SSInitEnumSibsStruct(lpstSibs,
                         lpstO97Scan->u.stGeneric.
                             dwVBAParentChildEntry);

    if (O97FindVBAStreams(lpstRoot,
                          lpstSibs,
                          lpstO97Scan) == FALSE)
    {
        // Failed finding associated VBA streams

        SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
        return(FALSE);
    }

    // Successfully got the indices of all important streams

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
    return(TRUE);
}


//********************************************************************
//
// Function:
//  int O97VBACheckMacrosDeletableCB()
//
// Description:
//  The callback checks the name and does the following:
//      1. If the name is "", "_*", or "dir", the function just
//         returns CONTINUE.
//      2. Otherwise, the function returns OPEN
//
// Returns:
//  SS_ENUM_CB_STATUS_OPEN          If the entry is a candidate
//  SS_ENUM_CB_STATUS_CONTINUE      If the entry is not
//
//********************************************************************

int O97VBACheckMacrosDeletableCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;
    (void)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Is it any of "", "_*", or "dir"

        if (WENDIAN(lpstEntry->uszName[0]) == '_' ||
            lpstEntry->uszName[0] == 0 ||
            SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszdir,
                       SS_MAX_NAME_LEN) == 0)
        {
            return(SS_ENUM_CB_STATUS_CONTINUE);
        }

        // The stream name is a valid module stream name

        return(SS_ENUM_CB_STATUS_OPEN);
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL O97VBACheckMacrosDeletable()
//
// Description:
//  Checks to see whether there are any modules left in the
//  VBA storage.
//
//  The function determines that there are no more modules
//  if all of the following conditions are met:
//      1. The names of streams in the VBA storage only
//         consist of the following:
//          - ""
//          - "_*"
//          - "dir"
//          - None of the above and the stream length is zero
//
// Returns:
//  TRUE        If all valid module streams were
//              successfully analyzed as empty
//  FALSE       Otherwise, including error
//
//********************************************************************

BOOL O97VBACheckMacrosDeletable
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPO97_SCAN          lpstO97Scan
)
{
    LPSS_ENUM_SIBS      lpstSibs;

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    // Determine whether there is any other valid module stream

    SSInitEnumSibsStruct(lpstSibs,
                         lpstO97Scan->dwVBAChildEntry);

    if (SSEnumSiblingEntriesCB(lpstRoot,
                               O97VBACheckMacrosDeletableCB,
                               NULL,
                               lpstSibs,
                               lpstStream) == SS_STATUS_OK)
    {
        // There was a valid module stream

        SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
        return(FALSE);
    }

    // The macros storage is empty

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
    return(TRUE);
}

