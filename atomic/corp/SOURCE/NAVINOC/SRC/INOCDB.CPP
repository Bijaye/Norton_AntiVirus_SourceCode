// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVINOC/VCS/inocdb.cpv   1.1   20 May 1997 17:34:30   BILL  $
//
// Description:
//      Routines to handle NAV 3.0's inoculation database.
//      Insures that the correct DB is open for Inoc*() use.
//
// Contains:
//      GetInocStruct()
//      GetAnotherInocStruct()
//      GetUnusedInocStruct()
//      ReleaseInocStruct()
//      InocInit()
//      InocEnd()
//      BuildInocDBName()
//
// See Also:
//      INOCLOSE.C for InocClose()
//      INOCOPEN.C for InocOpen()
//      and all the other INOC*.C files for Inoc*().
//************************************************************************
// $Log:   S:/NAVINOC/VCS/inocdb.cpv  $
// 
//    Rev 1.1   20 May 1997 17:34:30   BILL
// Remove intel only calls (certlibn) and fail all calls from NT clients.
// 
//    Rev 1.0   06 Feb 1997 20:58:20   RFULLER
// Initial revision
// 
//    Rev 1.5   09 Oct 1996 15:51:46   JBRENNA
// Ops ... forget to add include for NAVUTIL.H.
// 
//    Rev 1.4   09 Oct 1996 15:28:12   JBRENNA
// Change STRUPR() to NavStrUpr() so that DBCS characters are not upper cased.
// 
//    Rev 1.3   26 Jun 1996 17:52:50   GWESTER
// Ported in changes and fixes from LuigiPlus.
// 
//    Rev 1.2   22 Mar 1996 10:06:02   jworden
// DBCS Enable
// 
//    Rev 1.1   14 Feb 1996 17:07:26   JWORDEN
// Process internal file names in OEM mode to allow for the innoculation
// of file names that include certain non-ANSI translatable characters
// 
//    Rev 1.0   30 Jan 1996 15:53:20   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 13:22:24   BARRY
// Initial revision.
// 
//    Rev 1.9   23 Mar 1995 17:00:04   SZIADEH
// changed CHAR to char
// 
//    Rev 1.8   23 Mar 1995 14:48:38   SZIADEH
// added UNC support.
// 
//    Rev 1.7   03 Feb 1995 16:10:44   BRAD
// Got working and added Long File name
// 
//    Rev 1.6   28 Nov 1994 20:13:44   MFALLEN
// Now also works for DOS platform.
// 
//    Rev 1.5   28 Nov 1994 18:56:38   MFALLEN
// CERTLIB is alive for W16
// 
//    Rev 1.4   28 Nov 1994 14:14:22   MFALLEN
// Compiles fine for W16 and hopefully also for W32
// 
//    Rev 1.3   16 May 1994 18:31:46   MFALLEN
// Removed old log junk.
// 
//    Rev 1.2   12 May 1994 10:44:10   MFALLEN
// cts4.h 2 ctsn.h
// 
//    Rev 1.1   25 Apr 1994 16:00:30   MFALLEN
// Just trying to make code compile. It's not working yet.
// 
//    Rev 1.0   19 Apr 1994 19:19:48   MFALLEN
// Initial revision.
// 
//    Rev 1.15   13 Sep 1993 03:42:14   BARRY
// Free lpInoculate links as they become unused to avoid fragmenting memory
// 
//    Rev 1.14   24 Aug 1993 13:37:06   BARRY
// Removed Inoc.lpQuick.  Locks on the fly now so DOS VMM can cope better
// 
//    Rev 1.13   23 Aug 1993 21:19:18   BARRY
// Fixed memory leak and now can cope without hQuick if necessary
// 
//    Rev 1.12   23 Aug 1993 00:30:02   BARRY
// Added MODULE_NAME and fixed MemUnlock arguments for VMM debugging
// 
//    Rev 1.11   30 Jul 1993 12:57:12   CGUC
// Adjusted FIL4_PROTECT 
// 
//    Rev 1.10   07 Jul 1993 23:14:44   CJKUO
// Change inocinit to also remember the inocfloppies flag
// 
//    Rev 1.9   16 Jun 1993 19:46:16   CJKUO
// Open R/W stuff
// 
//    Rev 1.8   15 Jun 1993 17:12:10   CJKUO
// Changed name to NAVINOC.DAT
// 
//    Rev 1.7   24 May 1993 18:17:34   CJKUO
// First attempt at making database opens and closes LIFO.
// Tested to the effect that it still works.  But no code exists 
// yet to take advantage of LIFO nature.
// 
// 
//    Rev 1.6   19 May 1993 20:50:20   BRAD
// 
//    Rev 1.5   19 May 1993 20:45:52   BRAD
// Added call to GetInocStructGivenFile().
// 
//    Rev 1.4   16 May 1993 16:30:24   CJKUO
// Add BuildInocDBName, change InocInit
// 
//    Rev 1.3   11 May 1993 15:54:02   DALLEE
// Call MemFreePtr() in InocEnd() to free memory.
// 
//    Rev 1.2   11 May 1993 15:05:46   CJKUO
// Add InocEnd (release all memory, reset drive) and InocInit (save drive)
// 
//    Rev 1.1   10 May 1993 16:45:08   CJKUO
// Allow any name for inocDB
// 
//    Rev 1.0   05 May 1993 20:20:52   CJKUO
// Initial revision.
//
//************************************************************************

