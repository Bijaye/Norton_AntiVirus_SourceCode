/////////////////////////////////////////////////////////////////////
// AT_COUNT.CPP - Created - 10/27/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVALERT/VCS/at_count.cpv   1.0   18 Apr 1997 13:56:48   DSACKIN  $
//
// Description:
//    This source file will contain the code to the CTargetCounter
//    class, as well as the CTargetCounterList class.  This class
//    was desiged to handle counting of target types as they are
//    added and removed from the AlertTargetList.  This class was
//    needed because all of the targets are being stored in one large
//    list, and the only way to count the items would be to iterate
//    the entire list and count them.  This way, we can keep tallys
//    and get a count very rapidly.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVALERT/VCS/at_count.cpv  $
// 
//    Rev 1.0   18 Apr 1997 13:56:48   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:11:48   RFULLER
// Initial revision
// 
//    Rev 1.1   12 Nov 1996 22:44:26   KSACKIN
// Modified classes to be MFC Extension classes.  This meant changing the
// exported functions back, and then using a macro in ihe class definition.
// 
//    Rev 1.0   08 Nov 1996 15:33:48   KSACKIN
// Initial revision.
// 
//    Rev 1.0   29 Oct 1996 15:32:58   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <afx.h>
#include "AT_TGT.h"


/////////////////////////////////////////////////////////////////////
// CTargetCounter Class. (derived from a CObject for list storage)


/////////////////////////////////////////////////////////////////////
// Dynamic Object Creation and Runtime Type Identifier creation.

IMPLEMENT_DYNCREATE(CTargetCounter, CObject)


/////////////////////////////////////////////////////////////////////
// CTargetCounter::CTargetCounter
//
// Purpose:
//    Constructor for the CTargetCounter class.  This call will
//    initialize the data members of this class.  The
//    m_dwTargetTypeCount variable will be initialized to 1 because
//    if you are creating a new one, you are trying to initialize a
//    new type.
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
CTargetCounter::CTargetCounter(
    int nTargetType  // = 0 (default)
    )
{
   m_nTargetType = nTargetType;
   m_dwTargetTypeCount = 1;
}


/////////////////////////////////////////////////////////////////////
// CTargetCounter::~CTargetCounter
//
// Purpose:
//    Destructor for the CTargetCounter class.
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
CTargetCounter::~CTargetCounter( void )
{
}

/////////////////////////////////////////////////////////////////////
// CTargetCounter::DecrementCount
//
// Purpose:
//    This function will decrement the number of targets of this
//    type.  If the count of targets of this type is already zero,
//    this function will return FALSE, otherwise, it will decrement
//    the value and return TRUE.
//
// Parameters:
//    None.
//
// Returns:
//    Whether the value was decremented or not.  This function will
//    return FALSE if the count is already zero.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/27/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CTargetCounter::DecrementCount( void )
{
BOOL bRetVal=TRUE;

    if ( 0 != m_dwTargetTypeCount )
        {
                                        // Value is not 0, so decrement the
                                        // value and return true.
        m_dwTargetTypeCount--;
        }
    else
        {
                                        // Value was already zero, so return
                                        // FALSE, and do not decrement the value.
        bRetVal = FALSE;
        }

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// CTargetCounterList class. (Derived from a CObList for list
// functions).

/////////////////////////////////////////////////////////////////////
// Dynamic Object Creation and Runtime Type Identifier creation.

IMPLEMENT_DYNCREATE(CTargetCounterList, CObList)


/////////////////////////////////////////////////////////////////////
// CTargetCounterList::CTargetCounterList
//
// Purpose:
//    Constructor for the CTargetCounterList class.
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
CTargetCounterList::CTargetCounterList( void )
{
}


/////////////////////////////////////////////////////////////////////
// CTargetCounterList::~CTargetCounterList
//
// Purpose:
//    Destructor for the CTargetCounterList class.
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
CTargetCounterList::~CTargetCounterList( void )
{
                                        // Destroy all elements in our list.
    DestroyAll();
}

/////////////////////////////////////////////////////////////////////
// CTargetCounterList::GetTargetCount
//
// Purpose:
//    Get the count of a particular type of target, or a set of
//    targets.  The default value for the target type is TARGET_ALL
//    to get the total number of targets in the list.  You can also
//    specify a single target type, or a set of target types.
//
// Parameters:
//
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 10/27/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CTargetCounterList::GetTargetCount(
    int nTargetType // = TARGET_ALL
    )
{
CTargetCounter *lpcTargetCounter;
DWORD dwTargetCount=0;
POSITION rPosition;

                                        // Loop through the entire list.
    for ( rPosition = GetHeadPosition(); rPosition; )
        {
                                        // Get a pointer to each counter.
        lpcTargetCounter = (CTargetCounter *)GetNext( rPosition );

                                        // See if it is in the subset of target
                                        // types to count.
        if ( lpcTargetCounter->GetTargetType() & nTargetType )
            {
                                        // Correct type.  Add it to our total.
            dwTargetCount += lpcTargetCounter->GetTargetCount();
            }
        }

                                        // Return total of all targets asked
                                        // for.
    return dwTargetCount;
}

/////////////////////////////////////////////////////////////////////
// CTargetCounterList::FindTargetCounterOfType
//
// Purpose:
//    This function will find a target counter with a particular type
//    within our list of target counters.  If the counter is not
//    found, we will return NULL, otherwise, we will return a pointer
//    to the requested counter.
//
// Parameters:
//    int nTargetType - [IN] Type of target counter to search for.
//
// Returns:
//    Pointer to the requested CTargetCounter class, or NULL if a
//    counter of that type was not found.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 10/27/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CTargetCounter *CTargetCounterList::FindTargetCounterOfType(
    int nTargetType
    )
{
CTargetCounter *lpcTargetCounter=NULL;
POSITION        rPosition;

                                        // Get the head position of the list
                                        // for iteration purposes.
    rPosition = GetHeadPosition();

                                        // If there are elements in our list,
                                        // we are going to iterate throught the
                                        // list and try to find a counter for
                                        // this type.
    if ( NULL != rPosition )
        {
        do
            {
                                        // Loop through the target counters
                                        // until we find the one that we are
                                        // looking for, or we hit the end of
                                        // the list.
            lpcTargetCounter = (CTargetCounter *)GetNext( rPosition );
            }
        while ( (rPosition) &&
                (lpcTargetCounter->GetTargetType() != nTargetType) );

                                        // If we hit the end of the list and
                                        // the target type is still not
                                        // correct, we will return NULL because
                                        // the target was not found.
        if ( lpcTargetCounter->GetTargetType() != nTargetType )
            {
            lpcTargetCounter = NULL;
            }
        }

    return lpcTargetCounter;
}

