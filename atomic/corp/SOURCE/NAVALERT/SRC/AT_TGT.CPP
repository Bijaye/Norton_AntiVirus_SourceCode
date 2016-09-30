/////////////////////////////////////////////////////////////////////
// AT_TGT.CPP - Created - 10/23/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVALERT/VCS/at_tgt.cpv   1.1   22 May 1997 14:30:16   DSACKIN  $
//
// Description:
//    Implementation file for all of the target types available for
//    alerting.  This file will also contain the functions for the
//    base CAlertTarget type that all of the specific Alert Target
//    types will be derived from.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVALERT/VCS/at_tgt.cpv  $
// 
//    Rev 1.1   22 May 1997 14:30:16   DSACKIN
// Converted back to TCHAR
//
//    Rev 1.0   18 Apr 1997 13:57:16   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:11:52   RFULLER
// Initial revision
//
//    Rev 1.4   20 Dec 1996 17:48:28   ksackin
// Added a member varible to the CNetworkMessage class of a dwResourceType.
// Also added access functions to get and set this variable.
//
//    Rev 1.3   04 Dec 1996 23:13:32   DSACKIN
// Changed all instances of LPCTSTR to LPCTSTR for use with non-UNICODE apps.
//
//    Rev 1.2   21 Nov 1996 16:31:40   ksackin
// Added functionality to call base class for it's options before adding each
// target's options.  This allows for pre and post processing.
//
//    Rev 1.1   12 Nov 1996 22:47:38   KSACKIN
// Modified classes to be MFC Extension classes.  This meant changing the
// exported functions back, and then using a macro in ihe class definition.
//
//    Rev 1.0   08 Nov 1996 15:34:04   KSACKIN
// Initial revision.
//
//    Rev 1.1   07 Nov 1996 01:05:26   KSACKIN
// Changed some string handling for UNICODE support, as well as modified some
// exported functions.
//
//    Rev 1.0   29 Oct 1996 15:33:18   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "afxwin.h"                     // Standard AFX include for windows.
#include "AT_Opts.h"                    // Main header file for options.
#include "AT_Str.h"                     // String resources.
#include "dllrcsta.h"                   // Resource state class.


/////////////////////////////////////////////////////////////////////
// CAlertTarget (Base class for all targets)


/////////////////////////////////////////////////////////////////////
// Dynamic Object Creation and Runtime Type Identifier creation.

IMPLEMENT_DYNCREATE(CAlertTarget, CObject)


/////////////////////////////////////////////////////////////////////
// CAlertTarget::CAlertTarget
//
// Purpose:
//    Contructor for an Alert Target base class.
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
CAlertTarget::CAlertTarget( void )
{
                                        // Set default Active State.
    m_bActive = 1;

                                        // Set default priority.
    m_nPriority = 2;
}

/////////////////////////////////////////////////////////////////////
// CAlertTarget::~CAlertTarget
//
// Purpose:
//    Destructor for an Alert Target base class.
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
CAlertTarget::~CAlertTarget( void )
{
}

