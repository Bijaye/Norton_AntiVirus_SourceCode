// ExchangeTest.cpp : Defines the entry point for the console application.
//

#include <stdafx.h>
#include "ExchangeServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

void usage()
{
	_tprintf( _T("ExchangeTest [save] - Detect Exchange Server assets to exclude\n" ) );
}

/******************************************************************************/
int _tmain( int argc, TCHAR* argv[], TCHAR* /*envp[]*/ )
{
	bool bSave = false;

	if( argc > 2 )
	{
		usage();
		return 1;
	}

	if( 2 == argc )
	{
		_tcsrev( argv[1] );

		if( !_tcsnicmp( argv[1], _T("evas"), 4 ) )
		{
			bSave = true;
		}
		else
		{
			usage();
			return 1;
		}
	}

	ExcludeExchangeServer( bSave );

	if( !bSave )
	{
		_tprintf( _T("\nDETECTION MODE ONLY!\n" ) );
		_tprintf( _T("NO MODIFICATION MADE TO THIS MACHINE!\n" ) );
	}

	return 0;
}
