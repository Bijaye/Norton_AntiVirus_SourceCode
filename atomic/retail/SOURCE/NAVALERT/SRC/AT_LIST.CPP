/////////////////////////////////////////////////////////////////////
// AT_LIST.CPP - Created - 10/23/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVALERT/VCS/at_list.cpv   1.0   18 Apr 1997 13:56:50   DSACKIN  $
//
// Description:
//    Implementation file for the CAlertTargetList class.  This class
//    will manage a list of Alert Targets.  It will contain Addition
//    and removal functions that will be aware of the priority of the
//    individual target, and will add them in order.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVALERT/VCS/at_list.cpv  $
// 
//    Rev 1.0   18 Apr 1997 13:56:50   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:11:48   RFULLER
// Initial revision
// 
//    Rev 1.3   16 Jan 1997 00:21:52   ksackin
// Added a RemoveNextTarget to compliment the GetNextTarget() function.  This
// new function will actually remove the next target of the type you are looking
// for from the list so that you can be responsable for deleting that target
// and not having the CAlertOptions class delete that under you.  This function
// actually removes that target from the list.
//
//    Rev 1.2   04 Dec 1996 23:12:38   DSACKIN
// No change.
//
//    Rev 1.1   12 Nov 1996 22:47:22   KSACKIN
// Modified classes to be MFC Extension classes.  This meant changing the
// exported functions back, and then using a macro in ihe class definition.
//
//    Rev 1.0   08 Nov 1996 15:33:50   KSACKIN
// Initial revision.
//
//    Rev 1.0   29 Oct 1996 15:33:02   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <afx.h>
#include "AT_TGT.H"


/////////////////////////////////////////////////////////////////////
// Dynamic Object Creation and Runtime Type Identifier creation.

IMPLEMENT_DYNCREATE(CAlertTargetList, CAlertTarget)


/////////////////////////////////////////////////////////////////////
// CAlertTargetList::CAlertTargetList
//
// Purpose:
//    Standard Constructor.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlertTargetList::CAlertTargetList( void )
{
    m_rPosition = NULL;
}

/////////////////////////////////////////////////////////////////////
// CAlertTargetList::~CAlertTargetList
//
// Purpose:
//  Standard Destructor.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlertTargetList::~CAlertTargetList( void )
{
}


