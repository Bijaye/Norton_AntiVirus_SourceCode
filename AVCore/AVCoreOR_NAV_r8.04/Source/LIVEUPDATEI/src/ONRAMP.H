/****************************************************************
Onramp.h
A component of the Onramp Project
Copyright 1996 (c) Symantec Corp.
SYMANTEC CONFIDENTIAL

Author:		Richard Sadowsky
Created:	April 16, 1996

Purpose: 

This module contains the defintion of the types needed 
to access the DoOnramp function exported by SymMal32.DLL.

****************************************************************/
#ifndef __ONRAMP_H
#define __ONRAMP_H


//the following define the final Onramp status codes
#define OAM_SUCCESS			0
#define OAM_NOTHINGNEW		1
#define OAM_HOSTBUSY		2
#define	OAM_USERCANCEL		3
#define OAM_ERROR			4
#define OAM_NOCARRIER		5
#define OAM_CRITICALERROR	6
#define OAM_SUCCESSREBOOT	7

#define	OAM_MAXSTRING		128

extern "C"
{
	// This key will be present at the time the finish panel
	// is initialized if a reboot is necessary
	const LPCTSTR kOnrampDelayKey	= "SOFTWARE\\Symantec\\LiveUpdate\\Clients\\Delay";

	// Pass this string to RegisterWindowMessage.  When the
	// DoOnramp function starts, a system-wide message
	// will be sent with the WPARAM set to the handle of the
	// parent app and the LPARAM set to the command line
	const LPCTSTR _kLUStart		= "LUStart";

	// Pass this string to RegisterWindowMessage.  When
	// LiveUpdate begins the download, a system-wide
	// message will be sent.
	const LPCTSTR _kLUStartDownload		= "LUStartDownload";

	// Pass this string to RegisterWindowMessage.  When
	// LiveUpdate completes the download, a system-wide
	// message will be sent.
	const LPCTSTR _kLUEndDownload		= "LUEndDownload";

	// Pass this string to RegisterWindowMessage.  When the
	// DoOnramp function is complete, a system-wide message
	// will be sent with the WPARAM set to one of the Onramp
	// status codes
	const LPCTSTR _kLUComplete		= "LUComplete";

	// Pass this string to RegisterWindowMessage.  When the
	// DoOnramp function is complete, a message will be sent
	// with the WPARAM set to a LiveUpdate error code and
	// the LPARAM set to a technical error code if one exists
	const LPCTSTR _kLUError			= "LUError";
}


//the Wizard Auto Mode Callback function
typedef	BOOL (WINAPI *OAMCALLBACK)(LPTSTR buffer, WORD sizeOfBuffer, WORD status, LPTSTR title, DWORD *userData);

typedef struct SOnrampAutoModeItemTAG
{
	int					fBitmap;					//resource ID of bitmap (bitmap in resources for SymMal32.DLL)
	OAMCALLBACK			fCallback;					//callback function to fill in text buffer
	TCHAR				fTitle[OAM_MAXSTRING];		//the Wizard bold title
	TCHAR				fCaption[OAM_MAXSTRING];	//the panel's caption
} SOnrampAutoModeItem;

typedef struct SOnrampAutoModeTAG
{
	SOnrampAutoModeItem	fWelcome;					//customization info for welcome panel
	SOnrampAutoModeItem	fFinish;					//customization info for finish panel
} SOnrampAutoMode, FAR *LPSOnrampAutoMode;

extern "C" __declspec(dllexport) int DoOnramp(HINSTANCE hInst, HWND hWnd, LPTSTR cmdLine, LPSOnrampAutoMode onrampAutoMode);
extern "C" __declspec(dllexport) LONG SetSequenceNumber(LPCTSTR productName, DWORD sequenceNumber);
//extern "C" __declspec(dllexport) LONG OnrampCleanup(LPCTSTR productName);
extern "C" __declspec(dllexport) LONG SetSequenceNumberEx(LPCTSTR productName, 
														  LPCTSTR productType, 
														  LPCTSTR productVersion, 
														  LPCTSTR productLanguage, 
														  DWORD sequenceNumber);
extern "C" __declspec(dllexport) LONG OnrampCleanupEx(LPCTSTR productName, 
													  LPCTSTR productVersion, 
													  LPCTSTR productLanguage);

// ***************************** Product Strings *******************************
#define IDS_LANGUAGE		4500
// *************************** Error Message Codes *****************************

