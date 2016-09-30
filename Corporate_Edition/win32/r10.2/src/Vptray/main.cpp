// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////////
//  FILE MAIN.CPP
//
//	Handle main message procceding for the Tray Application
//
//
//
////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <comdef.h>
#include <winioctl.h>
#include <string>
#include <atlsecurity.h>

#include "resource.h"
#include "clientreg.h"
#include "vpcommon.h"
#include "language.c"
#include "terminalsession.h"
#include "savrtmoduleinterface.h"
#include "user.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"
#include "LMCons.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "OSMigrat.h" //EA for Apllication migration of Win2K
#ifdef __cplusplus
}
#endif
#include "navver.h"
#include "sharedver.h"
#include "shfolder.h"

#include "oem_obj.h"

// these come from vptray.cpp
extern void OnOpen();
extern void OnRealTime();
extern void OnStart();
extern void OnStop();
extern HWND g_hNotifyPat;
extern HWND g_hNotifyMissingPats;



//information used to determine if the drive is a floppy
//from KB:Q163920
#ifndef VWIN32_DIOC_DOS_IOCTL
#define VWIN32_DIOC_DOS_IOCTL 1

typedef struct _DIOC_REGISTERS {
  DWORD reg_EBX;
  DWORD reg_EDX;
  DWORD reg_ECX;
  DWORD reg_EAX;
  DWORD reg_EDI;
  DWORD reg_ESI;
  DWORD reg_Flags;
}DIOC_REGISTERS, *PDIOC_REGISTERS;

#endif

// Intel x86 processor status fla
#define CARRY_FLAG  0x1


#pragma pack(1)
typedef struct _DOSDPB {
  BYTE    specialFunc;    // 
  BYTE    devType;        // 
  WORD    devAttr;        // 
  WORD    cCyl;           // number of cylinders
  BYTE    mediaType;      // 
  WORD    cbSec;          // Bytes per sector
  BYTE    secPerClus;     // Sectors per cluster
  WORD    cSecRes;        // Reserved sectors
  BYTE    cFAT;           // FATs
  WORD    cDir;           // Root Directory Entries
  WORD    cSec;           // Total number of sectors in image
  BYTE    bMedia;         // Media descriptor
  WORD    secPerFAT;      // Sectors per FAT
  WORD    secPerTrack;    // Sectors per track
  WORD    cHead;          // Heads
  DWORD   cSecHidden;     // Hidden sectors
  DWORD   cTotalSectors;  // Total sectors, if cbSec is zero
  BYTE    reserved[6];    // 
} DOSDPB, *PDOSDPB;
#pragma pack()

//IBARILE port CRT fix for Siebel Defect #1-O5GS2
void SetLoggedOff(DWORD dLoggedOff, bool bAll = false);
//IBARILE end CRT port

#define NAV_COMMON_APP_DATA     1
#define NAV_USER_APP_DATA       2

// Fix to defect 1-AV08P:  systray icon not recreated if shell restarts
// The following message is sent whenever the taskbar is re-created (i.e., when a user logs off and somebody logs back on) - not in our platform SDK
#define MESSAGE_NAME_TASKBAR_CREATED    _T("TaskbarCreated")

// SHFOLDER.DLL
typedef HRESULT (WINAPI *PFNSHGetFoldPathA)(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath);

HINSTANCE g_hInstance=0, // Application Instance Handles
		  g_hResource=0;

HICON g_hIcons16[4];	// 16x16 Tray icons
HICON g_hIcons32[4];	// 32x32 Tray icons

HMENU g_hMenu=0;		// handle to the menu that services the tray
HWND  g_hMain=0;		// handle to the window that services the tray

DWORD g_IconState;		// icon State
DWORD g_bShowIcon=0;	// should we show the icon
BOOL  g_bRunning=TRUE;  // is the tray running
BOOL  g_bMenuInited=FALSE;	//
BOOL  g_bShellResult=FALSE;	// did we fail to add to the shell?
char  g_szLang[4];
BOOL  g_bShowTray=TRUE;
BOOL  g_bLaunchedByInstall = FALSE; // did the installer launch us?
UINT  g_iTaskbarCreatedMsg = 0;     // Fix to defect 1-AV08P:  systray icon not recreated if shell restarts

HANDLE g_hEvent = NULL;
BOOL  g_bUserCanStopRTS = TRUE;
HMODULE g_hAdvapi = NULL;

SC_HANDLE g_schSCManager = NULL;
SC_HANDLE g_schService = NULL;

UINT g_nMainTimerID=0;
UINT g_nDelay = 1000;

const UINT IDTRAY =777;

// Custom Tray Messages
const UINT CM_TRAY_NOTIFY = WM_USER+101;
const UINT CM_TRAY_ACTION = WM_USER+102;
const UINT CM_START = WM_USER+10;


//
// Local function prototypes
//
BOOL SkipShutDownFloppyCheck();
DWORD GetSystemDataDir( DWORD dwFlags, LPTSTR lpSystemDataDir );
DWORD CreateUserDirectory(LPTSTR lpDataDirectory);

//
// Functions for starting process as user.
//
std::string g_lastErr;
enum SecObjType { objType_WindowStation, objType_Desktop };

bool LaunchAsCurrentUser(const TCHAR* commandLine);
bool GetLogonSID(HANDLE hToken, PSID *ppsid);
void FreeLogonSID(PSID *ppsid);
bool AddTokenPrivileges(HANDLE hToken);
bool AddAceToObject(HANDLE hObj, PSID psid, SecObjType objType);
bool StartInteractiveClientProcess (HANDLE hToken,
									const char* appName,
									const char* cmdLine,
									const char* directory);


UINT g_nTimerID;
UINT g_nExpire = 10;
UINT g_seconds = 0;
UINT g_PopDalay = 62;
UINT g_ToTalDalay = 800;
UINT g_nTimer = 0;

