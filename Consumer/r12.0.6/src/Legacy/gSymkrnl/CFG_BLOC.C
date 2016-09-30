/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/CFG_BLOC.C_v   1.0   26 Jan 1996 20:23:06   JREARDON  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/CFG_BLOC.C_v  $ *
// 
//    Rev 1.0   26 Jan 1996 20:23:06   JREARDON
// Initial revision.
// 
//    Rev 1.41   24 Aug 1994 15:25:50   DLEVITON
// ConfigReg calls.
//
//    Rev 1.40   07 Jun 1994 00:01:44   GVOGEL
// Changed '@Library' to '@Include:' in Geologist comment blocks.
//
//    Rev 1.39   20 May 1994 04:16:40   GVOGEL
// Added comment blocks for Geologist project.
//
//
//    Rev 1.38   15 Mar 1994 12:32:54   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.37   25 Feb 1994 15:05:06   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.36   25 Feb 1994 12:23:06   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.35   17 Feb 1994 20:00:40   PGRAVES
// Win32.
//
//    Rev 1.34   14 Feb 1994 09:49:34   JMILLARD
// merge fix from Quake 2 as follows
// fix 2 problems with leaving residual memory when a realloc fails
//
//    Rev 1.33   12 Oct 1993 17:56:28   EHU
// Ifdef'd out unused code for SYM_NLM.
//
//    Rev 1.32   18 Sep 1993 21:00:00   MARKL
// GMEM_ZEROINIT changed to GHND since the memory is to be
// moveable.
//
//    Rev 1.31   17 Sep 1993 18:10:00   JOHN
// Use CFG_NO_CRC_CHECKS flag to determine when CRC is tested.
//
//    Rev 1.30   23 Aug 1993 10:25:06   BARRY
// Fixed improper use of MemUnlock()
//
//    Rev 1.29   14 Aug 1993 16:08:06   BRAD
// Integrate QAK0 changes.
//
//    Rev 1.27.1.2   04 Aug 1993 12:47:58   PATRICKM
// Surrounded each call to DebugOutput() with '#if 0' because it will never
// be able to compile.
//
//
//    Rev 1.27.1.1   03 Aug 1993 15:53:32   SGULLEY
// changed SYM_WIN defines to DEBUG
//
//    Rev 1.27.1.0   02 Aug 1993 08:44:00   SCOTTP
// Branch base for version QAK0
//
//    Rev 1.27   09 Jul 1993 10:12:20   ED
// [Fix] Moved up one of the diagnostic messages for more accuracy
//
//    Rev 1.26   08 Jul 1993 09:28:36   ED
// [Enh-temporary] Lots of debug messages (SYM_WIN only) to try to track
// some strange things happening for strange reasons.
//
//    Rev 1.25   08 Jul 1993 08:48:26   ED
// [Fix] Fixed a reversed if statement in the diagnostic functions
//
//    Rev 1.24   08 Jul 1993 08:33:54   ED
// [Enh] Added some diagnostic functions
//
//    Rev 1.23   07 May 1993 15:49:30   EHU
// Added some includes for SYM_NLM for new platform.h, platnlm.h, file.h,
// and xapi.h.
//
//    Rev 1.22   19 Mar 1993 08:28:38   JMILLARD
// limit copies and compares to szBlockId to the size of szBlockId,
// otherwise 8-letter names and bCheckCRC wreak havoc on each other.
//
//    Rev 1.21   16 Mar 1993 18:24:50   JMILLARD
// Recalculate CRC when a block is simply copied over
//
//    Rev 1.20   15 Mar 1993 20:08:10   JMILLARD
// Put the CRC stuff back in - I believe it now works - the ifdef remains
// in case another problem arises
//
//
//    Rev 1.19   08 Mar 1993 13:35:36   ED
// Added support for the bCheckCRC field in CFGFILEHEADER and CFGITEMHEADER.
// If this field is set to zero, the CRC is not checked or written.  If non-
// zero, the CRC will be checked.  This allows us to remain backwards
// compatible with the Scorpio library, which will go out before the CRC stuff
// is fixed.
//
// The CRC code is still disabled, meaning that bCheckCRC is always set to
// zero when the file or blocks are written.  To enable it, comment out the
// DONT_USE_CRC #define.
//
//    Rev 1.18   08 Mar 1993 11:20:04   JMILLARD
// Temporarily comment out the CRC stuff again - seems to be a problem
// in Windows?
//
//    Rev 1.17   07 Mar 1993 19:23:22   MARKL
// Restored the validation of CRC in configuration file.  NOTE:  This may break
// a lot of things until the bug is found and fixed.  This needs to be fixed
// before Scorpio goes out.
//
//    Rev 1.16   18 Feb 1993 09:53:06   ED
// MEMSET new CFGITEMHEADER records to zero before filling them.  This
// makes sure the reserved space is cleared.
//
//    Rev 1.15   15 Feb 1993 21:03:52   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.14   12 Feb 1993 15:25:52   ENRIQUE
// No change.
//
//    Rev 1.13   23 Dec 1992 16:23:22   JMILLARD
// Work around SYM_NLM problem with very small addresses being confused with
// numeric id's. The fix, for now, is for SYM_NLM platform only.
//
//    Rev 1.12   16 Nov 1992 13:02:56   JMILLARD
// Temporarily comment out CRC checks until we fix the problem
// with CRC errors when we update a config file.
//
//    Rev 1.11   10 Nov 1992 16:04:08   EHU
// Fixed bug in ItemLocate where lpItemHeader wasn't always advanced.
//
//
//    Rev 1.10   29 Oct 1992 18:12:46   JMILLARD
// SYM_NLM uses same MemUnLock call as SYM_WIN
//
//    Rev 1.9   21 Oct 1992 14:35:38   ED
// Fixed a MEMSET in ConfigBlockFileGet that was killing the wrong buffer.
//
//    Rev 1.8   13 Oct 1992 14:09:58   ED
// Fixed a reversed MEMCPY in ConfigBlockSet.
//
//    Rev 1.7   12 Oct 1992 16:15:16   EHU
// SYM_NLM changes.
//
//
//    Rev 1.6   06 Oct 1992 20:47:30   EHU
// Added function to change a block id.
//
//
//    Rev 1.5   05 Oct 1992 16:36:20   EHU
// Moved prototypes of exported functions to symcfg.h.
//
//
//    Rev 1.4   29 Sep 1992 16:41:02   EHU
// Made some private functions public and renamed them to be more descriptive.
// These take LPCFGFILE as an argument so you aren't limited to the built-in
// filenames.
//
//    Rev 1.3   15 Sep 1992 10:49:12   ED
// Fixed minor problem
//
//    Rev 1.2   14 Sep 1992 14:53:14   MARKK
// Added module_name
//
//    Rev 1.1   08 Sep 1992 09:06:20   ED
// Final, version 1.0
//
//    Rev 1.0   03 Sep 1992 09:10:12   ED
// Initial revision.
 ************************************************************************/

