
#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "disk.h"
#include "file.h"
#include "dwrite.h"


#define MAX_NUM_SECTORS_READ_WRITE 128


int main( int argc, char* argv[] )
{

	ABSDISKREC	    stAbsDiskRec		= {0};
    LPPARTENTRYREC	lpPartEntryRec      = {0};
	BYTE			byPartNum = 0;

	TCHAR		    szTmpBuffer[12];
	TCHAR		    szFileName[SYM_MAX_PATH];
    BYTE			byMBR[4096] = {0};

	BYTE		    byDeviceNum			= 0;
	DWORD		    dwHeadNum			= 0;
	DWORD		    dwTrackNum			= 0;
	DWORD		    dwSecNum			= 0;
	BYTE		    byReadSize			= 0;
	
	LPBYTE		    lpbyBuffer			= 0;
	DWORD		    dwResult			= 0;
	int			    iSecSizeCode		= 0;
	WORD		    wSecSize			= 0;

	BOOL		    bRead				= FALSE;
	BOOL		    bWrite				= FALSE;

	HFILE		    hFile;
	DWORD		    dwFileLen			= 0;

	BYTE		    byResult			= 0;

	BOOL			bSecSize			= FALSE;


    // Get file name
	if( argv[1] != NULL )
	{
		STRCPY( szFileName, argv[1] );
	}else
	{
		printf("No file specified!\n\n");
		HelpScreen();
		return( 1 );
	}
	  
	// Get device number
    if( GetCmdLineOpts( "/DEV=", (LPTSTR)&szTmpBuffer, argc, argv ) )
	{
		byDeviceNum = (BYTE)StrToHex( szTmpBuffer );
	}else
	{
		printf("No device specified!\n\n");
		HelpScreen();
		return( 1 );
	}

	// Get track number
    if( GetCmdLineOpts( "/TRACK=", (LPTSTR)&szTmpBuffer, argc, argv ) )
	{
		dwTrackNum = atoi( szTmpBuffer );
	}else
	{
		printf("No track specified!\n\n");
		HelpScreen();
		return( 1 );
	}

	// Get head number
    if( GetCmdLineOpts( "/HEAD=", (LPTSTR)&szTmpBuffer, argc, argv ) )
	{
		dwHeadNum = atoi( szTmpBuffer );
	}else
	{
		printf("No head specified!\n\n");
		HelpScreen();
		return( 1 );
	}

	// Get sectors number
    if( GetCmdLineOpts( "/SEC=", (LPTSTR)&szTmpBuffer, argc, argv ) )
	{
		dwSecNum = atoi( szTmpBuffer );		
	}else
	{
		printf("No sector specified!\n\n");
		HelpScreen();
		return( 1 );
	}

	// Get number of sectors 
    if( GetCmdLineOpts( "/R=", (LPTSTR)&szTmpBuffer, argc, argv ) )
	{
		byReadSize = atoi( szTmpBuffer );
		if( (byReadSize > MAX_NUM_SECTORS_READ_WRITE) || ( szTmpBuffer[0] == NULL) )
		{
			printf("Read sectors number is to large. %d sectors is the Max.\n\n", MAX_NUM_SECTORS_READ_WRITE);
			HelpScreen();
			return( 1 );
		}else
		{
			bRead = TRUE;
		}

	}else if( GetCmdLineOpts( "/W", NULL, argc, argv ) )
	{
		bWrite = TRUE;
	}else
	{
		return( 1 );
	}

	// Get number of sectors of destination disk
    if( GetCmdLineOpts( "/SECSIZE=", (LPTSTR)&szTmpBuffer, argc, argv ) )
	{
		wSecSize = atoi( szTmpBuffer );
		if( (wSecSize > 4096) || ( szTmpBuffer[0] == NULL) )
		{
			printf("Destination disk's sector size is wrong. 4096 sectors is the Max.\n\n");
			HelpScreen();
			return( 1 );
		}else
		{
			bSecSize = TRUE;
		}

	}



	// Get disk geometery
    stAbsDiskRec.dn = byDeviceNum;
	if (DiskGetPhysicalInfo ( &stAbsDiskRec ) != 0)
    {
        printf("ERROR: Can not read from disk!\n");
        return (1);
    }

    // Get Master Boot Record
	stAbsDiskRec.dwTrack	= 0;
	stAbsDiskRec.dwHead		= 0;
	stAbsDiskRec.dwSector	= 1;
	stAbsDiskRec.numSectors	= 1;
	stAbsDiskRec.buffer		= (LPBYTE)&byMBR;

 	if (DiskAbsOperation( READ_COMMAND, &stAbsDiskRec ) != 0 )
	{
		printf("ERROR: Can not read from disk!\n");
        return (1);

	}

	// Windows 2000 work around for sectors per track bug
    for (byPartNum = 0; byPartNum < 4; byPartNum++)
	{
        lpPartEntryRec = (LPPARTENTRYREC)(byMBR + PART_TABLE_OFFSET) + byPartNum;

        // Is this an unused partition entry?

        if (lpPartEntryRec->system == SYSTEM_NOT_USED)
            continue;

        // the first partition is one tract into the physical disk.  So, if
        // relativeSectors is 32 (the start of the partition) then we assume
        // there are 32 sectory per tract - (The Windows2000 OS wrongly returns
        // 63 sectors per tract)
        if (lpPartEntryRec->relativeSectors == 32)
		{
			stAbsDiskRec.dwSectorsPerTrack = 32;
            break;
        }

    }	



	// Get sector size
	if (!bSecSize)
	{
		if( ( wSecSize =  GetSectorSize( &stAbsDiskRec ) ) == 0 )
		{
			printf("Invalid sector size!\n\n");
			HelpScreen();
			return( 2 );
		}
	}

	// Do some value checking here
	if( dwHeadNum > stAbsDiskRec.dwTotalHeads )
	{
		printf("Invalid head number!\n\n");
		HelpScreen();
		return( 2 );
	}

	if(  dwTrackNum > stAbsDiskRec.dwTotalTracks )
	{
		printf("Invalid track number!\n\n");
		HelpScreen();
		return( 2 );
	}

	if( ( dwSecNum < 1 ) || ( dwSecNum > stAbsDiskRec.dwSectorsPerTrack ) )
	{
		printf("Invalid sector number!\n\n");
		HelpScreen();
		return( 2 );
	}

	// Device number
	stAbsDiskRec.dn = byDeviceNum;
		
	// Head number to read/write
	stAbsDiskRec.dwHead = dwHeadNum;

	// Track number to read/write
	stAbsDiskRec.dwTrack = dwTrackNum;
	
	// Sector number to read/write
	stAbsDiskRec.dwSector = dwSecNum;


	
	// Read disk to file here
	if( (bRead) && (!bWrite) )
	{

		// Allocate memory
		lpbyBuffer = (LPBYTE)malloc( byReadSize * wSecSize );
		if( lpbyBuffer == NULL )
		{
			return( 3 );
		}
		
		// Clear memory
		memset( lpbyBuffer, 0, sizeof( byReadSize * wSecSize ) );

		// Number of sector to read
		stAbsDiskRec.numSectors = byReadSize;

		// Buffer to place data in
		stAbsDiskRec.buffer = lpbyBuffer;

		// Do disk read
		if( (dwResult = DiskAbsOperation( READ_COMMAND, &stAbsDiskRec ) ) != 0 )
		{
			printf("Device error, no media found!\n");
			free( lpbyBuffer );
			return( 3 );
		}

		if( (hFile = FileCreate( szFileName, 0 ) ) != HFILE_ERROR )
		{

			// Write sectors to file
			FileWrite( hFile, lpbyBuffer, wSecSize * byReadSize );
			FileClose( hFile );
			
			// Clear memory
			memset( lpbyBuffer, 0, sizeof( wSecSize * byReadSize ) );

			// Free memory
			free( lpbyBuffer );

		}else
		{
			printf("Error creating file %s!\n\n", szFileName );
			HelpScreen();
			return( 3 );
		}

		printf("\nDisk information for device: 0x%x\n", byDeviceNum );
		printf("============================================\n");
		printf("Total Tracks:    %d\n", stAbsDiskRec.dwTotalTracks );
		printf("Total Heads:     %d\n", stAbsDiskRec.dwTotalHeads );
		printf("Total Sectors:   %d\n", stAbsDiskRec.dwSectorsPerTrack );
		printf("Sector Size:     %d bytes\n\n", wSecSize );

		printf("Status:\n");
		printf("Number of sectors saved: %d\n", stAbsDiskRec.numSectors );



	// Write to file disk here
	}else if( (bWrite) && (!bRead) )
	{

		// Open file for read
		if( (hFile = FileOpen( szFileName, 0 ) ) != HFILE_ERROR )
		{
			if( (dwFileLen = FileLength( hFile ) ) != HFILE_ERROR )
			{				
			

				lpbyBuffer = (LPBYTE)malloc( dwFileLen );
				if( lpbyBuffer == NULL )
				{
					return( 3 );
				}

				memset( lpbyBuffer, 0, sizeof(dwFileLen) );


				FileRead( hFile, lpbyBuffer, dwFileLen );
				FileClose( hFile );

				// Number of sector to read
				stAbsDiskRec.numSectors = (BYTE)( dwFileLen / wSecSize );

				// Buffer to place data in
				stAbsDiskRec.buffer = lpbyBuffer;
		
				// Do disk read
				if( (dwResult = DiskAbsOperation( WRITE_COMMAND, &stAbsDiskRec ) ) != 0 )
				{
					printf("Device error, no media found!\n");
					free( lpbyBuffer );
					return( 3 );
				}


				free( lpbyBuffer );
			}

		}else
		{
			printf("Unable to open file %s!\n\n", szFileName );
			HelpScreen();
			return( 3 );
		}

		printf("\nDisk information for device: 0x%x\n", byDeviceNum );
		printf("============================================\n");
		printf("Total Heads:     %d\n", stAbsDiskRec.dwTotalHeads );
		printf("Total Tracks:    %d\n", stAbsDiskRec.dwTotalTracks );
		printf("Total Sectors:   %d\n", stAbsDiskRec.dwSectorsPerTrack );
		printf("Sector Size:     %d bytes\n\n", wSecSize );

		printf("Status:\n");
		printf("Number of sectors written: %d\n", stAbsDiskRec.numSectors );


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

    if( lpstABSDiskRec->dn < 0x80 )
    {
        lpstABSDiskRec->dwHead  = 0;
    }else
    {
        lpstABSDiskRec->dwHead  = 1;
    }

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
void HelpScreen()
{

#ifdef WIN32 
    printf("DWRITE32 1.0.1\n");
#else
    printf("DWRITE 1.0.1\n");
#endif

	printf("=====================================\n");

#ifdef WIN32
	printf("Usage:  DWRITE32.EXE filename /DEV=n /TRACK=n /HEAD=n /SEC=n [/R=n|/W] [SECSIZE=n]\n\n");
#else
	printf("Usage:  DWRITE.EXE filename /DEV=n /TRACK=n /HEAD=n /SEC=n [/R=n|/W] [SECSIZE=n]\n\n");
#endif

	printf("Options:\n");
	printf("   /DEV=     Device number ( e.g. A=0x00, B=0x01, C=0x80, etc...)\n");
	printf("   /HEAD=    Head number\n");
	printf("   /TRACK=   Track number\n");
	printf("   /SEC=     Sector number\n");
	printf("   /R=       Number of sectors to read from disk\n");
	printf("   /W        Write to disk\n");
	printf("   /SECSIZE= Size of sectors on destination disk (optional)\n\n");
	
	printf("*Note:\n");
	printf("   Maximum number of sectors that can be saved or written is %d\n\n", MAX_NUM_SECTORS_READ_WRITE);

#ifdef WIN32
	printf("Save example:\n");
	printf("  DWRITE32.EXE cdrive.mbr /DEV=0x80 /TRACK=0 /HEAD=0 /SEC=1 /R=1\n\n");
	printf("Restore example:\n");
	printf("  DWRITE32.EXE cdrive.mbr /DEV=0x80 /TRACK=0 /HEAD=0 /SEC=1 /W\n\n");
#else
	printf("Save example:\n");
	printf("  DWRITE.EXE cdrive.mbr /DEV=0x80 /TRACK=0 /HEAD=0 /SEC=1 /R=1\n\n");
	printf("Restore example:\n");
	printf("  DWRITE.EXE cdrive.mbr /DEV=0x80 /TRACK=0 /HEAD=0 /SEC=1 /W\n\n");
#endif
}
