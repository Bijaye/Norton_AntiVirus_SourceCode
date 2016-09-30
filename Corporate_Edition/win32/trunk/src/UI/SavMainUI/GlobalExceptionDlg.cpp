// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// CGlobalExceptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SavAssert.h"
#include "GlobalExceptionDlg.h"
#include "KnownRisksDlg.h"
#include "ClientReg.h"
#include "tstring.h"
#include <string>


// TODO brian_schlatter -- Create a resource DLL for GEDataStore and move these strings to it.
// Create a new interface to access these strings.

// CGlobalExceptionDlg dialog
//GE actions for UI display
const static tstring::tstring tstrBLOCK =      _T("BLOCK");
const static tstring::tstring tstrIGNORE =     _T("IGNORE");
const static tstring::tstring tstrLOGONLY =    _T("LOGONLY");
const static tstring::tstring tstrQUARANTINE = _T("QUARANTINE");
const static tstring::tstring tstrTERMINATE =  _T("TERMINATE");
const static tstring::tstring tstrUNK =        _T("UNKNOWN");

//GE Protection Technology for UI display
const static tstring::tstring tstrHPP   =   _T("HEURISTIC PROCESS PROTECTION");
const static tstring::tstring tstrRISK  =   _T("SECURITY RISK");
const static tstring::tstring tstrTP    =   _T("TAMPER PROTECTION");


IMPLEMENT_DYNAMIC(CGlobalExceptionDlg, CDialog)
CGlobalExceptionDlg::CGlobalExceptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGlobalExceptionDlg::IDD, pParent)
{
}

CGlobalExceptionDlg::~CGlobalExceptionDlg()
{
	
	// todo:: mark_roberton release the refs in the listview control.

	//
	//UnLoadGEDataStore();

}


void CGlobalExceptionDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CGlobalExceptionDlg)
    DDX_Control(pDX, IDC_ADMIN_GE_LIST, m_listAdminDefined);
    DDX_Control(pDX, IDC_GEH_ADD, m_btnAddException);
    //}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CGlobalExceptionDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBtnClickOk)
    ON_BN_CLICKED(IDHELP, OnBtnClickHelp)

    ON_COMMAND(IDC_GEH_ADD, OnBtnClickAdd)
    ON_COMMAND(IDC_GEH_EDIT, OnBtnClickEdit)
    ON_COMMAND(IDC_GEH_DELETE, OnBtnClickDelete)

	ON_COMMAND(ID_GEH_SECRISKS_KNOWN, OnAddKnownRiskExceptions)
	ON_COMMAND(ID_GEH_SECRISKS_FILES, OnAddFileExclusions)
	ON_COMMAND(ID_GEH_SECRISKS_DIRECTORIES, OnAddDirectoryExclusions)
	ON_COMMAND(ID_GEH_SECRISKS_EXTENSIONS, OnAddExtensionExclusions)
	ON_COMMAND(ID_GEH_HUERISTICPROT_FORCEDDET, OnAddForcedDetections)
	ON_COMMAND(ID_GEH_HUERISTICPROT_PROCESSES, OnAddHPPExceptions)
	ON_COMMAND(ID_GEH_TAMPERPROTECTION, OnAddTamperProtectionExceptions)
	//ON_NOTIFY(LVN_COLUMNCLICK, IDC_ADMIN_GE_LIST, &CGlobalExceptionDlg::OnLvnColumnclickAdminGeList)
END_MESSAGE_MAP()



// CGlobalExceptionDlg message handlers

BOOL CGlobalExceptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if ( SYM_FAILED(LoadGEDataStore() ))
	{
		std::cout << "this is crap!" << std::endl;
	}

	//Create datastore objects!
	CreateGEDataStores( );

	// TODO brian_schlatter: What does this do?
//	UpdateData(FALSE);

//    InitializeColumnHeaders();

	// TODO -- move these to a resource.
    CString strColumnTitleProtectionTechnology(_T("ProtectionTechnology"));
    CString strColumnOverride(_T("Override"));
    CString strColumnAction(_T("Action"));
	
    m_listAdminDefined.InsertColumn(dwColumnIndex_ProtectionTechnology, strColumnTitleProtectionTechnology, LVCFMT_LEFT, 135);
    m_listAdminDefined.InsertColumn(dwColumnIndex_Override,				strColumnOverride,					LVCFMT_LEFT, 220);
    m_listAdminDefined.InsertColumn(dwColumnIndex_Action,				strColumnAction,					LVCFMT_LEFT, 91);
	m_listAdminDefined.SetExtendedStyle( LVS_EX_FULLROWSELECT );

	PopulateListCtrl();

    // set focus and selection to the first item in the list
	m_listAdminDefined.SetItemState(0, (LVIS_FOCUSED | LVIS_SELECTED), (LVIS_FOCUSED | LVIS_SELECTED));


