// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////

#include "stdafx.h"
#include "ICFRestorePage.h"
#include "ccCoInitialize.h"

#define __CFW_ICFMGR__
#include "FWLoaders.h"

LRESULT CICFRestorePage::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		
	#ifdef _DEBUG
		MessageBox(_T("CICFRestorePage"),_T("attach"),NULL);
	#endif
		UINT aID[] = {IDC_TITLE};
		ReplacePropertiesFromIDs(aID);
		((CSetupInfo*)(m_pSetupInfo))->setRestoreICFSettings(TRUE);
		CheckRadioButton(IDC_ICFRESTORE_NO, IDC_ICFRESTORE_YES, IDC_ICFRESTORE_YES);

		return CBasePropPage<IDD_ICFRESTORE>::OnInitDialog(uMsg, wParam, lParam, bHandled);

	}

LRESULT CICFRestorePage::OnYes(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		((CSetupInfo*)(m_pSetupInfo))->setRestoreICFSettings(TRUE);
		return 0;
	}

LRESULT CICFRestorePage::OnNo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		((CSetupInfo*)(m_pSetupInfo))->setRestoreICFSettings(FALSE);
		return 0;
	}

int CICFRestorePage::OnWizardNext()
	{
		return 0;
	}

BOOL CICFRestorePage::OnSetActive()
	{
		BOOL retVal = TRUE;
		//Show the Page only if the Current ICF Settings are different from 
		//what was stored during Configwiz

		// caller thread need to init COM
		ICFManagerLoader ICFMgrLoader;
		IICFManagerPtr pICFManager;
		BOOL bChanged = FALSE;

		if( SYM_SUCCEEDED(ICFMgrLoader.CreateObject(&pICFManager)) &&
			SYM_SUCCEEDED(pICFManager->Init())&&
		    SYM_SUCCEEDED(pICFManager->HasStatusChanged(bChanged))
			)
			{
				if(bChanged)
				{
				      retVal = TRUE;
				}
				else
				{
					
					retVal = FALSE;
				}
			}
		else
		{
			CCTRACEE(_T("Couldnot create ICF Manager object"));
			retVal = FALSE;
		}
		return retVal;
	}




		