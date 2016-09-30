// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#pragma once
#include "afxwin.h"
#include "Resource.h"
#include "afxcmn.h"
#include "GEDataStoreUtils.h"

// CGlobalExceptionDlg dialog

class CGlobalExceptionDlg : public CDialog
{
		DECLARE_DYNAMIC(CGlobalExceptionDlg)

	public:
		CGlobalExceptionDlg(CWnd* pParent = NULL);   // standard constructor
		~CGlobalExceptionDlg();

		// Dialog Data
		//{{AFX_DATA(CAcceptedFilesPropertyPage)
		enum { IDD = IDD_GLOBAL_EXCEPTION_HANDLING };
		//}}AFX_DATA

	public:
		virtual BOOL OnInitDialog();
		afx_msg void OnBtnClickOk();
	    afx_msg void OnBtnClickHelp();

		afx_msg void OnBtnClickAdd();
		afx_msg void OnBtnClickEdit();
		afx_msg void OnBtnClickDelete();

		afx_msg void OnAddKnownRiskExceptions();
		afx_msg void OnAddFileExclusions();
		afx_msg void OnAddDirectoryExclusions();
		afx_msg void OnAddExtensionExclusions();
		afx_msg void OnAddForcedDetections();
		afx_msg void OnAddHPPExceptions();
		afx_msg void OnAddTamperProtectionExceptions();

		//afx_msg void OnSortProtectionTechnology();


	protected:
		void PopulateListCtrl();
		void CreateGEDataStores();
		//Convert the ENUM ACTION into a string for the UI
        const tstring::tstring& ConvertGEACTIONToString( const GlobalExceptionDataStore::GEACTION& gea );

		//Convert the ENUM PROTECH into a string for the UI
		const tstring::tstring& ConvertGEPROTECHToString( const GlobalExceptionDataStore::GEPROTECHTYPE& gept );

		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		DECLARE_MESSAGE_MAP()

	private:
		static const DWORD dwColumnIndex_ProtectionTechnology	= 0;
		static const DWORD dwColumnIndex_Override			    = 1;
		static const DWORD dwColumnIndex_Action				    = 2;

	    CButton		m_btnAddException;
	    CListCtrl	m_listAdminDefined;

		//GEDataStore Ptr objects
		GlobalExceptionDataStore::IRiskGEDataStorePtr      m_ptrRiskGEDataStore;
		GlobalExceptionDataStore::IFileHashGEDataStorePtr  m_ptrFileHashGEDataStore;
		GlobalExceptionDataStore::IFileNameGEDataStorePtr  m_ptrFileNameGEDataStore;
		GlobalExceptionDataStore::IDirectoryGEDataStorePtr m_ptrDirectoryGEDataStore;

		afx_msg void OnLvnColumnclickAdminGeList(NMHDR *pNMHDR, LRESULT *pResult);
};