//    m_btnAddException.LoadImages(IDR_HELPBAR_HELPSUPPORT_NORMAL, IDR_HELPBAR_HELPSUPPORT_NORMAL, IDR_HELPBAR_HELPSUPPORT_NORMAL, IDR_HELPBAR_HELPSUPPORT_NORMAL, CButtonEx::ScaleMode_StretchToFit);


	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGlobalExceptionDlg::CreateGEDataStores()
{
	// Create the Risk DataStore object.
	g_objRiskGEDataStoreLdr.CreateObject( &m_ptrRiskGEDataStore );
 
	// Create the FileHash DataStore object.
    g_objFileHashGEDataStoreLdr.CreateObject( &m_ptrFileHashGEDataStore );
	
	// Create the FileName DataStore object.
	g_objFileNameGEDataStoreLdr.CreateObject( &m_ptrFileNameGEDataStore );

	// Create the Directory DataStore object.
	g_objDirectoryGEDataStoreLdr.CreateObject( &m_ptrDirectoryGEDataStore );

}


void CGlobalExceptionDlg::OnBtnClickOk()
{	
    // Save and validate the configuration values. If anything fails here, then it will pop up
	// the appropriate message box and return FALSE.
    if (UpdateData(TRUE) == FALSE)
		return;

	// Close the dialog.
	OnOK();
}


void CGlobalExceptionDlg::OnBtnClickHelp()
{
}

void CGlobalExceptionDlg::OnBtnClickAdd()
{
    CMenu menuExceptions;
    CRect rectControlScreen;

    m_btnAddException.GetWindowRect(rectControlScreen);
    menuExceptions.LoadMenu(IDR_GEH_PROTECHTYPES);
    menuExceptions.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTBUTTON | TPM_CENTERALIGN, rectControlScreen.CenterPoint().x, rectControlScreen.CenterPoint().y, this);
}

void CGlobalExceptionDlg::OnBtnClickEdit()
{
	MessageBox(_T("OnBtnClickEdit"), _T("Called"));

	// Figure out which item is currently selected.

	// Now, pop up the appropriate dialog and autofill with its information.
}

void CGlobalExceptionDlg::OnBtnClickDelete()
{
	MessageBox(_T("OnBtnClickDelete"), _T("Called"));
	// Figure out which item is currently selected.

	// Now, delete it.
}

void CGlobalExceptionDlg::OnAddKnownRiskExceptions()
{
	CKnownRisksDlg krDlg;
	krDlg.DoModal();
}


void CGlobalExceptionDlg::OnAddFileExclusions()
{
	MessageBox(_T("OnAddFileExclusions"), _T("Called"));
}
void CGlobalExceptionDlg::OnAddDirectoryExclusions()
{
	MessageBox(_T("OnAddDirectoryExclusions"), _T("Called"));
}
void CGlobalExceptionDlg::OnAddExtensionExclusions()
{
	MessageBox(_T("OnAddExtensionExclusions"), _T("Called"));
}
void CGlobalExceptionDlg::OnAddForcedDetections()
{
	MessageBox(_T("OnAddForcedDetections"), _T("Called"));
}
void CGlobalExceptionDlg::OnAddHPPExceptions()
{
	MessageBox(_T("OnAddHPPExceptions"), _T("Called"));
}
void CGlobalExceptionDlg::OnAddTamperProtectionExceptions()
{
	MessageBox(_T("OnAddTamperProtectionExceptions"), _T("Called"));
}