/////////////////////////////////////////////////////////////////////
// CAlertTargetList::AddTarget
//
// Purpose:
//    This function will add a target item to it's own list.  Since
//    this class was derived from a CObList, it has member functions
//    to add and remove items from it's list already.
//
// Parameters:
//    CAlertTarget *lpcTargetToAdd - [IN] Address of the target to
//                                   add.  This target should be
//                                   allocated by the caller, and
//                                   stays with this list. (It should
//                                   not be deallocated by the
//                                   caller.)
//
// Returns:
//    BOOLEAN success of the addition.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertTargetList::AddTarget(
    CAlertTarget *lpcTargetToAdd
    )
{
CAlertTarget *  lpcCurrentTarget;
POSITION        rSavePos = m_rPosition;
BOOL            bInserted = FALSE;
BOOL            bRetVal = TRUE;


                                        // Reset the position of the current
                                        // iterator for this list.
    ResetPosition();

    try
        {
                                        // Iterate through the list to find a
                                        // target with a priority less than
                                        // yours (greater number).
        while ( ( lpcCurrentTarget = GetNextTarget() ) &&
                ( FALSE == bInserted ) )
            {
                                        // Check to see if we are at correct
                                        // spot.
            if ( lpcTargetToAdd->GetPriority() < lpcCurrentTarget->GetPriority() )
                {
                                        // Found an item with a greater
                                        // priority value than ours, so we are
                                        // going to insert ours before that
                                        // one. (Maintain sorted order)
                InsertBefore( m_rPosition, lpcTargetToAdd );

                bInserted = TRUE;
                }
            }

                                        // If this value is false, then we did
                                        // not find a value greater than ours,
                                        // or the list is empty.  If this
                                        // happens, we are going to add our at
                                        // the last position in the list.
        if ( FALSE == bInserted )
            {
            AddTail( lpcTargetToAdd );
            }
        }

                                        // If an error occurred when adding an
                                        // entry, we will catch it here.
    catch (CMemoryException)
        {
                                        // Return an error when adding the
                                        // element.
        bRetVal = FALSE;
        }

                                        // If we added the element successfully...
    if ( bRetVal == TRUE )
        {
                                        // Increment count of this target type.
        m_cTargetCounterList.IncrementTargetCount( lpcTargetToAdd->GetTargetType() );
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CAlertTargetList::RemoveTarget
//
// Purpose:
//     Function to remove a target from the internal list of targets.
//     When this function is called, the memory for that object is
//     going to be deallocated.  This does not need to be done by the
//     caller.
//
// Parameters:
//    CAlertTarget *lpcTargetToRemove - [IN] Address of a target to
//                                   item to delete.  This will most
//                                   likely be the result of a query
//                                   of this item followed by a
//                                   subsequent call to remove it.
//                                   This is to identify the item
//                                   specifically.
//
// Returns:
//    BOOLEAN of whether the item was deleted or not.  This will
//    return FALSE if the item cannot be found, or TRUE if it was
//    successfully deleted from the list.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertTargetList::RemoveTarget(
    CAlertTarget *lpcTargetToRemove
    )
{
BOOL bRetVal=TRUE;
POSITION rPosition;

                                        // Search for the item that you wish to
                                        // remove.
    rPosition = Find( lpcTargetToRemove );

                                        // If we successfully found the item,
                                        // Remove it.
    if ( rPosition )
        {
                                        // Remove the element.  They do not
                                        // support any exception here.
        RemoveAt( rPosition );

                                        // Decrement count of this target type.
        m_cTargetCounterList.DecrementTargetCount( lpcTargetToRemove->GetTargetType() );

                                        // Delete the item that we removed from
                                        // that list.
        delete( lpcTargetToRemove );
        }
    else
        {
                                        // Could not find the element, so fail
                                        // gracefully.
        bRetVal = FALSE;
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CAlertTargetList::GetTargetCount
//
// Purpose:
//    This function will return the number of targets of the target
//    type that you specify are in the list.  This function can take
//    a combination of target type flags for returning the total
//    number of targets, or simply a subset of target types.
//
// Parameters:
//    int nTargetType - [IN] Combination of target type flags.  This
//                           can be one target type, many target
//                           types, or all target types.  This
//                           parameter defaults to all target types.
//
// Returns:
//    DWORD count of targets in that target type specifier.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/27/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CAlertTargetList::GetTargetCount(
    int nTargetType  // = TARGET_ALL (default)
    )
{
    return m_cTargetCounterList.GetTargetCount( nTargetType );
}

/////////////////////////////////////////////////////////////////////
// CAlertTargetList::DestroyAll
//
// Purpose:
//    Function to destroy all entries in the list.  This function
//    does not ask for confirmation, so be sure that you really
//    want to clear all entries in this list.  This function also
//    calls through to the destructors for all items in the list.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/27/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CAlertTargetList::DestroyAll( void )
{
CAlertTarget *lpcRemovedTarget;

    while ( FALSE == IsEmpty() )
        {
                                        // Remove the top item from our list.
        lpcRemovedTarget = (CAlertTarget *) RemoveHead();

                                        // Delete the object (calling it's
                                        // destructor.)
        delete( lpcRemovedTarget );
        }

                                        // Reset the current iteration
                                        // position.
    ResetPosition();

                                        // Destroy all target type counters
                                        // as well.
    m_cTargetCounterList.DestroyAll();
}


/////////////////////////////////////////////////////////////////////
// CAlertTargetList::GetNextTarget
//
// Purpose:
//    This function will iterate through all of the current targets
//    that we have loaded in our target list.  First you should call
//    the ResetPosition function to ensure that you are at the
//    beginning of the list, and then you can iterate though based on
//    type, or simply get all targets.
//
// Parameters:
//    int nEventType - [IN] Parameter to screen events returned to
//                          only a certain type of AlertTarget.  This
//                          parameter defaults to TARGET_ALL.  This
//                          can also be a combination of flags for
//                          target types.
//
// Returns:
//    A pointer to the next item in the list, or NULL if the list is
//    either empty or at the end of the list.
//
// See Also:
//    AlertOps.h for list of Target Types.
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlertTarget * CAlertTargetList::GetNextTarget(
    int nEventType   // = TARGET_ALL
    )
{
CAlertTarget *lpcNextTarget;
                                        // Check to see if we are either at the
                                        // end of the list, or the list is
                                        // empty.
    if ( m_rPosition == NULL )
        {
                                        // If this is the case, then we are
                                        // going to return NULL.
        lpcNextTarget = NULL;
                                        // Alse, we are going to reset our
                                        // iterator so that you can iterate
                                        // again on a different type.
        ResetPosition();
        }
    else
        {
                                        // Otherwise, we are going to get the
                                        // next item and return a pointer to it.
        do
            {
            lpcNextTarget = (CAlertTarget *)GetNext( m_rPosition );
            }
                                        // Loop until found correct type.
        while ( (m_rPosition) &&
                (FALSE == (lpcNextTarget->GetTargetType() & nEventType)) );

                                        // Hit the end of the list, so we need
                                        // to check and see if the last entry
                                        // that we read was the correct type.
                                        // Otherwise, we need to zero it out.
                                        // Remember, the position gets incermented
                                        // after the item is read, so we still
                                        // could have an item of the wrong type.
        if ( !m_rPosition )
            {
                                        // Is it still the wrong type?
            if ( FALSE == (lpcNextTarget->GetTargetType() & nEventType) )
                {
                                        // return NULL that we did not find one.
                lpcNextTarget = NULL;
                                        // Hit the end of the list, so we need
                                        // to reset the list for the next
                                        // iterator.
                ResetPosition();
                }
            }
        }

    return lpcNextTarget;
}

/////////////////////////////////////////////////////////////////////
// CAlertTargetList::RemoveNextTarget
//
// Purpose:
//    This function will make a call to GetNextTarget to get the next
//    target of the type that you are looking for.  When and if it
//    finds a target of the type you are looking for, it will remove
//    that target from the current list of targets so that the memory
//    for this object will no longer be freed when the CAlertOptions
//    class gets destructed.  After you call this function, you will
//    be responsable for deleting this object yourself.
//
// Parameters:
//    int nEventType - [IN] Parameter to screen events returned to
//                          only a certain type of AlertTarget.  This
//                          parameter defaults to TARGET_ALL.  This
//                          can also be a combination of flags for
//                          target types.
//
// Returns:
//    A the next item in the list, or NULL if the list is
//    either empty or at the end of the list.  It will be your
//    responsability to delete this object when you are finished.
//
// See Also:
//    AlertOps.h for list of Target Types.
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlertTarget * CAlertTargetList::RemoveNextTarget(
    int nEventType   // = TARGET_ALL
    )
{
CAlertTarget *lpcNextAlertTarget=NULL;

                                        // Get the item that you are looking
                                        // for.
    lpcNextAlertTarget = GetNextTarget( nEventType );

                                        // If we actually got one...
    if ( NULL != lpcNextAlertTarget )
        {
                                        // Remove this object from this list so
                                        // that the caller of this function can
                                        // be responsable for deleting this
                                        // object, and not this list.
        RemoveAt( Find( lpcNextAlertTarget ) );
        }

    return lpcNextAlertTarget;
}

/////////////////////////////////////////////////////////////////////
// CAlertTargetList::ResetPosition
//
// Purpose:
//     Function to reset the position of the iterator variable.  This
//     should be done before each subsequent loop to retrieve a list
//     of events.  If the iterator is half way down the list when you
//     call this for a different type, it will continue from there,
//     instead of from the beginning.
//
// Parameters:
//     None.
//
// Returns:
//     None.
//
// See Also:
//     GetNextEvent();
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CAlertTargetList::ResetPosition( void )
{
   m_rPosition = GetHeadPosition();
}
