// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
// File:        QuarantineInfectedFileItem.cpp
//
// Description: This file contains the class CQuarantineInfectedFileItem.  It is 
//              derived from a CQuarItem base class, and implements
//              repair, restore, and save to quarantine funcitonality for
//              VBin Infected file objects.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : File created.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DWHWizrd.h"
#include "DWHWiz.h"
#include "QuarantineInfectedFileItem.h"
#include "wow64helpers.h"

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineInfectedFileItem::CQuarantineInfectedFileItem
//
// Description: Constructor for a CQuarantineInfectedFileItem object.  This class is
//              derived from a CQuarItem base class, and implements
//              repair, restore, and save to quarantine funcitonality for
//              VBin infected file objects.
//
// Parameters:  IVBin2 *    pVBin2      - [in] Virus bin interface pointer.
//              VBININFO *  pVBinInfo   - [in] VBININFO structure for this session.
//
// Returns:     Constructed CQuarantineInfectedFileItem object.
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
CQuarantineInfectedFileItem::CQuarantineInfectedFileItem( IVBin2 *pVBin2, VBININFO *pVBinInfo ) :
    CQuarItem( pVBin2, pVBinInfo )
{
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineInfectedFileItem::~CQuarantineInfectedFileItem
//
// Description: Destructor for a CQuarantineInfectedFileItem object.  This method
//              will release any information stored by this class.
//
// Parameters:  None.
//
// Returns:     None.
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
CQuarantineInfectedFileItem::~CQuarantineInfectedFileItem(void)
{
}


///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineInfectedFileItem::Repair
//
// Description: Repair method implemented from the base CQuarItem class.
//              This method will attempt a "Repair" on this infected file
//              object, and if successful, will set the item state of this
//              object to "Repairable".
//
// Parameters:  None.
//
// Returns:     DWH_OK - Always returns success.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
DWORD CQuarantineInfectedFileItem::Repair( void )
{
    auto    CDWHWizrdApp *  pDWHWizApp  = (CDWHWizrdApp*)AfxGetApp();
    auto    DWORD           dwState     = 0;
    auto    DWORD           dwError     = DWH_OK;
    auto    LPSTR           pTempFile   = NULL;
    auto    LPSTR           pNameString = NULL;

    pDWHWizApp->SetCurrentItem( this );

    // Is the file compressed? We don't repair 'em.
    if ( m_pVBinEventBlock && m_pVBinEventBlock->pdfi && !m_pVBinEventBlock->pdfi->bIsCompressed )
    {
        // Create a temp file. It gets deleted in the items destructor.
        if ( DWH_OK == CreateTempFile(&pTempFile) )
        {
            // Set the state flags.
            dwState = RV_STATE_INBIN|RV_STATE_COPY;

		    pDWHWizApp->DisableAP();

            // Extract from the virus bin to the temp file
            pDWHWizApp->m_pVirusAction2->GetCopyOfFile(NULL, m_stVBinInfo.LogLine, pTempFile, dwState);
            
            pNameString = m_pVBinEventBlock->Description;

            // And scan it ...
            pDWHWizApp->m_pScan->ScanOneFile(pTempFile, pNameString, FALSE);

		    pDWHWizApp->EnableAP();
        }
    }

    pDWHWizApp->SetCurrentItem( NULL );

    return dwError;
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineInfectedFileItem::Restore
//
// Description: Method to restore this infected file back to the local machine.
//              This is typically used after a successful repair has occurred.
//
// Parameters:  BOOL    bMoveToBackup - [in] Should we move this item to the
//                                           backup bin, if restore successful?
//
// Returns:     TRUE  - File restored to its original location successfully.
//              FALSE - Failed to restore infected file to original location.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
BOOL CQuarantineInfectedFileItem::Restore( BOOL bMoveToBackup )
{
    CDWHWizrdApp *  pDWHWizApp          = (CDWHWizrdApp*)AfxGetApp();
    PEVENTBLOCK     pEventBlk           = NULL;
    LPCTSTR         lpTempFile          = NULL;
    BOOL            bFileUpdated        = FALSE;
    BOOL            bSaveInQuarantine   = FALSE;
    PVBININFO       pVbinInfo           = NULL;
    TCHAR*          szPath              = NULL;
    BOOL            bRedirected         = FALSE;

    if ( GetState() == QUARITEM_STATE_REPAIRABLE )
    {
	    // Get the original event block for this item
        pEventBlk = GetVBinEvent();
	    
        // Get its temp file
        lpTempFile = GetDWHTemporaryFile();

        // Get the VBININFO for this item
        pVbinInfo = GetVBinInfo();

        // Restore the long File name if the file was originally an LFN.
        if ( (lstrcmp(pVbinInfo->FullPathAndLFN, "") != 0) &&
             (lstrcmp(pVbinInfo->Description, pVbinInfo->FullPathAndLFN) != 0) )
        {
            szPath = pVbinInfo->FullPathAndLFN;
        }
        else
        {
            szPath = pEventBlk->Description;
        }

        // If the file resides on a 64-bit redirected path, temporarily disable
        // file redirection.
        bRedirected = IsWow64RedirectedPath( szPath );
        if( !bRedirected || Wow64EnableFileRedirection( FALSE ) )
        {
            // Copy the temp file to the original location
            bFileUpdated = CopyFile(lpTempFile, szPath, TRUE);
        }
        else
        {
            // Failed to disable file redirection. We will
            // not be able to restore the file.
            bFileUpdated = FALSE;
            SetLastError( ERROR_ACCESS_DENIED );
        }

        if ( !bFileUpdated )
        {
            // Did it fail due to no path?
            if ( GetLastError() == ERROR_PATH_NOT_FOUND )
            {
                // Yep. Try to create it.
                CString szTempDir = szPath;

                szTempDir = szTempDir.Left( szTempDir.ReverseFind(TCHAR('\\')) );

                if ( !CreateMissingDir( (char *)LPCTSTR(szTempDir)) )
                {
                    // Couldn't create the directory. If we're running in
                    // silent mode, then save it as a repaired item in Quarantine.
                    if ( pDWHWizApp->GetDefWatchMode() == DWM_SILENT )
                    {
                        bSaveInQuarantine = TRUE;
                    }
                }
                else
                {
                    // Try again to replace the file in its original location.
                    bFileUpdated = ReplaceFile((LPTSTR)lpTempFile, szPath);
                }
            }
            else
            {
               if ( pDWHWizApp->GetDefWatchMode() == DWM_SILENT )
               {
                   bSaveInQuarantine = TRUE;
               }
            }
        }

        // Reenable file redirection -- it's ok to call this function redundantly.
        if( bRedirected )
        {
            Wow64EnableFileRedirection( TRUE );
        }

        if ( bFileUpdated )
        {
            SetState(QUARITEM_STATE_RESTORED_TO_ORIG_LOC);
        }
        else
        {
            if ( bSaveInQuarantine )
            {
                // TODO : KJS - Need to create a new quarantine session for these.
                //              We can have multiple items, so we need a session, and
                //              each item will have to know how to store itself into
                //              the new session that is created.

/*
                dwError = SaveRepairedFile( pQuarItem );

                if ( dwError == DWH_OK )
                {
                    SetState(QUARITEM_STATE_REPAIRED_FILE_IN_QUAR);
                    bFileUpdated = TRUE;
                }
*/
            }
            else
            {
                SetState(QUARITEM_STATE_NOT_RESTORED);
            }
        }
    }

    // If the file was successfully updated, then make the quarantined
    // file into a backup item.
    if ( bFileUpdated )
    {
        if ( bMoveToBackup )
            SetItemToBackup(); 
    }

    return bFileUpdated;

}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineInfectedFileItem::SaveToQuarantine
//
// Description: Save this infected (now repaired) file to a new quarantine session
//              with the new logging GUID that is passed in.  We do this in 
//              silent "Repair Only" mode.  If all items can be repaired for this
//              threat, all items are added to a new quarnatine record as a
//              "Repaired" item.
//
// Parameters:  IVBinSession *  pVBinSession        - [in] VBin session to add this infected file to.
//                                                         This may be NULL if this infected file came
//                                                         from the root of the quarantine folder. (legacy)
//              LPCTSTR         szNewLogSessionGUID - [in] New logging GUID to write out to the event block
//                                                         of this remediation object vbin record.  May
//                                                         be NULL if this is a stand-alone infection object,
//                                                         and not part of a session.
//
// Returns:     DWH_OK                  - Item copied to a new quarantine session successfully.
//              DWH_ERROR_IN_QUARANTINE - General quarantine failure.  Could not create / add items to new session.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
DWORD CQuarantineInfectedFileItem::SaveToQuarantine( IVBinSession *pVBinSession /*= NULL */, LPCTSTR szNewLogSessionGUID /* = NULL */  )
{
    PEVENTBLOCK pEventBlk = NULL;
    LPCTSTR     lpTempFile = NULL;
    DWORD       dwReturn = DWH_OK;
    VBININFO    vbi;
    DWORD       dwError = ERROR_SUCCESS;


    if ( GetState() == QUARITEM_STATE_REPAIRABLE )
    {
	    // Get the original event block for this item
        pEventBlk = GetVBinEvent();

	    // Get its temp file
        lpTempFile = GetDWHTemporaryFile();

        // Set up VBinInfo
        memset(&vbi,0,sizeof(VBININFO));
        vbi.Size = sizeof(VBININFO);
        vbi.dwStructureVersion = CURRENT_VBININFO_VERSION;
        vbi.dwRecordType = VBIN_RECORD_LEGACY_INFECTION;

        // Set the repaired items flag.
        vbi.Flags = VBIN_REPAIRED;

		// Indicate the action taken was repair
		pEventBlk->RealAction = AC_CLEAN;
		pEventBlk->WantedAction[0] = AC_CLEAN;
		pEventBlk->WantedAction[1] = AC_NOTHING;

        // Get the file name from the original event block.
        _tcscpy(vbi.Description, pEventBlk->Description);

        // Save the current time
        time(&vbi.ExtraInfo.stVBinTime);

        // Save the status info for the current file
        _stat(vbi.Description, &vbi.ExtraInfo.stFileStatus);

        // Create a GUID for this item (used by Scan & Deliver)
        UuidCreate(&vbi.ExtraInfo.stUniqueId);

        // Since we are creating a copy of all log records, we need
        // to stamp these with a new logging GUID so they can be correlated.
        if ( szNewLogSessionGUID && _tcslen( szNewLogSessionGUID ) )
            pEventBlk->LogSessionGUID = (char *)szNewLogSessionGUID;

        // Make sure that our quarantine session item index does not get
        // overwritten.  We want to preserve our quarantine order for restore purposes.
        vbi.dwSessionItemIndex = m_stVBinInfo.dwSessionItemIndex;

        // We need a log line
        m_pVBin2->CreateLogLine( vbi.LogLine, pEventBlk );


        /////////////////////////////////////////////////////////////////
        // Final Step : Add this item to quarantine, or to a passed-in
        //              quarantine session.

        if ( pVBinSession != NULL )
        {
            // We already have a quarantine session.  Save this item within
            // the session passed into this method.
            dwError = pVBinSession->AddItem( &vbi, (char *)lpTempFile, FALSE );
        }
        else
        {
            // This is a stand-alone object with no session.  Simply add it
            // to quarantine directly.
            dwError = m_pVBin2->AddItem( &vbi, (char *)lpTempFile );
        }

        if ( dwError != ERROR_SUCCESS )
        {
            dwReturn = DWH_ERROR_IN_QUARANTINE;
        }
    }

    return dwReturn;
}