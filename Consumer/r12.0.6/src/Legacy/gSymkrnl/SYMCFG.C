/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/SYMCFG.C_v   1.0   26 Jan 1996 20:23:00   JREARDON  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/SYMCFG.C_v  $ *
// 
//    Rev 1.0   26 Jan 1996 20:23:00   JREARDON
// Initial revision.
// 
//    Rev 1.61   09 Nov 1995 16:21:16   GDZIECI
// Moved changes to ConfigFileFlush() from QUAKE 6.
// 
//    Rev 1.60   17 Apr 1995 15:57:28   PATRICKM
// ConfigTerminate() now propagates the return code from ConfigFileTerminate()
// back to the caller.
// 
//    Rev 1.59   19 Oct 1994 18:35:56   JMILLARD
// exclude the last changes from NLM land - we aren't SYM_WIN but that doesn't make
// us DOS
// 
//    Rev 1.58   13 Oct 1994 15:07:00   PGRAVES
// Covers of Windows private profile APIs for non-Windows platforms.
//
//    Rev 1.57   02 Sep 1994 11:52:22   JMILLARD
// make the buffer 1 byte bigger in ConfigFileInit so that a text block
// has an EOS at the end
//
//    Rev 1.56   24 Aug 1994 15:25:40   DLEVITON
// ConfigReg calls.
//
//    Rev 1.55   04 May 1994 00:10:54   BRAD
// Don't use NFileOpen()
//
//    Rev 1.54   03 May 1994 22:56:06   BRAD
// Remove functions NFileOpen() and NFileCreate()
//
//    Rev 1.53   30 Mar 1994 11:55:06   PATRICKM
// Checked in from latest trunk src
//
//    Rev 1.52   23 Mar 1994 12:29:30   BRAD
// DiskGetType() NOW takes UINTs, not WORDs
//
//    Rev 1.51   21 Mar 1994 00:20:04   BRAD
// Cleaned up for WIN32
//
//    Rev 1.50   15 Mar 1994 12:35:06   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.49   25 Feb 1994 12:23:00   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.48   17 Feb 1994 20:00:26   PGRAVES
// Win32.
//
//    Rev 1.47   10 Feb 1994 18:45:26   PGRAVES
// Added changes from QAK2 branch to trunk.
//
//    Rev 1.46   27 Dec 1993 11:48:24   JOHN
// Changed ConfigGetCRC() to use an LPBYTE pointer to step through the
// buffer, rather than an LPSTR.  LPSTR uses signed characters on the
// Windows platform, which really screws up the algorithm.  Also changed
// the first parameter to the function from an LPCSTR to a "const void
// FAR *".  This is the correct way to pass a generic buffer pointer.
//
//    Rev 1.45   12 Oct 1993 18:01:04   EHU
// Ifdef'd out Config{Dis,En}ableCRC for SYM_NLM.
//
//    Rev 1.44   12 Oct 1993 17:41:54   EHU
// Ifdef'd out unused code for SYM_NLM.
//
//    Rev 1.43   17 Sep 1993 18:09:08   JOHN
// Added ConfigDisableCRC() and ConfigEnableCRC() functions.
//
//    Rev 1.42   30 Aug 1993 09:57:40   ED
// Removed the last change.
//
//    Rev 1.41   28 Aug 1993 15:08:40   JMILLARD
// move free of memory to end of ConfigFileInit, and free if any error
//
//    Rev 1.40   16 Aug 1993 08:38:32   ED
// Fixed changes made in the last two revisions that blew away the memory
// handle in the config file info block.
//
//    Rev 1.39   14 Aug 1993 16:07:16   BRAD
// Use new CRC algorithm.
//
//    Rev 1.38.1.1   03 Aug 1993 09:55:06   JMILLARD
// change to quicker crc
//
//    Rev 1.38.1.0   02 Aug 1993 08:44:12   SCOTTP
// Branch base for version QAK0
//
//    Rev 1.38   21 Jun 1993 16:08:12   DLEVITON
// Eliminated reference to obsolete version.h
//
//    Rev 1.37   09 Jun 1993 11:01:46   JMILLARD
// 1. fix problem with not freeing memory on error in ConfigFileInit
// 2. change CRC to calculate with WORD not UINT - UINT size floats with
// platform, and so is not appropriate for an interplatform quantity.
//
//    Rev 1.36   03 Jun 1993 01:41:34   BRAD
// Fixed 'ConfigFileFlush'.  Hopefully ending the ongoing changes to
// the code which determines what file name to use, if the file doesn't
// exist.  Now, if the file name is explicitly stated, that's the one
// we use, else we use that SymantecNewPath() call.
//
//    Rev 1.35   19 May 1993 15:41:12   DALLEE
// In ConfigFileFlush() put back call to ConfigNewSymantecPath().  If the
// config file does not exist, CFGFILE.szPath is set to
// "[drive]:\SYMANTEC\[filename]" even if the directory "[drive]:\SYMANTEC"
// does not exist.  ConfigNewSymantedPath() makes sure we create the file
// in a valid directory.
//
//
//    Rev 1.34   12 May 1993 17:52:12   EHU
// Ifdef out reference to ConfigTextInit for SYM_NLM since NAVSYM_NLM doesn't
// use any of the text stuff and it currently won't link with it in.
//
//    Rev 1.33   11 May 1993 00:39:20   BRAD
// Changed so file is opened in READ_ONLY mode, rather
// than READ_WRITE.
//
//    Rev 1.32   07 May 1993 15:51:18   EHU
// Added some includes for SYM_NLM for new platform.h, platnlm.h, file.h,
// and xapi.h.
//
//    Rev 1.31   04 May 1993 18:06:36   BRAD
// In ConfigFileInit(), if its a new file, we allocate at
// least 1 byte.  We need to do this, so we get a handle
// back from MemAlloc().  malloc() will return NULL, if the
// size is 0.
//
//    Rev 1.30   30 Mar 1993 20:50:42   MARKL
// Changed ifdef for ConfigFileFlush() to be more friendly.
//
//    Rev 1.29   30 Mar 1993 20:39:54   MARKL
// In ConfigFileFlush the following was being done if the file did not
// exist:
//         *lpConfig->szPath = '\0';
//         ConfigNewSymantecPath (...);
// This appears to destroy a perfectly good value in lpConfig->szPath
// that had been established in the ConfigFileInit() routine for all cases.
//
//
//    Rev 1.28   29 Mar 1993 14:47:30   DAVID
// Merged in changes from Quake 0100 branch:
// Search \SYMANTEC directory for SYMCFG.BIN on both program disk and
// boot disk if we can't find the file elsewhere.
//
//    Rev 1.27   15 Mar 1993 20:10:26   JMILLARD
// Fix problem with incorrect CRC's - the problem was that if a file got
// shorter, the write of the shorter contents left the file at its original
// size - this confused the CRC stuff, since it uses the file size to tell
// how much data there is.
//
//    Rev 1.26   16 Feb 1993 16:18:46   JOHN
// Changed IsFloppy() to just call DiskGetType(), now that the disk type
// code is in the same DLL as SYMCFG.
//
//    Rev 1.25   12 Feb 1993 16:17:00   ED
// Changed ConfigGetSymantecDir to look no more for SYMPRIM.DLL when
// trying to determine the common directory.  SYMPRIM.DLL has gone the
// way of the Dodo, so now we assume that WE are in the common directory
// and use our own path.
//
//    Rev 1.24   12 Feb 1993 15:26:02   ENRIQUE
// Main Config Module.
//
//    Rev 1.23   07 Feb 1993 11:39:16   DLEVITON
// Fixes for multiple concurrent users of the same file
//
//    Rev 1.22   13 Jan 1993 23:34:54   JOHN
// (1) Changed ConfigFlush() to return an error code.
// (2) Changed ConfigSearchSymantecPath() to search the PATH if it can't
//     find the file in the SYMANTEC directory.  This is done even if a
//     SYMANTEC environment variable exists.
//
//    Rev 1.21   15 Dec 1992 06:51:26   ED
// Default to the program path if no common directory is found
//
//    Rev 1.20   14 Dec 1992 11:40:20   ED
// Added a floppy check routine so files won't be created on floppies
// unless you really want them to.
//
//    Rev 1.19   14 Dec 1992 11:39:20   ED
// Added floppy checking for
//
//    Rev 1.18   02 Dec 1992 11:51:42   HENRI
// Added override of CFG_NO_CREATE in ConfigFileFlush.  This is
// accomplished by passing in CFG_FORCE_CREATE in the wFlags parameter.
//
//    Rev 1.17   12 Nov 1992 01:44:04   MARKL
// If-defed around code SYM_NLM doesn't need.
//
//    Rev 1.16   12 Nov 1992 01:35:32   MARKL
// Added the CFG_FULLPATH_BINARY & CFG_FULLPATH_TEXT option.  Having the
// flag CFG_FULLPATH_FILE set tells symcfg.c to simply take the file in
// lpConfig->lpBaseName and treat that as the fully qualified path of the
// configuration file.
//
//
//    Rev 1.15   31 Oct 1992 11:24:16   ED
// Fixed some quirks in ConfigFileFlush, like freeing handles for global files
// (not good!).  Also, we now return CFG_SUCCESS instead of the number of bytes
// written if the function is successful!
//
//    Rev 1.14   29 Oct 1992 18:13:44   JMILLARD
// ConfigFileFlush doesn't use parameter wFlag - temporarily assign to
// self to get rid of Watcom warning.
//
//    Rev 1.13   26 Oct 1992 15:51:50   MARKK
// Added null handle test
//
//    Rev 1.12   20 Oct 1992 12:15:58   ED
// Added Flush functions
//
//    Rev 1.11   12 Oct 1992 16:16:46   EHU
// SYM_NLM changes and bug fix in ConfigFileInit.
//
//
//    Rev 1.10   05 Oct 1992 16:35:54   EHU
// Moved prototypes of exported functions to symcfg.h.
//
//
//    Rev 1.9   05 Oct 1992 09:46:38   ED
// Added ConfigSearchSymantecPath and ConfigNewSymantecPath.
//
// Changed ConfigFileInit to only search for local files, not global files.
// If you have the SYMGLOB environment variable set, we'll look there for
// the global config files, otherwise, we don't bother looking.  It's a
// performance thing.
//
//
//    Rev 1.8   02 Oct 1992 07:20:52   ED
// Another fix in ConfigFileTerminate
//
//    Rev 1.7   02 Oct 1992 06:55:52   ED
// Fixed a minor problem with the last fix
//
//    Rev 1.6   01 Oct 1992 14:49:06   ED
// Added flags parameters to the init and terminate functions
//
//    Rev 1.5   29 Sep 1992 16:47:40   EHU
// Made some private functions public.  These take LPCFGFILE as an argument so
// you aren't limited to the built-in filenames.
//
//    Rev 1.4   18 Sep 1992 08:12:40   ED
// Fixed a few things
//
//    Rev 1.3   14 Sep 1992 14:52:12   MARKK
// Added module_name
//
//    Rev 1.2   08 Sep 1992 09:11:48   ED
// Spelling error
//
//    Rev 1.1   08 Sep 1992 09:06:14   ED
// Final, version 1.0
//
//    Rev 1.0   03 Sep 1992 09:10:16   ED
// Initial revision.
//
//    Rev 1.0   17 Aug 1992 09:58:00   ED
// Initial revision.
 ************************************************************************/

