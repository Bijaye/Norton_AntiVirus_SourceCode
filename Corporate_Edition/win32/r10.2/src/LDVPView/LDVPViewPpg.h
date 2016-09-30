// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: LDVPViewPpg.h
//  Purpose: Declaration of the CLDVPViewPropPage property page class.
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
// CLDVPViewPropPage class
//
//----------------------------------------------------------------
class CLDVPViewPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CLDVPViewPropPage)
	DECLARE_OLECREATE_EX(CLDVPViewPropPage)

// Constructor
public:
	CLDVPViewPropPage();

// Dialog Data
	//{{AFX_DATA(CLDVPViewPropPage)
	enum { IDD = IDD_PROPPAGE_LDVPVIEW };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CLDVPViewPropPage)
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