#define TECHBASE						5000

// a zero status means everything is OK
#define OK					0

#define MIN_ERROR			-1
// Internal errors
#define MIN_INTERNALERROR	-2
#define OUTMEM_PORT			-2		// Unable to allocate Comm Port object
#define OUTMEM_CLINK		-3		// Unable to allocate Comm Link object
#define OUTMEM_MDM			-4		// Unable to allocate Modem Control object
#define OUTMEM_ZMODEM		-5		// Unable to allocate ZMODEM File Transfer object
#define OUTMEM_MONITOR		-6		// Unable to allocate File Transfer Monitor 
#define OUTMEM_SCRIPT		-7		// Unable to allocate memory for copy of script command
#define OUTMEM_DQUEUE		-8
#define OUTMEM				-9
#define COMTHREADERR		-10		// Comm thread execution error
#define COMTHRDWAITFAIL		-11		// Wait for event in comm thread failed
#define FTP_THRDWAITFAIL	-12		// Wait for event in Ftp thread failed
#define LSV_THRDWAITFAIL	-13		// Wait for event in Ftp thread failed
#define FTP_THREADERROR		-14		// error creating FTP connect handler thread
#define LSV_THREADERROR		-15		// error creating FTP connect handler thread
#define RESOURCEERR			-16		// error creating signal event
#define LOCFILECREATEERROR	-17		// error opening/creating local file
#define LOCFILEPATHTOOLONG	-18		// requested receive file path is too long
#define REMFILEPATHTOOLONG	-19
#define TEMPDIRNOTFOUND		-20		// windows was unable to find a temp directory
#define TEMPDIRCREATEERROR	-21		// unable to create a temp directory
#define NOTENUFTEMPSPACE	-22		// not enough space for temp files
#define TAPI_NOTAVAILABLE	-23
#define NOTAPILOCATIONFOUND	-24
#define MAX_INTERNALERROR	-24

// comm port problems
#define MIN_COMPORTERROR	-25
#define COMINUSE			-25		// comm port is already in use by someone else
#define COMOPENERROR		-26		// Error opening comm port
#define COMSETUPERROR   	-27 	// Comm port setup error
#define RECVTIMEOUT			-28		// Timeout receiving a char from comm port
#define COMWRITEERROR		-29		// Unable to send data to comm port
#define RPORTNOTOPEN		-30		// Attempt to write to a comm port that is not open
#define SPORTNOTOPEN		-31		// Attempt to write to a comm port that is not open
#define COMOVERFLOWERROR	-32		// Comm port overflow error
#define COMOVERRUNERROR		-33		// Comm port overrun error
#define COMFRAMEERROR		-34 	// Comm port framing error
#define COMPARITYERROR		-35 	// Comm port parity error
#define COMBREAKRECVD		-36 	// Break received at comm port
#define COMBUFFERFULL		-37		// Comm port buffer is full
#define COMISINVALID		-38		// Specified comm port is invalid
#define COMCLOSEERROR		-39		// Error closing comm port
#define MAX_COMPORTERROR	-39

// modem problems
#define MIN_MODEMERROR		-41
#define NOMODEMFOUND		-41		// No Modem was found
#define MODEMINITERROR		-42		// Modem initialization error
#define MODEMDIALERROR		-43		// Modem dial error
#define CONNECTFAILED		-44		// Modem connection to host failed
#define CONNECTFAILNOCARR	-45		// Modem failed to connect to host
#define CONNECTFAILBUSY		-46		// Modem connect to host failed because of busy signal
#define CONNECTFAILNODTONE	-47		// Modem connect to host failed because of no dial tone
#define CONNECTFAILTIMEOUT	-48 	// Modem connect to host failed because of no answer
#define RETRYCONNECTNOCARR	-49		// Modem failed to connect - will retry
#define RETRYCONNECTBUSY	-50		// Modem connect failed due of busy signal - will retry
#define RETRYCONNECTNODTONE	-51		// Modem connect failed due to no dial tone	- will retry
#define RETRYCONNECTTIMEOUT	-52 	// Modem connect failed because of no answer - will retry
#define NOCARRIER			-53 	// Modem is not connected
#define NOTINITIALIZED		-54 	// Attempt to dial before modem has been initialized
#define MODEMINUSE			-55		// Attempt to dial when modem is already being used
#define RNOTCONNECTED		-56 	// Attempt to read from a modem port that is not connected
#define DNOTCONNECTED		-57 	// Attempt to download on modem port that is not connected
#define IPORTNOTOPEN		-58 	// Attempt to initialize modem on comm port that is not open
#define FILERECVERR			-59 	// File Receive error
#define MODEMNOTREADY		-60 	// Modem is not ready ( DSR line low )
#define LINKERR				-61 	// Generic Problem accessing comm port or modem
#define MODEMINPUTERROR		-62 	// Error reading input from the modem
#define MAX_MODEMERROR		-62

