#include "platform.h"

#define SEC_SIZE_OFFSET		11

// Sector sizes
//=============================
#define SEC_SIZE_127		127
#define SEC_SIZE_256		256
#define SEC_SIZE_512		512
#define SEC_SIZE_1024		1024
#define SEC_SIZE_2048		2048
#define SEC_SIZE_4096		4096



BOOL GetCmdLineOpts( LPTSTR		lpszTag,
					 LPTSTR		lpszValue,
					 int		argc,
					 LPTSTR		argv[] );


DWORD StrToHex( LPTSTR lpszValue );

WORD GetSectorSize( LPABSDISKREC lpstABSDiskRec );

void HelpScreen();
