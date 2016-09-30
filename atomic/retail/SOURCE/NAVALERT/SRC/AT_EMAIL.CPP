/////////////////////////////////////////////////////////////////////
// AT_EMAIL.CPP - Created - 10/24/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVALERT/VCS/at_email.cpv   1.1   22 May 1997 14:24:04   DSACKIN  $
//
// Description:
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVALERT/VCS/at_email.cpv  $
// 
//    Rev 1.1   22 May 1997 14:24:04   DSACKIN
// Converted back to TCHAR
//
//    Rev 1.0   18 Apr 1997 13:57:10   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:11:52   RFULLER
// Initial revision
//
//    Rev 1.3   04 Dec 1996 23:12:34   DSACKIN
// Changed all LPCTSTR instances to LPCWSTR for use with non-Unicode apps.
//
//    Rev 1.2   21 Nov 1996 16:31:40   ksackin
// Added functionality to call base class for it's options before adding each
// target's options.  This allows for pre and post processing.
//
//    Rev 1.1   12 Nov 1996 22:47:32   KSACKIN
// Modified classes to be MFC Extension classes.  This meant changing the
// exported functions back, and then using a macro in ihe class definition.
//
//    Rev 1.0   08 Nov 1996 15:34:02   KSACKIN
// Initial revision.
//
//    Rev 1.1   07 Nov 1996 01:05:20   KSACKIN
// Changed some string handling for UNICODE support, as well as modified some
// exported functions.
//
//    Rev 1.0   29 Oct 1996 15:33:14   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <afx.h>
#include "AT_TGT.H"


/////////////////////////////////////////////////////////////////////
// CEMailTarget (derived from CAlertTarget)


/////////////////////////////////////////////////////////////////////
// Dynamic Object Creation and Runtime Type Identifier creation.

IMPLEMENT_DYNCREATE(CEMailTarget, CAlertTarget)


/////////////////////////////////////////////////////////////////////
// CEMailTarget::CEMailTarget
//
// Purpose:
//    Standard constructor for a CEMailTarget class.  This
//    constructor will initialize the data relative to this class.
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
// 10/24/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CEMailTarget::CEMailTarget( void )
{
    SetTargetType( TARGET_EMAIL );
}

/////////////////////////////////////////////////////////////////////
// CEMailTarget::~CEMailTarget
//
// Purpose:
//    Standard destructor for a CEMailTarget class.  This
//    destructor will destroy any allocated data.
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
// 10/24/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CEMailTarget::~CEMailTarget( void )
{
}


/////////////////////////////////////////////////////////////////////
// CEMailTarget::SetName
//
// Purpose:
//     Sets the name of the E-Mail recipient.
//
// Parameters:
//     LPCWSTR cszNewName - [IN] New name for this E-Mail target.
//
// Returns:
//     BOOLEAN as to whether we were able to assign that new name
//     or not.  If we cannot allocate memory for that name, then we
//     need to fail gracefully.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CEMailTarget::SetName(
    LPCTSTR cszNewName
    )
{
BOOL bRetVal = TRUE;

                                        // Try to set the new name, which may
                                        // involve needing to allocate more
                                        // memory.  If this fails, we are going
                                        // to catch the exception.
    try
        {
        m_cszName = cszNewName;
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
// CEMailTarget::SetAddress
//
// Purpose:
//     Sets the address of the E-Mail recipient.
//
// Parameters:
//     LPCWSTR cszNewAddress - [IN] New address for this E-Mail.
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
BOOL CEMailTarget::SetAddress(
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
// CEMailTarget::ConvertToPacked
//
// Purpose:
//    This function will convert the internal data for this target
//    type into a packed format for a packed text file.
//
// Parameters:
//    CString *lpcPackedString - [IN][OUT] Address of a CString to fill
//                                     with the packed information.
//
// Returns:
//    BOOLEAN Success of the string packing.  Will return FALSE if
//    memory allocation fails while packing the string.
//
//    Also returns a filled out packed string through the
//    lpcPackedString parameter.  This string will have the data for
//    this target type packed together in a text format.
//
// See Also:
//    CAlertTarget class for virtual base functions.
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CEMailTarget::ConvertToPacked(
    CString *lpcPackedString
    )
{
BOOL bRetVal=TRUE;

                                        // String may already have contents
                                        // from base class.

                                        // Pack the Name of the EMail
                                        // recipient.
   if ( FALSE == PackParameter( lpcPackedString, m_cszName ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }

                                        // Pack the Address of the EMail
   if ( FALSE == PackParameter( lpcPackedString, m_cszAddress ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }

Cleanup:

   return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CEMailTarget::ConvertFromPacked
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
BOOL CEMailTarget::ConvertFromPacked(
    CString *lpcPackedString
    )
{
BOOL bRetVal=TRUE;
                                        // Extract our parameters from this
                                        // string.  This is for support
                                        // with Pre and Post processing.

                                        // Extract the Name of the mail
                                        // recipient.
    if ( FALSE == ExtractParameter( lpcPackedString, &m_cszName ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }

                                        // Extract the address of the mail.
    if ( FALSE == ExtractParameter( lpcPackedString, &m_cszAddress ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }

Cleanup:

    return bRetVal;
}
