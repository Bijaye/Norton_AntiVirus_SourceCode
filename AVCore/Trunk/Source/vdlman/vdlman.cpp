
#include "platform.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef SYM_UNIX
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <utime.h>
    #include <unistd.h>
    #define _MAX_PATH    1024
#endif

#include "navvdl.h"
#include "vdlman.h"



int main( int argc, char* argv[] )
{

	DWORD	dwResult = 0;

    if( argc < 2 )
    {
        printf("ERROR: No parameter passed in!\n");
        help();
        return( 1 );
    }


    switch( argv[1][1] )
    {

		///////////////////////////////////////////////////
		// Simple Install
		///////////////////////////////////////////////////
		case 't':
		case 'T':
            if( argc == 4 )
            {
                dwResult = NavSimpleDefInstall( argv[2], argv[3] );
                if( dwResult != 0 )
				{
					printf("ERROR: %s\n", VDLReturnValues[dwResult] );
				}
            }else
            {
                help();
                return( 1 );
            }
			break;


		///////////////////////////////////////////////////
		// Simple Uninstall
		///////////////////////////////////////////////////
		case 'p':
		case 'P':
            if( argc == 3 )
            {
                NavSimpleDefUninstall( argv[2] );
            }else
            {
                help();
                return( 1 );
            }
			break;


        ///////////////////////////////////////////////////
		// Standard Install
		///////////////////////////////////////////////////
		case 'i':
        case 'I':
            if( argc == 4 )
            {
                dwResult = NavStandardDefsInstall( argv[2], argv[3] );
                if( dwResult != 0 )
				{
					printf("ERROR: %s\n", VDLReturnValues[dwResult] );
				}
            }else
            {
                help();
                return( 1 );
            }
            break;


		///////////////////////////////////////////////////
		// Standard Definitions Update
		///////////////////////////////////////////////////
        case 'u':
        case 'U':
            if( argc == 3 )
            {
                dwResult = NavStandardUpdateDefs( argv[2] );
				if( dwResult != 0 )
				{
					printf("ERROR: %s\n", VDLReturnValues[dwResult] );
				}

                
            }else
            {
                help();
                return( 1 );
            }
            break;


		///////////////////////////////////////////////////
		// Live Update 
		///////////////////////////////////////////////////
        case 'l':
        case 'L':
            if( argc == 4 )
            {
                //LiveUpdate( argv[2], argv[3] );
                
            }else
            {
                help();
                return( 1 );
            }
            break;


		///////////////////////////////////////////////////
		// Standard Uninstall
		///////////////////////////////////////////////////
        case 'x':
        case 'X':
            if( argc == 3 )
            {
                dwResult = NavStandardDefsUninstall( argv[2] );
                if( dwResult != 0 )
				{
					printf("ERROR: %s\n", VDLReturnValues[dwResult] );
				}

            }else
            {
                help();
                return( 1 );
            }
            break;
            
    }
    
	printf("Done!\n");
	return( 0 );
}


//********************************************************************
//
// Function:
//  VOID help( void )
//
// Parameters:
//	NONE
//	
// Description:
//	Prints out command line parameters
//
// Returns:
//	NONE
//
//*******************************************************************
void help( void )
{
	printf("\n");
#ifdef DEBUG
	printf("[Debug Build]\n");
#endif
	printf("Virus Definition Library Manager Version 1.0.1\n");
	printf("Usage: VDLMAN <option>\n");
	printf("Simple Options:\n");
	printf("    -t <APPID> <PATH>	Install defs for <APPID> from directory <PATH>\n");
	printf("    -p <APPID>          Update defs for <APPID>\n");
	printf("\nStandard Options:\n");
	printf("    -i <APPID> <PATH>   Install defs for <APPID> from directory <PATH>\n");
	printf("    -u <APPID>          Update defs for <APPID>\n");
    printf("    -l <APPID> <ARGS>   LiveUpdate for <APPID> with arguments <ARGS>\n");
	printf("    -x <APPID>          Unistall Defs for <APPID>\n");
	printf("    -h                  Help screen\n");
	printf("\n");

}

