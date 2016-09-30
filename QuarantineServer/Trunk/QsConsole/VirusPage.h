/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_VIRUSPAGE_H__9DEAC508_C832_11D2_8F46_3078302C2030__INCLUDED_)
#define AFX_VIRUSPAGE_H__9DEAC508_C832_11D2_8F46_3078302C2030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VirusPage.h : header file
//
#include "imagebutton.h"
#include "QPropPage.h"
#include "avscanner.h"

/////////////////////////////////////////////////////////////////////////////
// CVirusPage dialog

class CVirusPage : public CQPropPage
{
	DECLARE_DYNCREATE(CVirusPage)

// Construction
public:
	HRESULT Initialize( DWORD dwVirusID, CAVScanner* pScan);
	CVirusPage();
	~CVirusPage();

// Dialog Data
	//{{AFX_DATA(CVirusPage)
	enum { IDD = IDD_VIRUS_PAGE };
    CImageButton    m_Trigger;
    CImageButton    m_SizeStealth;
    CImageButton    m_Poly;
    CImageButton    m_Memory;
    CImageButton    m_FullStealth;
    CImageButton    m_Encrypting;
    CString m_sAliases;
    CString m_sComments;
    CString m_sInfects;
    CString m_sLength;
    CString m_sLikelihood;
    CString m_sVirusName;
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

    DWORD Release() 
        { 
        if( --m_dwRef == 0 )
            {
            delete this;
            return 0;
            }
        return m_dwRef;
        }



// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CVirusPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CVirusPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    CImageList m_ImageList;

private:
    void BuildInfectsString(
                            LPVOID  pVirusDef,
                            CAVScanner *pScan,
                            CString&      sBuffer 
                            );

    DWORD m_dwRef;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRUSPAGE_H__9DEAC508_C832_11D2_8F46_3078302C2030__INCLUDED_)
