

// mbrchg filename /DEV=n /SECNO=n

#include <stdio.h>
#include <stdlib.h>
#include "mbrchg.h"
#include "platform.h"
#include "disk.h"
#include "file.h"



int main( int argc, char* argv[] )
{
	
	LPBYTE		lpbyMBR;

	TCHAR		szTmpBuffer[256];

	BYTE		byPartTable[PARTITION_SIZE] = {0};
	WORD		wResult = 0;
	LPBYTE		lpbyImage = 0;
	TCHAR		szFileName[SYM_MAX_PATH];
	DWORD		dwFileLen = 0;
	HFILE		hFile;

	BYTE		byDevice = 0;
	BYTE		bySectNum = 0;
	WORD		wSectorSize = 0;

	BOOL		bPhysical = FALSE;

	ABSDISKREC	stABSDiskRec = {0};



	// Get parameters
	if( argc < 2 )
	{
		HelpScreen();
		return( 1 );
	}

	// Get Image file name
	strcpy( szFileName, argv[1] );
	
	// Get Device number
	if( GetCmdLineOpts("/DEV=", (LPTSTR)&szTmpBuffer, argc, argv ) )
	{
		byDevice = (BYTE)StrToHex( (LPTSTR)&szTmpBuffer );
	}else
	{
		printf("ERROR: No device specified!\n\n");
		HelpScreen();
		return( 1 );
	}

	// Get Device number
	if( GetCmdLineOpts("/SECNO=", (LPTSTR)&szTmpBuffer, argc, argv ) )
	{
		bySectNum = atoi(szTmpBuffer );	
		
		if( bySectNum <= 0 )
		{
			printf("ERROR: Invalid sector number!\n\n");
			HelpScreen();
			return( 1 );
		}

		bPhysical = TRUE;
	}else
	{
		bPhysical = FALSE;
	}

	// Get disk info
	GetPhysicalDiskInfo( &stABSDiskRec, byDevice );

	// Get sector size
	wSectorSize = GetSectorSize( &stABSDiskRec );

	// Allocate memory for MBR
	if( (lpbyMBR = (LPBYTE)malloc(wSectorSize) ) == NULL )
	{
		printf("ERROR: Not enough memory available!\n\n");
		HelpScreen();
		return( 1 );
	}


	// Get MBR
	if( (wResult = GetMBR( &stABSDiskRec, lpbyMBR ) ) != 0 )
	{
		printf("ERROR: Unable to access disk!\n\n");
		free( lpbyMBR );
		HelpScreen();
		return( 1 );
	}

	// Extract partition table
	GetPartTable( lpbyMBR, (LPBYTE)&byPartTable );



	if( (hFile = FileOpen( szFileName, 0 ) ) != HFILE_ERROR )
	{
		// Get file size
		dwFileLen = FileLength( hFile );
		
		// Allocate memory for file
		if( (lpbyImage = (LPBYTE)malloc( dwFileLen ) ) == NULL )
		{
			printf("ERROR: No memory available!\n\n");
			free( lpbyMBR );
			HelpScreen();
			return( 1 );
		}


		FileRead( hFile, lpbyImage, dwFileLen );
		FileClose( hFile );


	}else
	{
		free( lpbyMBR );
		free( lpbyImage );
		printf("ERROR: Can not open file %s!\n\n", szFileName );
		HelpScreen();
		return( 1 );
	}


	wResult = UpdateImagePartition( (LPBYTE)&byPartTable , lpbyImage );
	

	if( bPhysical )
	{
		// Insert clean MBR at given offset
		wResult = InsertCleanMBR( &stABSDiskRec,
								  lpbyMBR,
								  lpbyImage,
								  wSectorSize,
								  bySectNum );
	}

	// Write out file
	if( (hFile = FileCreate( szFileName, 0 ) ) != HFILE_ERROR )
	{		
		FileWrite( hFile, lpbyImage, dwFileLen );
		FileClose( hFile );
	}

	// Free allocated memory
	free( lpbyMBR );
	free( lpbyImage );

	return( 0 );
}



WORD GetPhysicalDiskInfo( LPABSDISKREC	lpstABSDiskRec,
						  BYTE			byDevice )
{
	lpstABSDiskRec->dn = byDevice;
	DiskGetPhysicalInfo ( lpstABSDiskRec );
	return( 0 );
}

