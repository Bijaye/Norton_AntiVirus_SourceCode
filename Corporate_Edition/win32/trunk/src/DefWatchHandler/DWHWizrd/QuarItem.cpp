// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// QuarItem.cpp: implementation of the CQuarItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DWHWizrd.h"
#include "QuarItem.h"
#include "dwhwiz.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// *************************************************************************
//
// Name:        CQuarItem::CQuarItem()
//
// Description: Default constructor
//
// Parameters:  None
//
// Returns:     None
//
// *************************************************************************
// 4/8/99 - TCashin : Function added.
// *************************************************************************
CQuarItem::CQuarItem()
{
    m_pVBin2 = NULL;
    m_pNewEventBlock = NULL;
    m_dwState = 0;
}


// *************************************************************************
//
// Name:        CQuarItem::CQuarItem( IVBin2* pIVBin2 )
//
// Description: Constructor override to provide an interface to a IVBin2
//              COM object.
//
// Parameters:  IVBin2*     pIVBin2     [in] - Pointer to an IVBin2 interface.
//              VBININFO *  pVBinInfo   [in] - The VBinInfo structure for this object.
//
// Returns:     None
//
// *************************************************************************
// 4/8/99  - TCashin : Function added.
// 1/12/05 - Ksackin : Moved to IVBin2 interface.
// *************************************************************************
CQuarItem::CQuarItem(IVBin2* pIVBin2, VBININFO *pVBinInfo)
{
    m_pVBin2 = pIVBin2;
    m_pNewEventBlock = NULL;

    SetVBinInfo( pVBinInfo );
}


// *************************************************************************
//
// Name:        CQuarItem::~CQuarItem()
//
// Description: Destructor
//
// Parameters:  None
//  
// Returns:     None
//
// *************************************************************************
// 4/8/99  - TCashin : Function added.
// 1/12/05 - Ksackin : Moved to IVBin2 interface.
// *************************************************************************
CQuarItem::~CQuarItem()
{
    DeleteTempFile();

    if (m_pVBinEventBlock)
        m_pVBin2->DestroyEvent(m_pVBinEventBlock);

    if (m_pNewEventBlock)
        m_pVBin2->DestroyEvent(m_pNewEventBlock);
}


// *************************************************************************
//
// Name:        CQuarItem::SetVBinInfo()
//
// Description: Initializes this CQuarItems VBININFO structure.
//
// Parameters:  PVBININFO lpVBinInfo [in] - pointer to a VBININFO structure
//                                          to use for this object. 
//
// Returns:     TRUE if the VBININFO and its corresponding event block are
//              set up correctly. Otherwise FALSE.
//
// *************************************************************************
// 4/8/99 - TCashin : Function added.
// *************************************************************************
BOOL CQuarItem::SetVBinInfo(PVBININFO lpVBinInfo)
{
    memcpy(&m_stVBinInfo, lpVBinInfo, sizeof(VBININFO));

    if ( SetVBinEvent() )
        return TRUE;
    else
        return FALSE;
}


// *************************************************************************
//
// Name:        CQuarItem::GetVBinInfo()
//
// Description: Returns a pointer to this CQuarItems VBININFO structure.
//
// Parameters:  None
//
// Returns:     PVBININFO - Pointer to a VBININFO structure.
//
// *************************************************************************
// 4/8/99 - TCashin : Function added.
// *************************************************************************
PVBININFO CQuarItem::GetVBinInfo()
{   
    return &m_stVBinInfo;
}


// *************************************************************************
//
// Name:        CQuarItem::SetVBinEvent()
//
// Description: Create an event block based on the contents of the VBININFO
//              structure.
//
// Parameters:  None.
//
// Returns:     TRUE is the event block was created, otherwise FALSE.
//
// *************************************************************************
// 4/8/99 - TCashin : Function added.
// *************************************************************************
BOOL CQuarItem::SetVBinEvent()
{
    BOOL    bRet = FALSE;
    DWORD   dwError = ERROR_SUCCESS;

    if ( m_pVBin2 )
    {
        // Create the event block for this item. It has all 
        // the interesting data. It get released in the destructor.
        dwError = m_pVBin2->CreateEvent(&m_pVBinEventBlock, m_stVBinInfo.LogLine);

        if (dwError == ERROR_SUCCESS)
            bRet = TRUE;
    }

    return bRet;
}