/////////////////////////////////////////////////////////////////////
// CAlertTarget::ConvertCommonToPacked
//
// Purpose:
//    This function will take care of converting the common
//    functionality for a CAlertTarget base class into a packed
//    string for outputing to a file.
//
// Parameters:
//    CString *lpcPackedString - [OUT] String to pack with params.
//
// Returns:
//    TRUE  - Function was successfull.
//    FALSE - An unexpected error occurred.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertTarget::ConvertCommonToPacked(
    CString *lpcPackedString  // = NULL
    )
{
BOOL bRetVal=FALSE;
TCHAR szTempStr[20];

                                        // Bail if we were sent a NULL pointer
                                        // as our string.  Return FALSE.
    if ( NULL == lpcPackedString )
        {
        return FALSE;
        }

                                        // Empty the contents of the
                                        // destination string.
   lpcPackedString->Empty();

                                        // Pack target active state.
   if ( FALSE == PackParameter( lpcPackedString, _itot( (int)m_bActive, szTempStr, 10 ) ) )
        {
        goto Cleanup;
        }

                                        // Pack target priority.
   if ( FALSE == PackParameter( lpcPackedString, _itot( m_nPriority, szTempStr, 10 ) ) )
        {
        goto Cleanup;
        }
                                        // IF all packing went well, return
                                        // TRUE, otherwise, our return value
                                        // will be false.
   bRetVal = TRUE;


Cleanup:

   return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CAlertTarget::ConvertCommonFromPacked
//
// Purpose:
//    This function will take care of converting the common
//    functionality for a CAlertTarget base class from a packed
//    string into our data members.
//
// Parameters:
//    CString *lpcPackedString - [IN][OUT]  String to unpack.  We
//                                 will actually extract our parameters
//                                 from this list so that each of our
//                                 derived classes will only have the
//                                 options that they need.
//
// Returns:
//
// See Also:
//    TRUE  - Function was successfull.
//    FALSE - An unexpected error occurred (probably in converting).
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertTarget::ConvertCommonFromPacked(
    CString *lpcPackedString   // = NULL
    )
{
CString cszParameter;
BOOL bRetVal = FALSE;

                                        // Extract the active state of this
                                        // object.
    if ( FALSE == ExtractParameter( lpcPackedString, &cszParameter ) )
        {
        goto Cleanup;
        }
                                        // Convert this priority to a BOOL,
                                        // and into our member variable.
    m_bActive = (BOOL)_ttoi( cszParameter );

                                        // Extract the priority of this
                                        // object.
    if ( FALSE == ExtractParameter( lpcPackedString, &cszParameter ) )
        {
        goto Cleanup;
        }
                                        // Convert this priority to an integer,
                                        // and into our member variable.
    m_nPriority = _ttoi( cszParameter );

                                        // Return TRUE if no errors occurred up
                                        // to this point.
    bRetVal = TRUE;


Cleanup:

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// CAlertTarget::ConvertToPacked
//
// Purpose:
//    Virtual function, meant to be overridden.  If the default values
//    are called by this, then a FALSE is returned. (No derived class)
//    This is so that we can implement run time type information.
//
// Parameters:
//    CString *lpcPackedString - [OUT] String to pack with params.
//                                     (default NULL).
//
// Returns:
//    BOOLEAN as to whether the function was successfull or not.  If
//    the default of NULL was passed, we assume that either this was
//    in error, or the base virtual class was called.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertTarget::ConvertToPacked(
    CString *lpcPackedString  // = NULL
    )
{
    return FALSE;
}

/////////////////////////////////////////////////////////////////////
// CAlertTarget::ConvertFromPacked
//
// Purpose:
//    Virtual function, meant to be overridden.  If the default values
//    are called by this, then a FALSE is returned. (No derived class)
//    This is so that we can implement run time type information.
//
// Parameters:
//    CString *lpcPackedString - [IN][OUT] String to extract parameters
//                                   from for this structures information.
//  //
// Returns:
//    BOOLEAN as to whether the function was successfull or not.  If
//    the default of NULL was passed, we assume that either this was
//    in error, or the base virtual class was called.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertTarget::ConvertFromPacked(
    CString *lpcPackedString   // = NULL
    )
{
    return FALSE;
}


/////////////////////////////////////////////////////////////////////
// CAlertTarget::SetPriority
//
// Purpose:
//    The function will set the priority for a particular target.  It
//    will also check to make sure that it is a valid priority.
//
// Parameters:
//    int nNewPriority - [IN] New priority for this target.
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CAlertTarget::SetPriority(
    int nNewPriority
    )
{
                                        // Set new priority.
    m_nPriority = nNewPriority;
}

/////////////////////////////////////////////////////////////////////
// CAlertTarget::SetActive
//
// Purpose:
//    The function will set the Active state flag for a particular
//    target.
//
// Parameters:
//    BOOL bNewActiveState - [IN] New Active state for this target.
//
// Returns:
//    None.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/21/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CAlertTarget::SetActive(
    BOOL bNewActiveState
    )
{
                                        // Set Active State.
    m_bActive = bNewActiveState;
}



