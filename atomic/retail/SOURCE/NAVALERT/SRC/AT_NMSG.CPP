/////////////////////////////////////////////////////////////////////
// AT_NMSG.CPP - Created - 10/24/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVALERT/VCS/at_nmsg.cpv   1.1   22 May 1997 14:29:40   DSACKIN  $
//
// Description:
//    Implementation file for the CNetMsgTarget class.  This class is
//    derived from the CAlertTarget base class.  It will contain
//    the data for a NetMsg target opteration, as well as access
//    functions to retrieve and set data members.  Also, this class
//    will be able to pack all of it's data into a formatted
//    string to write to a file, as well as decode a packed string
//    that was sent to it.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVALERT/VCS/at_nmsg.cpv  $
// 
//    Rev 1.1   22 May 1997 14:29:40   DSACKIN
// Converted back to TCHAR.
//
//    Rev 1.0   18 Apr 1997 13:57:04   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:11:50   RFULLER
// Initial revision
//
//    Rev 1.4   20 Dec 1996 17:50:52   ksackin
// Added a member variable to the CNetworkMessage class of a dwResourceType
// DWORD.  Also added access function to get and set this variable.
//
//    Rev 1.3   04 Dec 1996 23:13:26   DSACKIN
// Changed all instances of LPCTSTR to LPCWSTR for use with non-UNICODE apps.
//
//    Rev 1.2   21 Nov 1996 16:31:26   ksackin
// Added functionality to call base class for it's options before adding each
// target's options.  This allows for pre and post processing.
//
//    Rev 1.1   12 Nov 1996 22:47:28   KSACKIN
// Modified classes to be MFC Extension classes.  This meant changing the
// exported functions back, and then using a macro in ihe class definition.
//
//    Rev 1.0   08 Nov 1996 15:33:58   KSACKIN
// Initial revision.
//
//    Rev 1.1   07 Nov 1996 01:05:16   KSACKIN
// Changed some string handling for UNICODE support, as well as modified some
// exported functions.
//
//    Rev 1.0   29 Oct 1996 15:33:10   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <afxwin.h>
#include "AT_TGT.H"


/////////////////////////////////////////////////////////////////////
// CNetMsgTarget class.  (Derived from a CAlertTarget)


/////////////////////////////////////////////////////////////////////
// Dynamic Object Creation and Runtime Type Identifier creation.

IMPLEMENT_DYNCREATE(CNetMsgTarget, CAlertTarget)


/////////////////////////////////////////////////////////////////////
// CNetMsgTarget::CNetMsgTarget
//
// Purpose:
//    Constructor for the CNetMsgTarget class.  This function will
//    initialize any information specific to a NetMsg Target.
//
// Parameters:
//    None.
//
// Returns:
//    Instance of this class.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/24/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CNetMsgTarget::CNetMsgTarget( void )
{
    SetTargetType( TARGET_MESSAGE_NETWORK );

                                        // Set the default resource type.
    m_dwResourceType = 0;
}

/////////////////////////////////////////////////////////////////////
// CNetMsgTarget::~CNetMsgTarget
//
// Purpose:
//    Destructor for the CNetMsgTarget class.  This function will
//    free any information that was allocated and clean up after
//    this target type.
//
// Parameters:
//    None.
//
// Returns:
//    Instance of this class.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/24/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CNetMsgTarget::~CNetMsgTarget( void )
{
}

/////////////////////////////////////////////////////////////////////
// CNetMsgTarget::SetAddress
//
// Purpose:
//     Sets the address of the Network Message alert recipient.
//
// Parameters:
//     LPCWSTR cszNewAddress - [IN] New address for this Network
//                                  alert message.
//
// Returns:
//     BOOLEAN as to whether we were able to assign the new address
//     or not.  If we cannot allocate memory for that address, then
//     we need to fail gracefully.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetMsgTarget::SetAddress(
    LPCTSTR cszNewAddress
    )
{
BOOL bRetVal = TRUE;

                                        // Try to set the new address, which
                                        // may involve needing to allocate more
                                        // memory.  If this fails, we are going
                                        // to catch the exception.
    try
        {
        m_cszAddress = cszNewAddress;
        }

                                        // Catch the memory allocation
                                        // exception, and return FALSE.
    catch(CMemoryException)
        {
        bRetVal = FALSE;
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CNetMsgTarget::SetResourceType
//
// Purpose:
//     Sets the resource type of a net message target.  This will be
//     so that we can format the net message string before we send
//     it to the net messenger.  This was also so that we could
//     support a custom string that could be sent to the Net Messenger
//     for things such as user notification.
//
// Parameters:
//    DWORD dwNewResourceType - [IN] The new resource type of this
//                                   object.
//
// Returns:
//     None.
//
// See Also:
//     NResCont.h for defines of the resource types.
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetMsgTarget::SetResourceType(
    DWORD dwNewResourceType
    )
{
                                        // Set our resource type to the
                                        // parameter passed.
   m_dwResourceType = dwNewResourceType;
}


/////////////////////////////////////////////////////////////////////
// CNetMsgTarget::ConvertToPacked
//
// Purpose:
//    Function to convert the local members of this class to a packed
//    string used for saving this type of a target.
//
// Parameters:
//    CString *lpcPackedString - [IN][OUT] Address of a CString to fill
//                                     with the packed information.
//
// Returns:
//    BOOLEAN Success of the string packing.  Will return FALSE if
//    memory allocation fails while packing the string.
//
//    Returns a filled out packed string through the
//    lpcPackedString parameter.  This string will have the data for
//    this target type packed together in a text format.
//
// See Also:
//    CAlertTarget class for virtual base functions.
//
/////////////////////////////////////////////////////////////////////
// 10/24/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetMsgTarget::ConvertToPacked(
    CString *lpcPackedString
    )
{
BOOL bRetVal=TRUE;

                                        // String may already have contents
                                        // from base class.

                                        // Pack the Address.
   if ( FALSE == PackParameter( lpcPackedString, m_cszAddress ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }
                                        // Pack the Resource Type.
   if ( FALSE == PackNumericParameter( lpcPackedString, m_dwResourceType ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }

Cleanup:

   return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CNetMsgTarget::ConvertFromPacked
//
// Purpose:
//    This function will convert the packed data sent to it through
//    the lpcPackedString parameter into the internal data for this
//    target type.
//
// Parameters:
//    CString *lpcPackedString - [IN][OUT] String to extract parameters
//                                   from for this structures information.
//
// Returns:
//    BOOLEAN Success of the string unpacking.  Will return FALSE if
//    an error occurs while unpacking the string or if verification
//    of the data parsed fails.
//
// See Also:
//    CAlertTarget class for virtual base functions.
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetMsgTarget::ConvertFromPacked(
    CString *lpcPackedString
    )
{
BOOL bRetVal=TRUE;
                                        // Extract our parameters from this
                                        // string.  This is for support
                                        // with Pre and Post processing.

                                        // Extract the target machine's
                                        // address.
    if ( FALSE == ExtractParameter( lpcPackedString, &m_cszAddress ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }
                                        // Extract the Resource Type.
   if ( FALSE == ExtractDWORDParameter( lpcPackedString, &m_dwResourceType ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }


Cleanup:

    return bRetVal;
}
