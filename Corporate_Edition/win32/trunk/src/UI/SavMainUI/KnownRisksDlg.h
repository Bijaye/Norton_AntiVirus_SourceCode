// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2004, 2005, Symantec Corporation, All Rights Reserved.

#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"
#include <vector>			// std::vector
#include <map>

//#include "ListCtrlEx.h"
#include "RiskConfiguration.h"

//#include "ExpandedThreat.h"	// CExpandedThreat


// ** ENUMS **/
enum ERiskConfigColumn
	// All of the values for a particular column start with "evCol".
	// Everything else conveys information about the enum values.
	// For example, evFirstActionColumn <= evColAnyAction < evAfterLastActionColumn
{
	evInvalidColumn = -1,
	// The first column must be 0.
		evColThreatName,
	evFirstActionColumn,
		evColFirstAction = evFirstActionColumn,
		evColSecondAction,
	evAfterLastActionColumn,
	evFirstRatingColumn = evAfterLastActionColumn,
		evColOverallRating = evFirstRatingColumn,
		evColPrivacyRating,
		evColPerformanceRating,
		evColStealthRating,
		evColRemovalRating,
	evAfterLastRatingColumn,
		evColDependent = evAfterLastRatingColumn,
	evAfterLastColumn
};


// CKnownRisksDlg dialog
class CKnownRisksDlg : public CDialog
{
	DECLARE_DYNAMIC(CKnownRisksDlg)

public:
	CKnownRisksDlg(CWnd* pParent = NULL);
// Dialog Data
	enum { IDD = IDD_KNOWN_RISK_SELECTION };

	/** Returns the risks the user selected.
	  * Don't call this while this page is active.
	  * @return the selected risks
	  */
	const std::vector<CExpandedThreat> & GetSelectedThreats() const { return m_voSelectedRisks; }

    void AddExpandedThreat(const CExpandedThreat &oRisk);

private:
    typedef std::map<DWORD, CExpandedThreat> RiskMap;

	// The available threats for the selected threat category.
	RiskMap m_moRisks;
	// Risks that were selected.  This is populated only when the dialog is closed.
	std::vector<CExpandedThreat> m_voSelectedRisks;
	// The text to find when the user hits Find Next
//	CString m_strTextToFind;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	HRESULT UpdateWizardButtons();

	UINT FillListThread( LPVOID pVoid );

private:
	static int ERiskConfigColumnToSubitemIndex(ERiskConfigColumn eCol);
    static int CALLBACK ExceptionListCompareFunc(LPARAM lItem1, LPARAM lItem2, LPARAM lThis);
	static CExpandedThreat::ERatingType ERiskConfigColumnToERatingType(ERiskConfigColumn eColumn);
	static ERiskConfigColumn ERatingTypeToERiskConfigColumn(CExpandedThreat::ERatingType eRating);
	static double GetOverallRating(const CExpandedThreat &oRisk);
	static void	 SetupRiskListControl(CListCtrl &cRiskList,
								   int fERiskConfigColumnToSubitemIndex(ERiskConfigColumn) = UseAllColumns,
								   DWORD dwExtendedStyle = LVS_EX_FULLROWSELECT);
    static void UpdateRiskInListview(CListCtrl &cRiskList,
									   int iRiskItem,
									   const CExpandedThreat &oRisk,
									   int fERiskConfigColumnToSubitemIndex(ERiskConfigColumn) = UseAllColumns);

	static int UseAllColumns(ERiskConfigColumn eCol) { return eCol; }

private:
	// The list of risks for the selected threat categories.
//	CListCtrlEx m_cRisksList;
	CListCtrl m_cRisksList;
    // A flag used to prevent re-entry of OnItemchangedRisksList.
    bool        m_bInRisksListNotify;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnClickOk();
    afx_msg void OnBtnClickCancel();
	afx_msg void OnItemchangedRisksList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnColumnClickedRisksList( NMHDR* pNMHDR, LRESULT* pResult );
	void OnCustomDrawRisksList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnHelp();
  
};