void CGlobalExceptionDlg::PopulateListCtrl()
{
	int nCurRow = 0;
	std::vector<GlobalExceptionDataStore::IFileNameGEItemPtr>  vecFileNameItems;
	std::vector<GlobalExceptionDataStore::IFileHashGEItemPtr>  vecFileHashItems;
	std::vector<GlobalExceptionDataStore::IDirectoryGEItemPtr> vecDirectoryItems;

	//Risk based GEs
	tstring::tstring tstrGEProTech = ConvertGEPROTECHToString(GlobalExceptionDataStore::GEPT_RISK);
	if ( m_ptrRiskGEDataStore != NULL )
	{
		GlobalExceptionDataStore::IRiskGEItemPtr	ptrRiskGEItem;
		DWORD dwNumItems = 0;
		if ( SYM_FAILED (m_ptrRiskGEDataStore->InitList(GlobalExceptionDataStore::GEPT_RISK)) )
		{
			//debug;
		}
		if (SYM_FAILED ( m_ptrRiskGEDataStore->GetListCount(&dwNumItems) ))
		{
			//debug;
		}

		for (DWORD dwIndex = 0; dwIndex < dwNumItems ; dwIndex++)
		{
			if ( SYM_FAILED( m_ptrRiskGEDataStore->GetItemInList(dwIndex, ptrRiskGEItem.m_p)) )
			{
				//Debug;
				continue;
			}

			tstring::tstring tstrOverride;
			GlobalExceptionDataStore::GEACTION gea = GlobalExceptionDataStore::GEA_UNSET;

			if ( SYM_FAILED( ptrRiskGEItem->GetNameW( &tstrOverride )))
			{
				//debug;
				continue;
			}

			if ( SYM_FAILED( ptrRiskGEItem->GetFirstAction( &gea )))
			{
				//debug;
				continue;
			}

			m_listAdminDefined.InsertItem( nCurRow, tstrGEProTech.c_str() );
			m_listAdminDefined.SetItem(nCurRow, dwColumnIndex_Override,	LVIF_TEXT, tstrOverride.c_str(), NULL, NULL, NULL, NULL);

			tstring::tstring tstrAction = ConvertGEACTIONToString( gea );
			m_listAdminDefined.SetItem(nCurRow, dwColumnIndex_Action,	LVIF_TEXT, tstrAction.c_str(), NULL, NULL, NULL, NULL);
			// do another add ref since the listview control is referring to it.
			//ptrRiskGEItem->AddRef();
			m_listAdminDefined.SetItemData(nCurRow, (DWORD_PTR)ptrRiskGEItem.m_p);
		}
	}
	else
	{
	}


	if ( m_ptrFileHashGEDataStore != NULL )
	{
		if ( SYM_FAILED( m_ptrFileHashGEDataStore->Initialize( GlobalExceptionDataStore::GEPT_RISK ) ))
		{
			//debug;
			
		}

		if ( SYM_FAILED( m_ptrFileHashGEDataStore->GetExceptions( vecFileHashItems ) ))
		{
			//debug
		}

		GlobalExceptionDataStore::IFileHashGEItemPtr ptrFileHashGEItem;
		for ( DWORD dwIndex = 0; dwIndex < vecFileHashItems.size(); dwIndex++ )
		{
			ptrFileHashGEItem = vecFileHashItems[dwIndex];
			if ( ptrFileHashGEItem != NULL )
			{
				
				tstring::tstring tstrOverride;
				GlobalExceptionDataStore::GEACTION gea;

				if ( SYM_FAILED( ptrFileHashGEItem->GetNameW( &tstrOverride )))
				{
					//debug;
					continue;
				}

				if ( SYM_FAILED( ptrFileHashGEItem->GetFirstAction( &gea )))
				{
					//debug;
					continue;
				}

				m_listAdminDefined.InsertItem( nCurRow, tstrGEProTech.c_str() );
				m_listAdminDefined.SetItem(nCurRow, dwColumnIndex_Override,	LVIF_TEXT, tstrOverride.c_str(), NULL, NULL, NULL, NULL);

				tstring::tstring tstrAction = ConvertGEACTIONToString( gea );
				m_listAdminDefined.SetItem(nCurRow, dwColumnIndex_Action,	LVIF_TEXT, tstrAction.c_str(), NULL, NULL, NULL, NULL);
				// do another add ref since the listview control is referring to it.
				//ptrFileHashGEItem->AddRef();
				m_listAdminDefined.SetItemData(nCurRow, (DWORD_PTR)ptrFileHashGEItem.m_p);
			}
		}
		vecFileHashItems.clear();
	}
	else
	{
	}


	if ( m_ptrFileNameGEDataStore != NULL )
	{		
		if ( SYM_FAILED( m_ptrFileNameGEDataStore->Initialize( GlobalExceptionDataStore::GEPT_RISK ) ))
		{
			
			//debug;
		}

		if ( SYM_FAILED( m_ptrFileNameGEDataStore->GetExceptions( vecFileNameItems ) ))
		{
			//debug;
		}

		GlobalExceptionDataStore::IFileNameGEItemPtr ptrFileNameGEItem;
		for ( DWORD dwIndex = 0; dwIndex < vecFileNameItems.size(); dwIndex++ )
		{
			ptrFileNameGEItem = vecFileNameItems[dwIndex];
			if ( ptrFileNameGEItem != NULL )
			{
				
				tstring::tstring tstrOverride;
				GlobalExceptionDataStore::GEACTION gea;

				if ( SYM_FAILED( ptrFileNameGEItem->GetNameW( &tstrOverride )))
				{
					//debug;
					continue;
				}

				if ( SYM_FAILED( ptrFileNameGEItem->GetFirstAction( &gea )))
				{
					//debug;
					continue;
				}

				m_listAdminDefined.InsertItem( nCurRow, tstrGEProTech.c_str() );
				m_listAdminDefined.SetItem(nCurRow, dwColumnIndex_Override,	LVIF_TEXT, tstrOverride.c_str(), NULL, NULL, NULL, NULL);

				tstring::tstring tstrAction = ConvertGEACTIONToString( gea );
				m_listAdminDefined.SetItem(nCurRow, dwColumnIndex_Action,	LVIF_TEXT, tstrAction.c_str(), NULL, NULL, NULL, NULL);
				// do another add ref since the listview control is referring to it.
				//ptrFileNameGEItem->AddRef();
				m_listAdminDefined.SetItemData(nCurRow, (DWORD_PTR)ptrFileNameGEItem.m_p);
			}
		}
		vecFileNameItems.clear();
	}
	else
	{
		//debug;
	}

	if ( m_ptrDirectoryGEDataStore != NULL )
	{		
		if ( SYM_FAILED( m_ptrDirectoryGEDataStore->Initialize( GlobalExceptionDataStore::GEPT_RISK ) ))
		{
			//debug;
		}

		if ( SYM_FAILED( m_ptrDirectoryGEDataStore->GetExceptions( vecDirectoryItems ) ))
		{
			//debug;
		}

		GlobalExceptionDataStore::IDirectoryGEItemPtr ptrDirectoryGEItem;
		for ( DWORD dwIndex = 0; dwIndex < vecDirectoryItems.size(); dwIndex++ )
		{
			ptrDirectoryGEItem = vecDirectoryItems[dwIndex];
			if ( ptrDirectoryGEItem != NULL )
			{
				
				tstring::tstring tstrOverride;
				GlobalExceptionDataStore::GEACTION gea;

				if ( SYM_FAILED( ptrDirectoryGEItem->GetNameW( &tstrOverride )))
				{
					//debug;
					continue;
				}

				if ( SYM_FAILED( ptrDirectoryGEItem->GetFirstAction( &gea )))
				{
					//debug;
					continue;
				}

				m_listAdminDefined.InsertItem( nCurRow, tstrGEProTech.c_str() );
				m_listAdminDefined.SetItem(nCurRow, dwColumnIndex_Override,	LVIF_TEXT, tstrOverride.c_str(), NULL, NULL, NULL, NULL);

				tstring::tstring tstrAction = ConvertGEACTIONToString( gea );
				m_listAdminDefined.SetItem(nCurRow, dwColumnIndex_Action,	LVIF_TEXT, tstrAction.c_str(), NULL, NULL, NULL, NULL);
				// do another add ref since the listview control is referring to it.
				//ptrDirectoryGEItem->AddRef();
				m_listAdminDefined.SetItemData(nCurRow, (DWORD_PTR)ptrDirectoryGEItem.m_p);
			}
		}
		vecDirectoryItems.clear();
	}
	else
	{
		//debug;
	}

	//HPP GEs
	tstrGEProTech = ConvertGEPROTECHToString(GlobalExceptionDataStore::GEPT_HPP);
	if ( m_ptrFileHashGEDataStore != NULL )
	{
		if ( SYM_FAILED( m_ptrFileHashGEDataStore->Initialize( GlobalExceptionDataStore::GEPT_HPP ) ))
		{
			//debug;
			return;
		}

		if ( SYM_FAILED( m_ptrFileHashGEDataStore->GetExceptions( vecFileHashItems ) ))
		{
			//debug;
			return;
		}

		GlobalExceptionDataStore::IFileHashGEItemPtr ptrFileHashGEItem;
		for ( DWORD dwIndex = 0; dwIndex < vecFileHashItems.size(); dwIndex++ )
		{
			ptrFileHashGEItem = vecFileHashItems[dwIndex];
			if ( ptrFileHashGEItem != NULL )
			{
				
				tstring::tstring tstrOverride;
				GlobalExceptionDataStore::GEACTION gea;

				if ( SYM_FAILED( ptrFileHashGEItem->GetNameW( &tstrOverride )))
				{
					//debug;
					continue;
				}

				if ( SYM_FAILED( ptrFileHashGEItem->GetFirstAction( &gea )))
				{
					//debug;
					continue;
				}

				m_listAdminDefined.InsertItem( nCurRow, tstrGEProTech.c_str() );
				m_listAdminDefined.SetItem(nCurRow, dwColumnIndex_Override,	LVIF_TEXT, tstrOverride.c_str(), NULL, NULL, NULL, NULL);

				tstring::tstring tstrAction = ConvertGEACTIONToString( gea );
				m_listAdminDefined.SetItem(nCurRow, dwColumnIndex_Action,	LVIF_TEXT, tstrAction.c_str(), NULL, NULL, NULL, NULL);
				// do another add ref since the listview control is referring to it.
				//ptrFileHashGEItem->AddRef();
				m_listAdminDefined.SetItemData(nCurRow, (DWORD_PTR)ptrFileHashGEItem.m_p);
			}
		}
		vecFileHashItems.clear();
	}
	else
	{
		//debug;
	}


	if ( m_ptrFileNameGEDataStore != NULL )
	{		
		if ( SYM_FAILED( m_ptrFileNameGEDataStore->Initialize( GlobalExceptionDataStore::GEPT_HPP ) ))
		{
			//debug;;
		}

		if ( SYM_FAILED( m_ptrFileNameGEDataStore->GetExceptions( vecFileNameItems ) ))
		{
			//debug;;
		}

		GlobalExceptionDataStore::IFileNameGEItemPtr ptrFileNameGEItem;
		for ( DWORD dwIndex = 0; dwIndex < vecFileNameItems.size(); dwIndex++ )
		{
			ptrFileNameGEItem = vecFileNameItems[dwIndex];
			if ( ptrFileNameGEItem != NULL )
			{
				
				tstring::tstring tstrOverride;
				GlobalExceptionDataStore::GEACTION gea;

				if ( SYM_FAILED( ptrFileNameGEItem->GetNameW( &tstrOverride )))
				{
					//debug;
					continue;
				}

				if ( SYM_FAILED( ptrFileNameGEItem->GetFirstAction( &gea )))
				{
					//debug;
					continue;
				}

				m_listAdminDefined.InsertItem( nCurRow, tstrGEProTech.c_str() );
				m_listAdminDefined.SetItem(nCurRow, dwColumnIndex_Override,	LVIF_TEXT, tstrOverride.c_str(), NULL, NULL, NULL, NULL);

				tstring::tstring tstrAction = ConvertGEACTIONToString( gea );
				m_listAdminDefined.SetItem(nCurRow, dwColumnIndex_Action,	LVIF_TEXT, tstrAction.c_str(), NULL, NULL, NULL, NULL);
				// do another add ref since the listview control is referring to it.
				//ptrFileNameGEItem->AddRef();
				m_listAdminDefined.SetItemData(nCurRow, (DWORD_PTR)ptrFileNameGEItem.m_p);
			}
		}
		vecFileNameItems.clear();
	}
	else
	{
		//debug;
	}

	//TP GEs

	
}

