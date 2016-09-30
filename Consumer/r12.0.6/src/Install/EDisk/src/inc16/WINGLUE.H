/* Copyright 1991 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   V:/INCLUDE/VCS/WINGLUE.H_V   1.14   01 Sep 1992 16:08:54   BASIL  $ *
 *                                                                      *
 * Description:                                                         *
 *  Include file for my emulation of some of the windows routines.	*
 *                                                                      *
 *  Major Changes:							* 
 *    HANDLE - Defined to be a LPVOID, not a WORD		        * 
 *									* 
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************ 
 * $Log:   V:/INCLUDE/VCS/WINGLUE.H_V  $ *
 * 
 *    Rev 1.14   01 Sep 1992 16:08:54   BASIL
 * Commented out CALLBACK because it was breaking the zip engine calls in install
 * 
 *    Rev 1.13   12 Aug 1992 18:06:10   HENRI
 * Added defines for string routines and timer ticks that are 
 * already in the Windows environment.
 * 
 *    Rev 1.12   12 Aug 1992 17:37:40   ED
 * More additions for Quake.
 * 
 *    Rev 1.11   12 Aug 1992 11:28:34   ED
 * Added check for inclusion of WINDOWS.H so
 * that WINGLUE will not attempt to redefine
 * things that WINDOWS.H already has.
 * 
 *    Rev 1.10   01 Jul 1992 13:55:40   ED
 * Fixed up some Windows compatibility problems
 * 
 *    Rev 1.9   19 Mar 1992 11:08:40   MARKK
 * Removed Debugging
 * 
 *    Rev 1.8   16 Feb 1992 19:38:14   ED
 * #ifndef'ed the TRUE/FALSE #defines
 * 
 *    Rev 1.7   26 Dec 1991 14:32:52   BARRY
 * Finally added a header so we can keep track of the revisions.
 ************************************************************************/

/*  If defined, the following flags inhibit definition
 *     of the indicated items.
 *
 *  NOGDICAPMASKS     - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
 *  NOVIRTUALKEYCODES - VK_*
 *  NOWINMESSAGES     - WM_*, EM_*, LB_*, CB_*
 *  NOWINSTYLES       - WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
 *  NOSYSMETRICS      - SM_*
 *  NOMENUS	      - MF_*
 *  NOICONS	      - IDI_*
 *  NOKEYSTATES       - MK_*
 *  NOSYSCOMMANDS     - SC_*
 *  NORASTEROPS       - Binary and Tertiary raster ops
 *  NOSHOWWINDOW      - SW_*
 *  OEMRESOURCE       - OEM Resource values
 *  NOATOM	      - Atom Manager routines
 *  NOCLIPBOARD       - Clipboard routines
 *  NOCOLOR	      - Screen colors
 *  NOCTLMGR	      - Control and Dialog routines
 *  NODRAWTEXT	      - DrawText() and DT_*
 *  NOGDI	      - All GDI defines and routines
 *  NOKERNEL	      - All KERNEL defines and routines
 *  NOUSER	      - All USER defines and routines
 *  NOMB	      - MB_* and MessageBox()
 *  NOMEMMGR	      - GMEM_*, LMEM_*, GHND, LHND, associated routines
 *  NOMETAFILE	      - typedef METAFILEPICT
 *  NOMINMAX	      - Macros min(a,b) and max(a,b)
 *  NOMSG	      - typedef MSG and associated routines
 *  NOOPENFILE	      - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
 *  NOSCROLL	      - SB_* and scrolling routines
 *  NOSOUND	      - Sound driver routines
 *  NOTEXTMETRIC      - typedef TEXTMETRIC and associated routines
 *  NOWH	      - SetWindowsHook and WH_*
 *  NOWINOFFSETS      - GWL_*, GCL_*, associated routines
 *  NOCOMM	      - COMM driver routines
 *  NOKANJI	      - Kanji support stuff.
 *  NOHELP            - Help engine interface.
 *  NOPROFILER	      - Profiler interface.
 *  NODEFERWINDOWPOS  - DeferWindowPos routines
 */

#ifndef _INC_WINDOWS

#ifndef	INCL_WINGLUE
#define INCL_WINGLUE

