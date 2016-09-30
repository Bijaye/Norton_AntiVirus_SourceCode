// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: WProtect32Doc.h
//  Purpose: CWProtect32Doc definition file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#if !defined (WPROTECT_DOC_INCLUDED)
#define WPROTECT_DOC_INCLUDED

#include "LdvpTaskfs.h"

//#ifndef CLIENTUSER		// include client scan Definitions
//#define CLIENTUSER
//#endif


//----------------------------------------------------------------
//
// CWProtect32Doc class
//
//----------------------------------------------------------------
class CWProtect32Doc : public CDocument
{
private:
	
	CLDVPTaskFS		*m_ptrTaskWnd;	

	BOOL ValidateSettings();

	BOOL TaskExists( const CString &strTaskname );


public:
	DWORD	m_fRecording;
	BOOL	m_bRecordingStarted;
	
	void CancelRecord();

protected: // create from serialization only
	CWProtect32Doc();

	DECLARE_DYNCREATE(CWProtect32Doc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWProtect32Doc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWProtect32Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWProtect32Doc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif
