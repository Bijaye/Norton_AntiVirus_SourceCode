#include "stdafx.h"
#include "blockedappqueue.h"

extern DWORD g_dwSessionID;

DWORD CBlockedAppQueue::m_dwMaxActiveBlocked = 0;

#define EVENTVAL_CONTAINER_STATUS_REMOVED_UI 254636 // User closed the UI, remove the BAG
#define EVENTVAL_CONTAINER_STATUS_POINTER_BAG 94523

CBlockedAppQueue::CBlockedAppQueue(void)
{
    m_Options.m_eCOMModel = ccLib::CCoInitialize::eMTAModel;
    m_Options.m_bPumpMessages = TRUE;
}

CBlockedAppQueue::~CBlockedAppQueue(void)
{
    // Delete the dqBAG item data
    //
    for ( iterBAGQueue iter = m_qBAG.begin ();
          iter != m_qBAG.end ();
          iter ++ )
    {
        if ( *iter )
        {
            delete (*iter);
            (*iter) = NULL;
        }
    }

    // Nuke the rest
    m_qBAG.erase ( m_qBAG.begin (), m_qBAG.end () );
}

//////////////////////////////////////////////////////////////////////
// CBlockedAppQueue::processQueue()

// Add this to the BAG queue if it doesn't exist.
//
void CBlockedAppQueue::processQueue()
{
    while( !IsTerminating() )
    {
        // Get the item off the Q.
        //
        CEventData* pEventData = NULL;
        if ( !getQueueFront( pEventData ) || !pEventData)         // Already removed?
            return;

        long lContainerStatus = 0;
        pEventData->GetData( AV::Event_Container_Status, lContainerStatus );

        switch ( lContainerStatus )
        {
        case AV::Event_Container_Status_COMPLETED:
        case AV::Event_Container_Status_CANCELLED:
        case AV::Event_Container_Status_ABORTED:
            {
            ccLib::CSingleLock lock( &m_csBAGQueue, INFINITE, FALSE );
            Remove ( *pEventData );
            }
            break;

        case AV::Event_Container_Status_BLOCKED:
            {
                // Is this for our session?
                //
                long lSessionID = 0;

                pEventData->GetData ( AV::Event_Base_propSessionID, lSessionID );

                if ( lSessionID == g_dwSessionID )
                {
                    // Lock the Q for searching
                    //CCTRACEI (_T("CBlockedAppQueue::processQueue - Locking BAGQ"));
                    ccLib::CSingleLock lock( &m_csBAGQueue, INFINITE, FALSE );

                    if ( m_qBAG.size () > 0 )
                    {
                        CBlockedAppGroup* pOldBAG = NULL;

                        if ( findBAG ( *pEventData, pOldBAG ))
                        {
                            CCTRACEI (_T("CBlockedAppQueue::processQueue - Duplicate ED"));
                            pOldBAG->AddDup ();  // keep the Q locked while we call AddDup().
                            break;
                        }
                    }

                    // It's a new event.
                    //
                    CCTRACEI (_T("CBlockedAppQueue::processQueue - Adding new BAG"));
                    CBlockedAppGroup* pNewBAG = new CBlockedAppGroup(*pEventData, this);

                    // Currently showing the max, put it in the queue to show later.
                    m_qBAG.push_back (pNewBAG);

                    showNextBAG();
                }
                else
                    CCTRACEE (_T("CBlockedAppQueue::processQueue - block with wrong session data"));
            }
            break;
        case EVENTVAL_CONTAINER_STATUS_REMOVED_UI:
            {
            // We get here if we are closing a UI ourselves
            ccLib::CSingleLock lock( &m_csBAGQueue, INFINITE, FALSE );
            CBlockedAppGroup* pBAG = NULL;
            pEventData->GetData ( EVENTVAL_CONTAINER_STATUS_POINTER_BAG, (long&) pBAG);
            if ( pBAG )
            {
                deleteBAG (pBAG);
                showNextBAG ();
            }
            }
            break;
        }// end switch
        
        if ( pEventData)
        {
            delete pEventData;
            pEventData = NULL;
        }
    } // end while
}