//------------------------------------------------------------------------------------------------
//convert the ENUM Action into a string for the UI
// @param gea                 The Action for the GE
// 
// return       tstring       The corresponding output string for the input GEACTION
//------------------------------------------------------------------------------------------------
const tstring::tstring& CGlobalExceptionDlg::ConvertGEACTIONToString( const GlobalExceptionDataStore::GEACTION& geo )
{

	switch (geo)
	{
	case GlobalExceptionDataStore::GEA_BLOCK:
		return tstrBLOCK;
	case GlobalExceptionDataStore::GEA_IGNORE:
		return tstrIGNORE;
	case GlobalExceptionDataStore::GEA_LOGONLY:
		return tstrLOGONLY;
	case GlobalExceptionDataStore::GEA_QUARANTINE:
		return tstrQUARANTINE;
	case GlobalExceptionDataStore::GEA_TERMINATE_PROCESS_ONLY:
		return tstrTERMINATE;
	default:
		return tstrUNK;		
	};

}

//------------------------------------------------------------------------------------------------
//convert the ENUM protection technology into a string for the UI
// @param gept                The Protech for the GE
// 
// return       tstring       The corresponding output string for the input GEACTION
//------------------------------------------------------------------------------------------------
const tstring::tstring& CGlobalExceptionDlg::ConvertGEPROTECHToString( const GlobalExceptionDataStore::GEPROTECHTYPE& gept )
{

	switch (gept)
	{
	case GlobalExceptionDataStore::GEPT_HPP:
		return tstrHPP;
	case GlobalExceptionDataStore::GEPT_RISK:
		return tstrRISK;
	case GlobalExceptionDataStore::GEPT_TP:
		return tstrTP;
	default:
		return tstrUNK;		
	};

}



/*void CGlobalExceptionDlg::OnLvnColumnclickAdminGeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
*/