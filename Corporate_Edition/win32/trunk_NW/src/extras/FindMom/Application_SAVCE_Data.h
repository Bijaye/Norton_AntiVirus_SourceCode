// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef __APPLICATION_SAVCE_DATA_H__
#define __APPLICATION_SAVCE_DATA_H__

#include <objbase.h>

// SAVCE specific constants and functions pulled from product source.

// This data has been lifted verbatim from the following SAVCE source files:
// //depot/Norton_AntiVirus/Corporate_Edition/Win32/trunk/src/EMailScanners/Notes/Shared/install.h:1
// //depot/Norton_AntiVirus/Corporate_Edition/Win32/trunk/src/EMailScanners/Notes/Shared/install.c:1
// //depot/Core_Technology/LDVP_Shared/Trunk2/src/include/vpcommon.h:3
// //depot/Core_Technology/LDVP_Shared/Trunk2/src/include/ClientReg.h:2
// //depot/Norton_AntiVirus/Corporate_Edition/Common/trunk/src/PScan/classes.h:10
// //depot/Core_Technology/LDVP_Shared/Trunk2/src/include/cba.h:1

//
// Registry keys
//

#ifdef WIN64
	#define szReg_Key_Main					"Software\\Wow6432Node\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion"
#else
	#define szReg_Key_Main					"Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion"
#endif // WIN64

#define szReg_Key_DomainData				"DomainData"
#define szReg_Key_RTS						"RealTimeScan"
#define szReg_Key_LocalScans				"LocalScans"
#define szReg_Key_ScheduleKey				"Schedule"
#define szReg_Key_PatternManager			"PatternManager"
#define szReg_Key_ProductControl			"ProductControl"
#define szReg_Key_VirusSweep				"VirusSweep"

#define szReg_Key_Storages					"Storages"
#define szReg_Key_Storage_File				"FileSystem"
#define szReg_Key_Storage_Exchange			"MicrosoftExchangeClient"
#define szReg_Key_Storage_Notes				"LotusNotes"

//
// Registry values
//

#define szReg_Val_Type						"Type"
#define szReg_Val_Parent					"Parent"
#define szReg_Val_HomeDir					"Home Directory"
#define szReg_Val_ProductVersion			"ProductVersion"

#define szReg_Val_AgentIPPort				"AgentIPPort"

#define szReg_Value_Name					"Name"
#define szReg_Value_ScanStatus				"Status"
#define szReg_Val_MessageBox				"MessageBox"
#define szReg_Value_DisplayStatusDlg		"DisplayStatusDialog"
#define szReg_Val_Enabled					"Enabled"
#define szReg_Val_Schedule_LastStart		"LastStart"
#define szReg_Val_ScanAllDrives				"ScanAllDrives"
#define szReg_Val_RunNow					"YES"
#define szReg_Val_TimeOfLastScan			"TimeOfLastScan"

#define szReg_Val_UpdateFromParent			"UpdateClients"

#define szReg_Val_StartDownLoadNow			"StartDownLoadNow"
#define szReg_Val_ProcessGRCNow				"ProcessGRCNow"

#define szReg_Val_RTSScanOnOff				"OnOff"
#define szReg_Val_Heuristics				"Heuristics"
#define szReg_Val_HeuristicsLevel			"HeuristicsLevel"
#define szReg_Val_ScanOnAccess				"Reads"
#define szReg_Val_ScanOnAccess2				"Execs"
#define szReg_Val_ScanOnModify				"Writes"

//
// File names
//

#define RTVSCAN_NT					"RTVSCAN.EXE"
#define RTVSCAN_9X					"RTVSCN95.EXE"
#define VIRSCAN_DEF_FILE			_T("VIRSCAN1.DAT")
#define SHFOLDER_DLL				_T("\\SHFOLDER.DLL")

//Functions
#ifdef _UNICODE
	#define SH_GET_FOLDER_PATH "SHGetFolderPathW"
#else
	#define SH_GET_FOLDER_PATH "SHGetFolderPathA"
#endif

//
// Directories
//
#define SYMANTEC_COMMON_DATA_DIR    "Symantec\\Symantec AntiVirus Corporate Edition"

//Included for backward compatibility with legacy products.
#define NORTON_COMMON_DATA_DIR		"Symantec\\Norton AntiVirus Corporate Edition"

#define HARD_CODED_SYMANTEC_COMMON_DATA_DIR		_T("C:\\Documents and Settings\\All Users\\Application Data\\Symantec\\Symantec AntiVirus Corporate Edition\\7.5")
#define SYMANTEC_DEFSET_DIR_TEMPLATE			_T("%s\\Common Files\\Symantec Shared\\VirusDefs\\%02d%02d%02d.%03d")	
//
// Scan status constants
//

