// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
// File:        QuarantineSessionItem.cpp
//
// Description: This file contains the class CQuarantineSessionItem.  It is 
//              derived from a CQuarItem base class, and implements
//              repair, restore, and save to quarantine funcitonality for
//              VBin session objects.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : File created.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DWHWizrd.h"
#include "DWHWiz.h"
#include "QuarantineSessionItem.h"
#include "SortedVBinEnumerator.h"


///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineSessionItem::CQuarantineSessionItem
//
// Description: Constructor for a CQuarantineSessionItem object.  This class is
//              derived from a CQuarItem base class, and implements
//              repair, restore, and save to quarantine funcitonality for
//              VBin session objects.
//
// Parameters:  IVBin2 *    pVBin2      - [in] Virus bin interface pointer.
//              VBININFO *  pVBinInfo   - [in] VBININFO structure for this session.
//
// Returns:     Constructed CQuaratineSessionItem object.
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
CQuarantineSessionItem::CQuarantineSessionItem( IVBin2 *pVBin2, VBININFO *pVBinInfo ) :
    CQuarItem(pVBin2, pVBinInfo)
{
    CSortedVBinEnumerator   cSortedVBinEnumerator;
    DWORD                   dwItemCount     = 0;
    CDWHWizrdApp *          pDWHWizApp      = (CDWHWizrdApp*)AfxGetApp();
    IVBinSession *          pVBinSession    = NULL;
    HANDLE                  hVBinFind       = NULL;
    VBININFO                vbiChildInfo    = {0};
    CQuarItem *             pChildItem      = NULL;

    // Fill out our infected file array, and our potential 
    // remediation restoration array.
    if ( pVBin2 != NULL )
    {
        if ( ERROR_SUCCESS == pVBin2->OpenSession( pVBinInfo->dwSessionID, &pVBinSession ) )
        {
            hVBinFind = pVBinSession->FindFirstItem( &vbiChildInfo );

            if ( hVBinFind )
            {
                do
                {
                    // Don't try and create wrapper objects for system snapshot
                    // quarantine records.
                    if ( vbiChildInfo.dwRecordType != VBIN_RECORD_SYSTEM_SNAPSHOT )
                    {
                        // Add this VBININFO item to our sorted enumerator.
                        cSortedVBinEnumerator.AddItem( vbiChildInfo );
                    }
                }
                while ( pVBinSession->FindNextItem( hVBinFind, &vbiChildInfo ) );

                // Close our find session.
                pVBinSession->FindClose( hVBinFind );

                // Sort these VBinItems by index;
                cSortedVBinEnumerator.SortByIndex();

                // Get the count of items in our list.
                cSortedVBinEnumerator.GetCount( dwItemCount );

                // Loop through all items from last to first, restoring each item.
                for ( int nLoop = 0; nLoop < dwItemCount; nLoop++)
                {
                    // Get the element at our loop's position.
                    cSortedVBinEnumerator.GetAt( nLoop, vbiChildInfo );

                    // Use our factory method to create a specific quarantine item
                    // for this child.
                    pChildItem = pDWHWizApp->CreateSpecificQuarantineItem( &vbiChildInfo );

                    switch ( vbiChildInfo.dwRecordType )
                    {
                        case VBIN_RECORD_LEGACY_INFECTION:
                            m_arInfectedQuarItems.Add( pChildItem );
                            break;

                        case VBIN_RECORD_REMEDIATION:
                            m_arRemediationQuarItems.Add( pChildItem );
                            break;

                        default:
                            // Not an infected file, or a remediation.
                            // We don't have to handle these for sessions.
                            // This could be a snapshot, or something we don't
                            // know about yet.
                            break;
                    };
                }
            }

            pVBinSession->Release();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineSessionItem::~CQuarantineSessionItem
//
// Description: Destructor for a CQuarantineSessionItem object.  This method
//              will release any information stored by this class.
//
// Parameters:  None.
//
// Returns:     None.
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
CQuarantineSessionItem::~CQuarantineSessionItem(void)
{
	// Cleanup our array of infected and remediation
	// quarantine items.
	CleanupInfectedItemArray();
	CleanupRemediationItemArray();
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineSessionItem::CleanupInfectedItemArray
//
// Description: Method used by the descructor of this class to cleanup and
//              free all objects stored in our infected item array.
//
// Parameters:  None.
//
// Returns:     None.
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
void CQuarantineSessionItem::CleanupInfectedItemArray()
{
    CQuarItem*  pItem       = NULL;
    int         iArraySize  = 0;
    int         i           = 0;

    iArraySize = m_arInfectedQuarItems.GetSize();

    for (i=0;i<iArraySize;i++)
    {
        pItem = (CQuarItem*)m_arInfectedQuarItems.GetAt(i);
        delete pItem;
    }

    m_arInfectedQuarItems.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineSessionItem::CleanupRemediationItemArray
//
// Description: Method used by the descructor of this class to cleanup and
//              free all objects stored in our remediation item array.
//
// Parameters:  None.
//
// Returns:     None.
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
void CQuarantineSessionItem::CleanupRemediationItemArray()
{
    CQuarItem*  pItem       = NULL;
    int         iArraySize  = 0;
    int         i           = 0;

    iArraySize = m_arRemediationQuarItems.GetSize();

    for (i=0;i<iArraySize;i++)
    {
        pItem = (CQuarItem*)m_arRemediationQuarItems.GetAt(i);
        delete pItem;
    }

    m_arRemediationQuarItems.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineSessionItem::Repair
//
// Description: Method for attempting repair all objects within this quarantine
//              session object.  The status of this repair will be updated as
//              "Repairable" only if all items within the session can be 
//              repaired.  We will only attempt to repair infected file objects,
//              as, by definition, remediation actions are not infected.
//
// Parameters:  None.
//
// Returns:     DWH_OK                      - All items repaired successfully.
//              DWH_ERROR_REPAIR_IN_QUAR    - Failed to repair one or more items.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
DWORD CQuarantineSessionItem::Repair(void)
{
    DWORD dwReturn;

    // Only attempt to repair the infected files from this 
    // quarantine session.

    dwReturn = RepairAllInfectedFiles();

    return dwReturn;
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineSessionItem::Restore
//
// Description: Method to restore all items from this quarantined object
//              gack to the machine.  This method is typically called after
//              a successfull repair has been performed.
//
// Parameters:  BOOL    bMoveToBackup - [in] Should we move this item to the
//                                           backup bin, if restore successful?
//
// Returns:     TRUE  - Session restored successfully.
//              FALSE - One or more objects failed to restore properly.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
BOOL CQuarantineSessionItem::Restore( BOOL bMoveToBackup )
{
    // Restore all items from session, starting with remediations in 
    // reverse order, then all infected objects.
    if ( RestoreAllRemediations() && RestoreAllInfectedFiles()  )
    {
        // Successfully restored all infected objects and
        // their remediations.  Set this item to a backup now.
        SetItemToBackup();

        return TRUE;
    }

    // Failed to restore all objects.
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineSessionItem::RepairAllInfectedFiles
//
// Description: Repair all infected files within this quarantine session.
//              This method will process all infected file objects, and 
//              attempt to repair them.
//
// Parameters:  None.
//
// Returns:     DWH_OK                   - All items repaired successfully.
//              DWH_ERROR_REPAIR_IN_QUAR - One or more items could not be repaired.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
DWORD CQuarantineSessionItem::RepairAllInfectedFiles( void )
{
    CQuarItem * lpItem              = NULL;
    DWORD       dwError             = DWH_OK;
    int         iNumItems           = m_arInfectedQuarItems.GetSize();
    BOOL        bRepairedAllFiles   = TRUE;

    // Only attempt to repair the infected files from this 
    // quarantine session.

    for ( int i=0;i<iNumItems;i++)
    {
        lpItem = (CQuarItem*)m_arInfectedQuarItems.GetAt(i);

        if ( lpItem )
        {
            lpItem->Repair();

            // If we can't successfully repair any item,
            // fail out of our processing.
            if ( lpItem->GetState() != QUARITEM_STATE_REPAIRABLE )
            {
                bRepairedAllFiles = FALSE;
                break;
            }
        }
    }

    // If all items were rapaired successfully, update our session
    // object with a state saying that the session is repairable.
    if ( bRepairedAllFiles )
        SetState( QUARITEM_STATE_REPAIRABLE );
    else
        dwError = DWH_ERROR_REPAIR_IN_QUAR;

    return dwError;
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineSessionItem::RestoreAllInfectedFiles
//
// Description: Restore all infected file objects back to the machine.
//              This method will process all infected file objects, and 
//              attempt to restore them to their original location.
//
// Parameters:  None.
//
// Returns:     TRUE  - All items restored successfully.
//              FALSE - One or more infected file objects failed to be restored.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
BOOL CQuarantineSessionItem::RestoreAllInfectedFiles(void)
{
    CQuarItem * lpItem      = NULL;
    BOOL        bSuccess    = TRUE;
    int         iNumItems   = m_arInfectedQuarItems.GetSize();

    // Restore all items in reverse order. (ccEraser rules)
    for ( int i = iNumItems-1; i >= 0; i-- )
    {
        lpItem = (CQuarItem*)m_arInfectedQuarItems.GetAt(i);

        if ( lpItem )
        {
            // Restore all infected files, but don't move them to a
            // "Backup" state, since they are not main records, but
            // a single item from a quarantine session.  If all items
            // succeed, the entire session will move to a "Backup" item.
            if ( !lpItem->Restore( FALSE ) )
                bSuccess = FALSE;
        }
    }

    return bSuccess;
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineSessionItem::RestoreAllRemediations
//
// Description: Restore all remediation objects in this quarantine session back 
//              to the machine.  This method will process all remediation  
//              objects, and  attempt to restore them to their original location.
//              This method should be called after restoring the infected files,
//              as many remediations are dependent on the referenced files being 
//              restored back to the machine before their loadpoint reference
//              can be restored.
//
// Parameters:  None.
//
// Returns:     TRUE  - All items restored successfully.
//              FALSE - One or more remediation objects failed to be restored.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
BOOL CQuarantineSessionItem::RestoreAllRemediations(void)
{
    CQuarItem * lpItem      = NULL;
    BOOL        bSuccess    = TRUE;
    int         iNumItems   = m_arRemediationQuarItems.GetSize();

    // Restore all items in reverse order. (ccEraser rules)
    for ( int i = iNumItems-1; i >= 0; i-- )
    {
        lpItem = (CQuarItem*)m_arRemediationQuarItems.GetAt(i);

        if ( lpItem )
        {
            // Attempt to restore this item, but if successful, don't
            // move this item's quarantine state to "Backup", since that is
            // done at the session level.
            if ( !lpItem->Restore( FALSE ) )
                bSuccess = FALSE;
        }
    }

    return bSuccess;
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineSessionItem::SaveToQuarantine
//
// Description: Save all objects within this quarantine session to a new
//              quarantine session.  This method is used after a successful
//              repair has happened in silent mode when we are set to 
//              "Repair Only", and leave repaired items in quarantine.
//              This function will create a new session, and stamp all items
//              going into this new session with a new logging GUID so that
//              these items will not get confused with other backup items.
//
// Parameters:  IVBinSession *  pVBinSession        - [in] VBin session to add this quarantine item to.
//                                                         this is provided fromthe base class, but does
//                                                         not apply to session objects, since they are
//                                                         not contained in a session.  Default value is NULL.
//              LPCTSTR         szNewLogSessionGUID - [in] Base class method.  Not used for this object,
//                                                         since we will create our own new GUID that all
//                                                         contained Infection and remediation objects will
//                                                         apply.
//
// Returns:     DWH_OK                  - Item copied to a new quarantine session successfully.
//              DWH_ERROR_IN_QUARANTINE - General quarantine failure.  Could not create / add items to new session.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
DWORD CQuarantineSessionItem::SaveToQuarantine( IVBinSession *pVBinSession /*= NULL */, LPCTSTR szNewLogSessionGUID /* = NULL */ )
{
    PEVENTBLOCK pEventBlk       = NULL;
    PEVENTBLOCK pNewEventBlk    = NULL;
    LPCTSTR     lpTempFile = NULL;
    DWORD       dwReturn = DWH_OK;
    VBININFO    vbiSessionInfo;
    TCHAR       szLogSessionGUID[40];


    if ( GetState() == QUARITEM_STATE_REPAIRABLE )
    {
        pNewEventBlk = GetNewEvent();

        // Have we created a new event block yet?
        if ( NULL == pNewEventBlk )
        {
	        // Get the original event block for this item
            pEventBlk = GetVBinEvent();

            // Make a copy of this event so we can modify it's values.
            if ( ERROR_SUCCESS == m_pVBin2->CopyEvent( pEventBlk, &pNewEventBlk ) )
            {
                // Save this as our new event block.
                SetNewEvent( pNewEventBlk );
            }
            else
            {
                // We're pretty much sunk here.  Return an error.
                dwReturn = DWH_ERROR_IN_QUARANTINE;
            }
        }

        if ( pNewEventBlk != NULL && pEventBlk != NULL)
        {
	        // Get its temp file
            lpTempFile = GetDWHTemporaryFile();

            // Set up VBinInfo
            memset(&vbiSessionInfo,0,sizeof(VBININFO));
            vbiSessionInfo.Size = sizeof(VBININFO);
            vbiSessionInfo.dwStructureVersion = CURRENT_VBININFO_VERSION;
            vbiSessionInfo.dwRecordType = VBIN_RECORD_SESSION_OBJECT;

            // Set the repaired items flag.
            vbiSessionInfo.Flags = VBIN_REPAIRED;

		    // Indicate the action taken was repair
		    pNewEventBlk->RealAction = AC_CLEAN;
			pEventBlk->WantedAction[0] = AC_CLEAN;
			pEventBlk->WantedAction[1] = AC_NOTHING;

            // Get the file name from the original event block.
            _tcscpy(vbiSessionInfo.Description, pNewEventBlk->Description);

            // Save the current time
            time(&vbiSessionInfo.ExtraInfo.stVBinTime);

            // Save the status info for the current file
            _stat(vbiSessionInfo.Description, &vbiSessionInfo.ExtraInfo.stFileStatus);

            // Create a GUID for this item (used by Scan & Deliver)
            UuidCreate(&vbiSessionInfo.ExtraInfo.stUniqueId);

            // Create a new log session GUID for this set of quarantine items.
            CreateLogGUIDString( szLogSessionGUID );
            szNewLogSessionGUID = (LPCTSTR)szLogSessionGUID;
            pNewEventBlk->LogSessionGUID = (char *)szNewLogSessionGUID;

            // We need a log line
            m_pVBin2->CreateLogLine( vbiSessionInfo.LogLine, pNewEventBlk );


            /////////////////////////////////////////////////////////////////
            // Final Step : Create new quarantine session, and add all 
            //              repaired items from the current session to it.

            if ( ERROR_SUCCESS == m_pVBin2->CreateSession( &vbiSessionInfo, &pVBinSession ) )
            {
                // Add all infected objects to quarantine.
                if (( DWH_OK != SaveQuarItemList( pVBinSession, szNewLogSessionGUID, &m_arInfectedQuarItems    ) ) ||
                    ( DWH_OK != SaveQuarItemList( pVBinSession, szNewLogSessionGUID, &m_arRemediationQuarItems ) ))
                {
                    // Discard this new session, since we failed to
                    // move everything over.
                    pVBinSession->DeleteSession();

                    // Failed to save some items.
                    dwReturn = DWH_ERROR_IN_QUARANTINE;
                }
            }
            else
            {
                // Failed to create quarantine session.
                dwReturn = DWH_ERROR_IN_QUARANTINE;
            }
        }
    }

    return dwReturn;
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineSessionItem::SaveQuarItemList
//
// Description: Helper function to save a quarantine item list to the specified
//              quarantine session.  This method will be used for both infection
//              lists, and remediation lists.
//
// Parameters:  IVBinSession *  pVBinSession        - [in] VBin session to save items to.
//              LPCTSTR         szNewLogSessionGUID - [in] New log session GUID for all saved records.
//              CPtrArray *     lparQuarItems       - [in] Array of quarantine items to save.
//
// Returns:     DWH_OK                      - Item copied to a new quarantine session successfully.
//              DWH_ERROR_IN_QUARANTINE     - General quarantine failure.  Could not create / add items to new session.
//              DWH_ERROR_REPAIR_IN_QUAR    - Error adding repair to quarantine.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
DWORD CQuarantineSessionItem::SaveQuarItemList( IVBinSession *pVBinSession, LPCTSTR szNewLogSessionGUID, CPtrArray *lparQuarItems )
{
    CQuarItem * lpItem      = NULL;
    DWORD       dwReturn    = DWH_OK;
    int         iNumItems   = lparQuarItems->GetSize();

    for ( int i=0; i<iNumItems; i++ )
    {
        lpItem = (CQuarItem*)lparQuarItems->GetAt(i);

        if ( lpItem )
        {
            // Save this item to the specified session.
            dwReturn = lpItem->SaveToQuarantine( pVBinSession, szNewLogSessionGUID );

            // If we have an error, fail out of saving this list.
            if ( dwReturn != DWH_OK )
                break;
        }
    }

    return dwReturn;
}

///////////////////////////////////////////////////////////////////////////////
// Name:        CQuarantineSessionItem::CreateLogGUIDString
//
// Description: Helper function to create a new log GUID string for a new session
//              of quarantine records to be saved. When we call SaveToQuarantine
//              for these session objects, we need to generate a new GUID so that
//              records will not get confused with the originals.
//
// Parameters:  LPTSTR  szNewLogGUID    - [out] String containing new log GUID.
//
// Returns:     TRUE  - New GUID generated successfully.
//              FALSE - Failed to generate new Log GUID.
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/2005  - KSackin : Function created.
///////////////////////////////////////////////////////////////////////////////
BOOL CQuarantineSessionItem::CreateLogGUIDString( LPTSTR szNewLogGUID )
{
    UUID    uuidNewLoggingGUID;
    LPTSTR  szTempString        = NULL;
    BOOL    bReturn             = FALSE;

    // Create a new GUID.
    if ( RPC_S_OK == UuidCreate( &uuidNewLoggingGUID ) )
    {
        if ( RPC_S_OK == UuidToString( &uuidNewLoggingGUID, (unsigned char **) &szTempString ) )
        {
            // Copy the returned string to our output parameter.
            _tcscpy( szNewLogGUID, szTempString );

            // Free up the string that was allocated for us.
            RpcStringFree( (unsigned char **)&szTempString );

            bReturn = TRUE;
        }
    }

    return bReturn;
}