// script file problems
#define MIN_SCRIPTFILEERROR	-65
#define SFILEOPENERR		-65		// Script file open error
#define SFILEREADERR		-66		// Script file read error
#define SLINETOOLONG		-67		// Script file line too long
#define SCRIPTNOTFOUND		-68		// Named script not found in script file
#define UNMATCHEDELSE		-69		// Found else not matched to if statement
#define UNMATCHEDENDIF		-70		// Found endif not matched to if statement
#define NOENDIF				-71		// If statement without matching endif
#define INVALIDCMD			-72		// Invalid command found
#define MAX_SCRIPTFILEERROR	-72

// errors returned by FTP and local server HALs 
#define MIN_FTPERROR		-75
#define INETOPENERROR		-75		// error initializing internet interface
#define FTPOPENERROR		-76		// error opening an FTP connection
#define LOCSERVNOTAVAIL		-77		// local server was not available
#define FTP_REMFILEOPENERR	-78		// open of remote file on FTP link failed
#define LSV_REMFILEOPENERR	-79		// open of remote file on LAN link failed
#define FTP_READREMFILEERR	-80		// error reading from remote file during FTP file transfer
#define LSV_READREMFILEERR	-81		// error reading from remote file during LAN file transfer
#define FTP_WRITELOCFILEERR	-82		// error writing to local file during FTP file transfer
#define LSV_WRITELOCFILEERR	-83		// error writing to local file during LAN file transfer
#define FTP_REMFILENOTFOUND	-84		// FTP transfer failed because the remote file was not found
#define LSV_REMFILENOTFOUND	-85		// LAN transfer failed because the remote file was not found
#define FTP_FILESMISSING	-86		// One or more necessary FTP files were missing
#define LSV_REMPATHINVALID	-87
#define LSV_REMPATHNOTAVAIL	-88
#define MAX_FTPERROR		-88

#define MIN_DISKSPACEERROR	-91
#define CURDIRERROR			-91		// the current directory could not be determined
#define DRIVENOTVALID		-92		// attempt to acces a drive that does not exist
#define FREESPACEERROR		-93		// could not determine the available disk space on a drive
#define NOTENUFDISKSPACE	-94		// not enough disk space available to download a file
#define MAX_DISKSPACEERROR	-94		

// HAL API problems
#define MIN_APIERROR		-101
#define OP_ABORT			-101	// Operator abort
#define ALR_INUSE			-102 	// Comm port is already in use
#define COMMLINKBUSY		-103 	// Attempted operation on Comm link that was already busy
#define FTP_NOTCONNECTED	-104	// attempt to do FTP or internet function when not yet connected
#define LSV_NOTCONNECTED	-105	// attempt to do local network function when server not available 
#define DIN_USE				-106	// download already in use
#define BADPHONENUM			-107	// a bad phone number was passed to us
#define INVALIDTARGETDIR	-108	// specified target directory not valid
#define INVALIDHOST			-109	// could not get data from Host file
#define MAX_APIERROR		-109

// script processing problems  - these definitions are tentative
// the actual values can determined by the script itself
#define SC_ERRORBASE		-120
#define SC_UNKNOWNERROR		SC_ERRORBASE
#define SC_LOGINERROR		-121	// Error logging into host BBS
#define SC_PSWDERROR		-122	// Password is invalid
#define SC_NAVIGATEERROR	-123	// BBS menu navigation error
#define SC_FILENOTFOUND		-124	// Cannot find a file ( probably to download )
#define SC_MAXDEFINEDERROR	-124

#define COMMDEVICENOTFOUND	-150	// No suitable communications device found
#define HALNOTINITIALIZED	-151	// Unable to initialize HAL module
#define STARTDOWNLOADFAILED -152	// Failure connecting