/*@Library: SYMKRNL*/                   // Used by Geologist project.

#ifdef SYM_NLM
#include <stdlib.h>
#endif

#include "platform.h"
#ifdef SYM_WIN32
#include "winreg.h"
#endif
#include "xapi.h"
#include "file.h"
#include "symcfg.h"

MODULE_NAME;

// #define DONT_USE_CRC             // uncomment if CRC is a problem

BYTE    NCFG_SIGNATURE[8] = "NCFG1.0";

extern  CFGFILE rLocalBinary;
extern  CFGFILE rGlobalBinary;

// ---------------------------------------------------------------------------
// Local Prototypes
// ---------------------------------------------------------------------------
LPCFGITEMHEADER LOCAL PASCAL ItemLocate (LPCFGFILEHEADER lpFileHeader, DWORD dwID);
LPCFGFILEHEADER LOCAL PASCAL DeleteItem (LPCFGFILE lpFile,LPCFGFILEHEADER lpFileHeader, LPCFGITEMHEADER lpDest);
LPCFGFILEHEADER LOCAL PASCAL ReallocItem (LPCFGFILE lpFile, LONG lSize);
UINT LOCAL PASCAL CreateItem (LPCFGFILE lpFile,LPCFGFILEHEADER lpFileHeader, LPCFGBLOCKREC lpBlockRec);

// ---------------------------------------------------------------------------
// Macros to make things easier
// ---------------------------------------------------------------------------
#define BLOCK_DATA(block)       (LPBYTE)((LPBYTE)(block)+(block)->wHeaderSize)
#define FIRST_BLOCK(head)       (LPCFGITEMHEADER)(((LPBYTE)(head))+(head)->wHeaderSize)
#define NEXT_BLOCK(block)       (LPCFGITEMHEADER)(((LPBYTE)(block))+(block)->wHeaderSize+(block)->wDataSize)
#define SET_ID_VALUE(head,id)   (*(WORD FAR *) &((head)->szBlockID[0]) = LOWORD(id))
#define GET_ID_VALUE(head)      (*(WORD FAR *) &((head)->szBlockID[0]))
#define BLOCK_SIZE(block)       ((block)->wHeaderSize+(block)->wDataSize)

// work-around for SYM_NLM problem with addresses < 0xffff
//  for now we assume no numeric ids in the SYM_NLM world, they
//  are always pointers to strings. We need a more general solution.
//

#ifdef SYM_NLM
#define NAMED_ID(id)            (TRUE)
#else

// this reflects the default rule for distinguishing numeric ids and pointers
// to strings - numeric ids must be <= 0xffff - pointers must be > 0xffff

#define NAMED_ID(id)            (HIWORD (id) != 0)
#endif

#define GET_NUMERIC_ID(id)      (LOWORD (id))

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Initializes a binary configuration file.  If it is a new file,  *
 *      the file header is created.                                     *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      CFG_SUCCESS                                                     *
 *      CFG_MEMORY_ERROR                                                *
 *      CFG_CHECKSUM_FAILED                                             *
 *      CFG_FILE_ERROR                                                  *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/19/1992 ED Created.                                               *
 ************************************************************************/

