// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if defined(SYM_WIN16)
   BOOL WINAPI SymUTILAttach(HINSTANCE hModule, UINT wDataSeg, UINT cbHeapSize,
                        LPCSTR lpszCmdLine);
#endif

#if defined(SYM_WIN32)
extern "C"
   BOOL WINAPI SymUTILAttach(HINSTANCE hInstance, DWORD dwReason, LPVOID c);
#endif




/*-----------------08-18-94 05:09pm-----------------
These are prototypes of functions that were once
 imported from SYMGUI.  To remove the cross dependency
 of SYMUTIL on SYMGUI, I copied these functions
 locally. - BF
--------------------------------------------------*/

VOID WINAPI SU_SetChildSmallFont ( // Sets child to small font
    HWND        hWnd,                   // [in] Parent handle
    WORD        wChildID);              // [in] ID of child control

VOID WINAPI SU_SetChildLargeFont ( // Sets child to large/bold font
    HWND        hWnd,                   // [in] Parent handle
    WORD        wChildID);              // [in] ID of child control

WORD WINAPI SU_FontGet(
    HWND  hWnd,                         // Window to get DC from...
    HFONT FAR *lphFont,                 // If NULL, then free cached fonts
    int nWeight,                        // FW_NORMAL, FW_BOLD, or custom wt
    int nHeight );                      // If zero, then use default height

LPSTR WINAPI SULoadStr(WORD wId);
