/////////////////////////////////////////////////////////////////////////////////
//
// Description:
//		Tools to changes the internal date of virscan1.dat
//
//
//
/////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>


#if defined (SYM_WIN32)
	#include "windows.h"	//replaced "platform.h" August 31, 2000 emurillo

#elif (SYM_UNIX)
	#include "platform.h" 
    #define _MAX_PATH    2048
#endif

#include "avendian.h"


void help()
{
	printf("\n");
	printf("CHGVDATE Version 1.0.0\n");
	printf("This tool changes the internal date and sequence\n");
	printf("number of virscan1.dat file.\n\n");
	printf("usage: chgvdate virscan1.dat mm dd yyyy seqno\n");
	printf("   example: chgvdate virscan1.dat 1 15 2000 1\n\n");
}

int main( int argc, char* argv[] )
{

	BYTE 	byMonth  = 0;
	BYTE	byDay    = 0;
	BYTE	byYear   = 0;
	DWORD	dwSeqNo  = 0;
	
	char	szFileName[_MAX_PATH];
	FILE*	fpDataFile;

	// Check parameters
	if( argc != 6 )
	{
		help();
		return( 1 );
	}

	// Get month parameter
	byMonth = atoi(argv[2]);
	
	// Get day parameter
	byDay = atoi(argv[3]);
	
	// Get year parameter
	byYear = ( (atoi(argv[4]) ) - 1900 );
	
	// Get sequence number parameter
	dwSeqNo = atol(argv[5]);
	
    // Endianize if needed
	dwSeqNo = DWENDIAN( dwSeqNo );

	// Open VIRSCAN1.DAT file
	if( (fpDataFile = fopen( argv[1], "r+b" ) ) == NULL )
	{
		printf("ERROR: Unable to open file %s!\n", szFileName );
		return( 1 );
	}

	// Write squence number to file
	fseek( fpDataFile, 0x04, SEEK_SET );
	if( fwrite( &dwSeqNo, sizeof(dwSeqNo), 1, fpDataFile ) < 1 )
	{
		printf("ERROR: No access!\n");
		return( 1 );
	}
	
	// Write month to file
	fseek( fpDataFile, 0x08, SEEK_SET );
	if( fwrite( &byMonth, sizeof(byMonth), 1, fpDataFile ) < 1 )
	{
		printf("ERROR: No access!\n");
		return( 1 );
	}
	
	// Write day to file
	fseek( fpDataFile, 0x09, SEEK_SET );
	if( fwrite( &byDay, sizeof(byDay), 1, fpDataFile ) < 1 )
	{
		printf("ERROR: No access!\n");
		return( 1 );
	}

	// Write year to file
	fseek( fpDataFile, 0x0a, SEEK_SET );
	if(fwrite( &byYear, sizeof(byYear), 1, fpDataFile ) < 1 )
	{
		printf("ERROR: No access!\n");
		return( 1 );
	}
	

	// Close file
	fclose( fpDataFile );

	return( 0 ); 
}





