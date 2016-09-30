

#define	VDL_STATUS_OK				0
#define	VDL_INIT_ERROR				10
#define VDL_INIT_INSTALL_APP_ERROR	11
#define VDL_NO_APPID_CREATED		12
#define VDL_PREUPDATE_FAILED		13
#define VDL_POSTUPDATE_FAILED		14
#define VDL_INIT_INSTALL_ERROR		15
#define VDL_CANT_USE_NEWEST_DEFS	16
#define VDL_ERROR_COPYING_FILES		17
#define VDL_STOP_USING_DEFS_ERROR	18
#define VDL_INIT_WINDOWS_APP_ERROR	19
#define	VDL_DEFS_ARE_NEWEST_ERROR	20
#define VDL_GET_NEWEST_DATE_ERROR	21
#define VDL_GET_CURRENT_DATE_ERROR	22
#define VDL_GET_CURRENT_DEFS_ERROR	23
#define VDL_USE_NEWEST_DEFS_ERROR	24

#define	MAXRETVALUES				255

char VDLReturnValues[MAXRETVALUES][50] = {
    "VDL_STATUS_OK",
    "VDL_NO_APPID_CREATED",
    "VDL_PREUPDATE_FAILED",
    "VDL_POSTUPDATE_FAILED",
    "VDL_INIT_INSTALL_ERROR",
    "VDL_CANT_USE_NEWEST_DEFS",
    "VDL_ERROR_COPYING_FILES",
	"VDL_RESERVED",
	"VDL_RESERVED",
	"VDL_RESERVED",
	"VDL_INIT_ERROR",
	"VDL_INIT_INSTALL_APP_ERROR",
	"VDL_NO_APPID_CREATED",
	"VDL_PREUPDATE_FAILED",
	"VDL_POSTUPDATE_FAILED",
	"VDL_INIT_INSTALL_ERROR",
	"VDL_CANT_USE_NEWEST_DEFS",
	"VDL_ERROR_COPYING_FILES",
	"VDL_STOP_USING_DEFS_ERROR",
	"VDL_INIT_WINDOWS_APP_ERROR",
	"VDL_DEFS_ARE_NEWEST_ERROR",
	"VDL_GET_NEWEST_DATE_ERROR",
	"VDL_GET_CURRENT_DATE_ERROR",
	"VDL_GET_CURRENT_DEFS_ERROR",
	"VDL_USE_NEWEST_DEFS_ERROR"

};


//********************************************************************
//
// Function:
//  void help()
//
// Parameters:
//	NONE
//
// Description:
//  
//
// Returns:
//	NONE
//	
//********************************************************************
void help( void );

//********************************************************************
//
// Function:
//  WORD NavStandardDefsInstall()
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
WORD NavStandardDefsInstall
( 
	LPSTR  lpAppID,
    LPSTR  lpVirusDefDir
);

//********************************************************************
//
// Function:
//  WORD NavStandardUpdateDefs()
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
);


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
);












DWORD NavSimpleDefInstall
( 
	LPSTR  lpAppID,
    LPSTR  lpVirusDefDir
);


BOOL NavSimpleDefUninstall
( 
	LPSTR  lpAppID
);