#ifndef INCL_TYPES
#include <types.h>
#endif

#ifdef RC_INVOKED

/* Turn off a bunch of stuff to ensure that RC files compile OK. */
#define NOATOM
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI

#endif /* RC_INVOKED */


/*--------------------------------------------------------------------------*/
/*  General Purpose Defines						    */
/*--------------------------------------------------------------------------*/

#ifndef	TRUE
#define TRUE    	1
#endif

#ifndef	FALSE
#define FALSE   	0
#endif

#ifndef	NULL
#define NULL            0               /* null ptr                     */
#endif

#ifndef FAR
#define FAR		    far
#endif


#ifndef NEAR
#define NEAR		    near
#endif

#define LONG		    long
#define VOID		    void
#define PASCAL		    pascal

#ifndef _WIN16
#define WINAPI              _pascal
#else
#define WINAPI              _far _pascal
#endif
#define API                 WINAPI
//#define CALLBACK            _far _pascal

#ifndef NOMINMAX

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif  /* NOMINMAX */

#define MAKELONG(a, b)	    ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)	    ((WORD)(DWORD)(l))
#define HIWORD(l)	    ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)	    ((BYTE)(w))
#define HIBYTE(w)	    ((BYTE)(((WORD)(w) >> 8) & 0xFF))

typedef unsigned char	    BYTE;
typedef unsigned char	    BOOL;
typedef unsigned int	    WORD;
typedef unsigned long	    DWORD;
typedef char near	    *PSTR;
typedef char near	    *NPSTR;
typedef char far	    *LPSTR;
typedef const char FAR*     LPCSTR;
typedef BYTE near	    *PBYTE;
typedef BYTE far	    *LPBYTE;
typedef BYTE huge	    *HPBYTE;
typedef int near	    *PINT;
typedef int far 	    *LPINT;
typedef WORD near	    *PWORD;
typedef WORD far	    *LPWORD;
typedef long near	    *PLONG;
typedef long far	    *LPLONG;
typedef DWORD near	    *PDWORD;
typedef DWORD far	    *LPDWORD;
typedef void far	    *LPVOID;
typedef unsigned int  	    UINT;

#ifndef WIN_INTERNAL
typedef WORD		    HANDLE;
typedef HANDLE		    HWND;
#endif

/* Types use for passing & returning polymorphic values */
typedef UINT WPARAM;
typedef LONG LPARAM;
typedef LONG LRESULT;
#define MAKELPARAM(low, high)	((LPARAM)MAKELONG(low, high))
#define MAKELRESULT(low, high)	((LRESULT)MAKELONG(low, high))


typedef HANDLE		    *PHANDLE;
typedef HANDLE NEAR	    *SPHANDLE;
typedef HANDLE FAR	    *LPHANDLE;
typedef HANDLE		    HGLOBAL;
typedef HANDLE		    HLOCAL;
typedef HANDLE		    HFONT;
typedef HANDLE		    HINSTANCE;
typedef HANDLE		    HMODULE;
typedef HANDLE		    HTASK;
typedef HANDLE		    GLOBALHANDLE;
typedef HANDLE		    LOCALHANDLE;
typedef int (FAR PASCAL *FARPROC)();
typedef int (NEAR PASCAL *NEARPROC)();

typedef int HFILE;      /* Polymorphic with C runtime file handle type */
#define HFILE_ERROR ((HFILE)-1)



/*--------------------------------------------------------------------------*/
/*  KERNEL Section							    */
/*--------------------------------------------------------------------------*/

#ifndef NOKERNEL