#include "platform.h"
#include "options.h"
#include "xapi.h"
#include "cmbstr.h"
#include "inoc.h"
#include "file.h"
#include "disk.h"
#include "navutil.h"

//MODULE_NAME;

//************************************************************************
//************************************************************************

//************************************************************************
// GetAnotherInocStruct()
//
// Call this routine to get another inoculation db structure.
//
// Returns:
//      FALSE           Problem
//      TRUE            no problem
//************************************************************************

STATIC INOCInstances   FAR *lpInoculates = NULL;
STATIC TCHAR                OrigDrive;
STATIC TCHAR                szNetInocPath[SYM_MAX_PATH+1];
TCHAR                       SZ_INOC_FILE[] = "NAVINOC.DAT";
TCHAR                       SZ_INOCL_FILE[] = "NAVINOCL.DAT";


LPINOCINSTANCES LOCAL PASCAL GetAnotherInocStruct(VOID)
{
    INOCInstances FAR   *tempptr;
    HGLOBAL             hTemp;

    if ((hTemp = MemAlloc(GHND, sizeof(INOCInstances))) &&
        (tempptr = (INOCInstances FAR *)MemLock(hTemp))) 
        {
        ;
        }
    else 
        return(NULL);

    // Attach at beginning.  Effect: LIFO stack
    // (when taken in conjunction with InocClose)
    tempptr->_ii_next = lpInoculates;
    lpInoculates = tempptr;
    tempptr->_ii_flags = NULL;
    return(tempptr);
}

//************************************************************************
//
// Returns:
//      FALSE           Problem
//      TRUE            no problem
//************************************************************************

LPINOCULATE SYM_EXPORT WINAPI GetInocStructGivenFile(LPSTR lpszFile)
{
#ifdef _ALPHA_
	return NULL;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return NULL;

    auto    TCHAR   szShortFile[SYM_MAX_PATH+1];
    auto    TCHAR   szDriveName[SYM_MAX_PATH+1];
    auto    LPSTR   lpDriveName = (LPSTR) szDriveName;
    auto    BOOL    bLongFile = FALSE;

    SYM_ASSERT ( lpszFile );

    if (lpszFile)
        {
        NavStrUpr(lpszFile);

        if ( lpszFile[1] == ':' )
            {
            szDriveName[0] = lpszFile[0];
            szDriveName[1] = ':';
            szDriveName[2] = EOS;
            }
        else if ( IsUNCName (lpszFile) )
            UNC_GetServerVolumeName ( lpszFile, lpDriveName );
        else
            lpDriveName = NULL;

        NameReturnShortName(lpszFile, szShortFile);
        bLongFile = (BOOL) (STRLEN(szShortFile) > 144);
        }

    return (GetInocStruct(lpDriveName, bLongFile));
#endif
}


//************************************************************************
//
// Returns:
//      FALSE           Problem
//      TRUE            no problem
//************************************************************************

