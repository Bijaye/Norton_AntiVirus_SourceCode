// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2004, 2005, Symantec Corporation, All rights reserved.
// ParseCAD.cpp

#include "stdafx.h"
#include "ParseCAD.h"
#include <msiquery.h>
#include <tchar.h>
#include <stdio.h>
#include "SymSaferStrings.h"

#define array_sizeof(x) (sizeof(x) / sizeof(x[0]))
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))

static void ParseCAD_LogMessage( MSIHANDLE hInstall, LPCTSTR szString )
{
	MSIHANDLE hRec = MsiCreateRecord(1);

	if( hRec != NULL )
	{
		// Use OutputDebugString...
		OutputDebugString(szString);

		// ...as well as the log file.
		MsiRecordClearData(hRec);
		MsiRecordSetString(hRec, 0, szString);
		MsiProcessMessage(hInstall, INSTALLMESSAGE_INFO, hRec);
		MsiCloseHandle(hRec);
	}
}

///////////////////////////////////////////////////////////////////////////
//  Name:
//		ParseCAD
//  Purpose:
//		Parse the comma delimeted CustomActionData.
//
//  Return Values:
//
//  08/24/2004 Daniel Kowalyshyn - Function created.
//  09/01/2004 Rich Sutton       - Added var arg capability, moved to its own source file.
///////////////////////////////////////////////////////////////////////////
DWORD ParseCAD( MSIHANDLE hInstall, LPTSTR multipath, UINT count, ... )
{
	TCHAR	szMessString[DOUBLE_MAX_PATH]	= {0};
	TCHAR	*pdeststr			= NULL;
    TCHAR   *iterator           = multipath;
	UINT    commalocation       = 0;
    DWORD   actualcount         = 0;

	#ifdef _DEBUG
	sssnprintf( szMessString, sizeof(szMessString), "ParseCAD: comma-delimited CustomActionData %s", multipath );
	ParseCAD_LogMessage( hInstall, szMessString );			
	#endif

    va_list marker;
    va_start( marker, count );

    for( UINT n = 0; n < count; ++n )
    {
        TCHAR* strDest = va_arg( marker, TCHAR* );
        DWORD ulDestLen = va_arg( marker, DWORD );

        if( _tcslen( iterator ) == 0 )
        {
            // We are at the end of the string.

            *strDest = 0;
// ParseCAD_LogMessage commented out, otherwise it exposes the ServerPassword in the log file, only use in testing!
//	        _sntprintf( szMessString, array_sizeof(szMessString), "ParseCAD: string %lu: <no string>", n );
//	        ParseCAD_LogMessage( hInstall, szMessString );
        }
        else
        {
			pdeststr = _tcschr ( iterator, '\t' );            // find the location of the [TAB] '\t' delimiter

            if( pdeststr == NULL )
            {
                // No more delimiters, copy the last bit.

                _tcsncpy( strDest, iterator, ulDestLen );   // copy data to the caller's buffer

                strDest[ ulDestLen - 1 ] = 0;               // ensure NULL term

//	            _sntprintf( szMessString, array_sizeof(szMessString), "ParseCAD: string %lu: (len %lu) %s", n, _tcslen(iterator), strDest );
//	            ParseCAD_LogMessage( hInstall, szMessString );

                iterator += _tcslen( iterator );
            }
            else
            {
                // Copy the string to the caller's buffer.

	            commalocation = pdeststr - iterator;        // calc the offset of the comma location in the string

                _tcsncpy( strDest, iterator, MIN(ulDestLen, commalocation) ); // copy data to the caller's buffer

                strDest[ ulDestLen - 1 ] = 0;               // ensure NULL term

//	            _sntprintf( szMessString, array_sizeof(szMessString), "ParseCAD: string %lu: (len %lu) %s", n, commalocation, strDest );
//	            ParseCAD_LogMessage( hInstall, szMessString );

                iterator = pdeststr;
                iterator++;                                 // move past the comma
            }

            actualcount++;
        }
    }

    va_end( marker );

	return( actualcount );
}