LRESULT CALLBACK NotifyDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (OEMObj.IsOEMBuild())
	{
		if (!OEMObj.IsOEM_Initialized())
			return FALSE;
	}

    static x,y;
    static nWidth = 375,nHeight = 10;
    static RECT rc;
	TCHAR SAVCaption[MAX_PATH];
    TCHAR DisabledText[MAX_PATH];
    TCHAR EnabledText[MAX_PATH];
	static BOOL bEnabledDisabled;
    PAINTSTRUCT ps ;
    HBRUSH      hBrush ;
    HDC         hdc ;

    switch(uMsg)
    {


        case WM_INITDIALOG:
        {

			bEnabledDisabled = (BOOL) lParam;
            g_nTimerID = SetTimer(hWnd, 1, g_nExpire, NULL);
            SystemParametersInfo(SPI_GETWORKAREA,0,&rc,0);
            x = rc.right - 375;
            y = rc.bottom - 10;
            MoveWindow (hWnd, x , y , nWidth , nHeight, TRUE) ;
            ShowWindow(hWnd, SW_SHOW);
            break;
        }
        case WM_TIMER:
        {
			if(wParam == g_nTimerID)
			{
				g_seconds++;
				if(g_seconds > g_ToTalDalay)
				{
					KillTimer(hWnd, g_nTimerID);
					DestroyWindow(hWnd);
				}
				if( g_seconds < g_PopDalay )
				{
					y--;
					nHeight++;
					SendMessage(hWnd,WM_PAINT,0,0);
				}

				if( g_seconds > (g_ToTalDalay - g_PopDalay) )
				{
					y++;
					nHeight--;
					SendMessage(hWnd,WM_PAINT,0,0);
				}

			}
            break;
        }
        case WM_PAINT:
        {

            MoveWindow (hWnd, x , y, nWidth ,nHeight,TRUE) ;

			hdc = BeginPaint (hWnd, &ps) ;
            GetClientRect (hWnd, &rc) ;
            hBrush = CreateSolidBrush (RGB(255,255,0)) ;
            FillRect (hdc, &rc, hBrush) ;
            SetTextColor(hdc, RGB(0,0,0));
            SetBkColor(hdc, RGB(255,255,0));
			LoadString(g_hResource,IDS_APP_TITLE,SAVCaption,MAX_PATH);
            TextOut (hdc, 60, 10,SAVCaption,lstrlen (SAVCaption)) ;
			LoadString(g_hResource,IDS_APP_ENABLE,EnabledText,MAX_PATH);
			LoadString(g_hResource,IDS_APP_DISABLE,DisabledText,MAX_PATH);
            TextOut (hdc, 40, 30,bEnabledDisabled?EnabledText:DisabledText, lstrlen (bEnabledDisabled?EnabledText:DisabledText)) ;
			DrawIcon(hdc,4,15, LoadIcon(g_hInstance, (bEnabledDisabled?MAKEINTRESOURCE(IDI_TRAY_ON_YELLOW):MAKEINTRESOURCE(IDI_TRAY_OFF_YELLOW))));
            EndPaint (hWnd, &ps) ;
            DeleteObject (hBrush) ;
            break;
        }
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////
//	Show or Hide the tray Icon
void ShowIcon(BOOL bShow)
{
	if (bShow && g_bShowIcon==0)
	{
		g_bShowIcon=1;
		SendMessage(g_hMain,CM_TRAY_ACTION,	NIM_ADD ,g_IconState);
	}
	else if (!bShow && g_bShowIcon==1)
	{
		SendMessage(g_hMain,CM_TRAY_ACTION,	NIM_DELETE ,0);
		g_bShowIcon=0;

	}
}


/////////////////////////////////////////////////////////////////////////////////
// Setup the Menu Items
void InitMenu()
{
	if (g_bMenuInited) return;
	g_bMenuInited = TRUE;



					// get a handle to the system menu
	HMENU hMenu = GetSystemMenu(g_hMain,FALSE);
	HMENU hPopup = GetSubMenu(g_hMenu,0);

	int c=0,i=2;
	// remove all menus except move and restore
	while ( c++< 100) // just make sure we don't infinit loop
		if (!RemoveMenu(hMenu,i,MF_BYPOSITION)) break;

	// modify remaining options to show that of vptray
	TCHAR sName[MAX_PATH];
	GetMenuString(hPopup, 0, sName, MAX_PATH,MF_BYPOSITION ); // open client
	ModifyMenu(hMenu,0,MF_BYPOSITION,SC_MOVE, sName );
	GetMenuString(hPopup, 1, sName,MAX_PATH, MF_BYPOSITION ); // enable RTS
    ModifyMenu(hMenu,1,MF_BYPOSITION,SC_CLOSE, sName );
}


LRESULT OnSysCommand(WPARAM wParam)
{


	switch (wParam)
	{

	case SC_MOVE:
	case SC_DEFAULT:
 		OnOpen();
		return 1;
    case SC_CLOSE:
		OnRealTime();
		return 1;
	}
	return 0;

}


//////////////////////////////////////////////////////////////
//
// bool IsFloppy()
//
// determines whether or not the removable device is a floppy
// this will prevent us from popping up a UI if other removable devices
// are attached to the OS
DWORD IsFloppy(int iDrive)
{
	HANDLE h;
	TCHAR tsz[8];
	DWORD dwRc;

	//convert drive to number...
	if( iDrive >= 'a' && iDrive < 'z' ){
		iDrive -= ('a' - 1);
	}else{
		iDrive -= ('A' - 1);
	}

	/*
	On Windows NT, use the technique described in the Knowledge
	Base article Q115828 and in the "FLOPPY" SDK sample.
	*/ 
	sssnprintf(tsz, sizeof(tsz), TEXT("\\\\.\\%c:"), TEXT('@') + iDrive);
	h = CreateFile(tsz, 0, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	if (h != INVALID_HANDLE_VALUE)
	{
		DISK_GEOMETRY Geom[20];
		DWORD cb;

		if (DeviceIoControl (h, IOCTL_DISK_GET_MEDIA_TYPES, 0, 0,
								Geom, sizeof(Geom), &cb, 0) && cb > 0)
		{
			switch (Geom[0].MediaType)
			{
			case F5_1Pt2_512: // 5.25 1.2MB floppy
			case F5_360_512:  // 5.25 360K  floppy
			case F5_320_512:  // 5.25 320K  floppy
			case F5_320_1024: // 5.25 320K  floppy
			case F5_180_512:  // 5.25 180K  floppy
			case F5_160_512:  // 5.25 160K  floppy
				dwRc = 525;
				break;

			case F3_1Pt44_512: // 3.5 1.44MB floppy
			case F3_2Pt88_512: // 3.5 2.88MB floppy
			case F3_20Pt8_512: // 3.5 20.8MB floppy
			case F3_720_512:   // 3.5 720K   floppy
				dwRc = 350;
				break;

			default:
				dwRc = 0;
				break;
			}
		}
		else
			dwRc = 0;

		CloseHandle(h);
	}
	else
		dwRc = 0;

	return dwRc;
}

////////////////////////////////////////////////////////////////////////////////////
//	The Tray has recived a Query EndSession
//
LRESULT OnShutDown()
{
	char szDrive[MAX_PATH];
	UINT dType=(UINT)-1;
	char *p=szDrive;

	// Check the SkipShutDownFloppyCheck key first...
	if(SkipShutDownFloppyCheck())
		return TRUE;

	ZeroMemory(szDrive,MAX_PATH);
	GetLogicalDriveStrings(MAX_PATH,szDrive);

    // Find a the lowest letter removable drive
	while (*p !='\0')
	{
		dType = GetDriveType(p);
		if (dType==DRIVE_REMOVABLE && IsFloppy(*p) )
            break;
		while (*p != '\0')
            p++;
		p++;
	}
	if( dType != DRIVE_REMOVABLE )
		return TRUE;

    UINT oldMode = SetErrorMode(SEM_FAILCRITICALERRORS);

	char sError[1024],sTitle[MAX_PATH];

	// Don't allow shutdown until they remove it.
	if (GetFileAttributes(p) != (DWORD) -1)
	{
		LoadString(g_hResource,IDS_SHUTDOWNWARNING,sError,1024);
		LoadString(g_hResource,IDS_APP_TITLE,sTitle,MAX_PATH);
		MessageBox(g_hMain,sError,sTitle, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
        // Maybe the this is a laptop with the floppy disconnected
        // so display an alternate message
    	while (GetFileAttributes(p) != (DWORD) -1)
        {
		LoadString(g_hResource,IDS_LAPTOP_SHUTDOWNWARNING,sError,1024);
		MessageBox(g_hMain,sError,sTitle, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
        }
	}

    SetErrorMode(oldMode);
	return TRUE;
}





///////////////////////////////////////////////////////////////////////////////////
// Maps and Icon Index to an ICON
//
void MapIcon(int iIcon,UINT action=NIM_MODIFY)
{
	PostMessage(g_hMain,CM_TRAY_ACTION,action,iIcon);
}



///////////////////////////////////////////////////////////////////////////////////
//	OnTrayAction Performs an action on the TRAY ICON
//
void OnTrayAction(WPARAM Action,LPARAM iIcon)
{


	// if we are not docked to the tray this function has no meaning
	g_IconState=iIcon;


	if (!g_bShowIcon) return;

	NOTIFYICONDATA tnd; // tray notify data

    // Initialize the tray notify data.
    ZeroMemory(&tnd, sizeof(NOTIFYICONDATA));

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= g_hMain;
	tnd.uID			= IDTRAY;
	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.hIcon		=  g_hIcons16[iIcon];

	LoadString(g_hResource,IDS_APP_TITLE,tnd.szTip,64);
	tnd.uCallbackMessage	= CM_TRAY_NOTIFY;


	g_bShellResult = Shell_NotifyIcon(Action, &tnd);
}



/////////////////////////////////////////////////////////////////////////////////
// check to see if popupbuble is enabled
BOOL CheckPopUpMode()
{
	HKEY hKey = NULL;
	BOOL bEnablePopUp = FALSE;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,szReg_Key_Main,NULL,
		KEY_READ,&hKey)== ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(DWORD);
		DWORD dwValue=0;
		SymSaferRegQueryValueEx(hKey,"Enablevptraybubble",NULL,NULL,(LPBYTE)&dwValue,&dwSize);
		RegCloseKey(hKey);
		bEnablePopUp =(BOOL) dwValue;
	}
	return bEnablePopUp;
}

///////////////////////////////////////////////////////////////////////////////////
//	OnTrayNotify Handles Tray Notifications
//
void OnTrayNotify(LPARAM cmd)
{
	POINT pt;
	HMENU hPopup;

	switch (cmd)
	{
		case WM_RBUTTONDOWN:
			GetCursorPos( &pt);

			hPopup =GetSubMenu(g_hMenu,0);

			SetForegroundWindow(g_hMain);

			if (hPopup)
            {
  			    TrackPopupMenu(hPopup, TPM_RIGHTALIGN | TPM_LEFTBUTTON,
							pt.x, pt.y,0,g_hMain,NULL );
            }

			break;
		case WM_LBUTTONDBLCLK:
			OnOpen();
			break;
	}
}





///////////////////////////////////////////////////////////////////////////////////
//	TrayWndProc Handles Message for the window that services the tray Icon
//
//
LRESULT CALLBACK TrayWndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	static BOOL StopAskingIfServiceIsRunning = TRUE;
    static UINT nSaveTime = 0;
    static UINT nCount = 0;

	switch(Msg)
	{
		case WM_CREATE:

			// load icons
			g_hIcons16[0]= (HICON)LoadImage(g_hResource,MAKEINTRESOURCE(IDI_TRAY_ON),IMAGE_ICON,16,16,0);
			g_hIcons16[1]= (HICON)LoadImage(g_hResource,MAKEINTRESOURCE(IDI_TRAY_OFF),IMAGE_ICON,16,16,0);
			g_hIcons16[2]= (HICON)LoadImage(g_hResource,MAKEINTRESOURCE(IDI_TRAY_WARN),IMAGE_ICON,16,16,0);
			g_hIcons32[0]= (HICON)((LPCREATESTRUCT)lParam)->lpCreateParams;
			g_hIcons32[1]=  LoadIcon(g_hResource,MAKEINTRESOURCE(IDI_TRAY_OFF));
			g_hIcons32[2]=  LoadIcon(g_hResource,MAKEINTRESOURCE(IDI_TRAY_WARN));

			//create menu
			g_hMenu = LoadMenu(g_hResource,MAKEINTRESOURCE(IDR_POPUP));

			//LoadPlugins();

			// change open menu to bold default
			MENUITEMINFO minfo;

			minfo.cbSize = sizeof(MENUITEMINFO);
			minfo.fMask = MIIM_STATE;
			minfo.fState =  MFS_DEFAULT;
   			SetMenuItemInfo(g_hMenu,ID_DF_OPEN,FALSE,&minfo);

			// Check if PopUp Bubble enabled
			if(CheckPopUpMode())
			{
                g_schSCManager = OpenSCManager(NULL,NULL,GENERIC_READ);
                if(g_schSCManager)
                {
                    if(g_schService = OpenService(g_schSCManager,SERVICE_NAME,GENERIC_READ))
                    {
                        g_nMainTimerID = SetTimer(hWnd, 1, g_nDelay, NULL);
                    }
                }
			}

			// Fix to defect 1-AV08P:  systray icon not recreated if shell restarts
			g_iTaskbarCreatedMsg = RegisterWindowMessage(MESSAGE_NAME_TASKBAR_CREATED);

			return 0;
        case WM_TIMER:
			{
				if(g_nMainTimerID == wParam)
				{
                    if(++g_nTimer == 1) break;

                    if(g_nTimer >= 420)
					{
						KillTimer(hWnd, g_nMainTimerID);
						// Popup the bubble
						CreateDialogParam(g_hInstance, MAKEINTRESOURCE(IDD_POPUPBUBBLE),g_hMain, (DLGPROC)NotifyDlgProc, (LPARAM)0);

					}

					if( g_schSCManager && g_schService)
					{
						if(StopAskingIfServiceIsRunning == TRUE)
						{
							StopAskingIfServiceIsRunning = FALSE;

							SERVICE_STATUS  serviceStatus;
                            QueryServiceStatus(g_schService,&serviceStatus);

							switch (serviceStatus.dwCurrentState)
							{
							case SERVICE_CONTINUE_PENDING: //The service continue is pending.
							case SERVICE_PAUSE_PENDING: // The service pause is pending.
							case SERVICE_PAUSED: // The service is paused.
							case SERVICE_START_PENDING: // The service is starting.
								--g_nTimer;
								break;
							case SERVICE_RUNNING:
								{

									//IScanDialogs *pScanDlgs = NULL;
									//long		 lServicesState = 0;
									DWORD dwRunning = KEYVAL_NOTRUNNING;
									BOOL bServiceRunning = FALSE;
									HKEY pKey;
									DWORD dwSize = sizeof(dwRunning);
									//if( SUCCEEDED( CoCreateInstance( CLSID_ScanDialogs, NULL, CLSCTX_INPROC_SERVER, IID_IScanDialogs, (void**)&pScanDlgs) ) )
									//	pScanDlgs->GetServicesState( &lServicesState );
									//	pScanDlgs->Release();
									if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,	szReg_Key_Main,0,KEY_READ,&pKey) )
									{
										//Read the ServiceRunning value
										SymSaferRegQueryValueEx(	pKey,
															szReg_Val_ServiceRunning,
															NULL,
															NULL,
															(BYTE*)&dwRunning,
															&dwSize	);

										bServiceRunning = (BOOL)(dwRunning != KEYVAL_NOTRUNNING);
										RegCloseKey( pKey );
									}

									if( TRUE == bServiceRunning )
									{

										BOOL dwAPStatus = 0;
										if(nSaveTime == 0)
										{
											nSaveTime = g_nTimer;
											nCount++;
										}
										if( (g_nTimer - nSaveTime) >= 10*nCount )
										{

											if(nCount >= 32)
											{
												KillTimer(hWnd, g_nMainTimerID);
												// Popup the bubble
												CreateDialogParam(g_hInstance, MAKEINTRESOURCE(IDD_POPUPBUBBLE),g_hMain, (DLGPROC)NotifyDlgProc, (LPARAM)0);
											}
											else
											{
												nCount++;
	
												SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );
												if( pSAVRT.Get() != NULL )
                                                {
#if 0 //&? SAVRT 10.0 TODO: Threading/interface marshalling issues in STA
													if( SUCCEEDED( pSAVRT->GetEnabledState(&dwAPStatus) ) )
													{
														if( dwAPStatus == TRUE )
															KillTimer(hWnd, g_nMainTimerID);
													}
#endif //&? SAVRT 10.0 TODO: Threading/interface marshalling issues in STA
KillTimer(hWnd, g_nMainTimerID); //&? SAVRT 10.0 Delete this line when reimplementing above
												}
											}
										}
									}



								}
								break;
							default:
								;
							}
							StopAskingIfServiceIsRunning = TRUE;
						}
					}
				}

				break;
			}

		case WM_PAINT:
			if (IsIconic(hWnd))
			{
				PAINTSTRUCT pstr;
				HDC dc;

				// paint the Icon in its minimized state (NT 3.51 only)
				dc = BeginPaint(hWnd,&pstr);

				SendMessage(hWnd,WM_ICONERASEBKGND,(WPARAM) dc, 0);

				// Center icon in client rectangle
				int cxIcon = GetSystemMetrics(SM_CXICON);
				int cyIcon = GetSystemMetrics(SM_CYICON);
				RECT rect;
				GetClientRect(hWnd,&rect);
				int x = ((rect.right-rect.left) - cxIcon + 1) / 2;
				int y = ((rect.bottom-rect.top) - cyIcon + 1) / 2;

				// Draw the icon
				DrawIcon(dc,x, y, g_hIcons32[g_IconState]);
				EndPaint(hWnd,&pstr);
				return 0;
			}
			break;
		case WM_DESTROY:
			if (g_hMenu) DestroyMenu(g_hMenu);
			if(g_hAdvapi)
			{
				FreeLibrary(g_hAdvapi);
				g_hAdvapi = NULL;
			}
			OnStop();
			SendMessage(g_hMain,CM_TRAY_ACTION,	NIM_DELETE ,0);
			g_hMain=NULL;
			if(g_schSCManager)
			{
				CloseServiceHandle(g_schSCManager);
				if(g_schService)CloseServiceHandle (g_schService);
			}
			PostQuitMessage(0);
			break;
		case CM_TRAY_NOTIFY:
			OnTrayNotify(lParam);
			break;
		case CM_TRAY_ACTION:
			OnTrayAction(wParam,lParam);
			break;
		case WM_COMMAND:
			if (LOWORD(wParam)==ID_DF_OPEN)
				OnOpen();
			else if (LOWORD(wParam)==ID_DF_REALTIME)
				OnRealTime();
			break;
		case WM_QUERYENDSESSION:
			return OnShutDown();

		case WM_ENDSESSION: 
			if (lParam & ENDSESSION_LOGOFF)
			{
				SetLoggedOff(1);
				//LogDebugString("VPTRAY: WM_ENDSESSION - ENDSESSION_LOGOFF Found!\r\n");
			}
			else 
			{
				SetLoggedOff(0, TRUE);    // Reboot or Shutdown
				//LogDebugString("VPTRAY: WM_ENDSESSION - REBOOT Found!\r\n");
			}
			break;

		case WM_INITMENU:
			InitMenu();
			break;
		case WM_SYSCOMMAND:
			if (OnSysCommand(wParam)) return 0;
			break;
		case WM_QUERYOPEN:
			OnOpen();
			return 0;
		case CM_START:
			OnStart();
			return 0;

		default:
			// Fix to defect 1-AV08P:  systray icon not recreated if shell restarts
			// Restored icon to tray if TaskbarCreated message is received
			if ((Msg == g_iTaskbarCreatedMsg) && (g_iTaskbarCreatedMsg != 0))
			{
				if (g_bShowIcon)
				{
					PostMessage(hWnd, CM_TRAY_ACTION, NIM_ADD, g_IconState);
				}
			}
	}
	return DefWindowProc(hWnd,Msg,wParam,lParam);
};