//********************************************************************
//
// Function:
//  WORD NavStandardDefsInstall()
//
// Parameters:
//	LPSTR lpszAppID
//	LPSTR lpVirusDefDir
//
//
// Description:
//  
//
// Returns:
//
//********************************************************************
WORD NavStandardDefsInstall
( 
	LPSTR  lpszAppID,
	LPSTR  lpVirusDefDir
)
{
	HVIRUSDEF	hVirusDef;
	char		szTempDir[_MAX_PATH];
	BOOL		bDefsChanged = FALSE;
	char		szNewestDefsPath[_MAX_PATH];

	///////////////////////////////////////////////////////
	// Initialize definitions engine
	///////////////////////////////////////////////////////
	printf("Initializing Virus Defintions Engine...\n");
    if ( !VirusDefInit( &hVirusDef ) )
    {
		VirusDefClose( hVirusDef );
		return ( VDL_INIT_ERROR );
    }


	///////////////////////////////////////////////////////
	// Register an AppID
	///////////////////////////////////////////////////////
	printf("Registering AppID...\n");
	if( !VirusDefInitInstallApp( hVirusDef,
		                         lpszAppID,
								 NULL,
								 NULL ) )
	{
		VirusDefClose( hVirusDef );
		return( VDL_INIT_INSTALL_ERROR );
	}

	///////////////////////////////////////////////////////
	// Pre Definition Update
	///////////////////////////////////////////////////////
	if( !VirusDefPreUpdate( hVirusDef,
		                    (LPSTR)&szTempDir,
							sizeof(szTempDir),
							VirusDefInstallScript) )
	{
		VirusDefClose( hVirusDef );
		return( VDL_PREUPDATE_FAILED );
	}


	///////////////////////////////////////////////////////
	// Copy virus defintions to TMP directory
	///////////////////////////////////////////////////////
	if( !VirusDefCopyDefs( hVirusDef,
	                       lpVirusDefDir,
	                       szTempDir ) )
	{
		VirusDefClose( hVirusDef );
		return( VDL_ERROR_COPYING_FILES );
	}


	///////////////////////////////////////////////////////
	// Post Definition Update
	///////////////////////////////////////////////////////
	if( !VirusDefPostUpdate( hVirusDef ) )
	{
		VirusDefClose( hVirusDef );
		return( VDL_POSTUPDATE_FAILED );
	}


	///////////////////////////////////////////////////////
	// Use newset definitions
	///////////////////////////////////////////////////////
	if( !VirusDefUseNewest( hVirusDef,
		                    (LPSTR)&szNewestDefsPath,
							_MAX_PATH,
							&bDefsChanged) )
	{
		VirusDefClose( hVirusDef );
		return( VDL_USE_NEWEST_DEFS_ERROR );
	}else
	{
		printf("Newest definitions are: %s\n", szNewestDefsPath );
		if(bDefsChanged)
		{
			printf("The current definitions have changed!\n");
		}else
		{
			printf("The current definitions have NOT changed!\n");
		}
	}


	///////////////////////////////////////////////////////
	// Close Definition Engine
	///////////////////////////////////////////////////////
	VirusDefClose( hVirusDef );


	return( VDL_STATUS_OK );
}


