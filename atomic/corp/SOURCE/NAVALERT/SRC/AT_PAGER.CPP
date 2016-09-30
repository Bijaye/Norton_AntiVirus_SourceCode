/////////////////////////////////////////////////////////////////////
// AT_PAGER.CPP - Created - 10/24/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVALERT/VCS/at_pager.cpv   1.1   22 May 1997 14:30:08   DSACKIN  $
//
// Description:
//    Implementation file for the CPagerTarget class.  This class is
//    derived from the CAlertTarget base class.  It will contain
//    the data for a paging target opteration, as well as access
//    functions to retrieve and set data members.  Also, this class
//    will be able to pack all of it's data into a formatted
//    string to write to a file, as well as decode a packed string
//    that was sent to it.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVALERT/VCS/at_pager.cpv  $
// 
//    Rev 1.1   22 May 1997 14:30:08   DSACKIN
// Converted back to TCHAR
//
//    Rev 1.0   18 Apr 1997 13:57:06   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:11:50   RFULLER
// Initial revision
//
//    Rev 1.3   04 Dec 1996 23:13:30   DSACKIN
// Changed all instances of LPCTSTR to LPCWSTR for use with non-UNICODE apps.
//
//    Rev 1.2   21 Nov 1996 16:31:28   ksackin
// Added functionality to call base class for it's options before adding each
// target's options.  This allows for pre and post processing.
//
//    Rev 1.1   12 Nov 1996 22:47:30   KSACKIN
// Modified classes to be MFC Extension classes.  This meant changing the
// exported functions back, and then using a macro in ihe class definition.
//
//    Rev 1.0   08 Nov 1996 15:34:00   KSACKIN
// Initial revision.
//
//    Rev 1.1   07 Nov 1996 01:05:18   KSACKIN
// Changed some string handling for UNICODE support, as well as modified some
// exported functions.
//
//    Rev 1.0   29 Oct 1996 15:33:12   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <afx.h>
#include "AT_TGT.H"


/////////////////////////////////////////////////////////////////////
// CPagerTarget class.  (Derived from a CAlertTarget)


/////////////////////////////////////////////////////////////////////
// Dynamic Object Creation and Runtime Type Identifier creation.

IMPLEMENT_DYNCREATE(CPagerTarget, CAlertTarget)


/////////////////////////////////////////////////////////////////////
// CPagerTarget::CPagerTarget
//
// Purpose:
//    Constructor for the CPagerTarget class.  This function will
//    initialize any information specific to a Pager Target.
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
CPagerTarget::CPagerTarget( void )
{
    SetTargetType( TARGET_PAGER );
}

/////////////////////////////////////////////////////////////////////
// CPagerTarget::~CPagerTarget
//
// Purpose:
//    Destructor for the CPagerTarget class.  This function will
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
CPagerTarget::~CPagerTarget( void )
{
}

/////////////////////////////////////////////////////////////////////
// CPagerTarget::SetPhoneNumber
//
// Purpose:
//     Sets the phone number of the page recipient.
//
// Parameters:
//     LPCWSTR cszNewPhoneNumber-[IN] New phone number for this page.
//
// Returns:
//     BOOLEAN as to whether we were able to assign the new number
//     or not.  If we cannot allocate memory for that number, then
//     we need to fail gracefully.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CPagerTarget::SetPhoneNumber(
    LPCTSTR cszNewPhoneNumber
    )
{
BOOL bRetVal = TRUE;

                                        // Try to set the new number, which
                                        // may involve needing to allocate more
                                        // memory.  If this fails, we are going
                                        // to catch the exception.
    try
        {
        m_cszPhoneNumber = cszNewPhoneNumber;
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
// CPagerTarget::SetMessage
//
// Purpose:
//     Sets the message for this page recipient.
//
// Parameters:
//     LPCWSTR cszNewMessage - [IN] New message for this page.
//
// Returns:
//     BOOLEAN as to whether we were able to assign the new message
//     or not.  If we cannot allocate memory for that message, then
//     we need to fail gracefully.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CPagerTarget::SetMessage(
    LPCTSTR cszNewMessage
    )
{
BOOL bRetVal = TRUE;

                                        // Try to set the new message, which
                                        // may involve needing to allocate more
                                        // memory.  If this fails, we are going
                                        // to catch the exception.
    try
        {
        m_cszMessage = cszNewMessage;
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
// CPagerTarget::ConvertToPacked
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
BOOL CPagerTarget::ConvertToPacked(
    CString *lpcPackedString
    )
{
BOOL bRetVal=TRUE;

                                        // String may already have contents
                                        // from base class.

                                        // Pack the Phone Number.
   if ( FALSE == PackParameter( lpcPackedString, m_cszPhoneNumber ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }

                                        // Pack the Message.
   if ( FALSE == PackParameter( lpcPackedString, m_cszMessage ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }

Cleanup:

   return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CPagerTarget::ConvertFromPacked
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
BOOL CPagerTarget::ConvertFromPacked(
    CString *lpcPackedString
    )
{
BOOL bRetVal=TRUE;
                                        // Extract our parameters from this
                                        // string.  This is for support
                                        // with Pre and Post processing.

                                        // Extract the Phone Number.
    if ( FALSE == ExtractParameter( lpcPackedString, &m_cszPhoneNumber ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }

                                        // Extract the message for this Page.
    if ( FALSE == ExtractParameter( lpcPackedString, &m_cszMessage ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }

Cleanup:

    return bRetVal;
}
