// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
// File:        QuarantineRemediationItem.cpp
//
// Description: This file contains the class CQuarantineRemediationItem.  It is 
//              derived from a CQuarItem base class, and implements
//              repair, restore, and save to quarantine funcitonality for
//              ccEraser remediation objects.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : File created.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DWHWizrd.h"
#include "DWHWiz.h"
#include "QuarantineRemediationItem.h"


///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineRemediationItem::CQuarantineRemediationItem
//
// Description: Constructor for a CQuarantineRemediationItem object.  This class is
//              derived from a CQuarItem base class, and implements
//              repair, restore, and save to quarantine funcitonality for
//              ccEraser Remediation objects.
//
// Parameters:  IVBin2 *    pVBin2      - [in] Virus bin interface pointer.
//              VBININFO *  pVBinInfo   - [in] VBININFO structure for this remediation.
//
// Returns:     Constructed CQuarantineRemediationItem object.
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
CQuarantineRemediationItem::CQuarantineRemediationItem( IVBin2 *pVBin2, VBININFO *pVBinInfo ) :
    CQuarItem(pVBin2, pVBinInfo)
{
}


///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineRemediationItem::~CQuarantineRemediationItem
//
// Description: Destructor for a CQuarantineRemediationItem object.  This method
//              will release any information stored by this class.
//
// Parameters:  None.
//
// Returns:     None.
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
CQuarantineRemediationItem::~CQuarantineRemediationItem(void)
{
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineRemediationItem::Repair
//
// Description: Repair method implemented from the base CQuarItem class.
//              For remediation objects, there is no "Repair" since these items
//              are, by definition, not infected.  This method simply returns
//              success, and sets this item's state to repairable.
//
// Parameters:  None.
//
// Returns:     DWH_OK - Always returns success.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
DWORD CQuarantineRemediationItem::Repair(void)
{
    // Remediation objects cannot be "Repaired", but we are
    // going to set our flags saying that we are "Repaired"
    // so that we can be restored.

    SetState( QUARITEM_STATE_REPAIRABLE );

    return DWH_OK;
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineRemediationItem::Restore
//
// Description: Method to restore this remediation object back to the 
//              local machine.
//
// Parameters:  BOOL    bMoveToBackup - [in] Should we move this item to the
//                                           backup bin, if restore successful?
//
// Returns:     TRUE  - Object restored successfully.
//              FALSE - Failed to restore remediation object.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
BOOL CQuarantineRemediationItem::Restore( BOOL bMoveToBackup )
{
    IVBinSession *  pVBinSession        = NULL;
    VBININFO *      pvbiRemediationInfo = GetVBinInfo();
    BOOL            bReturn             = FALSE;

    // Verify parameters first.
    if ( !m_pVBin2 || !pvbiRemediationInfo )
        return FALSE;

    // Open the session that this object resides in.
    if ( ERROR_SUCCESS == m_pVBin2->OpenSession( pvbiRemediationInfo->dwSessionID, &pVBinSession ) )
    {
        if ( pVBinSession )
        {
            // Extract this remediation object from quarantine, and put 
            // it back in its original location.
            if ( ERROR_SUCCESS == pVBinSession->ExtractItem( pvbiRemediationInfo->RecordID, NULL, FALSE ) )
                bReturn = TRUE;
        }

        pVBinSession->Release();
    }

    return bReturn;
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineRemediationItem::SaveToQuarantine
//
// Description: Save this remediation item to the specified quarantine session
//              with the new logging GUID that is passed in.
//
// Parameters:  IVBinSession *  pVBinSession        - [in] VBin session to add this quarantine item to.
//              LPCTSTR         szNewLogSessionGUID - [in] New logging GUID to write out to the event block
//                                                         of this remediation object vbin record.
//
// Returns:     DWH_OK                  - Item copied to a new quarantine session successfully.
//              DWH_ERROR_IN_QUARANTINE - General quarantine failure.  Could not create / add items to new session.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
DWORD CQuarantineRemediationItem::SaveToQuarantine( IVBinSession *pVBinSession /*= NULL */, LPCTSTR szNewLogSessionGUID /* = NULL */  )
{
    PEVENTBLOCK pEventBlk = NULL;
    LPCTSTR     lpTempFile = NULL;
    DWORD       dwReturn = DWH_OK;
    VBININFO    vbi;
    DWORD       dwError = ERROR_SUCCESS;

    // First, extract this remediation object to a temp file.
    // this is only done here for remediations, since they never
    // had to be repaired during an earlier step.
    dwReturn = ExtractToTempFile();

    if ( DWH_OK == dwReturn )
    {
	    // Get the original event block for this item
        pEventBlk = GetVBinEvent();

	    // Get its temp file
        lpTempFile = GetDWHTemporaryFile();

        // Set up VBinInfo
        memset(&vbi,0,sizeof(VBININFO));
        vbi.Size = sizeof(VBININFO);
        vbi.dwStructureVersion = CURRENT_VBININFO_VERSION;
        vbi.dwRecordType = VBIN_RECORD_REMEDIATION;

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

        // We need a log line
        m_pVBin2->CreateLogLine( vbi.LogLine, pEventBlk );

        // Make sure that our quarantine session item index does not get
        // overwritten.  We want to preserve our quarantine order for restore purposes.
        vbi.dwSessionItemIndex = m_stVBinInfo.dwSessionItemIndex;

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

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineRemediationItem::ExtractToTempFile
//
// Description: Extract this remediation object from it's quarantine
//              record to a temp file.  This will be done in order to transfer
//              this remediation from one session to another.  We first extract
//              it to a temp file, then add it to the newly designated session.
//
// Parameters:  None.
//
// Returns:     DWH_OK                  - Item extracted to generated temp file name.
//              DWH_ERROR_IN_QUARANTINE - Failed to extract object to temp file.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
DWORD CQuarantineRemediationItem::ExtractToTempFile()
{
    auto    CDWHWizrdApp *  pDWHWizApp      = (CDWHWizrdApp*)AfxGetApp();
    auto    DWORD           dwError         = DWH_ERROR_IN_QUARANTINE;
    auto    LPSTR           pTempFile       = NULL;
    auto    LPSTR           pNameString     = NULL;
    auto    IVBinSession *  pVBinSession    = NULL;

    // Create a temp file. It gets deleted in the items destructor.
    if ( DWH_OK == CreateTempFile(&pTempFile) )
    {
		pDWHWizApp->DisableAP();

        if ( ERROR_SUCCESS == m_pVBin2->OpenSession( m_stVBinInfo.dwSessionID, &pVBinSession  ) )
        {
            // Extract this item in it's raw state.
            if ( ERROR_SUCCESS == pVBinSession->ExtractItem_Raw( m_stVBinInfo.RecordID, pTempFile, FALSE ) )
            {
                // Everything was successful.  Set our state to repaired,
                // and return back a success.
                dwError = DWH_OK;
            }

            pVBinSession->Release();
        }

		pDWHWizApp->EnableAP();
    }

    return dwError;
}