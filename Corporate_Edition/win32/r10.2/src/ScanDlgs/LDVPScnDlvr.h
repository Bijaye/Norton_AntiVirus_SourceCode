// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// LDVPScnDlvr.h: interface for the CLDVPScnDlvr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LDVPSCNDLVR_H__29EF73C1_DE0F_11D2_A455_00A02438707D__INCLUDED_)
#define AFX_LDVPSCNDLVR_H__29EF73C1_DE0F_11D2_A455_00A02438707D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLDVPScnDlvr  
{
public:
	CLDVPScnDlvr();
	CLDVPScnDlvr(CResultsView *ResultsView);
	virtual ~CLDVPScnDlvr();

	DWORD SendVBinFilesToSARC();
	DWORD MakeScnDlvrItems();

protected:
	CResultsView* m_resultsview;
};

#endif // !defined(AFX_LDVPSCNDLVR_H__29EF73C1_DE0F_11D2_A455_00A02438707D__INCLUDED_)
