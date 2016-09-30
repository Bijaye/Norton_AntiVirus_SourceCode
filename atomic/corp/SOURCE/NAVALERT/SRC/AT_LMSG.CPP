/////////////////////////////////////////////////////////////////////
// AT_LMSG.CPP - Created - 10/24/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVALERT/VCS/at_lmsg.cpv   1.1   22 May 1997 14:29:34   DSACKIN  $
//
// Description:
//    Implementation file for the CLocalMsgTarget class.  This class
//    is derived from the CAlertTarget base class.  It will contain
//    the data for a paging target opteration, as well as access
//    functions to retrieve and set data members.  Also, this class
//    will be able to pack all of it's data into a formatted
//    string to write to a file, as well as decode a packed string
//    that was sent to it.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVALERT/VCS/at_lmsg.cpv  $
// 
//    Rev 1.1   22 May 1997 14:29:34   DSACKIN
// Converted back to TCHAR.
//
//    Rev 1.0   18 Apr 1997 13:57:00   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:11:50   RFULLER
// Initial revision
//
//    Rev 1.3   04 Dec 1996 23:12:40   DSACKIN
// Changed all LPCTSTR instances to LPCWSTR for use with non-Unicode apps.
//
//    Rev 1.2   21 Nov 1996 16:31:36   ksackin
// Added functionality to call base class for it's options before adding each
// target's options.  This allows for pre and post processing.
//
//    Rev 1.1   12 Nov 1996 22:47:28   KSACKIN
// Modified classes to be MFC Extension classes.  This meant changing the
// exported functions back, and then using a macro in ihe class definition.
//
//    Rev 1.0   08 Nov 1996 15:33:56   KSACKIN
// Initial revision.
//
//    Rev 1.1   07 Nov 1996 01:05:12   KSACKIN
// Changed some string handling for UNICODE support, as well as modified some
// exported functions.
//
//    Rev 1.0   29 Oct 1996 15:33:08   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <afx.h>
#include "AT_TGT.H"


/////////////////////////////////////////////////////////////////////
// CLocalMsgTarget class.  (Derived from a CAlertTarget)


/////////////////////////////////////////////////////////////////////
// Dynamic Object Creation and Runtime Type Identifier creation.

IMPLEMENT_DYNCREATE(CLocalMsgTarget, CAlertTarget)


/////////////////////////////////////////////////////////////////////
// CLocalMsgTarget::CLocalMsgTarget
//
// Purpose:
//    Constructor for the CLocalMsgTarget class.  This function will
//    initialize any information specific to a LocalMsg Target.
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
CLocalMsgTarget::CLocalMsgTarget( void )
{
    SetTargetType( TARGET_MESSAGE_LOCAL );
}

/////////////////////////////////////////////////////////////////////
// CLocalMsgTarget::~CLocalMsgTarget
//
// Purpose:
//    Destructor for the CLocalMsgTarget class.  This function will
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
CLocalMsgTarget::~CLocalMsgTarget( void )
{
}

/////////////////////////////////////////////////////////////////////
// CLocalMsgTarget::SetAlertStatus
//
// Purpose:
//     Sets the alert status for the local machine.
//
// Parameters:
//     BOOL bNewAlertStatus-[IN] New alert status. This will be
//                               a BOOLEAN as to whether we
//                               should alert the local machine
//                               or not.
//
// Returns:
//     BOOLEAN as to whether we were able to assign the new alert
//     status or not.  If we cannot assign the alert status,
//     we need to fail gracefully.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CLocalMsgTarget::SetAlertStatus(
    BOOL bNewAlertStatus
    )
{
BOOL bRetVal = TRUE;

                                        // Set the new alert status.
    m_bAlertStatus = bNewAlertStatus;


                                        // For now, we really cannot fail, but
                                        // we are going to leave this here for
                                        // consistancy with all of the other
                                        // Set functions.
    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// CLocalMsgTarget::ConvertToPacked
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
BOOL CLocalMsgTarget::ConvertToPacked(
    CString *lpcPackedString
    )
{
TCHAR szTempString[10];
BOOL bRetVal=TRUE;

                                        // String may already have contents
                                        // from base class.

                                        // Convert the boolean value into a
                                        // temporary string.
   _itot( (int)m_bAlertStatus, szTempString, 10 );

                                        // Pack the Alert Status
                                        // temporary string.
   if ( FALSE == PackParameter( lpcPackedString, szTempString ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }

Cleanup:

   return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CLocalMsgTarget::ConvertFromPacked
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
BOOL CLocalMsgTarget::ConvertFromPacked(
    CString *lpcPackedString
    )
{
CString cszTempString;
BOOL bRetVal=TRUE;
                                        // Extract our parameters from this
                                        // string.  This is for support
                                        // with Pre and Post processing.

                                        // Extract the Alert Status.
    if ( FALSE == ExtractParameter( lpcPackedString, &cszTempString ) )
        {
        bRetVal = FALSE;
        goto Cleanup;
        }

                                        // Have the parameter as a string, so
                                        // convert the string back to a number
                                        // (and then a boolean)
    m_bAlertStatus = (BOOL) _ttoi( (LPCTSTR) cszTempString );


Cleanup:

    return bRetVal;
}
