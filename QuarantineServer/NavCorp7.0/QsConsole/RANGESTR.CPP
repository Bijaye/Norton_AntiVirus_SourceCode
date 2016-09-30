/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/RANGESTR.CPv   1.0   06 Mar 1998 11:16:24   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// Imlementations of CRangedString - Snaggled from the SpeedDisk NT code
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/RANGESTR.CPv  $
// 
//    Rev 1.0   06 Mar 1998 11:16:24   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "RangeStr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



////////////////////////////////////////////////////////////////////////////
// Function name	: CRangedString::SetRangedString
//
// Description	    : Constructs a ranged str.  
//
// Return type		: void 
//
// Argument         : DWORD xwSizeInBytes - number of bytes to format.
//
////////////////////////////////////////////////////////////////////////////
// 3/4/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CRangedString::SetRangedString( DWORD xwSizeInBytes )
{
    if( xwSizeInBytes < 1024 )
	{
        Format( IDS_BYTES_STR, xwSizeInBytes );
    }
    else
	{
        CString acsBracket[3];
        acsBracket[0].LoadString(IDS_KILOBYTES);
        acsBracket[1].LoadString(IDS_MEGABYTES);
        acsBracket[2].LoadString(IDS_GIGABYTES);
        int iBracket = 0;
        while( iBracket < 3 ) {
            if( xwSizeInBytes < 1024 * 1024 ) {
                break;
            }
            xwSizeInBytes /= 1024;
            iBracket++;
        }
        int iTemp = (int) xwSizeInBytes;     // Now that its been cut down to size we can do the rest in 32 bit math
        int iOnes = iTemp / 1024;
        if( iTemp < 10000 ) {
            int iTenths = (iTemp % 1024) / 100;
            int iHundredths = ((iTemp % 1024) % 100) / 10;
            Format( (LPCTSTR) _T("%d.%d%d %s"), iOnes, iTenths, iHundredths, (LPCTSTR) acsBracket[iBracket] );
        }
        else if( iTemp < 100000 ) {
            int iTenths = (iTemp % 1024) / 100;
            Format( (LPCTSTR) _T("%d.%d %s"), iOnes, iTenths, (LPCTSTR) acsBracket[iBracket] );
        }
        else {
            Format( (LPCTSTR) _T("%d %s"), iOnes, (LPCTSTR) acsBracket[iBracket] );
        }
    }
}