void InitLang()
{

		char sPath[MAX_PATH];
		lstrcpy(g_szLang,"enu");

		if( GetLanguage( g_szLang, sPath, "%sRVPT.dll" ) )
		{
			HINSTANCE h = LoadLibrary(sPath);
			if (h) g_hResource = h;
		}
}

#ifdef _DEBUG
// On Win32, trigger a breakpoint to pop the debugger if requested.
static void AllowDebuggerToConnect()
{
	HKEY hMainKey;
	DWORD dwValue;
	DWORD dwValueType = REG_DWORD;
	DWORD dwSize = sizeof(dwValue);
	LONG lRet;

	lRet = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER "\\" szReg_Key_ProductControl, &hMainKey);
	if (lRet == ERROR_SUCCESS)
	{
		lRet = SymSaferRegQueryValueEx(hMainKey, szReg_Val_DebugOnStartupVPTray, NULL,
							&dwValueType, (LPBYTE)&dwValue, &dwSize);
		if (ERROR_SUCCESS == lRet)
		{
			if (dwValue != 0)
			{
				if (MessageBox(NULL, "The debug on startup value has been set.  Would you like to launch a debugger?", "Debug", MB_YESNO | MB_ICONQUESTION) == IDYES)
					DebugBreak();
			}
		}
		RegCloseKey(hMainKey);
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////
//	WinMain Entry Point of the Application
//
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpInputCmdLine, int nShowCmd )
{
	//
	// CreateProcessAsUser launched process needs to read command line explicitly with GetCommandLine
	// This path may occur if StartInteractiveClientProcess is called
	// 
	TCHAR *lpCmdLine = lpInputCmdLine;
	if ((lpCmdLine) && (_tcscmp(lpCmdLine, "") == 0))
		lpCmdLine = GetCommandLine();

    // Figure out if this is the initial launch by the installer.  If it is,
	// then re-launch the program under the security context of the currently
	// logged in user.
    if( lpCmdLine && strstr(lpCmdLine, "-l") )
	{
		// If LaunchAsCurrentUser() succeeds, it has launched a new instance of
		// vptray.exe as the currently logged on user.  When this happens, just
		// exit the current process.
		if(LaunchAsCurrentUser(lpCmdLine))
		{
			return 0;
		}
	}

    // Don't remain running if VPTRAY is running as SYSTEM as it prevents 
    // VPTray from running when the user logs in, and thus startup 
    // scans don't run.
    // Also a potential security issue.
    TCHAR szName[UNLEN+1] = _T("");
    DWORD dwLen = UNLEN+1;
    if (GetUserName(szName, &dwLen))
    {
        if (_tcscmp(szName, _T("SYSTEM")) == 0)
        {
            return 0;
        }
    }

    // Application Migration for Win2K.

	DWORD herr;
	TCHAR szMigrationMessage[1024];
	TCHAR szAppName[1024];

#ifdef _DEBUG
	AllowDebuggerToConnect();
#endif

    if(DetermineOSMigrationMessage() == 95)
	{
		LoadString(hInstance,IDS_OSMIGRATEDMESS95TO2K,szMigrationMessage,1024);
		LoadString(hInstance,IDS_PROG_NAME,szAppName,1024);
		MessageBox(NULL, szMigrationMessage, szAppName, MB_OK|MB_ICONERROR);
		return 0;
	}
	else if(DetermineOSMigrationMessage() == 98)
	{
		LoadString(hInstance,IDS_OSMIGRATEDMESS98TO2K,szMigrationMessage,1024);
		LoadString(hInstance,IDS_PROG_NAME,szAppName,1024);
		MessageBox(NULL, szMigrationMessage, szAppName, MB_OK|MB_ICONERROR);
		return 0;
	}

    // Create user private logs directories.  This functionality was previously
    // only available in LDVPVIEW and will not handle the cases where the user
    // never brings up VPC32.  There are still cases where this path is not
    // traversed, i.e. if the user does not log on interactively.  In those cases,
    // the user will not access his/her private logs.  The system common log
    // will always have all the entries for all users which will be available to
    // the administrator.
    CreateUserDirectory("Logs");

    // Initialize COM.
	CoInitialize( NULL );

	//create an instance of the singleton here so that we only load the modules into memory once 
	//during the lifetime of the application
	SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );
	if( pSAVRT.Get() == NULL ){
		return 0; //don't start vptray if the savrt dll isn't present
	}

	// Check for Terminal Server / Service.
	// Pass in false to the CTerminalSession constructor
	// b/c all we need to know is if we're on a remote session.
	CTerminalSession curSession(FALSE);

	if (TRUE == curSession.IsRemoteSession())
	{
		// Don't run on remote sessions
		return 0;
	}

    // don't allow multi-instance or failure to create Event
	g_hEvent = CreateEvent(NULL,TRUE,FALSE,"LDVPTRAY");
	herr = GetLastError();
	if (herr==ERROR_ALREADY_EXISTS || g_hEvent == NULL)
        return FALSE;

	HWND hFind;
	hFind = FindWindow(VPTRAY_CLASS, NULL);
	if (hFind)
		PostMessage(hFind,WM_CLOSE,0,0);

    // See if the current user has the rights to update
    // our hive in the registry. If so, he can load/unload
    // realtime protection.
    LONG    lResult = ERROR_SUCCESS;
    HKEY    hKey = NULL;

	// dwr - 1-WVPZL merge from 8.1.1
 	// 1-WVPZL --- Disabling and Enabling real-time protection is grayed out from tray icon but not GUI
 	//             For certain users.
 	// We really don't care about KEY_ALL_ACCESS here. Basically, we need to read, write, create.
 	// According to the help KEY_READ = STANDARD_RIGHTS_READ | KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_NOTIFY 
 	//						 KEY_WRITE = STANDARD_RIGHTS_WRITE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY
 	//
 	// lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGHEADER, 0, KEY_ALL_ACCESS, &hKey);
        lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGHEADER, 0, KEY_READ | KEY_WRITE, &hKey);

    if ( lResult == ERROR_SUCCESS )
    {
        g_bUserCanStopRTS = TRUE;
        RegCloseKey( hKey );
    }
    else
    {
        g_bUserCanStopRTS = FALSE;
    }

	//g_resource sould be set to language dll
	g_hInstance = g_hResource = hInstance;
	InitLang();