#include "platform.h"

#ifndef SYM_NLM
#include <stdlib.h>
#else
#include <fcntl.h>
#include <io.h>
#endif

#include "file.h"
#include "xapi.h"
#ifdef SYM_WIN32
#include "winreg.h"
#endif
#include "symcfg.h"

#ifndef SYM_NLM

#include "disk.h"
#ifdef SYM_WIN
#include "undoc.h"
#endif

#endif

MODULE_NAME;

#ifndef SYM_NLM
BOOL LOCAL PASCAL IsFloppy(char dl);

BYTE    ENV_PATH[]     = "PATH";        // PATH environment variable
BYTE    ENV_SYMANTEC[] = "SYMANTEC";    // SYMANTEC environment variable
BYTE    ENV_SYMGLOB[]  = "SYMGLOB";     // SYMGLOB environment variable

CFGFILE rLocalText      = { CFG_LOCAL_TEXT,    "SYMCFG.INI" };
CFGFILE rLocalBinary    = { CFG_LOCAL_BINARY,  "SYMCFG.BIN" };
CFGFILE rGlobalText     = { CFG_GLOBAL_TEXT,   "SYMGLOB.INI" };
CFGFILE rGlobalBinary   = { CFG_GLOBAL_BINARY, "SYMGLOB.BIN" };
#endif

