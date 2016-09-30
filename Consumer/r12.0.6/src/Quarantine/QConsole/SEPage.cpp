//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// SEPage.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "SEPage.h"
#include "qsfields.h"
#include "qconres.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSEPage property page

IMPLEMENT_DYNCREATE(CSEPage, CItemPage)

CSEPage::CSEPage() : CItemPage(CSEPage::IDD)
{
	//{{AFX_DATA_INIT(CSEPage)
	m_sFiles.LoadString(IDS_NO_FILE_SIDE_EFFECTS);
	m_sRegKeys.LoadString(IDS_NO_REG_SIDE_EFFECTS);
	m_sVirusName = _T("");
	//}}AFX_DATA_INIT

    m_ulVID = 0;
}

CSEPage::~CSEPage()
{
}

void CSEPage::DoDataExchange(CDataExchange* pDX)
{
	CItemPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSEPage)
	DDX_Text(pDX, IDC_REGKEYS_EDIT, m_sRegKeys);
	DDX_Text(pDX, IDC_FILES_EDIT, m_sFiles);
	DDX_Text(pDX, IDC_VIRUS_NAME_STATIC, m_sVirusName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSEPage, CItemPage)
	//{{AFX_MSG_MAP(CSEPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSEPage message handlers


////////////////////////////////////////////////////////////////////////////////
// Description	    : Handle WM_INITDIALOG message.
// 2/19/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
BOOL CSEPage::OnInitDialog() 
{
	CItemPage::OnInitDialog();

    // Set up virus info
    if( FAILED(m_pDoc->m_pQuarantine->GetVirusInformation(m_ulVID, &m_pScanVirusInfo) ) )
    {
        CCTRACEE(_T("%s - Failed to get the virus information for vid %d"), __FUNCTION__, m_ulVID);
        return TRUE;
    }

    // Set up text controls.
    char *pBuffer = new char[MAX_PATH];
    if( !pBuffer )
    {
        CCTRACEE(_T("%s - Failed to allocate the buffer"), __FUNCTION__);
        return TRUE;
    }
    DWORD dwSize = MAX_PATH;

    m_pScanVirusInfo->GetName(pBuffer, &dwSize);
    m_sVirusName = pBuffer;

    // Fill in the side effect info
    dwSize = MAX_PATH;
    DWORD dwCount = 0;
    if( SUCCEEDED(m_pItem->GetDWORD(QSERVER_ITEM_SE_COUNT, &dwCount)) && dwCount > 0 )
    {
        CString sRegKeys, sFiles, sCmp1, sCmp2;
        TCHAR szQueryName[MAX_PATH] = {0};
        DWORD dwCurBufSize = dwSize;

        // Add the items
        for( DWORD curNum=0; curNum<dwCount; curNum++ )
        {
            _stprintf(szQueryName, QSERVER_ITEM_SE_DESC, curNum);

            if( SUCCEEDED(m_pItem->GetString(szQueryName, pBuffer, &dwSize)) )
            {
                // Get the type
                DWORD dwType = -1;
                _stprintf(szQueryName, QSERVER_ITEM_SE_TYPE, curNum);
                if( SUCCEEDED(m_pItem->GetDWORD(szQueryName, &dwType)) )
                {
                    // Registry key type?
                    if( SE_TYPE_REGKEY == dwType )
                    {
                        // Only add it if it's not already present
                        sCmp1 = sRegKeys; sCmp1.MakeUpper();
                        sCmp2 = pBuffer; sCmp2.MakeUpper();

                        if( -1 == sCmp1.Find(sCmp2) )
                        {
                            sRegKeys += pBuffer;
                            sRegKeys += _T("\r\n");
                        }
                        else
                            CCTRACEI(_T("%s - Found %s already in the registry list. Not adding it again."), __FUNCTION__, pBuffer);
                    }
                    // File type?
                    else if( SE_TYPE_STARTUP == dwType ||
                            SE_TYPE_INI == dwType ||
                            SE_TYPE_BATCH == dwType )
                    {
                        // Only add it if it's not already present
                        sCmp1 = sFiles; sCmp1.MakeUpper();
                        sCmp2 = pBuffer; sCmp2.MakeUpper();

                        if( -1 == sCmp1.Find(sCmp2) )
                        {
                            sFiles += pBuffer;
                            sFiles += _T("\r\n");
                        }
                        else
                            CCTRACEI(_T("%s - Found %s already in the file list. Not adding it again."), __FUNCTION__, pBuffer);
                    }
                    // No/Unknown type?
                    else
                    {
                        CCTRACEE(_T("%s - Unknown or no side effect type for side effect stored data: %s"), __FUNCTION__, pBuffer);
                    }
                }
                else
                {
                    CCTRACEE(_T("%s - Failed to get the type for side effect %d"), __FUNCTION__, curNum);
                }
            }
            // See if the buffer size need to increase
            else if( pBuffer && dwCurBufSize < dwSize )
            {
                delete [] pBuffer;
                pBuffer = new char [dwSize];
                if( pBuffer )
                {
                    // Set the new buffer size
                    dwCurBufSize = dwSize;

                    // Re-try this query
                    --curNum;
                }
            }
        }

        // Set the data to the actual controls
        if( !sRegKeys.IsEmpty() )
            m_sRegKeys = sRegKeys;
        if( !sFiles.IsEmpty() )
            m_sFiles = sFiles;
    }

    // Blast data to controls
    UpdateData( FALSE );

    // Free the buffer
    if( pBuffer )
    {
        delete [] pBuffer;
        pBuffer = NULL;
    }

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