#if 0
	// GLEE 4/6/05
	// Grouper: if OEM is installed with /FACTORY=1 and the system has been resealed,
	// change the factory mode setting to 0
	// MLEE 4/13/05. Corrected Logic (!IsOEMObj.IsOEM_FactoryMode()) to (OEMObj.IsOEM_FactoryMode())
	if (OEMObj.IsOEMBuild() &&
		OEMObj.IsOEM_FactoryMode() &&
		OEMObj.IsOEM_Resealed())
		OEMObj.SetOEM_FactoryMode(FALSE);
#endif

	// don't show the tray if -s
	g_bShowTray =  !(lpCmdLine && strstr(lpCmdLine,"-s"));

    // Figure out if we have been iauched by the installer.  Currently, this determines
    // whether or not we show licensing alerts.

    g_bLaunchedByInstall = (lpCmdLine && strstr(lpCmdLine,"-i"));

	if (OEMObj.IsOEMBuild())
		g_bShowTray = OEMObj.IsOEM_ShowVPTray();

	// register the tray notification window
	WNDCLASS wc;
	ZeroMemory(&wc,sizeof(WNDCLASS));   // start with NULL defaults
	wc.lpfnWndProc = TrayWndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(g_hResource,MAKEINTRESOURCE(IDI_TRAY_ON));
	wc.lpszClassName = VPTRAY_CLASS;
	if (RegisterClass(&wc))
	{

		TCHAR sTitle[MAX_PATH];
		LoadString(g_hResource,IDS_APP_TITLE,sTitle,MAX_PATH);

		g_hMain = CreateWindow(VPTRAY_CLASS,sTitle,WS_OVERLAPPEDWINDOW,0,0,10,10,
			0,0,hInstance,wc.hIcon);
		if (g_hMain) PostMessage(g_hMain,CM_START,0,0);
	}

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (g_hNotifyPat && IsDialogMessage(g_hNotifyPat,&msg))
			continue;
		else if (g_hNotifyMissingPats && IsDialogMessage(g_hNotifyMissingPats,&msg))
			continue;

		DispatchMessage(&msg);
	}
	UnregisterClass( VPTRAY_CLASS, g_hInstance);
	CoUninitialize();
	return 0;
}