#define TRINOTRECEIVED		-160	// Error getting SYMTRI.ZIP from host
#define UNZIPENGINENOTFOUND	-161	// Unable to find unzip engine (SYMUZP32.DLL)
#define HASLATESTPACKAGE	-162	// CPackageList found latest package already instantiated.
#define TRINOTINSTANTIATED	-163	// Data corruption error (object COnramp)
#define TRIUNZIPERROR		-164	// Data corruption error (zip)

// DIS problems
#define PATHREGKEYNOTFOUND	-165	// The registry key for determining the path to copy to was not found.
#define DISSYNTAXERROR		-166	// Syntax error in the DIScript
#define DISNOTOKEN			-167	// No token exists by name given
#define DISNOFILEINUSEHANDLING	-168// In some versions of NT 3.51, MoveFileEx fails without giving an error

#define MAX_ERROR			-500

// Generic problems - user messages
#define IDS_FAILALLCONNECT				3078
#define IDS_MISSINGINTERNETFILES		3085
#define IDS_INTERNALERROR				3032
#define IDS_MEMORYERROR					3036
// Generic problems - technical messages
#define IDS_OUTMEM						OUTMEM + TECHBASE
#define IDS_INVALIDHOST					INVALIDHOST + TECHBASE

// Modem problems - user messages
#define IDS_CONNECTAGAIN				3038
#define IDS_BUSY						3039
#define IDS_NOANSER						3040
#define IDS_NOPHONELINE					3041
#define IDS_FAILDURINGDOWNLOAD			3042
#define IDS_MODEMDOWN					3043
#define IDS_PORTFAILURE					3090
// Modem problems - technical messages
#define IDS_NOMODEMFOUND				NOMODEMFOUND + TECHBASE
#define IDS_MODEMINITERROR				MODEMINITERROR + TECHBASE
#define IDS_MODEMDIALERROR				MODEMDIALERROR + TECHBASE
#define IDS_CONNECTFAILED				CONNECTFAILED + TECHBASE
#define IDS_CONNECTFAILNOCARR			CONNECTFAILNOCARR + TECHBASE
#define IDS_CONNECTFAILBUSY				CONNECTFAILBUSY + TECHBASE
#define IDS_CONNECTFAILNODTONE			CONNECTFAILNODTONE + TECHBASE
#define IDS_CONNECTFAILTIMEOUT			CONNECTFAILTIMEOUT + TECHBASE
#define IDS_RETRYCONNECTNOCARR			RETRYCONNECTNOCARR + TECHBASE
#define IDS_RETRYCONNECTBUSY			RETRYCONNECTBUSY + TECHBASE
#define IDS_RETRYCONNECTNODTONE			RETRYCONNECTNODTONE + TECHBASE
#define IDS_RETRYCONNECTTIMEOUT			RETRYCONNECTTIMEOUT + TECHBASE
#define IDS_NOCARRIER					NOCARRIER + TECHBASE
#define IDS_NOTINITIALIZED				NOTINITIALIZED + TECHBASE
#define IDS_RNOTCONNECTED				RNOTCONNECTED + TECHBASE
#define IDS_IPORTNOTOPEN				IPORTNOTOPEN + TECHBASE
#define IDS_FILERECVERR					FILERECVERR + TECHBASE
#define IDS_MODEMNOTREADY				MODEMNOTREADY + TECHBASE
#define IDS_MODEMINPUTERROR				MODEMINPUTERROR + TECHBASE
#define IDS_LINKERR						LINKERR + TECHBASE

// Comm port problems - user messages
#define IDS_MODEMINUSE					3044
#define IDS_MODEMDRIVERFAIL				3045
#define IDS_CLOSEAPPS					3046
#define IDS_NOMODEMDRIVER				3094
#define IDS_NOLOCATION					3108
// Comm port problems - technical messages
#define IDS_TMODEMINUSE					MODEMINUSE + TECHBASE
#define IDS_COMINUSE					COMINUSE + TECHBASE
#define IDS_COMOPENERROR				COMOPENERROR + TECHBASE
#define IDS_COMSETUPERROR				COMSETUPERROR + TECHBASE
#define IDS_RECVTIMEOUT					RECVTIMEOUT + TECHBASE
#define IDS_COMWRITEERROR				COMWRITEERROR + TECHBASE
#define IDS_RPORTNOTOPEN				RPORTNOTOPEN + TECHBASE
#define IDS_SPORTNOTOPEN				SPORTNOTOPEN + TECHBASE
#define IDS_COMOVERFLOWERROR			COMOVERFLOWERROR + TECHBASE
#define IDS_COMOVERRUNERROR				COMOVERRUNERROR + TECHBASE
#define IDS_COMFRAMEERROR				COMFRAMEERROR + TECHBASE
#define IDS_COMPARITYERROR				COMPARITYERROR + TECHBASE
#define IDS_COMBREAKRECVD				COMBREAKRECVD + TECHBASE
#define IDS_COMBUFFERFULL				COMBUFFERFULL + TECHBASE
#define IDS_COMISINVALID				COMISINVALID + TECHBASE
#define IDS_COMCLOSEERROR				COMCLOSEERROR + TECHBASE