/* Loader Routines */
WORD	FAR PASCAL GetVersion(void);
WORD	FAR PASCAL GetNumTasks(void);
HANDLE	FAR PASCAL GetCodeHandle(FARPROC);
void    FAR PASCAL GetCodeInfo(FARPROC lpProc, LPVOID lpSegInfo);
HANDLE	FAR PASCAL GetModuleHandle(LPSTR);
int	FAR PASCAL GetModuleUsage(HANDLE);
int	FAR PASCAL GetModuleFileName(HANDLE, LPSTR, int);
int	FAR PASCAL GetInstanceData(HANDLE, NPSTR, int);
FARPROC FAR PASCAL GetProcAddress(HANDLE, LPSTR);
FARPROC FAR PASCAL MakeProcInstance(FARPROC, HANDLE);
void	FAR PASCAL FreeProcInstance(FARPROC);
HANDLE	FAR PASCAL LoadLibrary(LPSTR);
HANDLE  FAR PASCAL LoadModule(LPSTR, LPVOID);
BOOL    FAR PASCAL FreeModule(HANDLE);
void	FAR PASCAL FreeLibrary(HANDLE);
DWORD   FAR PASCAL GetFreeSpace(WORD);
WORD	FAR PASCAL WinExec(LPSTR, WORD);
void    FAR PASCAL DebugBreak();
void    FAR PASCAL OutputDebugString(LPSTR);
void    FAR PASCAL SwitchStackBack();
void    FAR PASCAL SwitchStackTo(WORD, WORD, WORD);
WORD    FAR PASCAL GetCurrentPDB(void);

#ifndef NOOPENFILE

/* OpenFile() Structure */
typedef struct tagOFSTRUCT
  {
    BYTE	cBytes;
    BYTE	fFixedDisk;
    WORD	nErrCode;
    BYTE	reserved[4];
    BYTE	szPathName[128];
  } OFSTRUCT;
typedef OFSTRUCT	    *POFSTRUCT;
typedef OFSTRUCT NEAR	    *NPOFSTRUCT;
typedef OFSTRUCT FAR	    *LPOFSTRUCT;

/* OpenFile() Flags */
#define OF_READ 	    0x0000
#define OF_WRITE	    0x0001
#define OF_READWRITE	    0x0002
#define OF_SHARE_COMPAT	    0x0000
#define OF_SHARE_EXCLUSIVE  0x0010
#define OF_SHARE_DENY_WRITE 0x0020
#define OF_SHARE_DENY_READ  0x0030
#define OF_SHARE_DENY_NONE  0x0040
#define OF_PARSE	    0x0100
#define OF_DELETE	    0x0200
#define OF_VERIFY	    0x0400
#define OF_CANCEL	    0x0800
#define OF_CREATE	    0x1000
#define OF_PROMPT	    0x2000
#define OF_EXIST	    0x4000
#define OF_REOPEN	    0x8000

int  FAR PASCAL OpenFile(LPSTR, LPOFSTRUCT, WORD);

/* GetTempFileName() Flags */
#define TF_FORCEDRIVE	    (BYTE)0x80

BYTE FAR PASCAL GetTempDrive(BYTE);
int  FAR PASCAL GetTempFileName(BYTE, LPSTR, WORD, LPSTR);
WORD FAR PASCAL SetHandleCount(WORD);

WORD FAR PASCAL GetDriveType(int);
/* GetDriveType return values */
#define DRIVE_REMOVABLE 2
#define DRIVE_FIXED     3
#define DRIVE_REMOTE    4

#endif /* NOOPENFILE */

#ifndef NOMEMMGR

/* Global Memory Flags */
#define GMEM_FIXED	    0x0000
#define GMEM_MOVEABLE	    0x0002
#define GMEM_NOCOMPACT	    0x0010
#define GMEM_NODISCARD	    0x0020
#define GMEM_ZEROINIT	    0x0040
#define GMEM_MODIFY	    0x0080
#define GMEM_DISCARDABLE    0x0100
#define GMEM_NOT_BANKED     0x1000
#define GMEM_SHARE	    0x2000
#define GMEM_DDESHARE	    0x2000
#define GMEM_NOTIFY	    0x4000
#define GMEM_LOWER	    GMEM_NOT_BANKED

#define GHND		    (GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR		    (GMEM_FIXED | GMEM_ZEROINIT)

#define GlobalDiscard(h) GlobalReAlloc(h, 0L, GMEM_MOVEABLE)