/////////////////////////////////////////////////////////////////////
// Parameter Packing and UnPacking Functions.



/////////////////////////////////////////////////////////////////////
// PackParameter
//
// Purpose:
//    This function will pack a parameter sent onto a current string
//    of packed parameters.
//
// Parameters:
//    CString *lpcPackedString  - [IN] [OUT] String with currently
//                                     packed parameters.  This string
//                                     will be concatenated with a
//                                     seperating character, and the
//                                     parameter to pack on.
//    LPCTSTR cszParameterToAdd - [IN] Parameter to add onto the end
//                                     of the currently packed string.
//
// Returns:
//    BOOLEAN as to whether the packing was successful or not.  A
//    FALSE return could be either a memory expantion failure for the
//    packed string or a string table read failure.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/24/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL PackParameter(
    CString *lpcPackedString,
    LPCTSTR cszParameterToAdd
    )
{
BOOL bRetVal=TRUE;
CString cszParameterSeparator;

                                        // Setup the module state so that
                                        // we may load strings from the correct
                                        // resource using the
                                        // CString::LoadString() member function.
    CDLLResourceState resourceSwitch;

                                        // Catch a memory exception if the
                                        // packed string has to expand.
    try
        {
        if ( 0 == lpcPackedString->GetLength() )
            {
                                        // If string has no length, this is the
                                        // first parameter to be packed.
            *lpcPackedString = cszParameterToAdd;
            }
        else
            {                           // Load the parameter separator.
            if ( TRUE == cszParameterSeparator.LoadString( IDS_PARAMETER_SEPARATOR ) )
                {
                                        // Add both the separator, and the
                                        // parameter.
                *lpcPackedString += cszParameterSeparator;
                *lpcPackedString += cszParameterToAdd;
                }
            }
        }
    catch( CMemoryException )
        {
                                        // If a memory exception occured,
                                        // return FALSE for packing this
                                        // parameter.
        bRetVal = FALSE;
        }


    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// PackNumericParameter
//
// Purpose:
//    This function will pack a numerical parameter sent onto a
//    current string of packed parameters.
//
// Parameters:
//    CString *lpcPackedString  - [IN] [OUT] String with currently
//                                     packed parameters.  This string
//                                     will be concatenated with a
//                                     seperating character, and the
//                                     parameter to pack on.
//    DWORD    dwParameterToAdd - [IN] Numerical parameter to add onto
//                                     the end of the currently packed
//                                     string.
//
// Returns:
//    BOOLEAN as to whether the packing was successful or not.  A
//    FALSE return could be either a memory expantion failure for the
//    packed string or a string table read failure.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/20/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL PackNumericParameter(
    CString *lpcPackedString,
    DWORD    dwParameterToAdd
    )
{
BOOL    bRetVal=TRUE;
CString cszParameterString;

                                        // Convert the numerical value to a
                                        // string.
    cszParameterString.Format( _T("%lu"), dwParameterToAdd );

                                        // Call through to the pack parameter
                                        // function after we have converted the
                                        // value of this parameter.
    bRetVal = PackParameter( lpcPackedString, cszParameterString );

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// ExtractParameter
//
// Purpose:
//    Function to extract a parameter from a CString parameter list.
//    To accomplish this, we are going to load up the parameter
//    separation character, search for it, and then chop it, and the
//    preceding parameter off of the packed parameter list.
//    We will return NULL if there is no parameter left in the
//    packed parameter string.
//
// Parameters:
//    CString *lpcPackedString - [IN] [OUT] String containing a list
//                                     of packed parameters.  This
//                                     string will have the first
//                                     parameter stripped from it when
//                                     it get's returned.
//    CString *lpcParameter    - [OUT] This string will be initialized
//                                     with the parameter taken from
//                                     the packed string.  If an error
//                                     occurs, this string will be
//                                     emptied.
//
// Returns:
//    BOOLEAN value as to whether the extraction was successful or
//    not.  This function will FAIL if either the packed string
//    is empty, or it cannot allocate enough memory for the
//    receiving CString object to receive the parameter.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/24/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL ExtractParameter(
    CString *lpcPackedString,
    CString *lpcParameter
    )
{
BOOL bRetVal=TRUE;
CString cszParameterSeparator;
int nParamPos;

                                        // Setup the module state so that
                                        // we may load strings from the correct
                                        // resource using the
                                        // CString::LoadString() member function.
    CDLLResourceState resourceSwitch;

                                        // Trim the packed string to check for
                                        // an empty string.
    lpcPackedString->TrimRight();
    lpcPackedString->TrimLeft();

                                        // Verify that the list is not empty.
    if ( lpcPackedString->GetLength() )
        {
                                        // Empty the returning parameter.
        lpcParameter->Empty();


                                        // Catch a memory exception if the
                                        // packed string has to expand.
        try
            {
                                        // Load the parameter separator.
            if ( cszParameterSeparator.LoadString( IDS_PARAMETER_SEPARATOR ) )
                {
                                        // Search for our separator.
                nParamPos = lpcPackedString->Find( cszParameterSeparator );

                                        // -1 will be returned if search failed.
                if ( -1 != nParamPos )
                    {
                                        // Extract the parameter off of the
                                        // front of the packed parameter list.
                    *lpcParameter = lpcPackedString->Left( nParamPos );

                                        // Trim right side of string.  The left
                                        // side was already trimmed above.
                    lpcParameter->TrimRight();

                                        // Chop off the first parameter, as
                                        // well as the separator.
                    *lpcPackedString = lpcPackedString->Mid( nParamPos + cszParameterSeparator.GetLength() );
                    }
                else
                    {

                    if ( 0 != lpcPackedString->GetLength() )
                        {
                                        // If the list still has a length, copy
                                        // the whole list to the parameter.
                                        // (Last entry)
                        *lpcParameter = *lpcPackedString;

                                        // Then we need to empty out the packed
                                        // string.
                        lpcPackedString->Empty();
                        }
                    else
                        {
                                        // String is empty.
                        bRetVal = FALSE;
                        }
                    }
                }
            }
        catch( CMemoryException )
            {
                                        // If a memory exception occured,
                                        // return FALSE for packing this
                                        // parameter.
            bRetVal = FALSE;
            }
        }
    else
        {
                                        // The list was empty, so we return
                                        // FALSE that we did not get a
                                        // parameter.
        bRetVal = FALSE;
        }


    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// ExtractDWRODParameter
//
// Purpose:
//    Function to extract a DWORD parameter from a CString parameter
//    list.  To accomplish this, we are going to call through to the
//    extract parameter function, and then convert the return to a
//    DWORD to send back to the calling function.
//
// Parameters:
//    CString *lpcPackedString - [IN] [OUT] String containing a list
//                                     of packed parameters.  This
//                                     string will have the first
//                                     parameter stripped from it when
//                                     it get's returned.
//    DWORD   *lpdwParameter   - [OUT] Parameter that is to be
//                                     returned from the extraction
//                                     function.
//
// Returns:
//
//    BOOLEAN value as to whether the extraction was successful or
//    not.  This function will FAIL if either the packed string
//    is empty, or it cannot allocate enough memory for the
//    receiving CString object to receive the parameter.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/20/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL ExtractDWORDParameter(
    CString    *lpcPackedString,
    DWORD      *lpdwParameter
    )
{
BOOL bRetVal=TRUE;
CString cszExtractedParameter;

    bRetVal = ExtractParameter( lpcPackedString, &cszExtractedParameter );

                                        // If we were able to extract a
                                        // parameter successfully, then we are
                                        // going to convert it to this storage
                                        // type and return the return value
                                        // from ExtractParameter().
    if ( TRUE == bRetVal )
        {
                                        // Convert the parameter back to a
                                        // DWORD, and send it back.
        *lpdwParameter = (DWORD)_ttol( cszExtractedParameter );
        }

    return bRetVal;
}