//********************************************************************
//
// Function:
//  WORD NavStandardDefsUninstall()
//
// Parameters:
//	LPSTR lpszAppID
//
// Description:
//  
//
// Returns:
//
//********************************************************************
WORD NavStandardUpdateDefs
(
	LPSTR  lpszAppID
)
{
	
	HVIRUSDEF	hVirusDef;
	char		szCurrDefsPath[_MAX_PATH];
	char		szNewestDefsPath[_MAX_PATH];
	BOOL		bDefsChanged = FALSE;
	WORD		wYear	= 0;
	WORD		wMonth	= 0;
	WORD		wDay	= 0;
	DWORD		dwRev	= 0;

	///////////////////////////////////////////////////////
	// Initialize definitions engine
	///////////////////////////////////////////////////////
	printf("Initializing Virus Defintions Engine...\n");
    if ( !VirusDefInit( &hVirusDef ) )
    {
		VirusDefClose( hVirusDef );
		return ( VDL_INIT_ERROR );
    }

	
	///////////////////////////////////////////////////////
	// Register an AppID
	///////////////////////////////////////////////////////
	printf("Registering AppID...\n");
	if( !VirusDefInitWindowsApp( hVirusDef,
		                         lpszAppID ) )
	{
		VirusDefClose( hVirusDef );
		return( VDL_INIT_WINDOWS_APP_ERROR );
	}


	///////////////////////////////////////////////////////
	// Are defs newest?
	///////////////////////////////////////////////////////
	printf("Checking to see if defs are the newest...\n");
	if( VirusDefAreNewest( hVirusDef ) )
	{
		printf("Defs are the newest!\n");
	}else
	{
		printf("Defs are NOT the newest!\n");
	}


	///////////////////////////////////////////////////////
	// Get Current Definitions
	///////////////////////////////////////////////////////
	if( !VirusDefGetCurrent( hVirusDef,
		                     szCurrDefsPath,
							 _MAX_PATH ) )
	{

     	///////////////////////////////////////////////////////
	   // Use newset definitions
	   ////////////////////////////////////////////////////////
        if( !VirusDefUseNewest( hVirusDef,
		                        (LPSTR)&szNewestDefsPath,
		      				    _MAX_PATH,
							    &bDefsChanged) )
        {
            VirusDefClose( hVirusDef );
		    return( VDL_USE_NEWEST_DEFS_ERROR );
	    }else
	    {
            printf("Newest definitions are: %s\n", szNewestDefsPath );

            if(bDefsChanged)
            {
                printf("The current definitions have changed!\n");
            }
            else
		    {
                printf("The current definitions have NOT changed!\n");
            }
        }
    
		return( VDL_STATUS_OK );
	}


	///////////////////////////////////////////////////////
	// Get Newest Definitions date
	///////////////////////////////////////////////////////
	if( !VirusDefGetNewestDate( hVirusDef,
		                        &wYear,
							    &wMonth,
							    &wDay,
							    &dwRev ) )
	{
		VirusDefClose( hVirusDef );
		return( VDL_GET_NEWEST_DATE_ERROR );
	}else
	{
		printf("Newest defs are: %d-%d-%d Revision %d\n", wMonth, wDay, wYear, dwRev );
	}


	///////////////////////////////////////////////////////
	// Get Current Definitions date
	///////////////////////////////////////////////////////
	if( !VirusDefGetCurrentDate( hVirusDef,
		                         &wYear,
		 					     &wMonth,
		   					     &wDay,
							     &dwRev ) )
	{
		VirusDefClose( hVirusDef );
		return( VDL_GET_CURRENT_DATE_ERROR );
	}else
	{
		printf("Current defs are: %d-%d-%d Revision %d\n", wMonth, wDay, wYear, dwRev );
	}


	
	///////////////////////////////////////////////////////
	// Use newset definitions
	///////////////////////////////////////////////////////
	if( !VirusDefUseNewest( hVirusDef,
		                    (LPSTR)&szNewestDefsPath,
							_MAX_PATH,
							&bDefsChanged) )
	{
		VirusDefClose( hVirusDef );
		return( VDL_USE_NEWEST_DEFS_ERROR );
	}else
	{
		printf("Newest definitions are: %s\n", szNewestDefsPath );
		if(bDefsChanged)
		{
			printf("The current definitions have changed!\n");
		}else
		{
			printf("The current definitions have NOT changed!\n");
		}
	}



	///////////////////////////////////////////////////////
	// Close Definition Engine
	///////////////////////////////////////////////////////
	VirusDefClose( hVirusDef );

	return( VDL_STATUS_OK );
}