/* **************************************************************************
@Name: BOOL SkipShutDownFloppyCheck()

@Description:
This function determines if the SkipFloppyCheckValue registry key is set. If
it is set, then we will NOT check to see if a floppy is in the drive.  This
is mainly for laptops with removeable floppy drives.  If a removeable floppy
is NOT connected, it takes a long time for the OS to determine that the drive
is not accessible.

@Parameters:
None.

4/7/00 - CLB - Fix for case where user is non-admin. Should have used KEY_QUERY_VALUE
			   instead of KEY_ALL_ACCESS.  Also fixed handle leak...

************************************************************************** */
BOOL SkipShutDownFloppyCheck() {
	const TCHAR szBelugaKey[] = "SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion\\Storages\\FileSystem\\RealTimeScan";
	const TCHAR szSkipFloppyCheckValue[] = "SkipShutDownFloppyCheck";
	HKEY hKey;
	DWORD dwType, dwSize, dwValue;

	BOOL bRetVal = FALSE;
	dwSize = sizeof(DWORD);

	// Open the main NAV registry key...
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, // handle to open key
									 szBelugaKey,		 // address of name of subkey to open
								 	 0,					 // reserved
									 KEY_QUERY_VALUE,	 // security access mask
									 &hKey)) {			 // address of handle to open key

		//
		// Does the value exist?
		//
		if(ERROR_SUCCESS == SymSaferRegQueryValueEx(hKey,						// handle to key to query
											szSkipFloppyCheckValue,		// address of name of value to query
											0,							// reserved
										    &dwType,					// address of buffer for value type
											(LPBYTE)&dwValue,			// address of data buffer
											&dwSize))					// address of data buffer size
			if(dwValue == 1)
				bRetVal = TRUE;

			else
				bRetVal = FALSE;

		RegCloseKey(hKey);
	}

	return bRetVal;
}

