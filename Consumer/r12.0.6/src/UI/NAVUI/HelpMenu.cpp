// HelpMenu.cpp: implementation of the CHelpMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HelpMenu.h"

#include "HMIProductReg.h"
#include "HMISupport.h"
#include "HMIResponse.h"
#include "HMIProducts.h"
#include "HMIActivation.h"
#include "HMIManageService.h"
#include "HMIClubSymantec.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHelpMenu::CHelpMenu()
{
	try
	{
		m_wCurrentIndex = 0;
		WORD wIDBase = 0;

		// Put the initialization code here.
		//

		// NAV Help - taken care of by the Frame or Plugin View
		//
		CHelpMenuItem* pItem = NULL;
		if ( g_bStandAlone )
		{
			// Product Activation 
			//
			if(CHMIActivationFactory::ShouldShow())
			{
				if(CHMIActivationFactory::CreateObject(&pItem))
				{
					pItem->SetItemID ( wIDBase++ );
					m_vecHelpMenuItems.push_back ( pItem );

				}
			}

			// Product Reg           
			//
			if(CHMIProductRegFactory::ShouldShow())
			{
				if(CHMIProductRegFactory::CreateObject(&pItem))
				{
					pItem->SetItemID ( wIDBase++ );
					m_vecHelpMenuItems.push_back ( pItem );                
				}
			}
			// Tech Support 
			//
			if(CHMISupportFactory::ShouldShow())
			{
				if(CHMISupportFactory::CreateObject(&pItem))
				{
					pItem->SetItemID ( wIDBase++ );
					m_vecHelpMenuItems.push_back ( pItem );                
				}
			}

			// Symantec Response Center
			//
			if(CHMIResponseFactory::ShouldShow())
			{
				if(CHMIResponseFactory::CreateObject(&pItem))
				{
					pItem->SetItemID ( wIDBase++ );
					m_vecHelpMenuItems.push_back ( pItem ); 
				}
			}
			// More Symantec Solutions
			//
			if(CHMIProductsFactory::ShouldShow())
			{
				if(CHMIProductsFactory::CreateObject(&pItem))
				{
					pItem->SetItemID ( wIDBase++ );
					m_vecHelpMenuItems.push_back ( pItem );
				}
			}
            // Club Symantec
            //
            if(CHMIClubSymantecFactory::ShouldShow())
            {
                if(CHMIClubSymantecFactory::CreateObject(&pItem))
                {
                    pItem->SetItemID ( wIDBase++ );
                    m_vecHelpMenuItems.push_back ( pItem );
                }
            }
		}

		if(CHMIManageServiceFactory::ShouldShow())
		{
			if(CHMIManageServiceFactory::CreateObject(&pItem))
			{
				pItem->SetItemID ( wIDBase++ );
				m_vecHelpMenuItems.push_back ( pItem );

			}
		}
	}
	catch (...)
	{
	}
}

CHelpMenu::~CHelpMenu()
{
	iterHelpMenuItems pos;
	for (pos = m_vecHelpMenuItems.begin();
		pos != m_vecHelpMenuItems.end();
		pos ++)
	{
		CHelpMenuItem* menuItem = *pos;
		if ( menuItem )
		{
			delete menuItem;
			menuItem = NULL;
		}
	}
}

HRESULT CHelpMenu::ResetMenuItemEnum()
{
	m_wCurrentIndex = 0;
	return S_OK;
}

HRESULT CHelpMenu::GetNextMenuItem( WORD *pdwItemID,
								   SYMSW_MENUITEMTYPE *pdwItemType,
								   UINT *pnItemText)
{
	if ( m_wCurrentIndex >= m_vecHelpMenuItems.size() )
		return E_FAIL;

	*pdwItemID = m_vecHelpMenuItems[m_wCurrentIndex]->GetItemID ();
	*pdwItemType = m_vecHelpMenuItems[m_wCurrentIndex]->GetMenuType ();
	*pnItemText = m_vecHelpMenuItems[m_wCurrentIndex]->GetTextID ();
	m_wCurrentIndex++;
	return S_OK;
}

HRESULT CHelpMenu::DoMenuItem ( WORD dwItemID,
							   HWND hMainWnd)
{
	CHelpMenuItem* menuItem;

	iterHelpMenuItems pos;
	for (pos = m_vecHelpMenuItems.begin();
		pos != m_vecHelpMenuItems.end();
		pos ++)
	{
		if ( *pos )
		{
			menuItem = *pos;
			if ( dwItemID == menuItem->GetItemID ())
			{
				return menuItem->DoWork(hMainWnd);
			}
		}
	}

	return E_FAIL;
}