// FTP and LAN HAL problems - user messages
#define IDS_RETRYCONNECT				3047
#define IDS_LOSTCONNECTION				3073
// FTP and LAN HAL problems - technical messages
#define IDS_INETOPENERROR				INETOPENERROR + TECHBASE
#define IDS_FTPOPENERROR				FTPOPENERROR + TECHBASE
#define IDS_FTP_REMFILENOTFOUND			FTP_REMFILENOTFOUND + TECHBASE
#define IDS_FTP_REMFILEOPENERR			FTP_REMFILEOPENERR + TECHBASE
#define IDS_LOCFILECREATEERROR			LOCFILECREATEERROR + TECHBASE
#define IDS_FTP_READREMFILEERR			FTP_READREMFILEERR + TECHBASE
#define IDS_FTP_WRITELOCFILEERR			FTP_WRITELOCFILEERR + TECHBASE
#define IDS_FTP_FILESMISSING			FTP_FILESMISSING + TECHBASE
#define IDS_LOCSERVNOTAVAIL				LOCSERVNOTAVAIL + TECHBASE
#define IDS_LSV_REMFILENOTFOUND			LSV_REMFILENOTFOUND + TECHBASE
#define IDS_LSV_REMFILEOPENERR			LSV_REMFILEOPENERR + TECHBASE
#define IDS_LSV_READREMFILEERR			LSV_READREMFILEERR + TECHBASE
#define IDS_LSV_WRITELOCFILEERR			LSV_WRITELOCFILEERR + TECHBASE
#define IDS_LSV_REMPATHINVALID			LSV_REMPATHINVALID + TECHBASE
#define IDS_LSV_REMPATHNOTAVAIL			LSV_REMPATHNOTAVAIL + TECHBASE
#define IDS_LSV_NOTCONNECTED			LSV_NOTCONNECTED + TECHBASE

// No script problem user messages:
// Script problems - technical messages
#define IDS_SFILEOPENERR				SFILEOPENERR + TECHBASE
#define IDS_SFILEREADERR				SFILEREADERR + TECHBASE
#define IDS_SLINETOOLONG				SLINETOOLONG + TECHBASE
#define IDS_SCRIPTNOTFOUND				SCRIPTNOTFOUND + TECHBASE
#define IDS_UNMATCHEDELSE				UNMATCHEDELSE + TECHBASE
#define IDS_UNMATCHEDENDIF				UNMATCHEDENDIF + TECHBASE
#define IDS_NOENDIF						NOENDIF + TECHBASE
#define IDS_INVALIDCMD					INVALIDCMD + TECHBASE

// No script processing user messages:
// Script processing problems - technical messages
#define IDS_SC_LOGINERROR				SC_LOGINERROR + TECHBASE
#define IDS_SC_PSWDERROR				SC_PSWDERROR + TECHBASE
#define IDS_SC_NAVIGATEERROR			SC_NAVIGATEERROR + TECHBASE
#define IDS_SC_FILENOTFOUND				SC_FILENOTFOUND + TECHBASE

// Disk space problem user messages:
#define IDS_NEEDMORESPACE				3051
#define IDS_NEEDMORESPACEHELP			3200
// Disk space problems - technical messages
#define IDS_CURDIRERROR					CURDIRERROR + TECHBASE
#define IDS_DRIVENOTVALID				DRIVENOTVALID + TECHBASE
#define IDS_FREESPACEERROR				FREESPACEERROR + TECHBASE
#define IDS_NOTENUFDISKSPACE			NOTENUFDISKSPACE + TECHBASE