// LOCK THE QUEUE BEFORE ENTERING!!
//
bool CBlockedAppQueue::findBAG (CEventData& EventData, CBlockedAppGroup*& pBAGOut)
{
    CBlockedAppGroup TempBAG (EventData);

    for ( iterBAGQueue iter = m_qBAG.begin ();
          iter != m_qBAG.end ();
          iter ++ )
    {
        if ( *iter )
        {
            CBlockedAppGroup* pBAGInQ = (*iter);

            if ( TempBAG == *pBAGInQ )
            {
                pBAGOut = pBAGInQ;
                return true;
            }
        }
    }

    return false;
}

// BAG Sink
// 
void CBlockedAppQueue::OnBAGUIClosed (CBlockedAppGroup* pBAG)
{
    // Put the value of this item into the Q to be closed.
    //
    CEventData* pCloseMe = new CEventData ();
    long lType = EVENTVAL_CONTAINER_STATUS_REMOVED_UI;
    pCloseMe->SetData ( EVENTVAL_CONTAINER_STATUS_POINTER_BAG, (long)pBAG );
    pCloseMe->SetData ( AV::Event_Container_Status, lType);
    pCloseMe->SetData ( AV::Event_Base_propSessionID, g_dwSessionID );
    AddItem (pCloseMe);
}

// LOCK THE QUEUE BEFORE ENTERING!!
//
void CBlockedAppQueue::showNextBAG ()
{
    // Anything in our Q for showing?
    //
    if ( m_qBAG.size () > 0 )
    {
        // The only think coming in is BlockedApp notifications.
        // If there's room in the UI thread pool then process it.
        if ( CBlockedAppUI::GetNumActiveUI() < (long) m_dwMaxActiveBlocked )
        {
            for ( iterBAGQueue iter = m_qBAG.begin ();
                  iter != m_qBAG.end ();
                  iter ++ )
            {
                // Call Show() on all the BAGs. If one is already showing, go to the next.
                //
                if ( (*iter) && (*iter)->Show())
                    return;
            }
        }
        else
            CCTRACEE (_T("CBlockedAppQueue::showNextBAG - too many BAUI's open %d"), CBlockedAppUI::GetNumActiveUI());
    }
}

// LOCK THE QUEUE BEFORE ENTERING!!
//
void CBlockedAppQueue::deleteBAG ( CBlockedAppGroup* pBAG )
{
    if ( pBAG )
    {
        for ( iterBAGQueue iter = m_qBAG.begin ();
                iter != m_qBAG.end ();
                iter ++ )
        {
            // Match by address.
            if ( *iter == pBAG )
            {
                // Matched. Remove the entry
                //
                m_qBAG.erase ( iter );

                // Free the memory
                delete pBAG;
                pBAG = NULL;
                CCTRACEI (_T("CBlockedAppQueue::deleteBAG - Removing entry in Q"));
                return;
            }
        }
    }
}

// LOCK THE QUEUE BEFORE ENTERING!!
//
void CBlockedAppQueue::Remove ( CEventData& EventData)
{
    // Search each item and remove it.
    iterBAGQueue iter = m_qBAG.begin ();
    bool bMatched = false;

    while ( iter != m_qBAG.end () )
    {
        if (*iter)
        {
            CBlockedAppGroup* pBAGInQ = (*iter);

            if ( pBAGInQ->Remove (EventData))
            {
                // Matched. Remove the entry
                //
                bMatched = true;
                CCTRACEI (_T("CBlockedAppQueue::Remove - Removed BAG"));

                // Free the memory
                iter = m_qBAG.erase ( iter );
                delete pBAGInQ;
                pBAGInQ = NULL;
                continue;
            }
        }
        iter++;
    }

    if ( bMatched )
        showNextBAG ();
    else
        CCTRACEE (_T("CBlockedAppQueue::Remove - No BAG found"));
}

void CBlockedAppQueue::SetMaxActiveBlocked ( DWORD dwMaxActiveBlocked)
{
    m_dwMaxActiveBlocked = dwMaxActiveBlocked;
}