/////////////////////////////////////////////////////////////////////
// CTargetCounterList::IncrementTargetCount
//
// Purpose:
//    This function will increment the count of a particular type of
//    target.  If this is the first time this has been called for a
//    particular type of target, we are going to create a new
//    instance of a CTargetCounter with this type and initialize it
//    to a count value of 1.
//
// Parameters:
//    int nTargetType - [IN] Target type to increment or create.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/27/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CTargetCounterList::IncrementTargetCount(
    int nTargetType
    )
{
CTargetCounter *lpcTargetCounter;

                                        // Search for a target counter of this
                                        // type already created.
    if ( lpcTargetCounter = FindTargetCounterOfType( nTargetType ) )
        {
                                        // Found a target counter for this type
                                        // of target, so increment it's count.
        lpcTargetCounter->IncrementCount();
        }
    else
        {
                                        // If we did not find a target counter
                                        // of this type in the list, we are
                                        // going to create one, and then add it
                                        // to the list.
        lpcTargetCounter = new CTargetCounter( nTargetType );

                                        // Add this counter to our list.
        AddTail( lpcTargetCounter );
        }
}

/////////////////////////////////////////////////////////////////////
// CTargetCounterList::DecrementTargetCount
//
// Purpose:
//    This function will decrement the count of a particular type of
//    target.  If this target type is not found, or the count on this
//    type is already zero, this function will return FALSE,
//    otherwise, it will decrement the value and return TRUE.
//
// Parameters:
//    int nTargetType - [IN] Target type to decrement.
//
// Returns:
//    BOOLEAN success of search and decrement.  If the target type
//    cannot be found, or if the value is zero already, return FALSE.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/27/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CTargetCounterList::DecrementTargetCount(
    int nTargetType
    )
{
CTargetCounter *lpcTargetCounter;
BOOL bRetVal = TRUE;

                                        // Search for a target counter of this
                                        // type already created.
    if ( lpcTargetCounter = FindTargetCounterOfType( nTargetType ) )
        {
                                        // Found a target counter for this type
                                        // of target, so decement it's count.
                                        // This function will return FALSE if
                                        // the count is already zero.
        if ( FALSE == lpcTargetCounter->DecrementCount() )
            {
                                        // Counter was already at zero, so we
                                        // are going to return FALSE.
            bRetVal = FALSE;
            }
        }
    else
        {
                                        // Could not find a target counter of
                                        // that type, so we are going to return
                                        // FALSE.
        bRetVal = FALSE;
        }

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// CTargetCounterList::DestroyAll
//
// Purpose:
//    This function will remove all target counters from the list,
//    and free all memory associated with them.
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
void CTargetCounterList::DestroyAll( void )
{
CTargetCounter *lpcRemovedTargetCounter;

    while ( FALSE == IsEmpty() )
        {
                                        // Remove the top item from our list.
        lpcRemovedTargetCounter = (CTargetCounter *) RemoveHead();

                                        // Delete the object (calling it's
                                        // destructor.)
        delete( lpcRemovedTargetCounter );
        }
}