// *************************************************************************
//
// Name:        CQuarItem::GetVBinEvent()
//
// Description: Returns a pointer to the event block created from the Virus
//              Bin data. This is the original event block.
//
// Parameters:  None
//
// Returns:     PEVENTBLOCK - Pointer to an event block. 
//
// *************************************************************************
// 4/8/99 - TCashin : Function added.
// *************************************************************************
PEVENTBLOCK CQuarItem::GetVBinEvent()
{
    return m_pVBinEventBlock;
}


// *************************************************************************
//
// Name:        CQuarItem::CreateTempFile()
//
// Description: Creates a temporary file for this CQuarItem. The extension
//              for the file we create matches the original extension of the
//              Quarantined file.
//
// Parameters:  LPSTR* lpTempFile [out] - Pointer to a LPSTR that get the
//                                        that gets the file name.
//
// Returns:     DWH_OK:             Temp file was created.
//              DWH_NO_TEMP_FILE:   Error creating temp file
//
// *************************************************************************
// 4/8/99 - TCashin : Function added.
//**************************************************************************
DWORD CQuarItem::CreateTempFile(LPSTR* lpTempFile)
{
    auto    char    szTempPath[MAX_PATH] = {0};
    auto    char    szTempFileName[MAX_PATH] = {0};
    auto    LPSTR   lpOrigExt = NULL;
    auto    DWORD   dwTicks = 0;
    auto    UINT    uError = 0;
    auto    DWORD   dwReturn = DWH_OK;

    // Get the temp path
    GetTempPath(MAX_PATH, szTempPath);

    // Get the  system time
    dwTicks = GetTickCount();

    // Create a temp file for the quarantine server package
    uError = GetTempFileName(szTempPath, "DWH", dwTicks, szTempFileName);

    if ( uError != 0 )
    {
        // Update the data member
        m_szTempFile = szTempFileName;

        // Make the temp file name have the same extension as
        // the original file.
        lpOrigExt = strrchr(m_stVBinInfo.Description, '.');

        m_szTempFile.Replace( _T(".TMP"), lpOrigExt );

        // update the caller's pointer.
        *lpTempFile = m_szTempFile.GetBuffer(MAX_PATH);
    }
    else
        dwReturn = DWH_NO_TEMP_FILE;

    return dwReturn;
}


// *************************************************************************
//
// Name:        CQuarItem::DeleteTempFile()
//
// Description: Deletes the temp file used by the CQuarItem
//
// Parameters:  None
//
// Returns:     TRUE is the file was deleted, otherwaise FALSE.
//
// *************************************************************************
// 4/8/99 - TCashin : Function added.
//**************************************************************************
BOOL CQuarItem::DeleteTempFile()
{
    DWORD   dwFileAttributes = 0;

    // Make sure the read-only attribute is not set
    dwFileAttributes = GetFileAttributes( LPCTSTR(m_szTempFile) );
    SetFileAttributes( LPCTSTR(m_szTempFile), 
                       dwFileAttributes & ~FILE_ATTRIBUTE_READONLY );
    
    // Delete the file
    return DeleteFile( LPCTSTR(m_szTempFile) );
}


// *************************************************************************
//
// Name:        CQuarItem::SetNewEvent()
//
// Description: Set the pointer to the new event block for this CQuarItem.
//
// Parameters:  PEVENTBLOCK lpEventBlk: Pointer to an event block
//
// Returns:     Currently returns TRUE in all cases
//
// *************************************************************************
// 4/8/99 - TCashin : Function added.
// *************************************************************************
BOOL CQuarItem::SetNewEvent(PEVENTBLOCK lpEventBlk)
{
    m_pNewEventBlock = lpEventBlk;

    return TRUE;
}