#define S_STARTING        1
#define S_STARTED         2
#define S_STOPPING        3
#define S_DONE            4
#define S_SCANNING_DIRS   5
#define S_SCANNING_BOOT   6
#define S_SCANNING_MEM    7
#define S_SCANNING_FILES  8
#define S_NEVER_RUN       9
#define S_ABORTED         10
#define S_QUEUED          11
#define S_DELAYED         12
#define S_SCANNING_PROCS  13

//
// Event types
//

#define GL_EVENT_PATTERN_DOWNLOAD 16

//
// Lotus Notes client detection
//

#define NOTES_NOT_INSTALLED     0
#define NOTES4_INSTALLED        4
#define NOTES5_INSTALLED        5

#define SERVERSETUP			_T("ServerSetup")
#define CLIENTSETUP			_T("Setup")
#define NOTESSECTION		_T("Notes")

#define S_CLIENT			0x1
#define S_SERVER			0x2

DWORD IsNotesClient4Installed();
DWORD IsNotesClient5Installed(LPTSTR lpsNotesPath);
DWORD GetNotesInstallPath(LPTSTR installPath, DWORD maxLen);

//
// CBA strutures
//

#define CBA_PROTOCOL_IP        0
#define CBA_PROTOCOL_IPX       1

/**********************************************************************
;  CBA.H -- TYPEDEF -- CBA_IPX - CBA IPX Info (ORDER MUST NOT CHANGE) !!!
;*********************************************************************/
typedef struct {
  unsigned long  ipxNet;
  unsigned char  ipxNode[6];
}CBA_IPX;

/**********************************************************************
;  CBA.H -- TYPEDEF -- CBA_NETADDR
;*********************************************************************/
typedef struct {
  unsigned short   usPort;
  unsigned char    ucInternal;
  unsigned char    ucReserved;
  union {
    unsigned long  ipAddr;
    CBA_IPX        ipx;
    unsigned char  nbName[16];
  }netAddr;
}CBA_NETADDR;

/**********************************************************************
;  CBA.H -- TYPEDEF -- CBA_Addr  -- First TWO parms MUST NOT CHANGE
;*********************************************************************/
typedef struct {
  unsigned short maxPacketSize;   // Negotiated with Remote
  unsigned short addrSize;        // sizeof(CBA_Addr)
  CBA_NETADDR    dstAddr;
  unsigned char  macAddr[6];
  unsigned char  ucFlags;
  unsigned char  ucProtocol;
}CBA_Addr;


//
// COM interfaces and GUIDs
//

interface IVirusProtect : public IUnknown
{
	virtual HRESULT __stdcall CreateByIID( const IID& iid, void** ppv ) = 0;  			// creates a new object based on the Class of the current object;
	virtual HRESULT __stdcall ControlDLL( DWORD p1, DWORD p2 ) = 0;						// misc control functions
	virtual HRESULT __stdcall SetEncriptionSeed( DWORD Seed )=0;						// must set or transman will not work
};

extern IID IID_IVirusProtect;

// fwd declaration of interface we don't support
interface IScanCallback;

interface IConsoleMisc : public IVirusProtect
{
	virtual ULONG __stdcall LoadCallback( interface IScanCallback* ScanCallback ) = 0;	// when you release the callback will also be released
	virtual ULONG __stdcall StartVirusNotify( char* Computer ) = 0;						// tell a computer that you want virus events
	virtual ULONG __stdcall StopVirusNotify( char* Computer ) = 0;						// tell a computer that you don't want virus events
	virtual ULONG __stdcall SendControlMessage( char* Computer, DWORD MessageID ) = 0;	// send a control message to a computer
	virtual ULONG __stdcall GetAddressingInfo( char* Computer, DWORD* Count, DWORD* prots, CBA_Addr* AddressList ) = 0;
																						// get a computers addressing info.
	virtual ULONG __stdcall RemoveClient( char* Computer ) = 0;							// I am a client, Remove Me.
};

extern CLSID CLSID_CliProxy;
extern CLSID CLSID_Transman;

extern IID IID_IConsoleMisc;
extern IID IID_IVirusProtect;

//
// Transman exports
//

#define TRANSMAN_DLL_NAME	"Transman.dll"

typedef DWORD (*SENDVIRUSALERT)( void* EventBlock );
typedef DWORD (*RETURNPONG)( char* Server, char* Doamin, char* Mom, void* Pong );

#define ORDINAL_INITTRANSMAN	127
typedef DWORD (*INITTRANSMAN)( SENDVIRUSALERT cb, RETURNPONG cb2 );

#define ORDINAL_CVTCBA2GRC		161
typedef DWORD (*CVTCBA2GRC)( DWORD );

#define ORDINAL_DEINITTRANSMAN	140
typedef DWORD (*DEINITTRANSMAN)( DWORD );

#define ORDINAL_QUICKFIND		170
typedef DWORD (*QUICKFINDALLLOCALCOMPUTERS)( void );
//
//	SendCOM_REMOVE_PROGRAM commands
//

#define SERVER_SERVICE_RELOAD_PONG_DATA  2

#endif // __APPLICATION_SAVCE_DATA_H__

