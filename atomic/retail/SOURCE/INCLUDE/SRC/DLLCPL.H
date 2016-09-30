#ifndef DLLCPL_H
#define DLLCPL_H


typedef struct tagDLLCPL
   {
   UINT        uPriority;
   HICON       hIcon;
   UINT        uIconID;
   DLGPROC     lpfnProc;
   UINT        uDlgTemplate;
   UINT        uTitle;
   UINT        uLabel;
   } DLLCPL, FAR *LPDLLCPL;

//=======================================================================
// Prototypes
//=======================================================================


BOOL SYM_EXPORT WINAPI NavOptsAdminDlgProc ( // Install/Update Options
    HWND        hDlg,                   // Dialog box handle
    WORD        wMessage,               // Message to process
    WPARAM      wParam,                 // wParam to process
    LPARAM      lParam);                // lParam to process

#endif