// *************************************************************************
//
// Name:        CQuarItem::GetNewEvent()
//
// Description: Return a pointer to the new event block for this CQuarItem.
//
// Parameters:  None
//
// Returns:     PEVENTBLOCK - Pointer to an event block.
//
// *************************************************************************
// 4/8/99 - TCashin : Function added.
// *************************************************************************
PEVENTBLOCK CQuarItem::GetNewEvent()
{
    return m_pNewEventBlock;
}


// *************************************************************************
//
// Name:        CQuarItem::GetDWHTemporaryFile()
//
// Description: Returns a pointer to the temp file for this CQuarItem
//
// Parameters:  None
//
// Returns:     LPCTSTR - Pointer to the file name
//
// *************************************************************************
// 4/8/99 - TCashin : Function added.
// *************************************************************************
LPCTSTR CQuarItem::GetDWHTemporaryFile()
{
    return LPCTSTR(m_szTempFile);
}

void CQuarItem::SetState(DWORD dwState)
{
    m_dwState = dwState;
}

DWORD CQuarItem::GetState()
{
    return m_dwState;
}



// *************************************************************************
//
// Name:		CQuarItem::SetItemToBackup()				
//
// Description: Sets this quarantine item to a "Backup" item.
//
// Parameters:	None.
//
// Returns:		DWH_OK on success.
//
// *************************************************************************
// 7/20/99   - TCashin : Function added
// 1/12/2005 - KSackin : Moved this function to the QuarItem base class.
//                       This will be used by all derived classes of this QuarItem.
// *************************************************************************
DWORD CQuarItem::SetItemToBackup( void )
{
    DWORD       dwError = 0;
    DWORD       dwRetValue = DWH_OK;
    PEVENTBLOCK pTempEventBlock = NULL;
    VBININFO    vbi = {0};
    
    // Make a temp copy of the original event block
    m_pVBin2->CopyEvent( GetVBinEvent(), &pTempEventBlock ) ;

    // Check for a valid event block pointer.
    if ( !pTempEventBlock )
        goto Cleanup;

    // Is there a backup ID in the event block?
    if ( pTempEventBlock->dwBackupID )
    {
        // If so, see if there is a backup file.
        dwError = m_pVBin2->GetInfo( pTempEventBlock->dwBackupID, &vbi );

        if ( dwError == ERROR_SUCCESS )
        {
            // There is a backup. We must delete the current quarantine item.
            dwError = m_pVBin2->Delete( pTempEventBlock->VBinID );

            // That's it.
            goto Cleanup;
        }
    }

    // Get the quarantine info for this guy.
    dwError = m_pVBin2->GetInfo( pTempEventBlock->VBinID, &vbi );

    // If we don't get the vbin info, we're sunk.
    if ( dwError != ERROR_SUCCESS )
    {
        dwRetValue = DWH_ERROR_IN_QUARANTINE;
        goto Cleanup;
    }

    // Set the backup flag
    vbi.Flags = VBIN_BACKUP;

    // Set the real action to backup
    pTempEventBlock->RealAction = AC_BACKUP;

    // Create our own log line because the event block is different
    dwError = m_pVBin2->CreateLogLine( vbi.LogLine, pTempEventBlock );

    // If we can't create the log line, we're toast.
    if ( dwError != ERROR_SUCCESS )
    {
        dwRetValue = DWH_ERROR_IN_QUARANTINE;
        goto Cleanup;
    }

    // And update the Quarantine file.
    dwError = m_pVBin2->SetInfo( pTempEventBlock->VBinID, &vbi );

    // If we don't get the vbin info, we're dead.
    if ( dwError != ERROR_SUCCESS )
    {
        dwRetValue = DWH_ERROR_IN_QUARANTINE;
    }

Cleanup:
    
    // Get rid of the event block
    if ( pTempEventBlock )
        m_pVBin2->DestroyEvent(pTempEventBlock);


    return dwRetValue;
}

