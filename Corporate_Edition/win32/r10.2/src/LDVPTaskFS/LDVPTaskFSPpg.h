// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: LDVPTaskFSPpg.h
//  Purpose: Declaration of the CLDVPTaskFSPropPage property page class.
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

//----------------------------------------------------------------
//
// CLDVPTaskFSPropPage class
//
//----------------------------------------------------------------
class CLDVPTaskFSPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CLDVPTaskFSPropPage)
	DECLARE_OLECREATE_EX(CLDVPTaskFSPropPage)

// Constructor
public:
	CLDVPTaskFSPropPage();

// Dialog Data
	//{{AFX_DATA(CLDVPTaskFSPropPage)
	enum { IDD = IDD_PROPPAGE_LDVPTASKFS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CLDVPTaskFSPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