///////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////
WORD GetMBR( LPABSDISKREC	lpstABSDiskRec,
			 LPBYTE			lpbyBuffer )
{
	
	// Clear Buffer
	lpbyBuffer[0] = 0;
	
	lpstABSDiskRec->buffer = lpbyBuffer;
	
	lpstABSDiskRec->dwTrack		= 0;
	lpstABSDiskRec->dwHead		= 0;
	lpstABSDiskRec->dwSector	= 1;
	lpstABSDiskRec->numSectors	= 1;

	return( DiskAbsOperation( READ_COMMAND, lpstABSDiskRec) );
}


///////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////
WORD GetPartTable( LPBYTE lpbyBuffer, LPBYTE lpbyPartTable )
{
	
	for( int i=0; i<PARTITION_SIZE; i++ )
	{
		lpbyPartTable[i] = lpbyBuffer[PARTITION_OFFSET+i];
	}
	
	return( 0 );
}


///////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////
WORD UpdateImagePartition( LPBYTE lpbyPartTable, LPBYTE lpbyImageFile )
{


	for( int i=0; i<PARTITION_SIZE; i++ )
	{
		lpbyImageFile[PARTITION_OFFSET+i] = lpbyPartTable[i];
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////
WORD GetBootRecord( LPABSDISKREC	lpstABSDiskRec,
				    LPBYTE			lpbyBootRec )
{
	
	// Clear Buffer
	lpbyBootRec[0] = 0;
	
	lpstABSDiskRec->buffer = lpbyBootRec;
	

	lpstABSDiskRec->dwTrack		= 0;
	lpstABSDiskRec->dwHead		= 1;
	lpstABSDiskRec->dwSector	= 1;
	lpstABSDiskRec->numSectors	= 1;

	return( DiskAbsOperation( READ_COMMAND, lpstABSDiskRec) );

}


///////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////
WORD GetSectorSize( LPABSDISKREC lpstABSDiskRec ) 
				   
{
	
	WORD	wSectorSize = 0;
	BYTE	byHBR[4096] = {0};
	WORD	wResult = 0;

	
	lpstABSDiskRec->buffer = (LPBYTE)&byHBR;
	
	lpstABSDiskRec->dwTrack		= 0;
	lpstABSDiskRec->dwHead		= 1;
	lpstABSDiskRec->dwSector	= 1;
	lpstABSDiskRec->numSectors	= 1;

	if( (DiskAbsOperation( READ_COMMAND, lpstABSDiskRec) ) != 0 )
	{
		return( 0 );
	}

	// Get high byte
	wSectorSize = byHBR[SEC_SIZE_OFFSET+1];
	wSectorSize <<= 8;

	// Get low byte
	wSectorSize += byHBR[SEC_SIZE_OFFSET];

	// Do Validation of sector size
	switch( wSectorSize )
	{

		case SEC_SIZE_127:
			return( wSectorSize );

		case SEC_SIZE_256:
			return( wSectorSize );

		case SEC_SIZE_512:
			return( wSectorSize );

		case SEC_SIZE_1024:
			return( wSectorSize );
		
		case SEC_SIZE_2048:
			return( wSectorSize );
		
		case SEC_SIZE_4096:
			return( wSectorSize );

		default:
			return( 0 );

	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////
WORD InsertCleanMBR( LPABSDISKREC	lpstABSDiskRec,
					 LPBYTE			lpbyMBR,
					 LPBYTE			lpbyImage,
					 WORD			wSectorSize,
					 BYTE			bySectorNum )
{
	DWORD	dwFileOffset = 0;
	
	dwFileOffset = ConvertPhySectorToLong( lpstABSDiskRec, 0, 0, bySectorNum );

	dwFileOffset *= wSectorSize;
	for( int i=0; i<wSectorSize; i++ )
	{
		lpbyImage[dwFileOffset+i] = lpbyMBR[i];
	}
	
	return( 0 );
}

///////////////////////////////////////////////////////////
//
//	Description:
//		Gets parses command line options.
//
//		Name:	
//				GetCmdLineOpts()
//
//		Parameters:
//				TCHAR		cOptionMarker		
//				LPTSTR		lpszTag				
//				TCHAR		cParameterMarker	
//				LPTSTR		lpszValue
//				int			argc
//				LPTSTR		argv[]
//
//		Returns:
//				TRUE
//				FALSE
//
///////////////////////////////////////////////////////////
BOOL GetCmdLineOpts( LPTSTR		lpszTag,
					 LPTSTR		lpszValue,
					 int		argc,
					 LPTSTR		argv[] )
{

	int		iCmdOpts;
	TCHAR	szArg[255];
	LPTSTR	lpszBuffer;
	TCHAR	szTag[255];


	if( lpszTag == NULL )
		return( FALSE );
	
	// Clear result if not null
	if( lpszValue != NULL )
		lpszValue[0] = NULL;

	// Make a copy and upper case string
	strcpy( szTag, lpszTag );
    _strupr( szTag );

	// Iterate between options
	for( iCmdOpts = 1; iCmdOpts < argc; iCmdOpts++)
	{

		// Make local copy of argv[n]
		strcpy( szArg, argv[iCmdOpts] );

		// Upper case string
        _strupr( szArg );

		// Check for marker
		if( szArg[0] == szTag[0])
		{

			// Find tag and return value
			if( (lpszBuffer = strstr( szArg,  szTag ) ) != NULL )
			{
				if( lpszValue != NULL )
				{
					strcpy(lpszValue, &argv[iCmdOpts][strlen(szTag)]);
				}

				return( TRUE );	
			}
			
		
		}

	}
	
	return( FALSE );
}


///////////////////////////////////////////////////////////
//
//	Description:
//		Gets parses command line options.
//
//		Name:	
//				StrToHex()
//
//		Parameters:
//				LPTSTR		lpszValue
//
//		Returns:
//				DWORD value
//
///////////////////////////////////////////////////////////
DWORD StrToHex( LPTSTR lpszValue )
{

	DWORD			dwResult = 0;
	TCHAR			szBuffer[12];
	unsigned int	iOffset;


	// Make copy of value and upper case
	strcpy( szBuffer, lpszValue );
    _strupr( szBuffer );


	// Check for 0x prefix
	if( (szBuffer[0] == '0') && ( szBuffer[1] == 'X') )
	{
		iOffset = 2;	
	}else
	{
		iOffset = 0;
	}
	

	// iterate through array
	for( iOffset; iOffset<strlen(szBuffer); iOffset++ )
	{
		switch(szBuffer[iOffset])
		{

			case '0':
				(dwResult <<= 4);
				dwResult += 0x0;
				break;

			case '1':
				(dwResult <<= 4);
				dwResult += 0x1;
				break;

			case '2':
				(dwResult <<= 4);
				dwResult += 0x2;
				break;

			case '3':
				(dwResult <<= 4);
				dwResult += 0x3;
				break;

			case '4':
				(dwResult <<= 4);
				dwResult += 0x4;
				break;

			case '5':
				(dwResult <<= 4);
				dwResult += 0x5;
				break;

			case '6':
				(dwResult <<= 4);
				dwResult += 0x6;
				break;

			case '7':
				(dwResult <<= 4);
				dwResult += 0x7;
				break;

			case '8':
				(dwResult <<= 4);
				dwResult += 0x8;
				break;

			case '9':
				(dwResult <<= 4);
				dwResult += 0x9;
				break;

			case 'A':
				(dwResult <<= 4);
				dwResult += 0xA;
				break;

			case 'B':
				(dwResult <<= 4);
				dwResult += 0xB;
				break;

			case 'C':
				(dwResult <<= 4);
				dwResult += 0xC;
				break;

			case 'D':
				(dwResult <<= 4);
				dwResult += 0xD;
				break;

			case 'E':
				(dwResult <<= 4);
				dwResult += 0xE;
				break;

			case 'F':
				(dwResult <<= 4);
				dwResult += 0xF;
				break;

		
		}

	}

	return( dwResult );
}



void HelpScreen()
{
	printf("MBR Change 1.0.0\n");
	printf("===================================\n");

#ifdef WIN32    
	printf("Usage: MBRCHG32.EXE filename /DEV=n [/SECNO=n]\n\n");
#else
	printf("Usage: MBRCHG.EXE filename /DEV=n [/SECNO=n]\n\n");
#endif

    printf("Options:\n");
	printf("   /DEV=     Device number ( e.g. A=0x00, B=0x01, C=0x80, etc...)\n");
	printf("   /SECNO=   Sector number to place original MBR\n");

}