LPINOCULATE SYM_EXPORT WINAPI GetInocStruct(LPSTR lpDriveName, BOOL bLongFile)
{
#ifdef _ALPHA_
	return NULL;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return NULL;

    INOCInstances FAR   *tempptr;
    UINT                uLongFlag;
    static BOOL         bCreate;
    TCHAR               szBuffer[SYM_MAX_PATH+1];
    TCHAR               szTemp[SYM_MAX_PATH+1];


    uLongFlag = (bLongFile) ? II_LONG : 0;

    if ( lpDriveName != NULL )
        if ( IsUNCName ( lpDriveName ) )
            for (tempptr = lpInoculates; tempptr; tempptr = tempptr->_ii_next)
                {
                if ((tempptr->_ii_flags & (II_INUSE|II_ISOPEN))  &&
                    !STRICMP (tempptr->_ii_UNCName, lpDriveName) &&
                    (tempptr->_ii_flags & II_LONG) == uLongFlag)
                       return(&(tempptr->_ii_inoculate));
                }
        else
            for (tempptr = lpInoculates; tempptr; tempptr = tempptr->_ii_next)
                {
                if ((tempptr->_ii_flags & (II_INUSE|II_ISOPEN)) &&
                    tempptr->_ii_drive == lpDriveName[0]  &&
                    (tempptr->_ii_flags & II_LONG) == uLongFlag)
                        return(&(tempptr->_ii_inoculate));
                }

    // If there is an active drive open, it's not the one we want.
    // Find one that's open and close it so we can open the correct one.
    for (tempptr = lpInoculates; tempptr; tempptr = tempptr->_ii_next) {
        if (tempptr->_ii_flags & (II_INUSE|II_ISOPEN))
            {
            // Found structure.
            if (tempptr->_ii_flags & II_ISWRITE)
                bCreate = TRUE;
            else
                bCreate = FALSE;

            if (lpDriveName== NULL || lpDriveName[0] == 0 )
                return(&(tempptr->_ii_inoculate));

            InocClose();

            // Create new DB name to open.
            BuildInocDBName(szTemp, bLongFile);
            STRCPY ( szBuffer, lpDriveName );
            STRCAT ( szBuffer, szTemp+2 );

            if (InocOpen(szBuffer, bCreate) == FALSE)
                return(FALSE);

            // Because the databases do not contain drive letters, we
            // must set the default drive to same drive as DB.
            // else
            //    DiskSet(lpDriveName);
            break;
        }
    }

    // If there were no entries in use, none are opened yet
    if (NULL == tempptr) 
        return(FALSE);

    // Otherwise, the InocOpen created a valid entry.

    if ( lpDriveName != NULL )
        if ( IsUNCName ( lpDriveName ) )
            for (tempptr = lpInoculates; tempptr; tempptr = tempptr->_ii_next)
                {
                if ((tempptr->_ii_flags & (II_INUSE|II_ISOPEN))  &&
                    !STRICMP (tempptr->_ii_UNCName, lpDriveName) &&
                    (tempptr->_ii_flags & II_LONG) == uLongFlag)
                       return(&(tempptr->_ii_inoculate));
                }
        else
            for (tempptr = lpInoculates; tempptr; tempptr = tempptr->_ii_next)
                {
                if ((tempptr->_ii_flags & (II_INUSE|II_ISOPEN)) &&
                    tempptr->_ii_drive == lpDriveName[0]  &&
                    (tempptr->_ii_flags & II_LONG) == uLongFlag)
                        return(&(tempptr->_ii_inoculate));
                }


    return(FALSE);
#endif
}
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

