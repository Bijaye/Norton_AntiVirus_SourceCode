#ifndef _SCAN_INSTALL
#define _SCAN_INSTALL


// ********************************************************* //
// Possible scanning options the calling program can specify //
// ********************************************************* //

#define SCAN_MEMORY             (0x0001)
#define SCAN_BOOTS              (0x0002)
#define SCAN_FLOPPY             (0x0004)
#define SCAN_FIXED              (0x0008)
#define SCAN_REMOTE             (0x0010)
#define SCAN_HIGH_MEMORY        (0x0020)
#define SCAN_NOUI               (0x8000)

// ************************************************************************** //
// *  Possible error conditions that the callback func can be called with   * //
// ************************************************************************** //

#define SCANERR_VIRUSINFILE     1
#define SCANERR_VIRUSINBOOT     2
#define SCANERR_VIRUSINMEM      3
#define SCANERR_NOSUCHDRIVE     4
#define SCANERR_OUTAMEMORY      5
#define SCANERR_READERROR       6
#define SCANERR_USERSTOPED      7
#define SCANERR_DOSERROR        8
#define SCANERR_CALLSYMANTEC    9

// ************************************************************************** //
// *  Possible status conditions the callback func can be called with
// ************************************************************************** //
#define SCANINSTALL_SCANMEMORY_START    20
#define SCANINSTALL_SCANMEMORY_END      21
#define SCANINSTALL_SCANBOOTS_START     22
#define SCANINSTALL_SCANBOOTS_END       23
#define SCANINSTALL_SCANDIR             24      
#define SCANINSTALL_SCANDIR_END         25
#define SCANINSTALL_SCANFILE            26      // Sent before a file is scanned
#define SCANINSTALL_CHECKINGFILE        27      // Sent before checking a file

// ************************************************************************** //
// RESOURCE IDS of dialogs in NAVINST.DLL
// ************************************************************************** //
#define ID_NAVICON0  1
#define ID_NAVICON1  2
#define ID_NAVICON2  3
#define ID_NAVICON3  4

#define IDD_SCANDLG           100
#define ID_ICON                10
#define ID_FILLBAR             11
#define ID_DIRECTORY           12
#define ID_FILE                13
#define ID_STOP                14

#define IDD_MEMSCANDLG        200
#define IDI_MS_FILLMEM         10
#define IDI_MS_FILLMBOOT       11
#define IDI_MS_FILLBOOT        12

// ************************************************************************** //
// RESOURCE IDS of strings
// ************************************************************************** //

#define IDS_HARD_DRIVE            100

// ********************************************************* //

#define SIZE_WORK                256
#define NAV_SELECTOROF           "NavProp_SELECTOROF"
#define NAV_OFFSETOF             "NavProp_OFFSETOF"

// ********************************************************* //

typedef BOOL (CALLBACK* CALLBACKPROC)(struct tagCALLBACKSTRUCT FAR *);

typedef struct tagSCANINSTALL
{
    HWND            hParent;            // Parent
    HINSTANCE       hInstance;
    CALLBACKPROC    lpfnProc;           // Callback function address
    WORD            wFlags;             // use #define SCAN_xxx
    LPSTR           lpszCaption;        // Caption to use, NULL default
    DWORD           dwCookie;           // User defined
} SCANINSTALL, FAR *LPSCANINSTALL;


                                        // Same structure as above, but
                                        // will be filled out by 16-bit
                                        // applications

typedef struct tagSCANINSTALL16
{
    WORD          hParent;
    WORD          hInstance;
    CALLBACKPROC  lpfnProc;
    WORD          wFlags;
    LPSTR         lpszCaption;
} SCANINSTALL16, FAR *LPSCANINSTALL16;

typedef struct tagINTERNAL
{
    LPSCANINSTALL   lpScan;             // Application provided SCANINSTALL
    BOOL            bDoScan;            // Internal Flag to scan
    LPSTR           lpScanList;         // List of drives to scan
    char            szFullPath     [SYM_MAX_PATH+1];
    char            szTheDirectory [SYM_MAX_PATH+1];
    HWND            hMommyWindow;
    DWORD           dwScannedBytes;
    DWORD           dwTotalBytes;
    TIMERPROC       lpTimerProc;
    HVCONTEXT       hVContext;

} INTERNAL, FAR * LPINTERNAL;

typedef struct tagCALLBACKSTRUCT
{
    HWND            hParent;            // Parent
    HINSTANCE       hInstance;
    WORD            wError;             // Flags type of error
    LPSTR           lpFileName;         // Inf. File or directory
    LPSTR           lpVirusName;        // Virus infected with
    LPINTERNAL      lpInternal;         // Pointer to INTERNAL structure
    DWORD           dwCookie;           // User defined
} CALLBACKSTRUCT, FAR *LPCALLBACKSTRUCT;


#ifdef __cplusplus
extern "C" {
#endif

VOID SYM_EXPORT WINAPI ScanInstall (
    LPSCANINSTALL   lpScan
    );

BOOL SYM_EXPORT WINAPI ScanMemBoot (
    HWND        hDlg,
    LPINTERNAL  lpInt
    );

BOOL SYM_EXPORT WINAPI StartScan (
    HWND        hParent,
    LPINTERNAL  lpInt
    );

VOID SYM_EXPORT WINAPI SetSmallFont (
    HWND        hWnd,
    UINT        uChildID
    );

VOID SYM_EXPORT WINAPI LP_SET (
    HWND        hwnd,
    LPVOID      lpVoid
    );

VOID SYM_EXPORT WINAPI LP_FREE (
    HWND        hwnd
    );

LPVOID SYM_EXPORT WINAPI LP_GET (
    HWND        hwnd
    );

BOOL WINAPI InternalMemScan (
    HWND        hParent,
    LPINTERNAL  lpInternal
    );

VOID SYM_EXPORT CALLBACK StatsTimer  (
    HWND        hDlg, 
    UINT        message,
    WPARAM      wParam,
    LPARAM      lParam );

BOOL SYM_EXPORT WINAPI CallBackProc (
    HWND        hParent,
    LPINTERNAL  lpInt,
    UINT        uWhatHappened,
    HVLVIRUS      hVirus );

HVCONTEXT WINAPI InitVirusLibrary (     // Initialize virus library
    LPSTR   lpDatFile,                  // [in] path to DAT file
    LPSTR   lpInfFile );                // [in] path to INF file

VOID WINAPI UnloadVirusLibrary (        // Unload virus library
    HVCONTEXT hContext );               // [in] our nav pointer

#ifdef __cplusplus
}
#endif

#endif