// *************************************************************************
//
// Name:	    CQuarItem::CreateMissingDir
//
// Description: Method to create a missing directory.  This is used when
//              trying to restore this item from quarantine when the
//              original path may not exist any longer.
//
// Parameters:  LPTSTR lpFullPath - [in] Full path to missing directory.
//
// Returns:     TRUE  - Directory created successfully.
//              FALSE - Failed to create directory.
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
BOOL CQuarItem::CreateMissingDir(LPTSTR lpFullPath)
{
    int     iDelim = 0;
    CString sFullPath = lpFullPath;
    CString sDir;
    BOOL    bRet = FALSE;
    DWORD   dwError;


    // Strip the file name
    iDelim = sFullPath.ReverseFind( TCHAR('\\') );

    if ( iDelim != -1 )
    {
        sDir = sFullPath.Left( iDelim );

        bRet = ::CreateDirectory( lpFullPath, NULL );

        if ( !bRet )
        {
            dwError = GetLastError();

            if ( dwError == ERROR_PATH_NOT_FOUND )
            {
                if ( CreateMissingDir((char *)LPCTSTR(sDir)) )
                {
                    bRet = ::CreateDirectory( lpFullPath, NULL );
                }
            }
        }
    }

    return bRet;
}

// *************************************************************************
//
// Name:		CQuarItem::ReplaceFile()		
//
// Description: In prompt mode, put up a message box saying we're going to
//              overwrite the original file, then put up a SaveAs dialog if 
//              appropriate. Then delete the original file.
//              
//              Then in all case, copy the temp file to the original.
//
// Parameters:	LPTSTR lpTempFile - pointer to te temp file name
//				LPTSTR lpNewFile - pointer to the original file name
//
// Returns:		The return value of CopyFile()
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
BOOL CQuarItem::ReplaceFile(LPTSTR lpTempFile, LPTSTR lpNewFile)
{
    CDWHWizrdApp *  pDWHWizApp          = (CDWHWizrdApp*)AfxGetApp();
    DWORD           dwAttr = 0;
    CString         sOriginalFileName = lpNewFile;
    CString         sFileName;
    CString         sFilePath;

    if ( pDWHWizApp->GetDefWatchMode() == DWM_PROMPT )
    {
        dwAttr = GetFileAttributes( sOriginalFileName );
        while( dwAttr != 0xFFFFFFFF )
        {
            // Prompt the user for overwrite
            CString sTitle, sText;
            sTitle.LoadString( IDS_RESTORE_OVERWRITE_TITLE );
            sText.Format( IDS_RESTORE_OVERWRITE_FORMAT, sOriginalFileName );

            if( ::MessageBox( NULL, sText, sTitle, MB_ICONWARNING | MB_YESNO ) != IDYES )
            {
                // CString sFilter((LPCTSTR) IDS_ADD_FILTER_STRING);
                CString sFilter(_T("*.*"));
                CFileDialog dlg( FALSE,                 // Save
                                 NULL,                  // No default extension
                                 sOriginalFileName,     // Initial file name
                                 OFN_FILEMUSTEXIST |    // Flags
                                 OFN_HIDEREADONLY |
                                 OFN_PATHMUSTEXIST,
                                 sFilter,               // Filter string
                                 NULL );                // Parent window

                // Fire off dialog
                if( dlg.DoModal() == IDCANCEL )
                    {
                    return FALSE;
                    }

                // Save off file name and path
                sFileName = dlg.GetFileName();
                sOriginalFileName = dlg.GetPathName();
                sFilePath.ReleaseBuffer();
            }
            else
            {
                // User wants to overwrite this file, so delete it first.
                ::DeleteFile( sOriginalFileName );
                break;
            }

            // Get file attributes of this file
            dwAttr = GetFileAttributes( sOriginalFileName );
        }
    }
    return CopyFile(lpTempFile, sOriginalFileName, TRUE);
}