LPINOCULATE SYM_EXPORT WINAPI GetUnusedInocStruct(LPSTR InocDB, BOOL bCreate, BOOL bLongFile)
{
#ifdef _ALPHA_
	return NULL;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return NULL;

    INOCInstances FAR   *tempptr;
    INOCInstances FAR   *temp2ptr;
    TCHAR    szTemp[SYM_MAX_PATH+1];

    SYM_ASSERT (InocDB);

    // Any time we're opening a new database, it may signal a change
    // in the database path name.  Wipe out the old with the new.
    for (tempptr = lpInoculates; tempptr; tempptr = tempptr->_ii_next)
    {
        if (!(tempptr->_ii_flags & (II_INUSE|II_ISOPEN)))
            break;
    }
    if (NULL == tempptr)
        if (NULL == (tempptr = GetAnotherInocStruct()))
            return(NULL);

    tempptr->_ii_flags = II_INUSE|II_ISOPEN;
    if (bCreate)
        tempptr->_ii_flags |= II_ISWRITE;
    if (bLongFile)
        tempptr->_ii_flags |= II_LONG;

    if (IsUNCName ( InocDB ) )
        {
        UNC_GetServerVolumeName ( InocDB, szTemp);
        STRCPY (tempptr->_ii_UNCName, szTemp );
        }
    else
        {
        tempptr->_ii_drive = InocDB[0];
        }

    // Place ourselves at head of the list, for LIFO purposes
    for (temp2ptr = lpInoculates; temp2ptr; temp2ptr = temp2ptr->_ii_next)
        {
        if (temp2ptr->_ii_next == tempptr)
            break;
        }
    if (temp2ptr)
        {
        temp2ptr->_ii_next = tempptr->_ii_next;
        tempptr->_ii_next = lpInoculates;
        lpInoculates = tempptr;
        }
    return(&(tempptr->_ii_inoculate));
#endif
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
VOID SYM_EXPORT WINAPI ReleaseInocStruct(INOCULATE FAR *Inoc)
{
#ifdef _ALPHA_
	return;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return;

    INOCInstances FAR   *tempptr;
    INOCInstances FAR   *prev_tempptr;
    auto    CALLBACKREV1    rCallbacks;

    InitCertlib(&rCallbacks, INIT_CERTLIB_USE_DEFAULT);

    // NOTE:  Make sure prev_tempptr stays up to date.  (don't use 'continue')

    prev_tempptr = NULL ;
    for (tempptr = lpInoculates ; tempptr; tempptr = tempptr->_ii_next) {
        if ((tempptr->_ii_flags & (II_INUSE|II_ISOPEN)) &&
            (&tempptr->_ii_inoculate == Inoc)) {

            tempptr->_ii_flags = NULL;

            // Equivalent to tasks in InocClose()

            if ( Inoc->hMem )
                {
                DBC4_CLOSE ( Inoc->lpHeader ) ;
                MemRelease ( Inoc->hMem ) ;
                MemFree ( Inoc->hMem ) ;
                }

            // hQuick isn't locked and might not be allocated

            if ( Inoc -> hQuick )
                MemFree ( Inoc->hQuick ) ;

            if (Inoc->hWorkMem) {
                MemRelease (Inoc->hWorkMem );
                MemFree(Inoc->hWorkMem);
            }
            if (Inoc->hWorkMem2) {
                MemRelease (Inoc->hWorkMem2 );
                MemFree(Inoc->hWorkMem2);
            }

            Inoc->lpHeader   = NULL ;
            Inoc->lpWorkMem  = NULL ;
            Inoc->lpWorkMem2 = NULL ;
            Inoc->hMem = Inoc->hQuick  = Inoc->hWorkMem  = Inoc->hWorkMem2  = 0 ;

            // Remove this link to avoid fragmenting memory
               
            if ( prev_tempptr == NULL )
                lpInoculates = tempptr->_ii_next ;
            else
                prev_tempptr->_ii_next = tempptr->_ii_next;

            MemFreePtr (tempptr);

            return;
        }

        prev_tempptr = tempptr ;
    }
    return;
#endif
}

//************************************************************************
// InocInit()
//
// This routine stores the original drive so that InocEnd can restore the
// current drive.  Also, sets up the inoculation database directory.
//
// Parameters:
//      LPSTR   Directory in which inoculation database is stored
//
// Returns:
//      nothing
//************************************************************************
VOID SYM_EXPORT WINAPI InocInit(LPNAVOPTIONS lpNavOptions)
{
#ifdef _ALPHA_
	return;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return;

    if (0 == OrigDrive)
        OrigDrive = DiskGet();
    szNetInocPath[0] = DiskGet();
    szNetInocPath[1] = ':';
    STRCPY(szNetInocPath+2, lpNavOptions->inoc.szNetInocPath);
    RememberInocFloppy(lpNavOptions->inoc.bInocFloppies);
    return;
#endif    
}

//************************************************************************
// InocEnd()
//
// This routine frees all the INOCInstances used by the INOCULAT dll.
//
// Parameters:
//      none
//
// Returns:
//      nothing
//************************************************************************
// 5/11/93 DALLEE, Modified:  Call MemFreePtr to release memory
//************************************************************************

VOID SYM_EXPORT WINAPI InocEnd(VOID)
{
#ifdef _ALPHA_
	return;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return;

    INOCInstances FAR   *tempptr;
    INOCInstances FAR   *nextptr;

    for (tempptr = lpInoculates; tempptr; tempptr = tempptr->_ii_next) {
        if ((tempptr->_ii_flags & (II_INUSE|II_ISOPEN))) {
            ReleaseInocStruct(&tempptr->_ii_inoculate);
            tempptr->_ii_flags = NULL;
        }
    }
    for (tempptr = lpInoculates; tempptr; tempptr = nextptr) {
        nextptr = tempptr->_ii_next;
                                        // Call MemFreePtr to free memory
                                        // given only a pointer to the
                                        // locked block.
        MemFreePtr (tempptr);
    }
    lpInoculates = NULL;
    if (OrigDrive) {
        DiskSet((TCHAR)OrigDrive);
        OrigDrive = 0;
    }
    return;
#endif    
} // End InocEnd()

//************************************************************************
// BuildInocDBName(buffer, bLongFile)
//
// Parameters:
//      LPSTR lpBuffer
//      BOOL  bLongFile
//
// Returns:
//      buffer with inoculation database.
//************************************************************************

VOID SYM_EXPORT WINAPI BuildInocDBName(LPSTR lpBuffer, BOOL bLongFile)
{
#ifdef _ALPHA_
	return;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return;

    STRCPY(lpBuffer, szNetInocPath);
    NameAppendFile(lpBuffer, (bLongFile) ? SZ_INOCL_FILE : SZ_INOC_FILE);
#endif    
}