UINT WINAPI ConfigBlockInit (LPCFGFILE lpFile)
{
    LPCFGFILEHEADER     lpFileHeader;
    UINT                wErr = CFG_SUCCESS;
    WORD                wSize;
    LPBYTE              lpData;
    HGLOBAL             hOldMem;


    if (lpFile->dwSize == 0L)           // is this a brand new file?
        {
        hOldMem = lpFile->hMem;         // save to free if realloc fails
                                        // realloc the block to hold a header
        lpFile->hMem = MemReAlloc (lpFile->hMem,
                                sizeof(CFGFILEHEADER), GMEM_MOVEABLE);

        if (lpFile->hMem)
            {
            if (lpFileHeader = MemLock (lpFile->hMem))
                {
                                        // setup the file header
                MEMSET(lpFileHeader, 0, sizeof(CFGFILEHEADER));
                STRCPY (lpFileHeader->szSignature, NCFG_SIGNATURE);
                lpFileHeader->wHeaderSize = sizeof (CFGITEMHEADER);
                lpFileHeader->wCreatorID = NCFG_CREATOR;
                lpFileHeader->wVersion = NCFG_VERSION;

#define CFG_MIN_SIZE    (sizeof (CFGITEMHEADER))

                                        // record the file size
                lpFile->dwSize = CFG_MIN_SIZE;
                }

            MemUnlock (lpFile->hMem, lpFileHeader);
            }
        else
            {
            wErr = CFG_MEMORY_ERROR;
            MemFree( hOldMem );         // free the residual memory
            }
        }
    else
        {                               // file exists, so just check integrity
        if (lpFileHeader = MemLock (lpFile->hMem))
            {
// This next line of code can't be compiled, so why is it here?
#if 0
            DebugOutput (DBF_TRACE | DBF_APPLICATION, "cfg_bloc: init: total blocks = %d", lpFileHeader->wTotalBlocks);
#endif
            if (STRCMP (lpFileHeader->szSignature, NCFG_SIGNATURE) == 0)
                {
                wSize  = (WORD)(lpFile->dwSize - lpFileHeader->wHeaderSize);

                lpData = (LPBYTE) FIRST_BLOCK (lpFileHeader);

                                        // if the file was last written with
                                        // a CRC, verify it against the current
                                        // contents.  otherwise, ignore the CRC
#ifndef DONT_USE_CRC
                if (lpFileHeader->bCheckCRC && !(lpFile->wFlags & CFG_NO_CRC_CHECKS))
                    {
                    if (ConfigGetCRC (lpData, wSize) != lpFileHeader->wCheckSum)
                        wErr = CFG_CHECKSUM_FAILED;
                    }
#endif
                }
            else
                wErr = CFG_FILE_ERROR;

            MemUnlock (lpFile->hMem, lpFileHeader);
            }
        }

    return(wErr);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Initializes a binary configuration file.  If it is a new file,  *
 *      the file header is created.                                     *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      CFG_SUCCESS                                                     *
 *      CFG_MEMORY_ERROR                                                *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/19/1992 ED Created.                                               *
 ************************************************************************/

UINT WINAPI ConfigBlockTerminate (LPCFGFILE lpFile)
{
    LPCFGFILEHEADER     lpFileHeader;
    UINT                wErr = CFG_SUCCESS;
    WORD                wSize;
    LPBYTE              lpData;


    if (lpFileHeader = MemLock (lpFile->hMem))
        {
                                        // get the size of the block without
                                        // the file header
        wSize  = (WORD)(lpFile->dwSize - lpFileHeader->wHeaderSize);
                                        // point to the first data block
        lpData = (LPBYTE) FIRST_BLOCK (lpFileHeader);

                                        // CRC the contents of the entire file
                                        // following the header.  set the CRC
                                        // flag to indicate that we calculated
                                        // a CRC before writing
#ifndef DONT_USE_CRC
        lpFileHeader->wCheckSum = ConfigGetCRC (lpData, wSize);
        lpFileHeader->bCheckCRC = TRUE;
#else
        lpFileHeader->bCheckCRC = FALSE;
#endif

        MemUnlock (lpFile->hMem, lpFileHeader);
        }
    else
        wErr = CFG_MEMORY_ERROR;

    return(wErr);
}


/*@API:******************************************************************
@Declaration: UINT ConfigBlockGet(LPCFGBLOCKREC lpBlockRec)

@Description:
This function copies the data stored in the specified block.  Only the
number of bytes requested are actually transferred.  If the
block size is smaller than the size requested, the remaining
space in the buffer is zeroed.  If the block is not found,
the return buffer is zeroed.  If the specified buffer is NULL,
the block is deleted from the config file if found. \n

This function actually calls ConfigBlockFileGet, trying the
local and global files in the specified order.

@Parameters:
$lpBlockRec$ Points to a structure that receives the data block as well as
specifing where the block is in the data file. It also must provide the size
it expects the block to be.

@Returns:
The return values is one of the following:
    ~CFG_SUCCESS~ Indicating the block was copied successfully.
    ~CFG_MEMORY_ERROR~ Indicating that there was a problem locating the block.
    ~CFG_CHECKSUM_FAILED~ Indicating that the data stored in the data block
    did not pass the checksum.
    ~CFG_BLOCK_IS_LARGER~ Indicating that the configuration data block is
    larger than the block supplied.
    ~CFG_BLOCK_IS_SMALLER~ Indicating that the configuration data block is
    smaller than the block supplied.
    ~CFG_BLOCK_NOT_FOUND~ Indicating that the block desired could not be found.

@See: CFGFILE CFGBLOCKREC
@Include: symcfg.h
@Compatibility: Win16, Win32, and DOS
************************************************************************/
#ifndef SYM_NLM
UINT SYM_EXPORT WINAPI ConfigBlockGet (LPCFGBLOCKREC lpBlockRec)
{
    BOOL        bLocal = FALSE;
    BOOL        bGlobal = FALSE;
    UINT        wErr = CFG_BLOCK_NOT_FOUND;

    if (lpBlockRec->wFlags & CFG_LOCAL_FIRST)
        goto Local_First;

    while (TRUE)
        {                               // read the global config file
        if (!bGlobal && wErr != CFG_SUCCESS)
            {
            if (rGlobalBinary.dwSize > CFG_MIN_SIZE)
                wErr = ConfigBlockFileGet (&rGlobalBinary, lpBlockRec);

            bGlobal = TRUE;
            }
                                        // read the local config file
        else if (!bLocal && wErr != CFG_SUCCESS)
            {
Local_First:
            if (rLocalBinary.dwSize > CFG_MIN_SIZE)
                wErr = ConfigBlockFileGet (&rLocalBinary, lpBlockRec);

            bLocal = TRUE;
            }

        else                            // we tried both, so quit
            break;
        }

    return (wErr);
}
#endif


/*@API:******************************************************************
@Declaration: UINT ConfigBlockFileGet (LPCFGFILE lpFile,
LPCFGBLOCKREC lpBlockRec)

@Description:
This function gets a block of data from the configuration file specified. It
also checks the CRC of the data if a CRC was stored with the it. \n

If the data pointer in the CFGBLOCKREC is NULL than the configuration file
data block is deleted.

@Parameters:
$lpFile$ Points to a structure containing information about the configuration
file.

$lpBlockRec$ Points to a structure that receives the data block as well as
specifing where the block is in the data file. It also must provide the size
it expects the block to be.

@Returns:
The return values is one of the following:
    ~CFG_SUCCESS~ Indicating the block was copied successfully.
    ~CFG_MEMORY_ERROR~ Indicating that there was a problem locating the block.
    ~CFG_CHECKSUM_FAILED~ Indicating that the data stored in the data block
    did not pass the checksum.
    ~CFG_BLOCK_IS_LARGER~ Indicating that the configuration data block is
    larger than the block supplied.
    ~CFG_BLOCK_IS_SMALLER~ Indicating that the configuration data block is
    smaller than the block supplied.
    ~CFG_BLOCK_NOT_FOUND~ Indicating that the block desired could not be found.

@See: CFGFILE CFGBLOCKREC
@Include: symcfg.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
UINT SYM_EXPORT WINAPI ConfigBlockFileGet (LPCFGFILE lpFile, LPCFGBLOCKREC lpBlockRec)
{
    LPCFGITEMHEADER     lpItemHeader;
    LPCFGFILEHEADER     lpFileHeader;
    LPBYTE              lpBuffer;
    UINT                wErr = CFG_SUCCESS;
    UINT                wMoveSize;

                                        // lock the config file data
    if (lpFileHeader = (LPCFGFILEHEADER) MemLock (lpFile->hMem))
        {
                                        // find the block we want
        lpItemHeader = ItemLocate (lpFileHeader, lpBlockRec->dwBlockID);

        if (lpItemHeader != NULL)            // does it exist?
            {
                                        // point to the config block data
            lpBuffer = BLOCK_DATA(lpItemHeader);

                                        // if the block was last written with
                                        // a CRC, verify the current contents.
                                        // otherwise, ignore the block CRC
#ifndef DONT_USE_CRC
            if (lpItemHeader->bCheckCRC && !(lpFile->wFlags & CFG_NO_CRC_CHECKS))
                {
                if (ConfigGetCRC (lpBuffer, lpItemHeader->wDataSize) != lpItemHeader->wCRC)
                    {
                    wErr = CFG_CHECKSUM_FAILED;
                    goto Finish_Up;
                    }
                }
#endif
                                        // if the caller's data pointer is
                                        // NULL, delete the block
            if (lpBlockRec->lpData == NULL)
                {
                lpFileHeader = DeleteItem (lpFile, lpFileHeader, lpItemHeader);

                if (lpFileHeader == NULL)
                    wErr = CFG_MEMORY_ERROR;

                lpFile->bDirty = TRUE;
                goto Finish_Up;
                }
                                        // only copy what we have or what we
                                        // can handle
            wMoveSize = min (lpItemHeader->wDataSize, lpBlockRec->wSize);

                                        // copy the data
            MEMCPY(lpBlockRec->lpData, lpBuffer, wMoveSize);

                                        // return any notifications
            if (lpItemHeader->wDataSize > lpBlockRec->wSize)
                {
                wErr = CFG_BLOCK_IS_LARGER;
                }
            else if (lpItemHeader->wDataSize < lpBlockRec->wSize)
                {
                                        // zero the rest of the buffer
                MEMSET (((LPBYTE)lpBlockRec->lpData) + lpItemHeader->wDataSize,
                                0, lpBlockRec->wSize - lpItemHeader->wDataSize);
                wErr = CFG_BLOCK_IS_SMALLER;
                }
            }
        else if (wErr == CFG_SUCCESS)
            {
            wErr = CFG_BLOCK_NOT_FOUND;
                                        // zero the buffer if not found
                                        // (and not set to NULL!)
            if (lpBlockRec->lpData)
                MEMSET(lpBlockRec->lpData, 0, lpBlockRec->wSize);
            }

Finish_Up:
        MemUnlock (lpFile->hMem, lpFileHeader);
        }
    else
        wErr = CFG_MEMORY_ERROR;

    return (wErr);
}


/*@API:******************************************************************
@Declaration: UINT ConfigBlockSet(LPCFGBLOCKREC lpBlockRec)

@Description:
This function stores the specified data in the configuration file. This is
done either by copying it if the size has not changed, or by deleting the
old block and inserting a new block if the size has changed.  If the block
does not exist, it is created. \n

This function actually calls ConfigBlockFileSet, trying the local and global
files in the specified order.

@Parameters:
$lpBlockRec$ Points to a CFGBLOCKREC containing the block of data to write to
the configuration file.

@Returns:
The return value is one of the following values:
    ~CFG_SUCCESS~ If successful.
    ~CFG_MEMORY_ERROR~ Otherwise.

@See: CFGBLOCKREC ConfigBlockFileSet
@Include: symcfg.h
@Compatibility: Win16, Win32, and DOS
************************************************************************/
#ifndef SYM_NLM
UINT SYM_EXPORT WINAPI ConfigBlockSet (LPCFGBLOCKREC lpBlockRec)
{
    return(ConfigBlockFileSet(&rLocalBinary, lpBlockRec));
}
#endif //SYM_NLM


/*@API:******************************************************************
@Declaration: UINT ConfigBlockFileSet (LPCFGFILE lpFile,
LPCFGBLOCKREC lpBlockRec)

@Description:
This function stores the specified data in the configuration file. This is
done either by copying it if the size has not changed, or by deleting the
old block and inserting a new block if the size has changed.  If the block
does not exist, it is created.

@Parameters:
$lpFile$ Points to a CFGFILE structure containing information on the
configuration file.

$lpBlockRec$ Points to a CFGBLOCKREC containing the block of data to write to
the configuration file.

@Returns:
The return value is one of the following values:
    ~CFG_SUCCESS~ If successful.
    ~CFG_MEMORY_ERROR~ Otherwise.

@See: CFGFILE CFGBLOCKREC ConfigBlockSet
@Include: symcfg.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
UINT SYM_EXPORT WINAPI ConfigBlockFileSet (LPCFGFILE lpFile, LPCFGBLOCKREC lpBlockRec)
{
    LPCFGITEMHEADER     lpItemHeader;
    LPCFGFILEHEADER     lpFileHeader;
    LPBYTE              lpBuffer;
    UINT                wErr = CFG_SUCCESS;

                                        // lock the config file data
    if (lpFileHeader = (LPCFGFILEHEADER) MemLock (lpFile->hMem))
        {
#if 0
        DebugOutput (DBF_TRACE | DBF_APPLICATION, "cfg_bloc: setblock: total blocks = %d", lpFileHeader->wTotalBlocks);
#endif
                                        // find the block we want
        lpItemHeader = ItemLocate (lpFileHeader, lpBlockRec->dwBlockID);

        if (lpItemHeader != NULL)            // does it exist?
            {
                                        // point to the config block data
            lpBuffer = BLOCK_DATA(lpItemHeader);

                                        // if the block data is the same size
                                        // as the caller's data, just copy
                                        // over the old block.
            if (lpItemHeader->wDataSize == lpBlockRec->wSize)
                {
                lpFile->bDirty = TRUE;
                MEMCPY (lpBuffer, lpBlockRec->lpData, lpItemHeader->wDataSize);

                // recompute CRC for new contents

#ifndef DONT_USE_CRC
                lpItemHeader->wCRC        =
                    ConfigGetCRC(lpBlockRec->lpData, lpBlockRec->wSize);
                lpItemHeader->bCheckCRC   = TRUE;
#else
                lpItemHeader->bCheckCRC   = FALSE;
#endif
                goto Finish_Up;
                }
#if 0
            else
                DebugOutput (DBF_TRACE | DBF_APPLICATION, "cfg_bloc: setblock: block is smaller or larger");
#endif
                                        // otherwise remove the existing
                                        // block.  Then we'll create a new
                                        // block for the new data.
            lpFileHeader = DeleteItem (lpFile, lpFileHeader, lpItemHeader);

            if (lpFileHeader == NULL)
                 wErr = CFG_MEMORY_ERROR;
            }

        if (wErr == CFG_SUCCESS)              // create a new block for the data
            {
            wErr = CreateItem (lpFile, lpFileHeader, lpBlockRec);
            lpFile->bDirty = TRUE;
            }

Finish_Up:
        MemUnlock (lpFile->hMem, lpFileHeader);
        }
    else
        wErr = CFG_MEMORY_ERROR;

    return (wErr);
}

#ifndef SYM_NLM

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Copies the configuration file header and the block header for   *
 *      the specified block.  This function is used to get information  *
 *      on who created the file and the block.  The information cannot  *
 *      be changed by an application.  If the block header is not       *
 *      found, the buffer is zeroed.  The file header is always copied. *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      CFG_SUCCESS                                                     *
 *      CFG_BLOCK_NOT_FOUND                                             *
 *      CFG_MEMORY_ERROR                                                *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/20/1992 ED Created.                                               *
 ************************************************************************/


UINT SYM_EXPORT WINAPI ConfigGetBlockInfo (
    DWORD               dwBlockID,
    LPCFGFILEHEADER     lpFileHeader,
    LPCFGITEMHEADER     lpBlock)
{
    return(ConfigGetBlockFileInfo(&rLocalBinary, dwBlockID, lpFileHeader, lpBlock));
}

UINT SYM_EXPORT WINAPI ConfigGetBlockFileInfo (
    LPCFGFILE           lpFile,
    DWORD               dwBlockID,
    LPCFGFILEHEADER     lpFileHeader,
    LPCFGITEMHEADER     lpBlock)
{
    LPCFGITEMHEADER     lpItemHeader;
    LPBYTE              lpBuffer;
    UINT                wErr = CFG_SUCCESS;


    if (lpBuffer = MemLock (lpFile->hMem))
        {                               // copy the header data
        *lpFileHeader = *((LPCFGFILEHEADER) lpBuffer);

                                        // find the block we want
        if (lpItemHeader = ItemLocate ((LPCFGFILEHEADER) lpBuffer, dwBlockID))
            {
            lpBuffer = BLOCK_DATA(lpItemHeader);

            *lpBlock = *lpItemHeader;
            }
        else
            wErr = CFG_BLOCK_NOT_FOUND;

        MemUnlock (lpFile->hMem, lpBuffer);
        }
    else
        wErr = CFG_MEMORY_ERROR;

    return (wErr);
}



/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Changes the id of the block specified by dwOldID to dwNewID.    *
 *      If a block specified by dwNewID exists, it is first deleted.    *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      CFG_SUCCESS                                                     *
 *      CFG_BLOCK_NOT_FOUND                                             *
 *      CFG_MEMORY_ERROR                                                *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 10/06/1992 EFH Created.                                              *
 ************************************************************************/


/*@API:******************************************************************
@Declaration: UINT ConfigBlockFileIdChange(LPCFGFILE lpFile, DWORD dwOldID,
DWORD dwNewID)

@Description:
This function changes the id of the block specified by dwOldID to dwNewID.
If a block specified by dwNewID exists, it is first deleted.

@Parameters:
$lpFile$ Points to a CFGFILE structure with information about the
configuration file.

$dwOldID$ Specifies the original ID of the block.

$dwNewID$ Specifies the new ID to use for the block.

@Returns:
The return value can be one of the following values:
    ~CFG_SUCCESS~ If the operation was successful.
    ~CFG_BLOCK_NOT_FOUND~ If the old block ID was not found.
    ~CFG_MEMORY_ERROR~ If there was a problem with the CFGFILE structure
    supplied.

@Include: symcfg.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
UINT SYM_EXPORT WINAPI ConfigBlockFileIdChange (
    LPCFGFILE   lpFile,
    DWORD       dwOldID,
    DWORD       dwNewID)
{
    LPCFGITEMHEADER     lpItemHeader;
    LPCFGFILEHEADER     lpFileHeader;
    UINT                wErr = CFG_SUCCESS;

                                        // lock the config file data
    if (lpFileHeader = (LPCFGFILEHEADER) MemLock (lpFile->hMem))
        {
                                        // check if new block exists
        lpItemHeader = ItemLocate (lpFileHeader, dwNewID);

        if (lpItemHeader != NULL)       // delete new block if it exists
            {
            lpFileHeader = DeleteItem (lpFile, lpFileHeader, lpItemHeader);
            lpFile->bDirty = TRUE;

            if (lpFileHeader == NULL)
                {
                wErr = CFG_MEMORY_ERROR;
                goto Finish_Change;
                }
            }
                                        // check if old block exists
        lpItemHeader = ItemLocate (lpFileHeader, dwOldID);

        if (lpItemHeader != NULL)       // update its id if it exists
            {
            if (NAMED_ID(dwNewID))
                {
                lpItemHeader->wFlags |= CFG_BLK_IS_NAMED;
                STRNCPY(lpItemHeader->szBlockID,
                        (LPSTR) dwNewID,
                        BLOCK_ID_SIZE);
                }
            else
                {
                lpItemHeader->wFlags &= ~CFG_BLK_IS_NAMED;
                SET_ID_VALUE(lpItemHeader, dwNewID);
                }
            lpFile->bDirty = TRUE;
            }
        else if (wErr == CFG_SUCCESS)
            wErr = CFG_BLOCK_NOT_FOUND;

Finish_Change:
        MemUnlock (lpFile->hMem, lpFileHeader);
        }
    else
        wErr = CFG_MEMORY_ERROR;

    return (wErr);
}

#endif  // ifndef SYM_NLM

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/19/1992 ED Created.                                               *
 ************************************************************************/

LPCFGITEMHEADER LOCAL PASCAL ItemLocate (LPCFGFILEHEADER lpFileHeader, DWORD dwBlockID)
{
    LPCFGITEMHEADER     lpItemHeader;
    UINT                wItems;
    BOOL                bNamed;
    BOOL                bFound = FALSE;

                                        // get the total number of blocks
    wItems = lpFileHeader->wTotalBlocks;

                                        // point to the first block
    lpItemHeader = FIRST_BLOCK (lpFileHeader);

                                        // If the high-order word is NULL, the
                                        // block ID is the value in the
                                        // low-order word.  otherwise, it
                                        // points to a string.
    bNamed = NAMED_ID(dwBlockID);

    while (wItems-- && !bFound)
        {


                                        // check for a block name
        if (bNamed && lpItemHeader->wFlags & CFG_BLK_IS_NAMED)
            {
            if (STRNCMP (lpItemHeader->szBlockID,
                        (LPSTR) dwBlockID,
                        BLOCK_ID_SIZE) == 0)
                {
                bFound = TRUE;
                break;
                }
            }
                                        // check for a block ID value
        else if (GET_ID_VALUE (lpItemHeader) == GET_NUMERIC_ID(dwBlockID))
            {
            bFound = TRUE;
            break;
            }
                                        // move to the next block
        lpItemHeader = NEXT_BLOCK(lpItemHeader);
        }

    if (!bFound)
        lpItemHeader = NULL;

    return (lpItemHeader);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/20/1992 ED Created.                                               *
 ************************************************************************/

LPCFGFILEHEADER LOCAL PASCAL ReallocItem (LPCFGFILE lpFile, LONG lSize)
{
    LPCFGFILEHEADER     lpHeader;
    HGLOBAL             hOldMem;

    // &? Don't have lp to pass to MemUnlock() so using MemRelease instead.

    MemRelease (lpFile->hMem);          // unlock the block
    hOldMem = lpFile->hMem;             // save to free if realloc fails
                                        // adjust the block size
    lpFile->hMem = MemReAlloc (lpFile->hMem, lpFile->dwSize + lSize, GHND);

                                        // lock it down again
    lpHeader = (LPCFGFILEHEADER) MemLock (lpFile->hMem);

    if (lpFile->hMem == NULL)           // free the residual memory
        {
        MemFree (hOldMem);
        }

#if 0
    if (lpHeader == NULL)
        DebugOutput (DBF_TRACE | DBF_APPLICATION, "cfg_bloc: realloc failed (%l bytes)", lSize);
#endif

    return (lpHeader);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      CFG_SUCCESS                                                     *
 *      CFG_MEMORY_ERROR                                                *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/20/1992 ED Created.                                               *
 ************************************************************************/

UINT LOCAL PASCAL CreateItem (
    LPCFGFILE           lpFile,
    LPCFGFILEHEADER     lpFileHeader,
    LPCFGBLOCKREC       lpBlockRec)
{
    LPCFGITEMHEADER     lpItemHeader;
    LPBYTE              lpData;
    UINT                wSize;
    UINT                wErr = CFG_SUCCESS;
    UINT                wBlocks = lpFileHeader->wTotalBlocks;

                                        // determine the amount of stuff we
                                        // are adding to the file
    wSize = sizeof(CFGITEMHEADER) + lpBlockRec->wSize;

                                        // reallocate the block with enough
                                        // room at the end for the new block
                                        // and its header.  The memory for
                                        // the new block is initialized
                                        // to zero.
    if (lpFileHeader = ReallocItem (lpFile, (LONG) wSize))
        {
        lpFile->dwSize += (DWORD) wSize;    // increase the file size

                                        // start at the first block
        lpItemHeader = FIRST_BLOCK(lpFileHeader);

                                        // find the empty space after the
                                        // last block
        while (wBlocks--)
            lpItemHeader = NEXT_BLOCK(lpItemHeader);

        lpFileHeader->wTotalBlocks++;

#if 0
        DebugOutput (DBF_TRACE | DBF_APPLICATION, "cfg_bloc: create: total blocks = %d", lpFileHeader->wTotalBlocks);
#endif
                                        // create a new block header
        MEMSET(lpItemHeader, 0, sizeof(CFGITEMHEADER));
        lpItemHeader->wHeaderSize = sizeof(CFGITEMHEADER);
        lpItemHeader->wDataSize   = lpBlockRec->wSize;
        lpItemHeader->wCreatorID  = NCFG_CREATOR;
        lpItemHeader->wVersion    = NCFG_VERSION;
        lpItemHeader->wItemID     = NCFG_STD_BLOCK;
        lpItemHeader->wBlockNum   = lpFileHeader->wTotalBlocks;
                                        // CRC the contents of the block
                                        // following the header.  set the CRC
                                        // flag to indicate that we calculated
                                        // a CRC before writing
#ifndef DONT_USE_CRC
        lpItemHeader->wCRC        = ConfigGetCRC(lpBlockRec->lpData, lpBlockRec->wSize);
        lpItemHeader->bCheckCRC   = TRUE;
#else
        lpItemHeader->bCheckCRC   = FALSE;
#endif
                                        // if the hi-order word of the block
                                        // ID is zero, we have an ID value
                                        // in the low-order word.  otherwise,
                                        // the block ID is a pointer to a name
        if (NAMED_ID(lpBlockRec->dwBlockID))
            {
            lpItemHeader->wFlags |= CFG_BLK_IS_NAMED;
            STRNCPY (lpItemHeader->szBlockID,
                    (LPSTR) lpBlockRec->dwBlockID,
                    BLOCK_ID_SIZE);
            }
        else
            {
            SET_ID_VALUE (lpItemHeader, lpBlockRec->dwBlockID);
            }
                                        // copy the data to the block
        lpData = BLOCK_DATA(lpItemHeader);
        MEMCPY(lpData, lpBlockRec->lpData, lpBlockRec->wSize);
        }
    else
        wErr = CFG_MEMORY_ERROR;

    return (wErr);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/19/1992 ED Created.                                               *
 ************************************************************************/

LPCFGFILEHEADER LOCAL PASCAL DeleteItem (
    LPCFGFILE           lpFile,
    LPCFGFILEHEADER     lpFileHeader,
    LPCFGITEMHEADER     lpCurrentItem)
{
    LPCFGITEMHEADER     lpNextItem;
    LPCFGITEMHEADER     lpMoveItem;
    UINT                wBlockSize;
    int                 nShiftSize;
    UINT                wBlocks = lpFileHeader->wTotalBlocks;

                                        // the current item will be deleted,
                                        // so find out how big it is before
                                        // doing anything else
    nShiftSize = (int) BLOCK_SIZE (lpCurrentItem);

                                        // get the first block to be moved
    lpMoveItem = NEXT_BLOCK (lpCurrentItem);

                                        // how many blocks are left?
    wBlocks -= lpCurrentItem->wBlockNum;

                                        // shift all blocks forward until
                                        // the last block is found
    while (wBlocks--)
        {
        if (lpMoveItem->wHeaderSize != sizeof(CFGITEMHEADER))
            {
#if 0
            DebugOutput (DBF_TRACE | DBF_APPLICATION, "cfg_bloc: delete: bad header size");
#endif
            break;
            }
                                        // get the following block now,
                                        // because the header information
                                        // that allows us to get there will
                                        // be off after the move
        lpNextItem = NEXT_BLOCK(lpMoveItem);

                                        // get the block size
        wBlockSize = BLOCK_SIZE (lpMoveItem);

                                        // do the copy thing
        MEMCPY(lpCurrentItem, lpMoveItem, wBlockSize);

        lpCurrentItem->wBlockNum--;     // adjust the block sequence number

                                        // setup for the next move
        lpCurrentItem = NEXT_BLOCK (lpCurrentItem);
        lpMoveItem = lpNextItem;
        }

    lpFileHeader->wTotalBlocks--;       // adjust the block count downward

#if 0
    DebugOutput (DBF_TRACE | DBF_APPLICATION, "cfg_bloc: delete: total blocks = %d", lpFileHeader->wTotalBlocks);
#endif
                                        // shrink the memory block
    if (lpFileHeader = ReallocItem (lpFile, (LONG) -nShiftSize))
        lpFile->dwSize -= nShiftSize;       // adjust the file size downward

    return (lpFileHeader);
}

#ifndef SYM_NLM
// ************************************************************************
//                      DIAGNOSTIC FUNCTIONS
// ************************************************************************

UINT SYM_EXPORT WINAPI ConfigLocalEnumBlocks (CFGENUMPROC lpProc)
{
    if (rLocalBinary.dwSize > CFG_MIN_SIZE)
        ConfigFileEnumBlocks (&rLocalBinary, lpProc);

    return (NOERR);
}


UINT SYM_EXPORT WINAPI ConfigGlobalEnumBlocks (CFGENUMPROC lpProc)
{
    if (rGlobalBinary.dwSize > CFG_MIN_SIZE)
        ConfigFileEnumBlocks (&rGlobalBinary, lpProc);

    return (NOERR);
}


UINT SYM_EXPORT WINAPI ConfigFileEnumBlocks (LPCFGFILE lpFile, CFGENUMPROC lpProc)
{
    LPCFGITEMHEADER     lpItemHeader;
    LPCFGFILEHEADER     lpFileHeader;
    UINT                x, wItems;

                                        // lock the config file data
    if (lpFileHeader = (LPCFGFILEHEADER) MemLock (lpFile->hMem))
        {
                                        // get the total number of blocks
        wItems = lpFileHeader->wTotalBlocks;

                                        // point to the first block
        lpItemHeader = FIRST_BLOCK (lpFileHeader);

        for (x = 0; x < wItems; x++)
            {
                        // verify some verifiable things in the
                        // header to keep us from walking off
                        // the end

            if (lpItemHeader->wHeaderSize != sizeof(CFGITEMHEADER))
                {
#if 0
                DebugOutput (DBF_TRACE | DBF_APPLICATION, "cfg_bloc: enum: (item %d) header size is wrong", x);
#endif
                break;
                }

            if (lpItemHeader->wVersion != NCFG_VERSION)
                {
#if 0
                DebugOutput (DBF_TRACE | DBF_APPLICATION, "cfg_bloc: enum: (item %d) header version is not current", x);
#endif
                break;
                }

            if (lpItemHeader->wItemID != NCFG_STD_BLOCK)
                {
#if 0
                DebugOutput (DBF_TRACE | DBF_APPLICATION, "cfg_bloc: enum: (item %d) header is not an ED block", x);
#endif
                break;
                }

            if (lpProc (lpFileHeader, lpItemHeader) == FALSE)
                break;

            lpItemHeader = NEXT_BLOCK(lpItemHeader);
            }

        MemUnlock (lpFile->hMem, lpFileHeader);
        }

    return (NOERR);
}

#endif  // ifndef SYM_NLM
