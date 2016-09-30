// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// property sheet extension interface

#include "scaninf.h"


class CPropSheetEx : public CCmdTarget
{
	DECLARE_DYNCREATE(CPropSheetEx)
protected:
	CPropSheetEx();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropSheetEx)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPropSheetEx();
	static BOOL WINAPI PageDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static UINT WINAPI SheetProc(HWND hwnd, UINT msg, LPPROPSHEETPAGE pPage);

	DWORD m_nPageType;

	// Generated message map functions
	//{{AFX_MSG(CPropSheetEx)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_OLECREATE(CPropSheetEx)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CPropSheetEx)
	//}}AFX_DISPATCH


	BEGIN_INTERFACE_PART(ScanImpl, IScanDialogs)
		STDMETHOD(ShowDetectableVirusesNoProvider)(/*in*/LPTSTR buffer,/*in*/unsigned char __RPC_FAR *szComputerName );
		STDMETHOD(CreateConfigPage)(ULONG cMode,LONG  *pHandle);			
		STDMETHOD(ShowPasswordDlg)(void);
		STDMETHOD(GetServicesState)( long *lServiceState );
		STDMETHOD(LoadServices)();
		STDMETHOD(UnLoadServices)();
		STDMETHOD(ServiceRunningDlg)( BSTR bstrOption );
		STDMETHOD(ScanDropFiles)(long hDrop);
		STDMETHOD(ShowConfigHistoryDlg)(HWND hParent,long flags,LPUNKNOWN pUnk,LPUNKNOWN pUnk2);	
    END_INTERFACE_PART(ScanImpl)  

	DECLARE_INTERFACE_MAP()
private:
    // ** DATA TYPES **
	typedef UINT (APIENTRY *DragQueryFileWfunc)( HDROP, UINT, LPWSTR, UINT );
	typedef DWORD (WINAPI *GetShortPathNameWfunc)( LPCWSTR lpszLongPath, LPWSTR lpszShortPath, DWORD cchBuffer );
    
    // ** CONSTANTS **
    static const LPCTSTR ShellDllName;          // Name of SHELL32.DLL
    static const LPCTSTR KernelDllName;         // Name of KERNEL32.DLL

    // ** FUNCTION MEMBERS **
    // As DragQueryFile, but does it's best to retrieve a *good* ANSI MBCS filename for the identified file.
    // Filenames are stored in Unicode, which does not always translate to the shell's ANSI code page, resulting in
    // bad filenames.  The workaround code will return an alternate filename (SFN) usable to open the file.
    // FilenameBufferSize is in bytes.
    // Returns S_OK on success and no char translation issues detected, S_FALSE on success with translation done, E_FAIL if GetShortPathNameW
    // needed but not found, else the error code of the failure.
    HRESULT SafeDragQueryFile( HDROP dropHandle, UINT fileNumber, LPTSTR filenameBuffer, DWORD filenameBufferSize );
    
    // ** DATA MEMBERS **
	HMODULE					shellLibraryHandle;
	HMODULE					kernelLibraryHandle;
	static const LPCTSTR	DragQueryFileWexportName;
	static const LPCTSTR	GetShortPathNameWexportName;
	DragQueryFileWfunc		dragQueryFileWptr;
	GetShortPathNameWfunc	getShortPathnameWptr;
    bool                    osSupportsNoBestFitChars;
};