HANDLE FAR PASCAL GlobalAlloc(WORD, DWORD);
DWORD  FAR PASCAL GlobalCompact(DWORD);
DWORD  FAR PASCAL GlobalHandle(LPVOID);
HANDLE FAR PASCAL GlobalReAlloc(HANDLE, DWORD, WORD);
DWORD  FAR PASCAL GlobalSize(HANDLE);
WORD   FAR PASCAL GlobalFlags(HANDLE);
LPSTR  FAR PASCAL GlobalWire(HANDLE);
BOOL   FAR PASCAL GlobalUnWire(HANDLE);
HANDLE FAR PASCAL GlobalLRUNewest(HANDLE);
HANDLE FAR PASCAL GlobalLRUOldest(HANDLE);
VOID   FAR PASCAL GlobalNotify(FARPROC);
WORD   FAR PASCAL GlobalPageLock(HANDLE);
WORD   FAR PASCAL GlobalPageUnlock(HANDLE);
VOID   FAR PASCAL GlobalFix(HANDLE);
BOOL   FAR PASCAL GlobalUnfix(HANDLE);

#define	GlobalLock(h)		VMMLock(h)
#define	GlobalFree(h)		VMMFree(h)
#define	GlobalRelease(h)	VMMRelease(h)
#define	GlobalUnlock(h, p)	VMMUnlock(h, p)

#ifdef  MEMORY_TRACKING

HANDLE FAR PASCAL GlobalAllocDBG(WORD, DWORD, Byte near *, Word );
HANDLE FAR PASCAL GlobalFreeDBG(HANDLE *, Byte near *, Word );
DWORD  FAR PASCAL GlobalHandleDBG(LPVOID, Byte near *, Word );
LPVOID FAR PASCAL GlobalLockDBG(HANDLE, Byte near *, Word );
HANDLE FAR PASCAL GlobalReAllocDBG(HANDLE, DWORD, WORD, Byte near *, Word );
BOOL   FAR PASCAL GlobalReleaseDBG(HANDLE, LPVOID, Byte near *, Word );

#define	GlobalAlloc(f, s)	GlobalAllocDBG(f, s, szMemoryTrackFileName, __LINE__)
#define	GlobalHandle(p)		GlobalHandleDBG(p, szMemoryTrackFileName, __LINE__)
#define	GlobalReAlloc(h, s, f)	GlobalReAllocDBG(h, s, f, szMemoryTrackFileName, __LINE__)

#endif

/* Flags returned by GlobalFlags (in addition to GMEM_DISCARDABLE) */
#define GMEM_DISCARDED	    0x4000
#define GMEM_LOCKCOUNT	    0x00FF

HANDLE FAR PASCAL LockSegment(WORD);
HANDLE FAR PASCAL UnlockSegment(WORD);

/* Local Memory Flags */
#define LMEM_FIXED	    0x0000
#define LMEM_MOVEABLE	    0x0002
#define LMEM_NOCOMPACT	    0x0010
#define LMEM_NODISCARD	    0x0020
#define LMEM_ZEROINIT	    0x0040
#define LMEM_MODIFY	    0x0080
#define LMEM_DISCARDABLE    0x0F00

#define LHND		    (LMEM_MOVEABLE | LMEM_ZEROINIT)
#define LPTR		    (LMEM_FIXED | LMEM_ZEROINIT)

#define NONZEROLHND	    (LMEM_MOVEABLE)
#define NONZEROLPTR	    (LMEM_FIXED)

#define LNOTIFY_OUTOFMEM    0
#define LNOTIFY_MOVE	    1
#define LNOTIFY_DISCARD     2

WORD NEAR * PASCAL pLocalHeap;

#define LocalDiscard(h)     LocalReAlloc(h, 0, LMEM_MOVEABLE)

HANDLE	FAR PASCAL LocalAlloc(WORD, WORD);
WORD	FAR PASCAL LocalCompact(WORD);
HANDLE	FAR PASCAL LocalFree(HANDLE);
HANDLE	FAR PASCAL LocalHandle(WORD);
BOOL    FAR PASCAL LocalInit( WORD, WORD, WORD);
char NEAR * FAR PASCAL LocalLock(HANDLE);
FARPROC FAR PASCAL LocalNotify(FARPROC);
HANDLE	FAR PASCAL LocalReAlloc(HANDLE, WORD, WORD);
WORD	FAR PASCAL LocalSize(HANDLE);
BOOL	FAR PASCAL LocalUnlock(HANDLE);
WORD	FAR PASCAL LocalFlags(HANDLE);
WORD	FAR PASCAL LocalShrink(HANDLE, WORD);