//********************************************************************
//
// Function:
//  WORD NavStandardDefsUninstall()
//
// Parameters:
//	LPSTR lpszAppID
//
// Description:
//  
//
// Returns:
//
//********************************************************************
WORD NavStandardDefsUninstall
(
	LPSTR  lpszAppID
)
{
	
	HVIRUSDEF	hVirusDef;


	///////////////////////////////////////////////////////
	// Initialize definitions engine
	///////////////////////////////////////////////////////
	printf("Initializing Virus Defintions Engine...\n");
    if ( !VirusDefInit( &hVirusDef ) )
    {
		VirusDefClose( hVirusDef );
		return ( VDL_INIT_ERROR );
    }

	
	///////////////////////////////////////////////////////
	// Register an AppID
	///////////////////////////////////////////////////////
	printf("Registering AppID...\n");
	if( !VirusDefInitInstallApp( hVirusDef,
		                         lpszAppID,
								 NULL,
								 NULL ) )
	{
		VirusDefClose( hVirusDef );
		return( VDL_INIT_INSTALL_ERROR );
	}


	///////////////////////////////////////////////////////
	// Stop using definitions
	///////////////////////////////////////////////////////
	printf("Stopping definitions usage...\n");
	if( !VirusDefStopUsingDefs( hVirusDef ) )
	{
		VirusDefClose( hVirusDef );
		return( VDL_STOP_USING_DEFS_ERROR );
	}


	///////////////////////////////////////////////////////
	// Close Definition Engine
	///////////////////////////////////////////////////////
	VirusDefClose( hVirusDef );

	return( VDL_STATUS_OK );
}


//********************************************************************
//
// Function:
//  WORD NavSimpleDefInstall()
//
// Parameters:
//	LPSTR lpszAppID
//	LPSTR lpVirusDefDir
//
//
// Description:
//  
//
// Returns:
//
//********************************************************************
DWORD NavSimpleDefInstall
( 
	LPSTR  lpszAppID,
    LPSTR  lpVirusDefDir 
)
{
    HVIRUSDEF	hVirusDef;
	char		szAppID[260];
	DWORD		dwResult = VDL_STATUS_OK ;


	printf("Initializing Virus Defintions Engine...\n");
    if ( !VirusDefInit( &hVirusDef ) )
    {
		printf("ERROR: Initializing Definition Engine!\n");
		return ( FALSE );
    }


	printf("Install Virus Defintions...\n");
    if( (dwResult = VirusDefInstall(hVirusDef, (LPSTR)&szAppID, sizeof(szAppID), lpszAppID, lpVirusDefDir) )!= VIRDEF_OK )
	{
		return ( dwResult );
	}

	printf( "Generated AppID is: %s\n", szAppID );

    VirusDefClose( hVirusDef );
    return ( dwResult );
}


//********************************************************************
//
// Function:
//  WORD NavSimpleDefUninstall()
//
// Parameters:
//	LPSTR lpszAppID
//
//
// Description:
//
// Returns:
//
//********************************************************************
BOOL NavSimpleDefUninstall
(
	LPSTR lpszAppID	
)
{
	HVIRUSDEF	hVirusDef;
	
	printf("Initializing Virus Defintions Engine...\n");
    if ( !VirusDefInit( &hVirusDef ) )
    {
		printf("ERROR: Initializing Definition Engine!\n");
		return ( FALSE );
    }

	printf("Uninstalling Virus Defintions...\n");
	if( !VirusDefUninstall( hVirusDef, lpszAppID ) )
	{
		printf("ERROR: Uninstalling Virus Defintions!\n");
	}else
	{
		printf("Virus Definitions uninstalled successfully!\n");
	}

    VirusDefClose( hVirusDef );
    return ( TRUE );
}