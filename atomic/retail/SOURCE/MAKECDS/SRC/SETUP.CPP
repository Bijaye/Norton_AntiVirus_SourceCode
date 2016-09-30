//
// Copyright Symantec Corporation 1998
// 
//	Stub program that displays IBM licensing agreement and then 
//  launches NAV setup program.
//
//	Created by: RPULINT
//  Date	  : 09/14/1998
//

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "resource.h"

#define DECALPHA  1

BOOL CALLBACK LicenseDlgProc(HWND hWnd, UINT uMsg, 
							 WPARAM wParam, LPARAM lParam);
HINSTANCE g_hInstance = 0 ;

int WINAPI WinMain (HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCommandLine,
					int nCmdShow)
{

	g_hInstance = hInstance;                         
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_LICENSE), 0, (DLGPROC)LicenseDlgProc);

	return 0 ;
}

BOOL CALLBACK LicenseDlgProc(HWND hWnd, UINT uMsg, 
							 WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
		HFILE hFile = _lopen ("IBMAVLIC.TXT", OF_READ);
		if ( hFile != HFILE_ERROR)
		{
			long lFileLength = 0 ;                       
			char far * lpBuffer = NULL;

			char szOKTitle[512] ;
			char szCancelTitle[512] ;
			
			// OK & Cancel Text
			LoadString (g_hInstance, IDOK_TITLE, szOKTitle, sizeof(szOKTitle));
			LoadString (g_hInstance, IDCANCEL_TITLE, szCancelTitle, sizeof(szCancelTitle));
			SetDlgItemText(hWnd, IDOK, szOKTitle);
			SetDlgItemText(hWnd, IDCANCEL, szCancelTitle);
						
			lFileLength = _llseek(hFile, 0L, 2); // End of file
			if (lFileLength != HFILE_ERROR)
			{
				_llseek(hFile, 0L, 0);  // Beginning
				
#ifndef DECALPHA
				lpBuffer = (char  far *) _fmalloc ((size_t)lFileLength+1);
				_fmemset ((void far *)lpBuffer, 0, (size_t)lFileLength);
#else
				lpBuffer = (char  far *) malloc ((size_t)lFileLength+1);
				memset ((void far *)lpBuffer, 0, (size_t)lFileLength);
#endif

				lpBuffer[lFileLength] = 0 ;
				if ( 0 != _lread (hFile, (LPVOID)lpBuffer, (size_t)lFileLength))
				{
					lpBuffer[lFileLength] = 0 ;
					SetDlgItemText(hWnd, IDC_EDIT, lpBuffer);
				}
									
#ifndef DECALPHA
				_ffree ((char far *)lpBuffer);
#else
				free ((char far *)lpBuffer);
#endif

			}
			_lclose(hFile);
		}
		else
		{
			MessageBox(NULL, "Cannot find IBM License Agreement.\n\nExiting Installation.", "IBM License Agreement",MB_OK);
			EndDialog(hWnd, 0);
		}                                                         

		// Center the dialog
		int nWidth = GetSystemMetrics (SM_CXSCREEN);
		int nHeight = GetSystemMetrics (SM_CYSCREEN);
		RECT	dlgRect ;
		
		GetWindowRect ( hWnd, &dlgRect ) ;
		int nDialogWidth  = dlgRect.right - dlgRect.left ;
		int nDialogHeight = dlgRect.bottom - dlgRect.top  ;

		int nStartX = ( nWidth - nDialogWidth ) /  2;
		int nStartY = ( nHeight - nDialogHeight ) /  2;
		MoveWindow ( hWnd, nStartX, nStartY, nDialogWidth, nDialogHeight, TRUE);				

		
		return 0;
		}

	case WM_COMMAND:

		switch(LOWORD(wParam))
		{
		case IDCANCEL: 
			EndDialog(hWnd, 0);
			return 1;

		case IDOK: 
			WinExec( "SETUPNAV.EXE" , SW_SHOWNORMAL);
			EndDialog(hWnd, 0);
			return 1;
		
		}
	}
	return 0 ;
}
