// NumberString.cpp: implementation of the CNumberString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NumberString.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CNumberString::CNumberString
//
// Description  : 
//
// Return type  : 
//
// Argument     : DWORD dwNumber
//
///////////////////////////////////////////////////////////////////////////////
// 3/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CNumberString::CNumberString( DWORD dwNumber )
{
    // 
    // Format the string
    // 
    SetNumber( dwNumber );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CNumberString::~CNumberString
//
// Description  : 
//
// Return type  : 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CNumberString::~CNumberString()
{

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CNumberString::SetNumber
//
// Description  : 
//
// Return type  : void 
//
// Argument     : DWORD dwNumber
//
///////////////////////////////////////////////////////////////////////////////
// 3/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CNumberString::SetNumber( DWORD dwNumber )
{
    // 
    // Save off number.
    // 
    m_dwNumber = dwNumber;

    // 
    // Reformat number
    // 
    FormatNumber();
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CNumberString::FormatNumber
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CNumberString::FormatNumber()
{
    // 
    // Dump existing stuff
    //
    Empty(); 
    
    // 
    // Check to see if there is work for us to do.
    // 
    if( m_dwNumber < 1000 )
		{
        Format( _T("%d"), m_dwNumber );
		return;
		}
    
    // 
    // Build our string.
    // 
    DWORD dwDivisor = 1000000000;
    DWORD dwDivide;
    CString s;
    do
        {
        dwDivide = m_dwNumber / dwDivisor;
        if( dwDivide )
            {
            if( GetLength() )
                s.Format( _T("%03d,"), dwDivide );
            else
                s.Format( _T("%d,"), dwDivide );
            *this += s;
            }
        dwDivisor /= 1000;

        if( dwDivisor == 1 )
            {
            s.Format( _T("%03d"), m_dwNumber % 1000 );
            *this += s;
            }
        }
    while( dwDivisor != 1 );
}