/* Flags returned by LocalFlags (in addition to LMEM_DISCARDABLE) */
#define LMEM_DISCARDED	    0x4000
#define LMEM_LOCKCOUNT	    0x00FF

#endif /* NOMEMMGR */

LONG   FAR PASCAL SetSwapAreaSize(WORD);
LPSTR  FAR PASCAL ValidateFreeSpaces(void);
VOID   FAR PASCAL LimitEmsPages(DWORD);
BOOL   FAR PASCAL SetErrorMode(WORD);
VOID   FAR PASCAL ValidateCodeSegments(void);

LPSTR  FAR PASCAL GetDOSEnvironment(void);

WORD   FAR PASCAL AllocDStoCSAlias(WORD);
WORD   FAR PASCAL AllocSelector(WORD);
WORD   FAR PASCAL ChangeSelector(WORD, WORD);
WORD   FAR PASCAL FreeSelector(WORD);

#define UnlockResource(h)   GlobalRelease(h)

HANDLE FAR PASCAL FindResource(HANDLE, LPSTR, LPSTR);
HANDLE FAR PASCAL LoadResource(HANDLE, HANDLE);
BOOL   FAR PASCAL FreeResource(HANDLE);
LPSTR  FAR PASCAL LockResource(HANDLE);
FARPROC FAR PASCAL SetResourceHandler(HANDLE, LPSTR, FARPROC);
HANDLE FAR PASCAL AllocResource(HANDLE, HANDLE, DWORD);
WORD   FAR PASCAL SizeofResource(HANDLE, HANDLE);
int    FAR PASCAL AccessResource(HANDLE, HANDLE);

#define MAKEINTRESOURCE(i)  (LPSTR)((DWORD)((WORD)(i)))

#ifndef NORESOURCE


/* Predefined Resource Types */
#define RT_CURSOR	    MAKEINTRESOURCE(1)
#define RT_BITMAP	    MAKEINTRESOURCE(2)
#define RT_ICON 	    MAKEINTRESOURCE(3)
#define RT_MENU 	    MAKEINTRESOURCE(4)
#define RT_DIALOG	    MAKEINTRESOURCE(5)
#define RT_STRING	    MAKEINTRESOURCE(6)
#define RT_FONTDIR	    MAKEINTRESOURCE(7)
#define RT_FONT 	    MAKEINTRESOURCE(8)
#define RT_ACCELERATOR	    MAKEINTRESOURCE(9)
#define RT_RCDATA	    MAKEINTRESOURCE(10)



#endif /* NORESOURCE */

void   FAR PASCAL Yield(void);
HANDLE FAR PASCAL GetCurrentTask(void);

#ifndef NOATOM
typedef WORD		    ATOM;

#define MAKEINTATOM(i)	    (LPSTR)((DWORD)((WORD)(i)))

BOOL   FAR PASCAL InitAtomTable(int);
ATOM   FAR PASCAL AddAtom(LPSTR);
ATOM   FAR PASCAL DeleteAtom(ATOM);
ATOM   FAR PASCAL FindAtom(LPSTR);
WORD   FAR PASCAL GetAtomName(ATOM, LPSTR, int);
ATOM   FAR PASCAL GlobalAddAtom(LPSTR);
ATOM   FAR PASCAL GlobalDeleteAtom(ATOM);
ATOM   FAR PASCAL GlobalFindAtom(LPSTR);
WORD   FAR PASCAL GlobalGetAtomName(ATOM, LPSTR, int);
HANDLE FAR PASCAL GetAtomHandle(ATOM);
void FAR *PASCAL GetAtomTable(DWORD *lpwSize);

#endif /* NOATOM */

/* User Profile Routines */
WORD FAR PASCAL GetProfileInt(LPSTR, LPSTR, int);
int  FAR PASCAL GetProfileString(LPSTR, LPSTR, LPSTR, LPSTR, int);
BOOL FAR PASCAL WriteProfileString(LPSTR, LPSTR, LPSTR);
WORD FAR PASCAL GetPrivateProfileInt(LPSTR, LPSTR, int, LPSTR);
int  FAR PASCAL GetPrivateProfileString(LPSTR, LPSTR, LPSTR, LPSTR, int, LPSTR);
BOOL FAR PASCAL WritePrivateProfileString(LPSTR, LPSTR, LPSTR, LPSTR);