#ifdef SYM_NLM
#define OPEN_MODE               O_RDONLY
#else
#define OPEN_MODE               (OF_READ | DENY_WRITE_FILE)
#endif

#define MAX_PATH_BUFFER         512

#ifndef SYM_NLM
void LOCAL PASCAL StripSemiColons (LPSTR lpStrStart);
#endif
UINT LOCAL PASCAL ParsePath (LPSTR lpPathBuffer, LPSTR lpFullName,
                             LPCSTR lpSearchName, LPSTR pbFirstValid);

#ifndef SYM_NLM
/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Initialize all configuration files                              *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 09/08/1992 ED Created.                                               *
 ************************************************************************/

void SYM_EXPORT WINAPI ConfigInit (UINT wFlags)
{
    ConfigFileInit (&rLocalText, wFlags);
    ConfigFileInit (&rLocalBinary, wFlags);
    ConfigFileInit (&rGlobalText, wFlags);
    ConfigFileInit (&rGlobalBinary, wFlags);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Flush all configuration files                                   *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 09/08/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigFlush (UINT wFlags)
{
    UINT        uErr;

    uErr  = ConfigFileFlush (&rLocalText, wFlags);
    uErr |= ConfigFileFlush (&rLocalBinary, wFlags);
    uErr |= ConfigFileFlush (&rGlobalText, wFlags);
    uErr |= ConfigFileFlush (&rGlobalBinary, wFlags);

    if (uErr)
        {
        // We want to call ConfigFileFlush() for all the files, even if
        // we get errors on some of them.  Because we don't stop after
        // the first failure, we really can't return an exact error code.
        // Instead, we just return a generic "flush failed" error.
        uErr = CFG_FLUSH_ERROR;
        }

    return(uErr);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Terminate all configuration files                               *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      CFG_SUCCESS         no error                                    *
 *      CFG_FILE_ERROR      error writing config file                   *
 *      CFG_MEMORY_ERROR    config memory block trashed                 *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 09/08/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigTerminate (UINT wFlags)
{
    UINT    uRetval = CFG_SUCCESS;
    UINT    uRetval2;

    if ( (uRetval2 = ConfigFileTerminate (&rLocalText, wFlags)) != CFG_SUCCESS )
        uRetval = uRetval2;

    if ( (uRetval2 = ConfigFileTerminate (&rLocalBinary, wFlags)) != CFG_SUCCESS )
        uRetval = uRetval2;

    if ( (uRetval2 = ConfigFileTerminate (&rGlobalText, wFlags)) != CFG_SUCCESS )
        uRetval = uRetval2;

    if ( (uRetval2 = ConfigFileTerminate (&rGlobalBinary, wFlags)) != CFG_SUCCESS )
        uRetval = uRetval2;

    return (uRetval);
}
#endif  // ifndef SYM_NLM


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Finds the specified configuration file and initializes the      *
 *      record that maintains the file.                                 *
 *                                                                      *
 * Return Value:                                                        *
 *      CFG_SUCCESS                                                     *
 *      CFG_FILE_ERROR                                                  *
 *      CFG_MEMORY_ERROR                                                *
 *      CFG_FILE_NOT_FOUND                                              *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/17/1992 ED Created.                                               *
 * 28aug93 JMILLARD - move free of alloc'c memory end, and free on any
 *                    error - otherwise CRC errors didn't free the memory
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigFileInit (LPCFGFILE lpConfig, UINT wFlags)
{
    UINT        wErr = CFG_SUCCESS;
    UINT        wResult = (UINT) ERR;
    HFILE       hFile = HFILE_ERROR;
    LPSTR       lpBuffer;
    DWORD       dwSize;

    if (lpConfig->wClients > 0)
        {
        lpConfig->wClients++;
        wErr = CFG_SUCCESS;
        goto Exit;
        }

    lpConfig->hMem   = NULL;            // mark no memory allocated yet
    lpConfig->bDirty = FALSE;
    lpConfig->dwSize = 0;               // start with nothing

                                        // Try to prevent a caller from
                                        // screwing up global data by passing
                                        // in inappropriate flags.
    if (lpConfig->wFlags)
        wFlags &= 0x06C0;
    lpConfig->wFlags |= wFlags;

                                        // For fixed files, take the file
                                        // name is lpConfig->lpBaseName as
                                        // the fully qualified path.
    if (lpConfig->wFlags & CFG_FULLPATH_FILE)
        {
        STRCPY (lpConfig->szPath, lpConfig->lpBaseName);
        wResult = NOERR;
        }

#ifndef SYM_NLM
                                        // For global files, don't search for
                                        // them.  things get too slow if we do.
                                        // if the SYMGLOB environment variable
                                        // exists, we expect the global files
                                        // to be there, otherwise we assume
                                        // they don't exist.
    else if (lpConfig->wFlags & CFG_GLOBAL_FILE)
        {
        if (DOSGetEnvironment(ENV_SYMGLOB, lpConfig->szPath, sizeof
                                (lpConfig->szPath)) == NOERR)
            {
            NameAppendFile (lpConfig->szPath, lpConfig->lpBaseName);
            wResult = NOERR;
            }
        }
    else                                // do a real search for local files
        wResult = ConfigSearchSymantecPath (lpConfig->lpBaseName, lpConfig->szPath);
#endif

    lpConfig->bExists = (BOOL)(wResult == NOERR);

    if (lpConfig->bExists)
        {                               // open the config file
        hFile = FileOpen (lpConfig->szPath, OPEN_MODE);

                                        // see if something bad happened.
                                        // if so, ignore the found file
        if (hFile == HFILE_ERROR)
            lpConfig->bExists = FALSE;
        else                            // otherwise, size it
            lpConfig->dwSize = FileLength (hFile);
        }

    dwSize = lpConfig->dwSize;

                                        // malloc() returns NULL, if 0 bytes
                                        // is requested (new file).
    if (dwSize == 0)
        dwSize++;
                                        // allocate enough memory to hold
                                        // the file contents

                                        // the +1 on the size is so that there
                                        // is an EOS at the end of the file.
                                        // It simplifies some of the text
                                        // parsing

    if (lpConfig->hMem = MemAlloc (GHND | GMEM_DDESHARE, dwSize+1))
        {
        if (lpConfig->bExists)
            {                           // lock the memory and read the file
            if (lpBuffer = MemLock (lpConfig->hMem))
                {
                wErr = FileRead (hFile, lpBuffer, (UINT) lpConfig->dwSize);

                MemUnlock (lpConfig->hMem, lpBuffer);

                                        // if file error, get rid of the block
                if (wErr != (UINT) lpConfig->dwSize)
                    {
                    wErr = CFG_FILE_ERROR;
                    }
                else
                    wErr = CFG_SUCCESS;
                }

            FileClose (hFile);
            hFile = HFILE_ERROR;        // mark closed
            }
        }
    else
        wErr = CFG_MEMORY_ERROR;

    if (wErr == CFG_SUCCESS)
        {                               // create a header for a new file,
                                        // or check the integrity of an
                                        // existing file
#ifndef SYM_NLM
        if (lpConfig->wFlags & CFG_BINARY_FILE)
            wErr = ConfigBlockInit (lpConfig);
        else
            wErr = ConfigTextInit (lpConfig);
#else
        wErr = ConfigBlockInit (lpConfig);
#endif
        if (wErr == CFG_SUCCESS)
            lpConfig->wClients = 1;
        }
Exit:
    lpConfig->wError = wErr;

    // ensure file is closed

    if (hFile != HFILE_ERROR)
        {
        FileClose (hFile);
        }

    return (wErr);

}   // ConfigFileInit



UINT SYM_EXPORT WINAPI ConfigFileTerminate (LPCFGFILE lpConfig, UINT wFlags)
{
    UINT        wRet;

    if (lpConfig->hMem != NULL)
        wRet = ConfigFileFlush (lpConfig, wFlags);
    else
        wRet = CFG_MEMORY_ERROR;        // you already know this

    if (lpConfig->wClients > 0)
        lpConfig->wClients--;

    if (lpConfig->wClients == 0 && lpConfig->hMem != NULL)
        {
        MemFree (lpConfig->hMem);
        lpConfig->hMem = NULL;
        }

    return (wRet);
}


UINT SYM_EXPORT WINAPI ConfigFileFlush (LPCFGFILE lpConfig, UINT wFlags)
{
    UINT        wErr = CFG_SUCCESS;
    HFILE       hFile;
    LPSTR       lpBuffer;
                                        // Don't create the file if it didn't
                                        // exist when we started.  This is
                                        // useful for Install programs
    if (!(wFlags & CFG_FORCE_CREATE) && (lpConfig->wFlags & CFG_NO_CREATE))
        return (CFG_SUCCESS);
                                        // don't bother trying to flush a
                                        // global file
    if (lpConfig->wFlags & CFG_GLOBAL_FILE)
        return (CFG_SUCCESS);

    // used to call checksum here with ConfigBlockTerminate - but if
    // the file isn't dirty, it is a waste
    // of time, and is positively harmful if the file failed the CRC check
    // when it was opened. In that case there may be absolute garbage in the 
    // file and there may well be no valid header in the file. 
    // ConfigBlockTerminate then may get very confused.

    // ConfigBlockTerminate call was moved inside following code

    if (lpConfig->bDirty)
        {
                                        // checksum the file
        if (lpConfig->wFlags & CFG_BINARY_FILE)
            wErr = ConfigBlockTerminate (lpConfig);

        if (!lpConfig->bExists)
            {
                                        // If we gave a fullpath, then use
                                        // a full path, even if the file
                                        // currently doesn't exist.
            if ( !(lpConfig->wFlags & CFG_FULLPATH_FILE) )
                {
                *lpConfig->szPath = '\0';   // blank the path
                ConfigNewSymantecPath (lpConfig->szPath, lpConfig->lpBaseName);
                }

                                        // don't create files on floppies,
                                        // unless specified
#ifndef SYM_NLM
            if (!(wFlags & CFG_FLOPPY_CREATE) && IsFloppy(*lpConfig->szPath))
                {
                lpConfig->bDirty = FALSE;
                return (CFG_SUCCESS);
                }
#endif
            lpConfig->bExists = TRUE;
            }

            // recreate the config file to truncate it - I'm going to write
            // out the entire file, and I need the size to be exactly right
            // otherwise the CRC checks will fail.

        hFile = FileCreate (lpConfig->szPath, FA_NORMAL);

        if (hFile != HFILE_ERROR)
            {                           // lock the memory and write the file
            if (lpBuffer = MemLock (lpConfig->hMem))
                {
                wErr = FileWrite (hFile, lpBuffer, (UINT) lpConfig->dwSize);

                if (wErr != (UINT) lpConfig->dwSize)
                    wErr = CFG_FILE_ERROR;
                else
                    {
                    lpConfig->bDirty = FALSE;
                    wErr = CFG_SUCCESS;
                    }
                MemUnlock (lpConfig->hMem, lpBuffer);
                }

            FileClose (hFile);
            }
        else
            wErr = CFG_FILE_ERROR;
        }

    return (wErr);
}

#ifndef SYM_NLM

void SYM_EXPORT WINAPI ConfigDisableCRC(void)
{
    rLocalBinary.wFlags |= CFG_NO_CRC_CHECKS;
    rGlobalBinary.wFlags |= CFG_NO_CRC_CHECKS;
}

void SYM_EXPORT WINAPI ConfigEnableCRC(void)
{
    rLocalBinary.wFlags &= ~CFG_NO_CRC_CHECKS;
    rGlobalBinary.wFlags &= ~CFG_NO_CRC_CHECKS;
}


/*----------------------------------------------------------------------*
 * This function finds a configuration file.                            *
 *                                                                      *
 * The search order is:                                                 *
 *      0.  The Symantec common directory                               *
 *      1.  Check startup name in DOS environment                       *
 *      2.  Check in the current directory.                             *
 *      3.  Search the path.                                            *
 *      4.  Search \SYMANTEC on the drive we loaded from.               *
 *      5.  Search \SYMANTEC on the boot drive.                         *
 *                                                                      *
 * 'lpSearchName' is the file to search for.                            *
 * 'lpReturnname' must point to an array of at least 81 bytes.          *
 *                                                                      *
 * Returns:     NOERR   If file found (lpReturnName set)                *
 *              ERR     Otherwise (lpReturnName is garbage)             *
 *----------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI ConfigSearchSymantecPath (LPCSTR lpSearchName, LPSTR lpReturnName)
{
                                // get the name of the common directory
    if (ConfigGetSymantecDir(lpReturnName) == NOERR)
        {
        NameAppendFile(lpReturnName, lpSearchName);

        if (NameGetType(lpReturnName) & NGT_IS_FILE)
            return (NOERR);
        }

    if (ConfigSearchPath(lpReturnName, lpSearchName) == NOERR)
        {
        StripSemiColons(lpReturnName);
        NameAppendFile(lpReturnName, lpSearchName);
        return (NOERR);
        }

    DOSGetProgramName(lpReturnName);
    if (lpReturnName[0])
        {
        lpReturnName[3] = '\0';
        STRCAT(lpReturnName, ENV_SYMANTEC);
        NameAppendFile(lpReturnName, lpSearchName);

        if (NameGetType(lpReturnName) & NGT_IS_FILE)
            return (NOERR);

        lpReturnName[0] = DiskGetBootDrive();
        if (lpReturnName[0] && (NameGetType(lpReturnName) & NGT_IS_FILE))
            return (NOERR);
        }

    return ((UINT) ERR);
}



/*----------------------------------------------------------------------*
 * This function finds a configuration file.                            *
 *                                                                      *
 * The search order is:                                                 *
 *      1.  Check dir specified by first environment variable.          *
 *      2.  Check dir specified by second environment variable.         *
 *      3.  Check startup name in DOS environment                       *
 *      4.  Check in the current directory.                             *
 *      5.  Search the path.                                            *
 *                                                                      *
 * 'file' is the file to search for.                                    *
 * 'fullName' must point to an array of at least 81 bytes.              *
 *                                                                      *
 * Returns:     NOERR   If file found (fullName set)                    *
 *              ERR     Otherwise (fullName is garbage)                 *
 *----------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI ConfigSearchVarPath (
    LPCSTR      lpFirstVar,
    LPCSTR      lpSecondVar,
    LPCSTR      lpSearchName,
    LPSTR       lpReturnName,
    BOOL        bForceVariable)
{
    auto        BYTE    szBuffer[MAX_PATH_BUFFER];
    auto        BYTE    bFirstValid;


    if (lpFirstVar && DOSGetEnvironment((LPSTR) lpFirstVar, szBuffer, MAX_PATH_BUFFER) == NOERR)
        {
        if (ParsePath(szBuffer, lpReturnName, lpSearchName, &bFirstValid) == NOERR)
            return(NOERR);

        // Force ERR return if Env Var 1 exists...
        if (bFirstValid && bForceVariable)
            return ((UINT) ERR);
        }

    else if (lpSecondVar && DOSGetEnvironment((LPSTR)lpSecondVar, szBuffer, SYM_MAX_PATH) == NOERR)
        {
        if (ParsePath(szBuffer, lpReturnName, lpSearchName, &bFirstValid) == NOERR)
            return(NOERR);
        }

    else if (ConfigSearchPath(lpReturnName, lpSearchName) == NOERR)
        {
        StripSemiColons(lpReturnName);
        NameAppendFile(lpReturnName, lpSearchName);
        return (NOERR);
        }

    return ((UINT) ERR);
}

#endif  // !SYM_NLM

/*----------------------------------------------------------------------*
 * This routine generates a path name for creating a brand-new          *
 * configuration file.                                                  *
 *                                                                      *
 * It tries to return these directories in order:                       *
 *      1. The Symantec common directory                                *
 *      2. Current directory                                            *
 *----------------------------------------------------------------------*/

void SYM_EXPORT WINAPI ConfigNewSymantecPath (LPSTR lpReturnName, LPCSTR lpTargetName)
{
#ifndef SYM_NLM                             // just look in CWD for SYM_NLM
                                // get the name of the common directory
    if (ConfigGetSymantecDir (lpReturnName) == NOERR)
        {
        NameAppendFile(lpReturnName, lpTargetName);
        }
    else
#endif
        STRCPY(lpReturnName, lpTargetName);
}

#ifndef SYM_NLM

/*----------------------------------------------------------------------*
 * This routine generates a path name for creating a brand-new          *
 * configuration file.                                                  *
 *                                                                      *
 * It tries to return these directories in order:                       *
 *      0. The Symantec common directory                                *
 *      1. Dir specified by first environment var                       *
 *      2. Dir specified by second environment var                      *
 *      3. Dir on path containing search name                           *
 *      4. Current directory                                            *
 *----------------------------------------------------------------------*/

void SYM_EXPORT WINAPI ConfigNewVarPath (
    LPCSTR      lpFirstVar,
    LPCSTR      lpSecondVar,
    LPSTR       lpReturnName,
    LPCSTR      lpTargetName,
    LPCSTR      lpNeighbor)
{
                                // get the name of the common directory
    if (ConfigGetSymantecDir (lpReturnName) == NOERR)
        {
        NameAppendFile(lpReturnName, lpTargetName);
        }
                                // check the first environment variable
    else if (lpFirstVar && DOSGetEnvironment((LPSTR)lpFirstVar, lpReturnName, SYM_MAX_PATH) == NOERR)
        {
        StripSemiColons(lpReturnName);
        if (NameGetType(lpReturnName) & NGT_IS_DIR)
            NameAppendFile(lpReturnName, lpTargetName);
        }
                                // check the second environment variable
    else if (lpSecondVar && DOSGetEnvironment((LPSTR)lpSecondVar, lpReturnName, SYM_MAX_PATH) == NOERR)
        {
        StripSemiColons(lpReturnName);
        if (NameGetType(lpReturnName) & NGT_IS_DIR)
            NameAppendFile(lpReturnName, lpTargetName);
        }
    else if (lpNeighbor && ConfigSearchPath(lpReturnName, lpNeighbor) == NOERR)
        {
        if (NameGetType(lpReturnName) & NGT_IS_DIR)
            NameAppendFile(lpReturnName, lpTargetName);
        }

    if (!*lpReturnName)
        STRCPY(lpReturnName, lpTargetName);
}


/*----------------------------------------------------------------------*
 * Strip first semi-colon and all trailing chars from a string
 *----------------------------------------------------------------------*/

void LOCAL PASCAL StripSemiColons (LPSTR lpStrStart)
{
    LPSTR lpStr;

    for (lpStr = lpStrStart; *lpStr != EOS && *lpStr != ';'; ++lpStr)
        ;

    *lpStr = EOS;
}

/*-----------------------------------------------------------------------
 *
 * Description:
 *
 * Parameters:
 *
 * Return Value:
 *
 * See Also:
 *
 *-----------------------------------------------------------------------
 * 04/27/1992 PETERD Function Created.
 *----------------------------------------------------------------------*/

UINT LOCAL PASCAL ParsePath (
    LPSTR       lpPathBuffer,
    LPSTR       lpFullName,
    LPCSTR      lpSearchName,
    LPSTR       pbFirstValid)
{
    register    int     ip;
    auto        int     nNumPaths;
    auto        BYTE    szNameBuffer[SYM_MAX_PATH];
    auto        LPSTR   ptr;

    nNumPaths = DirParsePath(lpPathBuffer);

    ptr = lpPathBuffer;

    NameToFull(szNameBuffer, ptr);
    *pbFirstValid = (BYTE)(NameGetType(szNameBuffer) & NGT_IS_DIR);

    for (ip = 0; ip < nNumPaths; ip++)
        {
        StringMaxCopy (szNameBuffer, ptr, 64);
        NameAppendFile(szNameBuffer, lpSearchName);
        NameToFull(lpFullName, szNameBuffer);

        if (NameGetType(lpFullName) & NGT_IS_FILE)
            return(NOERR);

        ptr += STRLEN(ptr) + 1;
        }

    return((UINT) ERR);
}

/*----------------------------------------------------------------------*
 * This procedure searches the path for a file and returns the full     *
 * path where it found the file.                                        *
 *                                                                      *
 * The search order is as follows:                                      *
 *                                                                      *
 *      1.  Check startup name in DOS environment                       *
 *      2.  Check in the current directory.                             *
 *      3.  Search the path.                                            *
 *                                                                      *
 *                                                                      *
 * Returns:     NOERR   Found the file.                                 *
 *              ERR     Could not find the file.                        *
 *                                                                      *
 *              full_path       Contains the directory where file found *
 *              "x:\"           If there was no match, where x is the   *
 *                              current drive letter.                   *
 *                                                                      *
 * Note:        This procedure always returns a drive letter at the     *
 *              front of the path.  For example, "C:\WRITING", but      *
 *              there is no trailing '\' except for the root directory. *
 * NOTE:        full_path must be at least 80 characters long.          *
 *----------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI ConfigSearchPath (LPSTR lpReturnPath, LPCSTR lpFileToFind)
{
    auto        BYTE    szEnvPath[MAX_PATH_BUFFER];
    auto        BYTE    szPathname[SYM_MAX_PATH];
    auto        LPSTR   lpPtr;
    auto        int     nNumPaths;
    auto        int     i;

    /*                                                                  *
     * Check the DOS 3.x environment for our program's name             *
     *                                                                  */
    DOSGetProgramName(szEnvPath);       // Store program name in path
    if (*szEnvPath)                     // Do we have a name?
        {
        NameToFull(szPathname, szEnvPath);
        NameStripFile(szPathname);
        NameAppendFile(szPathname, lpFileToFind);

        if (NameGetType(szPathname) & NGT_IS_FILE)
            {
            NameStripFile(szPathname);
            STRCPY(lpReturnPath, szPathname);
            return(NOERR);
            }
        }
    /*                                                                  *
     * Check to see if the file's in the current directory.             *
     *                                                                  */
    NameToFull(szPathname, (LPSTR) lpFileToFind);
    if (NameGetType(szPathname) & NGT_IS_FILE)
        {
        NameStripFile(szPathname);
        STRCPY(lpReturnPath, szPathname);
        return(NOERR);
        }

    /*                                                                  *
     * Search through the DOS PATH.                                     *
     *                                                                  */
                                        /* Read PATH from environment   */
    DOSGetEnvironment (ENV_PATH, szEnvPath, sizeof(szEnvPath));
    nNumPaths = DirParsePath(szEnvPath);        // Separate into paths

    lpPtr = (LPSTR) szEnvPath;                  // Point to start of path
    for (i = 0; i < nNumPaths; i++)             // Search through path list
        {
        StringMaxCopy (szPathname, lpPtr, 64);
        NameAppendFile(szPathname, lpFileToFind);
        NameToFull(lpReturnPath, szPathname);

        if (NameGetType(lpReturnPath) & NGT_IS_FILE)
            {
            NameStripFile(lpReturnPath);
            return(NOERR);
            }

        lpPtr += STRLEN(lpPtr) + 1;
        }

    FileInitPath (lpReturnPath);
    lpReturnPath[0] = DiskGet();                /* Set drive letter             */
    return((UINT) ERR);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Returns the location of the Symantec common directory.  This    *
 *      is found using the SYMANTEC environment variable, or if that    *
 *      does not exist, by using the directory where SYMPRIM.DLL is     *
 *      located (only in Windows).                                      *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/


UINT SYM_EXPORT WINAPI ConfigGetSymantecDir (LPSTR lpDirName)
{
    BYTE        szEnvPath[MAX_PATH_BUFFER];
    UINT        wResult = (UINT) ERR;

    if (DOSGetEnvironment (ENV_SYMANTEC, szEnvPath, sizeof(szEnvPath)) == NOERR)
        {
        StringMaxCopy (lpDirName, szEnvPath, 64);

        if (NameGetType(lpDirName) & NGT_IS_DIR)
            wResult = NOERR;
        }

#ifdef  SYM_WIN
    if (wResult == ERR)
        {
        extern  HINSTANCE       hInst_SYMKRNL;

                                        // find out what the full path of
                                        // our wonderful little library is...
        GetModuleFileName (hInst_SYMKRNL, lpDirName, SYM_MAX_PATH);

        NameStripFile (lpDirName);      // strip the file, obviously
        wResult = NOERR;
        }
#endif

    if (wResult == ERR)                 // default to the program directory
        {
        DOSGetProgramName (lpDirName);  // get the program path
        NameStripFile (lpDirName);      // leave only the directory
        wResult = NOERR;
        }

    return (wResult);
}

#endif  // !SYM_NLM

#ifdef OLD_CRC

/************************************************************************
 *                                                                      *
 * Description: Routine to calculate a CCITT 16-bit CRC                 *
 *                                                                      *
 * Parameters:                                                          *
 *              pcData  - The data for which to get the CRC             *
 *              wLength - The length of the data                        *
 * Return Value:                                                        *
 *              The 16-bit CRC                                          *
 * See Also:                                                            *
 *                                                                      *
 * Notes:                                                               *
 *                     16   12   5                                      *
 *  The polynomial is x  + x  + x + 1                                   *
 *                                                                      *
 *  In binary, this is 1 001 0000 0010 0001, and in hex it is 0x11021   *
 *                                                                      *
 *  A 17 bit register is simulated by testing the MSB before shifting,  *
 *  which allows us to express the polynomial as a 16 bit value, 0x1021.*
 *  Due to the way the CRC is processed, the bits in the polynomial are *
 *  stored in reverse order. This makes the polynomial 0x8408.          *
 *                                                                      *
 *  To test the validity of the CRC, append the CRC to the end of the   *
 *  data for which the CRC was calculated. Now calculate the CRC of the *
 *  data and its CRC. If the new CRC equals CFG_CRC_OK_VALUE, the data  *
 *  is valid.                                                           *
 *                                                                      *
 ************************************************************************
 * 07/12/1991 TORSTEN Function Created.                                 *
 ************************************************************************/

#define POLY    0x8408

WORD SYM_EXPORT WINAPI ConfigGetCRC (LPCSTR lpData, WORD wLength)
{
    BYTE        byShift;
    WORD        wData;
    WORD        wCRC;

    wCRC = 0xFFFF;

    if (wLength == 0)
        return (~wCRC);

    do  {
        for (byShift = 0, wData = (WORD) 0xFF & *lpData++; byShift < 8; byShift++, wData >>= 1)
            if ((wCRC & 0x0001) ^ (wData & 0x0001))
                wCRC = (wCRC >> 1) ^ POLY;
            else
                wCRC >>= 1;
        } while (--wLength);

    wCRC = ~wCRC;
    wData = wCRC;
    wCRC = (wCRC << 8) | (wData >> 8 & 0xFF);

    return (wCRC);
}

#else

/* CRC */

#define   P    0x1021

    /* number of bits in CRC: don't change it. */
#define W 16

    /* this the number of bits per char: don't change it. */
#define B 8

WORD SYM_EXPORT WINAPI ConfigGetCRC (LPCSTR lpData, WORD wLength)
{

static WORD wCrcTable[1<<B] = {
    0x0000,  0x1021,  0x2042,  0x3063,  0x4084,  0x50a5,  0x60c6,  0x70e7,
    0x8108,  0x9129,  0xa14a,  0xb16b,  0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
    0x1231,  0x0210,  0x3273,  0x2252,  0x52b5,  0x4294,  0x72f7,  0x62d6,
    0x9339,  0x8318,  0xb37b,  0xa35a,  0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
    0x2462,  0x3443,  0x0420,  0x1401,  0x64e6,  0x74c7,  0x44a4,  0x5485,
    0xa56a,  0xb54b,  0x8528,  0x9509,  0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
    0x3653,  0x2672,  0x1611,  0x0630,  0x76d7,  0x66f6,  0x5695,  0x46b4,
    0xb75b,  0xa77a,  0x9719,  0x8738,  0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
    0x48c4,  0x58e5,  0x6886,  0x78a7,  0x0840,  0x1861,  0x2802,  0x3823,
    0xc9cc,  0xd9ed,  0xe98e,  0xf9af,  0x8948,  0x9969,  0xa90a,  0xb92b,
    0x5af5,  0x4ad4,  0x7ab7,  0x6a96,  0x1a71,  0x0a50,  0x3a33,  0x2a12,
    0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,  0x9b79,  0x8b58,  0xbb3b,  0xab1a,
    0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,  0x2c22,  0x3c03,  0x0c60,  0x1c41,
    0xedae,  0xfd8f,  0xcdec,  0xddcd,  0xad2a,  0xbd0b,  0x8d68,  0x9d49,
    0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,  0x3e13,  0x2e32,  0x1e51,  0x0e70,
    0xff9f,  0xefbe,  0xdfdd,  0xcffc,  0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
    0x9188,  0x81a9,  0xb1ca,  0xa1eb,  0xd10c,  0xc12d,  0xf14e,  0xe16f,
    0x1080,  0x00a1,  0x30c2,  0x20e3,  0x5004,  0x4025,  0x7046,  0x6067,
    0x83b9,  0x9398,  0xa3fb,  0xb3da,  0xc33d,  0xd31c,  0xe37f,  0xf35e,
    0x02b1,  0x1290,  0x22f3,  0x32d2,  0x4235,  0x5214,  0x6277,  0x7256,
    0xb5ea,  0xa5cb,  0x95a8,  0x8589,  0xf56e,  0xe54f,  0xd52c,  0xc50d,
    0x34e2,  0x24c3,  0x14a0,  0x0481,  0x7466,  0x6447,  0x5424,  0x4405,
    0xa7db,  0xb7fa,  0x8799,  0x97b8,  0xe75f,  0xf77e,  0xc71d,  0xd73c,
    0x26d3,  0x36f2,  0x0691,  0x16b0,  0x6657,  0x7676,  0x4615,  0x5634,
    0xd94c,  0xc96d,  0xf90e,  0xe92f,  0x99c8,  0x89e9,  0xb98a,  0xa9ab,
    0x5844,  0x4865,  0x7806,  0x6827,  0x18c0,  0x08e1,  0x3882,  0x28a3,
    0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,  0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
    0x4a75,  0x5a54,  0x6a37,  0x7a16,  0x0af1,  0x1ad0,  0x2ab3,  0x3a92,
    0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,  0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
    0x7c26,  0x6c07,  0x5c64,  0x4c45,  0x3ca2,  0x2c83,  0x1ce0,  0x0cc1,
    0xef1f,  0xff3e,  0xcf5d,  0xdf7c,  0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
    0x6e17,  0x7e36,  0x4e55,  0x5e74,  0x2e93,  0x3eb2,  0x0ed1,  0x1ef0
    };

    register WORD   wCRC;
    register LPSTR  cp = (LPSTR) lpData;
    register WORD   cnt = wLength;

    wCRC = 0xFFFF;

    if (wLength == 0)
        return (~wCRC);

    while( cnt-- )
        wCRC = (wCRC<<B) ^ wCrcTable[(wCRC>>(W-B)) ^ ((WORD) 0xFF & *cp++)];

    return( wCRC );
}

#endif  // else ifdef OLD_CRC






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
 ************************************************************************/

#ifndef SYM_NLM

BOOL LOCAL PASCAL IsFloppy(char dl)
{
    UINT wMajor, wMinor;

    DiskGetType(dl, &wMajor, &wMinor);

    return (BOOL)(wMajor == TYPE_REMOVEABLE && IsTypeFloppy(wMinor));
}

#endif

/************************************************************************
 *                                                                      *
 * Get/WritePrivateProfileInt/String covers for non-Windows platforms   *
 *                                                                      *
 ************************************************************************/
#ifndef SYM_WIN
#ifndef SYM_NLM                         // not NLM - not SYM_WIN is not DOS
                                        // For DOS only.
int SYM_EXPORT WINAPI GetPrivateProfileString(
    LPCSTR lpSection,
    LPCSTR lpEntry,
    LPCSTR lpDefault,
    LPSTR  lpBuffer,
    int    iSize,
    LPCSTR lpFileName )
{
    CFGFILE     cfgfile;
    CFGTEXTREC  cfgtextrec;
    UINT        uiStatus;
    
    MEMSET( &cfgfile, 0, sizeof( CFGFILE ) );
    cfgfile.wFlags     = CFG_FULLPATH_TEXT;
    cfgfile.lpBaseName = (LPBYTE)lpFileName;

    ConfigFileInit( &cfgfile, 0 );

    cfgtextrec.lpSwitch   = NULL;
    cfgtextrec.wFlags     = CFG_LOCAL_FIRST;
    cfgtextrec.lpSection  = (LPSTR)lpSection;
    cfgtextrec.lpEntry    = (LPSTR)lpEntry;
    cfgtextrec.wSource    = 0;

    lpBuffer[0] = EOS;

    uiStatus = ConfigFileReadString( &cfgfile, &cfgtextrec, lpBuffer, iSize );

    if ( uiStatus == 0 || uiStatus == (UINT)ERR )
        STRCPY( lpBuffer, lpDefault );

    ConfigFileTerminate( &cfgfile, 0 );

    return (int)STRLEN( lpBuffer );
}

int SYM_EXPORT WINAPI WritePrivateProfileString(
    LPCSTR lpSection,
    LPCSTR lpEntry,
    LPCSTR lpString,
    LPCSTR lpFileName )
{
    CFGFILE     cfgfile;
    CFGTEXTREC  cfgtextrec;
    UINT        uiStatus;

    MEMSET( &cfgfile, 0, sizeof( CFGFILE ) );
    cfgfile.wFlags     = CFG_FULLPATH_TEXT;
    cfgfile.lpBaseName = (LPBYTE)lpFileName;

    ConfigFileInit( &cfgfile, 0 );

    cfgtextrec.lpSwitch   = NULL;
    cfgtextrec.wFlags     = CFG_LOCAL_FIRST;
    cfgtextrec.lpSection  = (LPSTR)lpSection;
    cfgtextrec.lpEntry    = (LPSTR)lpEntry;
    cfgtextrec.wSource    = 0;

    uiStatus = ConfigFileWriteString( &cfgfile, &cfgtextrec, (LPSTR)lpString );

    ConfigFileTerminate( &cfgfile, 0 );

    return uiStatus == NOERR;
}

UINT SYM_EXPORT WINAPI GetPrivateProfileInt(
  LPCSTR lpSection,
  LPCSTR lpEntry,
  int    iDefault,
  LPCSTR lpFileName )
{
    CFGFILE     cfgfile;
    CFGTEXTREC  cfgtextrec;
    UINT        uiValue;
    
    MEMSET( &cfgfile, 0, sizeof( CFGFILE ) );
    cfgfile.wFlags     = CFG_FULLPATH_TEXT;
    cfgfile.lpBaseName = (LPBYTE)lpFileName;

    ConfigFileInit( &cfgfile, 0 );

    cfgtextrec.lpSwitch   = NULL;
    cfgtextrec.wFlags     = CFG_LOCAL_FIRST;
    cfgtextrec.lpSection  = (LPSTR)lpSection;
    cfgtextrec.lpEntry    = (LPSTR)lpEntry;
    cfgtextrec.wSource    = 0;

    if ( ConfigFileReadValue( &cfgfile, &cfgtextrec, &uiValue, NULL ) != NOERR )
        uiValue = (UINT)iDefault;

    ConfigFileTerminate( &cfgfile, 0 );

    return uiValue;
}

#endif
#endif

