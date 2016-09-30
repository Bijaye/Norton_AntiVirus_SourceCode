// Copyright 1997 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/NAVALERT/VCS/at_snmp.cpv   1.3   04 Jun 1997 20:35:46   DBUCHES  $
//
// Description:
//   Implements CSNMPTarget class - SNMP trap target.
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/NAVALERT/VCS/at_snmp.cpv  $
// 
//    Rev 1.3   04 Jun 1997 20:35:46   DBUCHES
// Updated ConvertToPacked() and ConvertFromPacked().  These functions
// now support serializing the SOCKADDR structure to NAVALERT.DAT.
// 
//    Rev 1.2   22 May 1997 14:30:14   DSACKIN
// Converted back to TCHAR
//
//    Rev 1.1   14 May 1997 15:32:32   GDZIECI
// Implemented GetAddress() and SetAddress() member functions.
//
//    Rev 1.0   09 May 1997 19:14:34   GDZIECI
// Initial revision.
//***************************************************************************

#include <afx.h>
#include "at_tgt.h"
#include "winsock.h"


/////////////////////////////////////////////////////////////////////
// Dynamic Object Creation and Runtime Type Identifier creation.

IMPLEMENT_DYNCREATE(CSNMPTarget, CAlertTarget)


//***************************************************************************
// CSNMPTarget::CSNMPTarget()
//
// Description:
//   CSNMPTarget class constructor.
//
// Parameters:
//   None
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 05/09/1997 GDZIECI Function Created.
//***************************************************************************

CSNMPTarget::CSNMPTarget()
{
    SetTargetType ( TARGET_SNMP );
}


//***************************************************************************
// CSNMPTarget::~CSNMPTarget()
//
// Description:
//   CSNMPTarget class destructor.
//
// Parameters:
//   None
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 05/09/1997 GDZIECI Function Created.
//***************************************************************************

CSNMPTarget::~CSNMPTarget()
{
}


/////////////////////////////////////////////////////////////////////
// CSNMPTarget::SetName
//
// Purpose:
//     Sets the name of the SNMP target.
//
// Parameters:
//     LPCWSTR cszNewName - [IN] New name for this SNMP target.
//
// Returns:
//     BOOLEAN as to whether we were able to assign that new name
//     or not.  If we cannot allocate memory for that name, then we
//     need to fail gracefully.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 05/09/97 - GDZIECI - Function Created (copied one of Ken's functions).
/////////////////////////////////////////////////////////////////////
BOOL CSNMPTarget::SetName (
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


//***************************************************************************
// CSNMPTarget::GetAddress()
//
// Description:
//   Retrieves the m_address member of the CSNMPTarget class. This is the
// address of the SNMP trap target stored in a SOCKADDR structure. It can be
// an IP or IPX address.
//
// Parameters:
//   LPSOCKADDR address     [in] target address
//
// Return Value:
//   TRUE if successful
//
// See Also:
//
//***************************************************************************
// 05/14/1997 GDZIECI Function Created.
//***************************************************************************

BOOL CSNMPTarget::GetAddress ( LPSOCKADDR address )
{
    memcpy ( address, &m_address, sizeof(SOCKADDR) );

    return TRUE;
}


//***************************************************************************
// CSNMPTarget::SetAddress()
//
// Description:
//   Sets the m_address member of the CSNMPTarget class. Target address is
// stored in a SOCKADDR structure. It can be an IP or IPX address. Port/socket
// field does not have to be set because the address is used to send an SNMP
// trap PDU, so the port/socket number is well known. The SNMP queue manager
// will set this field appropriately.
//
// Parameters:
//   LPSOCKADDR address     [in] target address
//
// Return Value:
//   TRUE if successful
//
// See Also:
//
//***************************************************************************
// 05/14/1997 GDZIECI Function Created.
//***************************************************************************

BOOL CSNMPTarget::SetAddress ( LPSOCKADDR address )
{
    memcpy ( &m_address, address, sizeof(SOCKADDR) );

    return TRUE;
}


/////////////////////////////////////////////////////////////////////
// CSNMPTarget::ConvertToPacked
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
// 05/09/97 - GDZIECI - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CSNMPTarget::ConvertToPacked ( CString *lpcPackedString )
{
    BOOL bRet;

    // String may already have contents from base class.
    // Pack the Name of the EMail
    bRet = PackParameter ( lpcPackedString, m_cszName );

    if (bRet)
    	{
        // Pack the family type
   		if( PackNumericParameter( lpcPackedString,
					    		m_address.sa_family ) == FALSE)
			{
			return FALSE;
			}
    	
        // Pack the data
		for(int i = 0; i < 14; i++)
            {
	   		if(PackNumericParameter( lpcPackedString, m_address.sa_data[i] ) == FALSE)
	   			return FALSE;
	   		}
    	}

    return bRet;
}


/////////////////////////////////////////////////////////////////////
// CSNMPTarget::ConvertFromPacked
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
// 05/09/96 - GDZIECI - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CSNMPTarget::ConvertFromPacked ( CString *lpcPackedString )
{
    BOOL bRet;
    DWORD dwExtractData;

    // Extract the Name of the SNMP target
    bRet = ExtractParameter ( lpcPackedString, &m_cszName );

    if (bRet)
    {
        // Extract the family type
		if( ExtractDWORDParameter( lpcPackedString, &dwExtractData ) == FALSE)
			return FALSE;
		m_address.sa_family = (u_short) dwExtractData;	

        // Extract the data
		for(int i = 0; i < 14; i++)
			{
			if( ExtractDWORDParameter( lpcPackedString, &dwExtractData ) == FALSE)
				return FALSE;
            m_address.sa_data[i] = (char) dwExtractData;		
			}
    }


    return bRet;
}
