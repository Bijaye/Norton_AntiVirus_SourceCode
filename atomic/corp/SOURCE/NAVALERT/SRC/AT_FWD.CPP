/////////////////////////////////////////////////////////////////////
// AT_FWD.CPP - Created - 10/24/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVALERT/VCS/at_fwd.cpv   1.1   22 May 1997 14:29:24   DSACKIN  $
//
// Description:
//    Implementation file for the CForwardTarget class.  This class is
//    derived from the CAlertTarget base class.  It will contain
//    the data for a Forward target opteration, as well as access
//    functions to retrieve and set data members.  Also, this class
//    will be able to pack all of it's data into a formatted
//    string to write to a file, as well as decode a packed string
//    that was sent to it.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVALERT/VCS/at_fwd.cpv  $
// 
//    Rev 1.1   22 May 1997 14:29:24   DSACKIN
// Converted back to TCHAR.
//
//    Rev 1.0   18 Apr 1997 13:56:58   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:11:50   RFULLER
// Initial revision
//
//    Rev 1.6   06 Dec 1996 13:57:06   ksackin
// Added resource state switching in the constructor for the CForwardTarget
// object.  This was so that we could load the dynamic keyword out of our
// dll's resource.
//
//    Rev 1.5   04 Dec 1996 23:12:36   DSACKIN
// Changed all LPCTSTR instances to LPCWSTR for use with non-Unicode apps.
//
//    Rev 1.4   26 Nov 1996 14:36:16   DSACKIN
// Set the default "dynamic" string for the protocol.
//
//    Rev 1.3   21 Nov 1996 16:31:36   ksackin
// Added functionality to call base class for it's options before adding each
// target's options.  This allows for pre and post processing.
//
//    Rev 1.2   18 Nov 1996 15:38:36   DSACKIN
// Added protocol information.
//
//    Rev 1.1   12 Nov 1996 22:47:26   KSACKIN
// Modified classes to be MFC Extension classes.  This meant changing the
// exported functions back, and then using a macro in ihe class definition.
//
//    Rev 1.0   08 Nov 1996 15:33:54   KSACKIN
// Initial revision.
//
//    Rev 1.1   07 Nov 1996 01:04:36   KSACKIN
// Changed some string handling for UNICODE support, as well as modified some
// exported functions.
//
//    Rev 1.0   29 Oct 1996 15:33:06   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <afxwin.h>
#include "AT_TGT.H"
#include "at_str.h"
#include "dllrcsta.h"                   // Resource state class.

/////////////////////////////////////////////////////////////////////
// CForwardTarget class.  (Derived from a CAlertTarget)


/////////////////////////////////////////////////////////////////////
// Dynamic Object Creation and Runtime Type Identifier creation.

IMPLEMENT_DYNCREATE(CForwardTarget, CAlertTarget)


/////////////////////////////////////////////////////////////////////
// CForwardTarget::CForwardTarget
//
// Purpose:
//    Constructor for the CForwardTarget class.  This function will
//    initialize any information specific to a Forward Target.
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
CForwardTarget::CForwardTarget( void )
{
                                        // Setup the module state so that
                                        // we may load strings from the correct
                                        // resource.
    CDLLResourceState resourceSwitch;

    SetTargetType( TARGET_FORWARD );

    m_cszProtocol.LoadString (IDS_DEFAULT_PROTOCOL);
}

/////////////////////////////////////////////////////////////////////
// CForwardTarget::~CForwardTarget
//
// Purpose:
//    Destructor for the CForwardTarget class.  This function will
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
CForwardTarget::~CForwardTarget( void )
{
}

/////////////////////////////////////////////////////////////////////
// CForwardTarget::SetAddress
//
// Purpose:
//     Sets the address of the Forward alert recipient.
//
// Parameters:
//     LPCWSTR cszNewAddress - [IN] New address for this Forward alert.
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
BOOL CForwardTarget::SetAddress(
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
// CForwardTarget::SetProtocol
//
// Purpose:
//     Sets the protocol to use when sending the forward
//
// Parameters:
//     LPCWSTR cszNewProtocol - [IN] New protocol for this Forward alert.
//
// Returns:
//     BOOLEAN as to whether we were able to assign the new address
//     or not.  If we cannot allocate memory for that address, then
//     we need to fail gracefully.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/18/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CForwardTarget::SetProtocol(
    LPCTSTR cszNewProtocol
    )
{
BOOL bRetVal = TRUE;

                                        // Try to set the new protocol, which
                                        // may involve needing to allocate more
                                        // memory.  If this fails, we are going
                                        // to catch the exception.
    try
        {
        m_cszProtocol = cszNewProtocol;
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
// CForwardTarget::ConvertToPacked
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
BOOL CForwardTarget::ConvertToPacked(
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

                                        // Pack the Protocol.
   if ( FALSE == PackParameter( lpcPackedString, m_cszProtocol ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }

Cleanup:

   return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CForwardTarget::ConvertFromPacked
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
BOOL CForwardTarget::ConvertFromPacked(
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

                                        // Extract the target machine's
                                        // protocol
    if ( FALSE == ExtractParameter( lpcPackedString, &m_cszProtocol ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }


Cleanup:

    return bRetVal;
}
