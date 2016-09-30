
#include "platform.h"
#include "disk.h"
#include <stdio.h>
#include <stdlib.h>


#define PARTITION_SIZE		66
#define PARTITION_OFFSET	446


#define SEC_SIZE_OFFSET		11



// Sector sizes
//=============================
#define SEC_SIZE_127		127
#define SEC_SIZE_256		256
#define SEC_SIZE_512		512
#define SEC_SIZE_1024		1024
#define SEC_SIZE_2048		2048
#define SEC_SIZE_4096		4096



WORD GetPhysicalDiskInfo( LPABSDISKREC	lpstABSDiskRec,
						  BYTE			byDevice );

WORD GetMBR( LPABSDISKREC	lpstABSDiskRec,
			 LPBYTE			lpbyBuffer );

WORD GetPartTable( LPBYTE lpbyBuffer, LPBYTE lpbyPartTable );

WORD UpdateImagePartition( LPBYTE lpbyPartTable, LPBYTE lpbyImageFile );

WORD GetSectorSize( LPABSDISKREC lpstABSDiskRec ) ;

WORD InsertCleanMBR( LPABSDISKREC	lpstABSDiskRec,
					 LPBYTE			lpbyMBR,
					 LPBYTE			lpbyImage,
					 WORD			wSectorSize,
					 BYTE			bySectorNum );

BOOL GetCmdLineOpts( LPTSTR		lpszTag,
					 LPTSTR		lpszValue,
					 int		argc,
					 LPTSTR		argv[] );


DWORD StrToHex( LPTSTR lpszValue );

void HelpScreen();