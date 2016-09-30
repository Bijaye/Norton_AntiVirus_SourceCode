// Copyright 1992 - 1997 Symantec, Peter Norton Group
// ************************************************************************
//                                                                      
// $Header:   S:/NAVWINX5/VCS/ABOUT.CPv   1.0   01 Jun 1998 16:07:42   DBuches  $
//                                                                      
// Description: 
//                                                                      
// See Also:                                                            
//                                                                      
// ************************************************************************
// $Log:   S:/NAVWINX5/VCS/ABOUT.CPv  $
// 
//    Rev 1.0   01 Jun 1998 16:07:42   DBuches
// Initial revision.
// 
//    Rev 1.0   19 May 1998 15:52:48   PAGBABI
// Initial revision.
// ************************************************************************


#include "stdafx.h"
#include "resource.h"

#include <initguid.h>
#include "about.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNavextAbout::CNavextAbout()
{
}


CNavextAbout::~CNavextAbout()
{
}


// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns: 
//      
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

HRESULT CNavextAbout::AboutHelper(UINT nID, LPOLESTR* lpPtr)
{
    if (lpPtr == NULL)
        return E_POINTER;

    CString s;

    // Needed for Loadstring
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    s.LoadString(nID);
    *lpPtr = reinterpret_cast<LPOLESTR>
            (CoTaskMemAlloc((s.GetLength() + 1)* sizeof(wchar_t)));

    if (*lpPtr == NULL)
        return E_OUTOFMEMORY;

	USES_CONVERSION;

    wcscpy(*lpPtr, T2OLE((LPTSTR)(LPCTSTR)s));

    return S_OK;
}


// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns: 
//      
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

STDMETHODIMP CNavextAbout::GetSnapinDescription(LPOLESTR* lpDescription)
{
    return AboutHelper(IDS_SNAPIN_DESC, lpDescription);
}


// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns: 
//      
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

STDMETHODIMP CNavextAbout::GetProvider(LPOLESTR* lpName)
{
    return AboutHelper(IDS_VENDOR, lpName);
}


// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns: 
//      
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

STDMETHODIMP CNavextAbout::GetSnapinVersion(LPOLESTR* lpVersion)
{
    return AboutHelper(IDS_VERSION, lpVersion);
}


// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns: 
//      
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

STDMETHODIMP CNavextAbout::GetSnapinImage(HICON* hAppIcon)
{
    if (hAppIcon == NULL)
        return E_POINTER;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *hAppIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_MAINAPP));

    ASSERT(*hAppIcon != NULL);
    return (*hAppIcon != NULL) ? S_OK : E_FAIL;
}


// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns: 
//      
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

STDMETHODIMP CNavextAbout::GetStaticFolderImage(HBITMAP* hSmallImage, 
                                                    HBITMAP* hSmallImageOpen,
                                                    HBITMAP* hLargeImage, 
                                                    COLORREF* cLargeMask)
{
    if (hSmallImage == NULL || 
        hSmallImageOpen == NULL ||
        hLargeImage == NULL ||
        cLargeMask == NULL)
        return E_POINTER;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *hSmallImage = ::LoadBitmap( AfxGetInstanceHandle(),
                                 MAKEINTRESOURCE(IDB_MAINAPP_16) );
    *hSmallImageOpen = ::LoadBitmap( AfxGetInstanceHandle(),
                                 MAKEINTRESOURCE(IDB_MAINAPP_16) );
    *hLargeImage = ::LoadBitmap( AfxGetInstanceHandle(),
                                 MAKEINTRESOURCE(IDB_MAINAPP_32) );
    *cLargeMask = RGB(255,0,255);   // magenta

    return S_OK;
}
