#ifndef __SNAVAPI_H
#define __SNAVAPI_H


// Define menu options
#define	SCAN_BOOT		1	
#define	SCAN_MBR		2
#define	SCAN_FILE		3
#define	SCAN_MEMORY		4
#define INFO_TEST_M		5
#define	NAVAPI_VER		6
#define NAVAPI_HELP		7


// Menu item structure
typedef struct optionlist{
	WORD	Option;
	char	szOptionStr[80];	
}OPTION_LIST, *LPOPTION_LIST;


///////////////////////////////////////////////////////////////////////////////
// Option List
///////////////////////////////////////////////////////////////////////////////
OPTION_LIST stOptionList[]={
#if defined (SYM_UNIX) || defined (SYM_NLM) 
	{SCAN_FILE,		"FILE"},
	{INFO_TEST_M,		"INFO"},
	{NAVAPI_VER,    "VER"},
	{NAVAPI_HELP,	"HELP"}
#else
	{SCAN_BOOT,		"BOOT"},
	{SCAN_MBR,		"MBR"},
	{SCAN_FILE,		"FILE"},
	{SCAN_MEMORY,	"MEMORY"},
	{INFO_TEST_M,		"INFO"},
	{NAVAPI_VER,    "VER"},
	{NAVAPI_HELP,	"HELP"}

#endif
};

///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void help()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void help ( );


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		WORD GetOption()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
WORD GetOption
(
	LPTSTR	lpszOption
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		WORD PrintDetailedHelp()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void PrintDetailedHelp(void);


#endif //__SNAVAPI_H