// All the following functions are available in different projects.  Since this
// code will not live too long, I am duplicating the functionality.  For future
// products common functions like these should be placed in a utils lib or dll.

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CreateUserDirectory
//
// Description  : Finds the non-roaming application data directory (as defined
//                by CSIDL_LOCAL_APPDATA) for the current user creates the
//                product subdirectories and the specific subdir that is passed
//                in ("Logs", "Temp", etc.) For example, the system provides
//
//                C:\Documents and Settings\tcashin\Local Settings\Application Data
//
//                and the function creates:
//
//                Symantec\Symantec AntiVirus Corporate Edition\7.5\Logs
//
// Return Values: ERROR_SUCCESS or standard error codes
//
// Argument     : [in]  LPTSTR lpDataDirectory - Pointer to the dir to create
//                             ("Logs", "Temp", etc.)
//
///////////////////////////////////////////////////////////////////////////////
// 5/16/2001    RCHINTA copied from LDPVIEW
///////////////////////////////////////////////////////////////////////////////
DWORD CreateUserDirectory(LPTSTR lpDataDirectory)
{
    DWORD           dwReturn = ERROR_SUCCESS;
    TCHAR           szTemp1[IMAX_PATH] = {0};
    TCHAR           szTemp2[IMAX_PATH] = {0};

    dwReturn = GetSystemDataDir(NAV_USER_APP_DATA, szTemp1);

    if (dwReturn == ERROR_SUCCESS )
    {
        sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s", szTemp1, SYMANTEC_COMPANY_NAME );
        if ( !CreateDirectory(szTemp2, NULL) )
        {
            dwReturn = GetLastError();

            if ( dwReturn != ERROR_ALREADY_EXISTS )
                goto All_Done;
        }

        _tcscpy(szTemp1, szTemp2);

        sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s", szTemp1, NAVCORP_DIRECTORY_NAME );
        if ( !CreateDirectory(szTemp2, NULL) )
        {
            dwReturn = GetLastError();

            if ( dwReturn != ERROR_ALREADY_EXISTS )
                goto All_Done;
        }

        _tcscpy(szTemp1, szTemp2);

        sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s", szTemp1, SHARED_VERSION_DATA_DIRECTORY );
        if ( !CreateDirectory(szTemp2, NULL) )
        {
            dwReturn = GetLastError();

            if ( dwReturn != ERROR_ALREADY_EXISTS )
                goto All_Done;            }

        _tcscpy(szTemp1, szTemp2);

        sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s", szTemp1, lpDataDirectory );
        if ( !CreateDirectory(szTemp2, NULL) )
        {
            dwReturn = GetLastError();
        }
    }

All_Done:

    return dwReturn;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetSystemDataDir
//
// Description  : Loads SHFOLDER.DLL and calls SHGetFolderPath() to get the
//                non-roaming application data directory (CSIDL_LOCAL_APPDATA)
//                for the current user.
//
//                Currently, this function requires Windows 2000 or later.
//
// Return Values: ERROR_SUCCESS or other error code.
//
// Argument     : [in]   DWORD  dwFlags - indicates whether to get app data or
//                              non-roaming user data
//                [out]  LPTSTR lpSystemDataDir - Buffer to get the directory
//
///////////////////////////////////////////////////////////////////////////////
// 5/16/2001 -   RCHINTA: copied from LDVPVIEW
///////////////////////////////////////////////////////////////////////////////
DWORD GetSystemDataDir( DWORD dwFlags, LPTSTR lpSystemDataDir )
{

    auto PFNSHGetFoldPathA pfnSHGetFolderPathA = NULL;

    auto DWORD          dwError = ERROR_SUCCESS;
    auto TCHAR          szAppData[MAX_PATH] = {0};
    auto TCHAR          szSystemDir[MAX_PATH] = {0};
    auto TCHAR          szSHFolderDLL[MAX_PATH] ={0};
    auto HINSTANCE      hFolderDLL = NULL;
    auto HRESULT        hr ;
    auto int            len = 0;


    if ( !lpSystemDataDir ||
         (dwFlags != NAV_COMMON_APP_DATA &&
          dwFlags != NAV_USER_APP_DATA) )
    {
        return ERROR_INVALID_PARAMETER;
    }

    // Get the system directory
    if ( GetSystemDirectory(szSystemDir, sizeof(szSystemDir)) )
    {
        // Append the DLL name
        sssnprintf ( szSHFolderDLL, sizeof(szSHFolderDLL), _T("%s\\shfolder.dll"), szSystemDir );

        // Load it.
        hFolderDLL = LoadLibrary( szSHFolderDLL );

        if ( hFolderDLL )
        {
            // Get the function
            pfnSHGetFolderPathA = (PFNSHGetFoldPathA)GetProcAddress( hFolderDLL, _T("SHGetFolderPathA") );

            if ( pfnSHGetFolderPathA )
            {
                if ( dwFlags == NAV_USER_APP_DATA )
                {
					// Per machine, specific user, non-roaming, create it here
					hr = pfnSHGetFolderPathA( NULL, CSIDL_LOCAL_APPDATA|CSIDL_FLAG_CREATE,
												NULL, 0, szAppData);
                }
                else
                {
                    // Per machine, application
                    hr = pfnSHGetFolderPathA( NULL, CSIDL_COMMON_APPDATA|CSIDL_FLAG_CREATE,
                                                NULL, 0, szAppData);
                }

                if ( SUCCEEDED(hr) )
                {
                    _tcscpy( lpSystemDataDir, szAppData );
                }
                else
                    dwError = GetLastError();
            }
            FreeLibrary( hFolderDLL );
        }
        else
            dwError = GetLastError();
    }
    else
        dwError = GetLastError();

    return dwError;
}


//-----------------------------------------------------------------------------
// NAME:	LaunchAsCurrentUser
//
// PURPOSE:	Re-launch vptray.exe as the currently logged on user.
//
// PARAMS:	commandLine [in]	Command line that this process was originally
//								launched with.  The re-launched process will
//								get the same command line without the "-I".
//
// RETURNS:	true if vptray.exe was successfully started, false if error.
//-----------------------------------------------------------------------------
bool LaunchAsCurrentUser(const char* commandLine)
{
	bool rc = false;
	HANDLE hUser = NULL;
	HANDLE tmpToken = NULL;
	bool debugRelaunch = false;

    if( commandLine && strstr(commandLine, "-debug_relaunch") )
		debugRelaunch = true;

	tmpToken = GetAccessTokenForLoggedOnUser();
	if(tmpToken && tmpToken != INVALID_HANDLE_VALUE)
	{
		HANDLE primaryToken = NULL;
		if(DuplicateTokenEx(tmpToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &primaryToken))
		{
			hUser = primaryToken;
			g_lastErr.append("DuplicateTokenEx() failed - ");
		}
		CloseHandle(tmpToken);
	}
	else
		g_lastErr.append("GetAccessTokenForLoggedOnUser() failed - ");

	if(hUser != NULL)
	{
		HKEY mainKey;
		std::string appDir;
		std::string appName;

		//Find out where LDVPTray is and load it
		//First, get the info from the main key
		if( RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
							szReg_Key_Main,
							0,
							KEY_READ,
							&mainKey) == ERROR_SUCCESS )
		{
			TCHAR path[MAX_PATH + 1] = {0};
			DWORD dwSize = MAX_PATH;
			if (SymSaferRegQueryStringValue (mainKey,
                                             szReg_Val_LocalAppDir,
                                             path,
                                             &dwSize) == ERROR_SUCCESS)
			{
				// Assign the path part of the app name and make sure the path ends
				// with a directory delimiter.
				appDir.assign(path);
				if(*(appDir.rbegin()) != '\\')	// if last char is not delim
					appDir.append(1, '\\');		// append delim

				// Append the executable name part of the app name.
				appName.assign(appDir);
				appName.append("vptray.exe");
			}
			else
			{
				g_lastErr.append("failed to read reg value: ");
				g_lastErr.append(szReg_Val_LocalAppDir);
			}

			RegCloseKey( mainKey );
		}
		else
		{
			g_lastErr.append("failed to open reg key ");
			g_lastErr.append(szReg_Key_Main);
		}

		if(!appName.empty())
		{
			std::string cmdLn(commandLine);
			std::string::size_type pos;

			// Remove the "-l" from the command line.
			pos = cmdLn.find("-l");
			if(pos != std::string::npos)
				cmdLn.erase(pos, 3);

			// Remove the "-s" from the command line.
			pos = cmdLn.find("-s");
			if(pos != std::string::npos)
				cmdLn.erase(pos, 3);

			if(StartInteractiveClientProcess(hUser, appName.c_str(), cmdLn.c_str(), appDir.c_str()))
				rc = true;
		} // END if(!appName.empty())
		
		CloseHandle(hUser);

	} // END if(hUser != NULL)
	else
		g_lastErr.append("failed to get access token for user");

	// If debugging request present in command line, then log the result of
	// re-launching vptray.exe as the logged in user.
	if(debugRelaunch)
	{
		HANDLE hFile =  CreateFile("C:\\vptray_debug.txt",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		DWORD bytes;

		if(g_lastErr.empty())
		{
			if(rc)
				g_lastErr.append("VPTray launch OK");
			else
				g_lastErr.append("relaunch of VPTray failed");
		}

		g_lastErr.append("\r\n");

		WriteFile(hFile, g_lastErr.c_str(), g_lastErr.size(), &bytes, NULL);
		CloseHandle(hFile);
	}

	return rc;
}


//-----------------------------------------------------------------------------
// NAME:	GetLogonSID
//
// PURPOSE:	Support LaunchAsCurrentUser().  Get the logged in user's SID.
//
// PARAMS:	htoken	[in]	User's access token.
//			ppsid	[out]	SID pointer.
//
// RETURNS:	true if success, false if error.
//-----------------------------------------------------------------------------
bool GetLogonSID(HANDLE hToken, PSID *ppsid)
{
	bool bSuccess = false;
	DWORD dwIndex;
	DWORD dwLength = 0;
	char *ptgMem = NULL;
	PTOKEN_GROUPS ptg = NULL;

	// Verify the parameter passed in is not NULL.
	if (NULL == ppsid)
		goto cleanup;					// return fail

	// Get required buffer size and allocate the TOKEN_GROUPS buffer.
	if(!GetTokenInformation(
			hToken,			// handle to the access token
			TokenGroups,	// get information about the token's groups
			(LPVOID) ptg,	// pointer to TOKEN_GROUPS buffer
			0,				// size of buffer
			&dwLength))		// receives required buffer size
	{
		if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			goto cleanup;				// return fail

		ptgMem = new(std::nothrow) char[dwLength];
		if(!ptgMem)
			goto cleanup;				// return fail

		ZeroMemory(ptgMem, dwLength);
		ptg = reinterpret_cast<PTOKEN_GROUPS>(ptgMem);
	}

	// Get the token group information from the access token.
	if(!GetTokenInformation(
		hToken,			// handle to the access token
		TokenGroups,	// get information about the token's groups
		(LPVOID) ptg,	// pointer to TOKEN_GROUPS buffer
		dwLength,		// size of buffer
		&dwLength))		// receives required buffer size
	{
		goto cleanup;					// return fail
	}

	bSuccess = true;					// indicate success

	// Loop through the groups to find the logon SID.
	for(dwIndex = 0; dwIndex < ptg->GroupCount; ++dwIndex)
	{
		if( (ptg->Groups[dwIndex].Attributes & SE_GROUP_LOGON_ID) ==  SE_GROUP_LOGON_ID )
		{
			char* ppsidMem = NULL;

			// Found the logon SID; make a copy of it.
			dwLength = GetLengthSid(ptg->Groups[dwIndex].Sid);
			ppsidMem = new(std::nothrow) char[dwLength];
			if(!ppsidMem)
			{
				bSuccess = false;	// return fail
				break;				// exit loop
			}
			ZeroMemory(ppsidMem, dwLength);
			*ppsid = reinterpret_cast<PSID>(ppsidMem);

			if(!CopySid(dwLength, *ppsid, ptg->Groups[dwIndex].Sid))
			{
				delete[] ppsidMem;
				*ppsid = NULL;
				bSuccess = false;	// return fail
				break;				// exit loop
			}

			break;					// exit loop
		}
	}

cleanup:

	// Cleanup dynamic memory.
	delete[] ptgMem;

	return bSuccess;
}


//-----------------------------------------------------------------------------
// NAME:	FreeLogonSID
//
// PURPOSE:	Support LaunchAsCurrentUser().  Free memory used to store SID.
//
// PARAMS:	ppsid	[in] Pointer to SID.
//
// RETURNS:	nothing
//-----------------------------------------------------------------------------
void FreeLogonSID(PSID *ppsid)
{
	char* ppsidMem = reinterpret_cast<char*>(*ppsid);
	delete[] ppsidMem;
}

//-----------------------------------------------------------------------------
// NAME:	AddAceToObject
//
// PURPOSE:	Support LaunchAsCurrentUser().  Allow a new process to access the
//			logged in user's windows station or desktop.
//
// PARAMS:	hdesh	[in] Handle to the logged in user's station or desktop.
//			psid	[in] Logged in user's SID.
//
// RETURNS:	true if success, false if error.
//-----------------------------------------------------------------------------
bool AddAceToObject(HANDLE hObj, PSID psid, SecObjType objType)
{
	ACCESS_ALLOWED_ACE		*pace = NULL;
	ACL_SIZE_INFORMATION	aclSizeInfo;
	BOOL					bDaclExist = FALSE;
	BOOL					bDaclPresent = FALSE;
	DWORD					dwNewAclSize = 0;
	DWORD					dwSidSize = 0;
	DWORD					dwSdSizeNeeded = 0;
	PACL					pacl = NULL;
	PACL					pNewAcl = NULL;
	PSECURITY_DESCRIPTOR	psd = NULL;
	PSECURITY_DESCRIPTOR	psdNew = NULL;
	SECURITY_INFORMATION	si = DACL_SECURITY_INFORMATION;
	char *psdMem=NULL, *psdNewMem=NULL, *pNewAclMem=NULL, *paceMem=NULL;
	bool rc = false;

	// Obtain the security descriptor for the object.
	if( !GetUserObjectSecurity(hObj, &si, psd, dwSidSize, &dwSdSizeNeeded) )
	{
		if( GetLastError() == ERROR_INSUFFICIENT_BUFFER )
		{
			psdMem = new(std::nothrow) char[dwSdSizeNeeded];
			if(!psdMem)
				goto cleanup;			// return fail
			ZeroMemory(psdMem, dwSdSizeNeeded);
			psd = reinterpret_cast<PSECURITY_DESCRIPTOR>(psdMem);

			psdNewMem = new(std::nothrow) char[dwSdSizeNeeded];
			if(!psdNewMem)
				goto cleanup;			// return fail
			ZeroMemory(psdNewMem, dwSdSizeNeeded);
			psdNew = reinterpret_cast<PSECURITY_DESCRIPTOR>(psdNewMem);

			dwSidSize = dwSdSizeNeeded;

			if( !GetUserObjectSecurity(hObj, &si, psd, dwSidSize, &dwSdSizeNeeded) )
				goto cleanup;			// return fail
		}
		else
			goto cleanup;				// return fail
	}

	// Create a new security descriptor.
	if( !InitializeSecurityDescriptor(psdNew, SECURITY_DESCRIPTOR_REVISION) )
		goto cleanup;					// return fail

	// Get the DACL from the security descriptor.
	if( !GetSecurityDescriptorDacl(psd, &bDaclPresent, &pacl, &bDaclExist) )
		goto cleanup;					// return fail

	// Initialize the ACL.
	ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
	aclSizeInfo.AclBytesInUse = sizeof(ACL);

	// Call only if the DACL is not NULL.
	if( pacl != NULL )
	{
		// Determine the size of the ACL information.
		if(!GetAclInformation(pacl, (LPVOID)&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION), AclSizeInformation))
			goto cleanup;				// return fail
	}

	// Calculate additional ACL size needed for each ACE.
	DWORD aclSizePerAce = sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psid) - sizeof(DWORD);

	// Number of ACEs.
	DWORD additionalAceCount = ((objType == objType_WindowStation) ? 2 : 1);

	// Compute the size of the new ACL.
	dwNewAclSize = aclSizeInfo.AclBytesInUse + (aclSizePerAce * additionalAceCount);

	// Allocate buffer for the new ACL.
	pNewAclMem = new(std::nothrow) char[dwNewAclSize];
	if(!pNewAclMem)
		goto cleanup;					// return fail
	ZeroMemory(pNewAclMem, dwNewAclSize);
	pNewAcl = reinterpret_cast<PACL>(pNewAclMem);

	// Initialize the new ACL.
	if(!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
		goto cleanup;					// return fail

	// If DACL is present, copy it to a new DACL.
	if(bDaclPresent)
	{
		// Copy the ACEs to the new ACL.
		if(aclSizeInfo.AceCount)
		{
			PVOID pTempAce;
			unsigned int i;

			for(i=0; i < aclSizeInfo.AceCount; ++i)
			{
				// Get an ACE.
				if(!GetAce(pacl, i, &pTempAce))
					break;				// exit loop

				// Add the ACE to the new ACL.
				if (!AddAce(pNewAcl, ACL_REVISION, MAXDWORD, pTempAce, ((PACE_HEADER)pTempAce)->AceSize))
					break;				// exit loop
			}

			if(i < aclSizeInfo.AceCount)// failed to copy all ACEs
				goto cleanup;			// return fail
		}
	}

	if(objType == objType_WindowStation)
	{
		DWORD winstaAll = (WINSTA_ENUMDESKTOPS | WINSTA_READATTRIBUTES |
			WINSTA_ACCESSCLIPBOARD | WINSTA_CREATEDESKTOP | WINSTA_WRITEATTRIBUTES |
			WINSTA_ACCESSGLOBALATOMS | WINSTA_EXITWINDOWS | WINSTA_ENUMERATE |
			WINSTA_READSCREEN | STANDARD_RIGHTS_REQUIRED);

		DWORD genericAccess = (GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | GENERIC_ALL);

		// Add the first ACE to the window station.
		size_t memSize = sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psid) - sizeof(DWORD);
		paceMem = new(std::nothrow) char[memSize];
		if(!paceMem)
			goto cleanup;				// return fail
		ZeroMemory(paceMem, memSize);
		pace = reinterpret_cast<ACCESS_ALLOWED_ACE *>(paceMem);

		pace->Header.AceType  = ACCESS_ALLOWED_ACE_TYPE;
		pace->Header.AceFlags = CONTAINER_INHERIT_ACE |
					INHERIT_ONLY_ACE | OBJECT_INHERIT_ACE;
		pace->Header.AceSize  = static_cast<WORD>(sizeof(ACCESS_ALLOWED_ACE) +
					GetLengthSid(psid) - sizeof(DWORD));
		pace->Mask            = genericAccess;

		if(!CopySid(GetLengthSid(psid), &pace->SidStart, psid))
			goto cleanup;				// return fail

		if(!AddAce(pNewAcl, ACL_REVISION, MAXDWORD, (LPVOID)pace, pace->Header.AceSize))
			goto cleanup;				// return fail

		// Add the second ACE to the window station.
		pace->Header.AceFlags = NO_PROPAGATE_INHERIT_ACE;
		pace->Mask            = winstaAll;

		if(!AddAce(pNewAcl, ACL_REVISION, MAXDWORD, (LPVOID)pace, pace->Header.AceSize))
			goto cleanup;				// return fail
	}
	else								// desktop object 
	{
		DWORD desktopAll = (DESKTOP_READOBJECTS | DESKTOP_CREATEWINDOW |
			DESKTOP_CREATEMENU | DESKTOP_HOOKCONTROL | DESKTOP_JOURNALRECORD |
			DESKTOP_JOURNALPLAYBACK | DESKTOP_ENUMERATE | DESKTOP_WRITEOBJECTS |
			DESKTOP_SWITCHDESKTOP | STANDARD_RIGHTS_REQUIRED);

		// Add ACE to the DACL.
		if(!AddAccessAllowedAce(pNewAcl, ACL_REVISION, desktopAll, psid))
			goto cleanup;				// return fail
	}

	// Set a new DACL for the security descriptor.
	if(!SetSecurityDescriptorDacl(psdNew, TRUE, pNewAcl, FALSE))
		goto cleanup;					// return fail

	// Set the new security descriptor for the object.
	if(!SetUserObjectSecurity(hObj, &si, psdNew))
		goto cleanup;					// return fail

	rc = true;							// indicate success.

