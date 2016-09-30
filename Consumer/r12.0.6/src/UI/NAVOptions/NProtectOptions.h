
#ifndef NPROTECT_OPTIONS_H
#define NPROTECT_OPTIONS_H

BOOL DisplayNProtectOptions(HWND hWndParent,int iPage);

int WINAPI PropSheetCallback(HWND hwnd, UINT uMsg, LPARAM lParam);

BOOL WINAPI AddPages(HPROPSHEETPAGE hPropSheetPage, LPARAM lParam);

#endif