WORD FAR PASCAL GetWindowsDirectory(LPSTR,WORD);
WORD FAR PASCAL GetSystemDirectory(LPSTR,WORD);

/* Catch() and Throw() */
typedef int		    CATCHBUF[9];
typedef int FAR 	    *LPCATCHBUF;

int  FAR PASCAL Catch(LPCATCHBUF);
void FAR PASCAL Throw(LPCATCHBUF, int);

void FAR PASCAL FatalExit(int);

void FAR PASCAL SwapRecording(WORD);

/* Character Translation Routines */
int   FAR PASCAL AnsiToOem(LPSTR, LPSTR);
BOOL  FAR PASCAL OemToAnsi(LPSTR, LPSTR);
void  FAR PASCAL AnsiToOemBuff(LPSTR, LPSTR, int);
void  FAR PASCAL OemToAnsiBuff(LPSTR, LPSTR, int);
LPSTR FAR PASCAL AnsiUpper(LPSTR);
WORD  FAR PASCAL AnsiUpperBuff(LPSTR, WORD);
LPSTR FAR PASCAL AnsiLower(LPSTR);
WORD  FAR PASCAL AnsiLowerBuff(LPSTR, WORD);
LPSTR FAR PASCAL AnsiNext(LPSTR);
LPSTR FAR PASCAL AnsiPrev(LPSTR, LPSTR);

/* Keyboard Information Routines */
#ifndef	NOKEYBOARDINFO
DWORD FAR PASCAL OemKeyScan(WORD);
WORD  FAR PASCAL VkKeyScan(WORD);
int   FAR PASCAL GetKeyboardType(int);
WORD  FAR PASCAL MapVirtualKey(WORD, WORD);
int   FAR PASCAL GetKBCodePage();
int   FAR PASCAL GetKeyNameText(LONG, LPSTR, int);
int   FAR PASCAL ToAscii(WORD wVirtKey, WORD wScanCode, LPSTR lpKeyState, LPVOID lpChar, WORD wFlags);
#endif

#ifndef  NOLANGUAGE
/* Language dependent Routines */
BOOL  FAR  PASCAL IsCharAlpha(char);
BOOL  FAR  PASCAL IsCharAlphaNumeric(char);
BOOL  FAR  PASCAL IsCharUpper(char);
BOOL  FAR  PASCAL IsCharLower(char);
#endif

LONG FAR PASCAL GetWinFlags(void);

#define WF_PMODE	0x0001
#define WF_CPU286	0x0002
#define WF_CPU386	0x0004
#define WF_CPU486	0x0008
#define WF_STANDARD	0x0010
#define WF_WIN286	0x0010
#define WF_ENHANCED	0x0020
#define WF_WIN386	0x0020
#define WF_CPU086	0x0040
#define WF_CPU186	0x0080
#define WF_LARGEFRAME	0x0100
#define WF_SMALLFRAME	0x0200
#define WF_80x87	0x0400

/* WEP fSystemExit flag values */
#define	WEP_SYSTEM_EXIT	1
#define	WEP_FREE_DLL	0

/****************************************************************************/
/*									    */
/* io routines								    */
/*									    */
/****************************************************************************/

#include <vdiskrec.h>

       WORD LargeWrite(WORD wFile, void far *fpData, DWORD dwCount, LPVDISKREC lpVDiskRec);
       WORD LargeRead (WORD wFile, void far *fpData, DWORD dwCount, LPVDISKREC lpVDiskRec);
       WORD _lwrite   (WORD wFile, void far *fpData, WORD wCount);
       WORD _lread    (WORD wFile, void far *fpData, WORD wCount);

#endif /* NOKERNEL */


#define lstrncmp                        _fstrncmp
#define lstrcpy                         _fstrcpy
#define lstrcmp                         _fstrcmp
#define lstrlen                         _fstrlen
#define lstrcat                         _fstrcat

#define GetTickCount()		        TimerTicks()

#endif /* INCL_WINGLUE */

#endif  /* _INC_WINDOWS */