cleanup:

	// Cleanup dynamic memory.
	delete[] psdMem;
	delete[] psdNewMem;
	delete[] pNewAclMem;
	delete[] paceMem;

	return rc;
}

//-----------------------------------------------------------------------------
// NAME:	AddTokenPrivileges
//
// PURPOSE:	Support LaunchAsCurrentUser().  Add privileges, necessary for
//			CreateProcessAsUser() to work, to the users access token.
//
// PARAMS:	hToken	[in] User's access token.
//
// RETURNS:	true if success, false if error.
//-----------------------------------------------------------------------------
bool AddTokenPrivileges(HANDLE hToken)
{
	LUID luid;
	TOKEN_PRIVILEGES* tp = NULL;
	size_t tpSize = sizeof(LUID_AND_ATTRIBUTES)*3 + sizeof(DWORD);

	char* tpMem = new(std::nothrow) char[tpSize];
	if(tpMem == NULL)
		return false;

	ZeroMemory(tpMem, tpSize);
	tp = reinterpret_cast<TOKEN_PRIVILEGES*>(tpMem);

	LookupPrivilegeValue(NULL, SE_ASSIGNPRIMARYTOKEN_NAME, &luid);
	tp->Privileges[0].Luid = luid;
	tp->Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	LookupPrivilegeValue(NULL, SE_INCREASE_QUOTA_NAME, &luid);
	tp->Privileges[1].Luid = luid;
	tp->Privileges[1].Attributes = SE_PRIVILEGE_ENABLED;

	tp->PrivilegeCount = 2;

	bool rc = (AdjustTokenPrivileges(hToken, FALSE, tp, 0, NULL, NULL) != FALSE);
	if(!rc)
		g_lastErr.append("AdjustTokenPrivileges() failed");

	delete[] tpMem;
	return rc;
}

