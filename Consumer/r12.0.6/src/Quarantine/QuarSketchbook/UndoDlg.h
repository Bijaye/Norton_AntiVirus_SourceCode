// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
// UndoDlg.h : header file
#pragma once


// CUndoDlg dialog

class CUndoDlg : public CDialog
{
	DECLARE_DYNAMIC(CUndoDlg)

public:
	CUndoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUndoDlg();

	enum UndoType
	{
	Save,
	SaveTo,
	SaveAs
	};

	UndoType m_type;;
	CString strDir;
	CString strFile;

// Dialog Data
	enum { IDD = IDD_UNDO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedUndosave();
	afx_msg void OnBnClickedUndosaveto();
	afx_msg void OnBnClickedUndosaveas();
};
