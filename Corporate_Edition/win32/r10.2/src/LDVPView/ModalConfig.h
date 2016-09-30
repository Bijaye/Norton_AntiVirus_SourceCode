// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ModalConfig.H
//  Purpose: Definition of the Modal Config sheet
//
//	Date: 3-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "RTSMonitorPage.h"
#include "LDVPPropSheet.h"

//----------------------------------------------------------------
//
// class CModalConfig
//
//----------------------------------------------------------------
class CModalConfig : public CLDVPPropSheet
{
	DECLARE_DYNAMIC(CModalConfig)

private:
	CRTSMonitor	m_pageRTSTask;

// Construction
public:

	CModalConfig(UINT nIDCaption, DWORD PageMask, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModalConfig)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CModalConfig();

	// Generated message map functions
protected:
	//{{AFX_MSG(CModalConfig)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