//-----------------------------------------------------------------------------
// NAME:	StartInteractiveClientProcess
//
// PURPOSE:	Support LaunchAsCurrentUser().  Launch a process, capable of
//			interacting with the desktop, as the logged in user.
//
// PARAMS:	hToken		[in] User's access token.
//			appName		[in] Name of application.
//			cmdLine		[in] Command line params for application.
//			directory	[in] Working directory for application.
//
// RETURNS:	true if success, false if error.
//-----------------------------------------------------------------------------
bool StartInteractiveClientProcess (HANDLE hToken,
									const char* appName,
									const char* cmdLine,
									const char* directory)
{
	HDESK       hdesk = NULL;
	HWINSTA     hwinsta = NULL, hwinstaSave = NULL;
	PSID pSid = NULL;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	bool bResult = false;

	if(!AddTokenPrivileges(hToken))
		return false;

	do
	{
		// Save a handle to the caller's current window station.
		hwinstaSave = GetProcessWindowStation();
		if( hwinstaSave == NULL )
		{
			g_lastErr.append("GetProcessWindowStation() failed");
			break;						// return fail
		}

		// Get a handle to the interactive window station.
		hwinsta = OpenWindowStation(
			"winsta0",					// the interactive window station
			FALSE,						// handle is not inheritable
			READ_CONTROL | WRITE_DAC);	// rights to read/write the DACL

		if(hwinsta == NULL) 
		{
			g_lastErr.append("OpenWindowStation() failed");
			break;						// return fail
		}

		// To get the correct default desktop, set the caller's
		// window station to the interactive window station.
		if(!SetProcessWindowStation(hwinsta))
		{
			g_lastErr.append("SetProcessWindowStation() failed");
			break;						// return fail
		}

		// Get a handle to the interactive desktop.
		hdesk = OpenDesktop(
			"default",				// the interactive window station
			0,						// no interaction with other desktop processes
			FALSE,					// handle is not inheritable
			READ_CONTROL |			// request the rights to read and write the DACL
			WRITE_DAC | 
			DESKTOP_WRITEOBJECTS | 
			DESKTOP_READOBJECTS);

		// Restore the caller's window station.
		if(!SetProcessWindowStation(hwinstaSave))
		{
			g_lastErr.append("SetProcessWindowStation() failed");
			break;						// return fail
		}

		if(hdesk == NULL)
		{
			g_lastErr.append("OpenDesktop() failed");
			break;						// return fail
		}

		// Get the SID for the client's logon session.
		if(!GetLogonSID(hToken, &pSid))
		{
			g_lastErr.append("GetLogonSID() failed");
			break;						// return fail
		}

		// Allow logon SID full access to interactive window station.
		if(!AddAceToObject(hwinsta, pSid, objType_WindowStation) ) 
		{
			g_lastErr.append("AddAceToObject() failed adding ACE to window station");
			break;						// return fail
		}

		// Allow logon SID full access to interactive desktop.
		if(!AddAceToObject(hdesk, pSid, objType_Desktop) )
		{
			g_lastErr.append("AddAceToObject() failed adding ACE to desktop");
			break;						// return fail
		}

		// Initialize the STARTUPINFO structure.
		// Specify that the process runs in the interactive desktop.
		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb= sizeof(STARTUPINFO);
		si.wShowWindow = SW_SHOW;
		si.dwFlags = STARTF_USESTDHANDLES;
		si.lpDesktop = TEXT("winsta0\\default");

		// Launch the process in the client's logon session.
		bResult = (CreateProcessAsUser(
			hToken,				// client's access token
			appName,			// file to execute
			const_cast<char*>(cmdLine),			// command line
			NULL,				// pointer to process SECURITY_ATTRIBUTES
			NULL,				// pointer to thread SECURITY_ATTRIBUTES
			FALSE,				// handles are not inheritable
			CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP,	// creation flags
			NULL,				// pointer to new environment block
			directory,			// name of current directory 
			&si,				// pointer to STARTUPINFO structure
			&pi					// receives information about new process
			) != FALSE);

		if(!bResult)
		{
			g_lastErr.append("CreateProcessAsUser() failed: ");
			DWORD lastErr = ::GetLastError();

			LPVOID msgBuf = NULL;
			if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
							FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL,
							lastErr,
							0,
							(LPTSTR)&msgBuf,
							0,
							NULL))
			{
				g_lastErr.append((LPCSTR)msgBuf);
			}
			LocalFree(msgBuf);

			if(lastErr == ERROR_PRIVILEGE_NOT_HELD)
			{
				// The client is unable to impersonate someone.  This means the
				// client is certanly not the admin.  Create a process as Joe user.
				ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
				ZeroMemory(&si, sizeof(STARTUPINFO));
				bResult = (CreateProcess(
					appName,			// file to execute
					const_cast<char*>(cmdLine),			// command line
					NULL,				// pointer to process SECURITY_ATTRIBUTES
					NULL,				// pointer to thread SECURITY_ATTRIBUTES
					FALSE,				// handles are not inheritable
					CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP,	// creation flags
					NULL,				// pointer to new environment block
					directory,			// name of current directory
					&si,				// pointer to STARTUPINFO structure
					&pi					// receives information about new process
					) != FALSE);
			}
		}

		if(bResult)
		{
			if(pi.hProcess)
				CloseHandle(pi.hProcess);
			if(pi.hThread)
				CloseHandle(pi.hThread);
		}
		else
			g_lastErr.append("all attempts to create process failed");

	}while(false);

	// Cleanup handles

	if(hwinstaSave != NULL)
		SetProcessWindowStation (hwinstaSave);

	// Free the buffer for the logon SID.
	if(pSid)
		FreeLogonSID(&pSid);

	// Close the handles to the interactive window station and desktop.
	if(hwinsta)
		CloseWindowStation(hwinsta);

	if(hdesk)
		CloseDesktop(hdesk);

	return bResult;
}