// No HAL internal problem user messages:
// HAL internal problems - technical messages
#define IDS_OUTMEM_PORT					OUTMEM_PORT + TECHBASE
#define IDS_OUTMEM_CLINK				OUTMEM_CLINK + TECHBASE
#define IDS_OUTMEM_MDM					OUTMEM_MDM + TECHBASE
#define IDS_OUTMEM_ZMODEM				OUTMEM_ZMODEM + TECHBASE
#define IDS_OUTMEM_MONITOR				OUTMEM_MONITOR + TECHBASE
#define IDS_OUTMEM_DQUEUE				OUTMEM_DQUEUE + TECHBASE
#define IDS_OUTMEM_SCRIPT				OUTMEM_SCRIPT + TECHBASE
#define IDS_COMTHREADERR				COMTHREADERR + TECHBASE
#define IDS_COMTHRDWAITFAIL				COMTHRDWAITFAIL + TECHBASE
#define IDS_FTP_THRDWAITFAIL			FTP_THRDWAITFAIL + TECHBASE
#define IDS_LSV_THRDWAITFAIL			LSV_THRDWAITFAIL + TECHBASE
#define IDS_FTP_THREADERROR				FTP_THREADERROR + TECHBASE
#define IDS_LSV_THREADERROR				LSV_THREADERROR + TECHBASE
#define IDS_RESOURCEERR					RESOURCEERR + TECHBASE
#define IDS_LOCFILEPATHTOOLONG			LOCFILEPATHTOOLONG + TECHBASE
#define IDS_REMFILEPATHTOOLONG			REMFILEPATHTOOLONG + TECHBASE

// HAL API user messages:
#define IDS_CHECKNETCONNECT				3056
#define IDS_LANHALGENERICFAIL			3093

// HAL API problems - technical messages
#define IDS_OP_ABORT					OP_ABORT + TECHBASE
#define IDS_ALR_INUSE					ALR_INUSE + TECHBASE
#define IDS_DNOTCONNECTED				DNOTCONNECTED + TECHBASE
#define IDS_COMMLINKBUSY				COMMLINKBUSY + TECHBASE
#define IDS_DIN_USE						DIN_USE + TECHBASE
#define IDS_FTP_NOTCONNECTED			FTP_NOTCONNECTED + TECHBASE
#define IDS_BADPHONENUM					BADPHONENUM + TECHBASE
#define IDS_INVALIDTARGETDIR			INVALIDTARGETDIR + TECHBASE

// MALHAL Initialize user messages:
#define IDS_REINSTALL					3052
#define IDS_RESTARTWINDOWS				3053
#define IDS_RUNONLYONCE					3055
// MALHAL Initialize technical messages:
#define IDS_COMMDEVICENOTFOUND			COMMDEVICENOTFOUND + TECHBASE
#define IDS_HALNOTINITIALIZED			HALNOTINITIALIZED + TECHBASE
#define IDS_STARTDOWNLOADFAILED			STARTDOWNLOADFAILED + TECHBASE

// Data Retrieval user messages:
#define IDS_TRYAGAINLATER				3054
#define IDS_CORRUPTEDBYDL				3081
// Data Retrieval technical messages:
#define IDS_TRINOTRECEIVED				TRINOTRECEIVED + TECHBASE
#define IDS_UNZIPENGINENOTFOUND			UNZIPENGINENOTFOUND + TECHBASE
#define IDS_HASLATESTPACKAGE			HASLATESTPACKAGE + TECHBASE
#define IDS_TRINOTINSTANTIATED			TRINOTINSTANTIATED + TECHBASE
#define IDS_TRIUNZIPERROR				TRIUNZIPERROR + TECHBASE

// DIS error user messages
#define IDS_INSTALLFAILS_THIS_OS		3201

// DIS error technical messages
#define IDS_PATHREGKEYNOTFOUND			PATHREGKEYNOTFOUND + TECHBASE
#define IDS_DISNOTOKEN					DISNOTOKEN + TECHBASE
#define IDS_DISSYNTAXERROR				DISSYNTAXERROR + TECHBASE
#define IDS_DISNOFILEINUSEHANDLING		DISNOFILEINUSEHANDLING + TECHBASE
// ***************************** End Error Message Codes **********************************



// ********************************* HAL Options ******************************************
#define IDS_AUTODETECT					3074
#define IDS_FIRSTHALSTRING				3075
#define IDS_FTP							3075
#define IDS_MODEM						3076
#define IDS_LAN							3077		// JRL
#define LASTHALSTRING					3078
#define IDS_CONNECTOPTIONS				3082
#define IDS_CONNECTFTP					3083


//  ***************************************************************************************

#define LAST_STRING_VALUE				3201


#endif


