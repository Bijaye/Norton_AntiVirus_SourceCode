// ResourceSummary.cpp : implementation file
//

#include "stdafx.h"
#include "AVISDFRL.h"
#include "ResourceSummary.h"
#include "DFMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceSummary dialog

// Class to display the resources summary dialog.

CResourceSummary::CResourceSummary(CWnd* pParent /*=NULL*/)
	: CDialog(CResourceSummary::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResourceSummary)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    m_JobIDArray.SetSize (0, 10);
    m_TotalCountArray.SetSize (0, 10);
    m_AvailableCountArray.SetSize (0, 10);
    m_FreeCountArray.SetSize (0, 10);
    m_TotalJobDoneArray.SetSize(0, 10);
}


void CResourceSummary::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResourceSummary)
	DDX_Control(pDX, IDC_LIST_RESOURCES_SUMMARY, m_ResourceSummaryList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResourceSummary, CDialog)
	//{{AFX_MSG_MAP(CResourceSummary)
	//}}AFX_MSG_MAP
    ON_MESSAGE(DFLAUNCHER_REFRESH_SUMMARY, OnDFLauncherRefreshSummary)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceSummary message handlers

BOOL CResourceSummary::OnInitDialog() 
{
	CDialog::OnInitDialog();
    LV_COLUMN lvc;
	
	CString JobString((LPCSTR) IDS_DF_JOB_STRING);
    CString TotalCountString ("Total");
    CString AvailableCountString ("Enabled");
    CString DownCountString("Disabled");
    CString FreeCountString ("Free");
    CString TotalJobCountString ("Job Count");

    lvc.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    lvc.iSubItem=0;
    lvc.pszText=(LPTSTR) ((LPCTSTR)JobString);
    lvc.cx=4 * m_ResourceSummaryList.GetStringWidth((LPCTSTR)JobString) + 20;
    lvc.fmt=LVCFMT_LEFT;
    m_ResourceSummaryList.InsertColumn(0,&lvc);

    lvc.iSubItem=1;
    lvc.pszText=(LPTSTR) ((LPCTSTR)TotalCountString);
    lvc.cx=m_ResourceSummaryList.GetStringWidth((LPCTSTR)TotalCountString) + 20;
    lvc.fmt=LVCFMT_LEFT;
    m_ResourceSummaryList.InsertColumn(1,&lvc);

    lvc.iSubItem=2;
    lvc.pszText=(LPTSTR) ((LPCTSTR)AvailableCountString);
    lvc.cx=m_ResourceSummaryList.GetStringWidth((LPCTSTR)AvailableCountString) + 20;
    lvc.fmt=LVCFMT_LEFT;
    m_ResourceSummaryList.InsertColumn(2,&lvc);

    lvc.iSubItem=3;
    lvc.pszText=(LPTSTR) ((LPCTSTR)DownCountString);
    lvc.cx=m_ResourceSummaryList.GetStringWidth((LPCTSTR)AvailableCountString) + 20;
    lvc.fmt=LVCFMT_LEFT;
    m_ResourceSummaryList.InsertColumn(3,&lvc);

    lvc.iSubItem=4;
    lvc.pszText=(LPTSTR) ((LPCTSTR)FreeCountString);
    lvc.cx=m_ResourceSummaryList.GetStringWidth((LPCTSTR)FreeCountString) + 20;
    lvc.fmt=LVCFMT_LEFT;
    m_ResourceSummaryList.InsertColumn(4,&lvc);

    lvc.iSubItem=5;
    lvc.pszText=(LPTSTR) ((LPCTSTR)TotalJobCountString);
    lvc.cx=m_ResourceSummaryList.GetStringWidth((LPCTSTR)TotalJobCountString) + 20;
    lvc.fmt=LVCFMT_LEFT;
    m_ResourceSummaryList.InsertColumn(5,&lvc);

    m_ResourceSummaryList.AddHeaderToolTip(0, "Data Flow Job Identification");
    m_ResourceSummaryList.AddHeaderToolTip(1, "Total number of defined resources");
    m_ResourceSummaryList.AddHeaderToolTip(2, "Number of resources enabled for processing");
    m_ResourceSummaryList.AddHeaderToolTip(3, "Number of disabled resources");
    m_ResourceSummaryList.AddHeaderToolTip(4, "Number of resources free to accept jobs");
    m_ResourceSummaryList.AddHeaderToolTip(5, "Number of Jobs executed by the resource");

    OnDFLauncherRefreshSummary(0, 0);
    m_ResourceSummaryList.AutoSizeColumns( 0 );
    m_ResourceSummaryList.AutoSizeColumns( 1 );
    m_ResourceSummaryList.AutoSizeColumns( 2 );
    m_ResourceSummaryList.AutoSizeColumns( 3 );
    m_ResourceSummaryList.AutoSizeColumns( 4 );
    m_ResourceSummaryList.AutoSizeColumns( 5 );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// The refresh interval for summary display is controlled by DFLauncherView.
// At each interval, the count arrays are updated and the message is sent
// to this window to refresh the display
LONG CResourceSummary::OnDFLauncherRefreshSummary(WPARAM /*w*/, LPARAM /*l*/)
{
    int itemCount = m_ResourceSummaryList.GetItemCount();
    CString origStr;

    m_ResourceSummaryList.SetRedraw(FALSE);
    int jobIDCount = m_JobIDArray.GetSize();
    for (int i = 0; i < jobIDCount; i++)
    {
        int nItem;
        if (!itemCount)
        {
            LV_ITEM lvi;

            lvi.iItem=i;
            lvi.mask=LVIF_TEXT | LVIF_STATE;
            lvi.iSubItem=0;
            lvi.pszText=m_JobIDArray[i].GetBuffer(1024);
            lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
            lvi.state = 0;

            nItem = m_ResourceSummaryList.InsertItem(&lvi);
            m_ResourceSummaryList.SetItemData(nItem, (DWORD) i);
        }
        else
        {
            for (int j = 0; j < itemCount; j++)
            {
                if (i == (int) m_ResourceSummaryList.GetItemData(j))
                    break;
            }
            nItem = j;
        }

        CString tmpStr;

        tmpStr.Format ("%ld", (long)m_TotalCountArray[i]);
        origStr = m_ResourceSummaryList.GetItemText(nItem, 1);
        if (origStr.Compare(tmpStr))
            m_ResourceSummaryList.SetItemText (nItem, 1, tmpStr.GetBuffer(1024));

        tmpStr.Format ("%ld", (long)m_AvailableCountArray[i]);
        origStr = m_ResourceSummaryList.GetItemText(nItem, 2);
        if (origStr.Compare(tmpStr))
            m_ResourceSummaryList.SetItemText (nItem, 2, tmpStr.GetBuffer(1024));

        tmpStr.Format ("%ld", (long)(m_TotalCountArray[i] - m_AvailableCountArray[i]));
        origStr = m_ResourceSummaryList.GetItemText(nItem, 3);
        if (origStr.Compare(tmpStr))
            m_ResourceSummaryList.SetItemText (nItem, 3, tmpStr.GetBuffer(1024));

        tmpStr.Format ("%ld", (long)m_FreeCountArray[i]);
        origStr = m_ResourceSummaryList.GetItemText(nItem, 4);
        if (origStr.Compare(tmpStr))
            m_ResourceSummaryList.SetItemText (nItem, 4, tmpStr.GetBuffer(1024));

        tmpStr.Format ("%ld", (long)m_TotalJobDoneArray[i]);
        origStr = m_ResourceSummaryList.GetItemText(nItem, 5);
        if (origStr.Compare(tmpStr))
            m_ResourceSummaryList.SetItemText (nItem, 5, tmpStr.GetBuffer(1024));
    }
    {
        int nItem;
        if (!itemCount)
        {
            LV_ITEM lvi;

            lvi.iItem=i+1;
            lvi.mask=LVIF_TEXT | LVIF_STATE;
            lvi.iSubItem=0;
            lvi.pszText=" ";
            lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
            lvi.state = 0;

            nItem = m_ResourceSummaryList.InsertItem(&lvi);
            m_ResourceSummaryList.SetItemData(nItem, (DWORD) i+1);

            m_ResourceSummaryList.SetItemText (nItem, 1, "=====");

            m_ResourceSummaryList.SetItemText (nItem, 2, "=====");

            m_ResourceSummaryList.SetItemText (nItem, 3, "=====");

            m_ResourceSummaryList.SetItemText (nItem, 4, "=====");

            m_ResourceSummaryList.SetItemText (nItem, 5, "========");
        }
    }
    {
        int nItem;
        if (!itemCount)
        {
            LV_ITEM lvi;

            lvi.iItem=i+2;
            lvi.mask=LVIF_TEXT | LVIF_STATE;
            lvi.iSubItem=0;
            lvi.pszText=" ";
            lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
            lvi.state = 0;

            nItem = m_ResourceSummaryList.InsertItem(&lvi);
            m_ResourceSummaryList.SetItemData(nItem, (DWORD) i+2);
        }
        else
        {
            for (int j = 0; j < itemCount; j++)
            {
                if (i+2 == (int) m_ResourceSummaryList.GetItemData(j))
                    break;
            }
            nItem = j;
        }

        CString tmpStr;
        int j;
        long totalCount;

        totalCount = 0;
        for (j = 0; j < jobIDCount; j++)
        {
            totalCount += (long)m_TotalCountArray[j];
        }
        tmpStr.Format ("%ld", totalCount);
        origStr = m_ResourceSummaryList.GetItemText(nItem, 1);
        if (origStr.Compare(tmpStr))
            m_ResourceSummaryList.SetItemText (nItem, 1, tmpStr.GetBuffer(1024));

        totalCount = 0;
        for (j = 0; j < jobIDCount; j++)
        {
            totalCount += (long)m_AvailableCountArray[j];
        }
        tmpStr.Format ("%ld", totalCount);
        origStr = m_ResourceSummaryList.GetItemText(nItem, 2);
        if (origStr.Compare(tmpStr))
            m_ResourceSummaryList.SetItemText (nItem, 2, tmpStr.GetBuffer(1024));

        totalCount = 0;
        for (j = 0; j < jobIDCount; j++)
        {
            totalCount += (long)(m_TotalCountArray[j] - m_AvailableCountArray[j]);
        }
        tmpStr.Format ("%ld", totalCount);
        origStr = m_ResourceSummaryList.GetItemText(nItem, 3);
        if (origStr.Compare(tmpStr))
            m_ResourceSummaryList.SetItemText (nItem, 3, tmpStr.GetBuffer(1024));

        totalCount = 0;
        for (j = 0; j < jobIDCount; j++)
        {
            totalCount += (long)m_FreeCountArray[j];
        }
        tmpStr.Format ("%ld", totalCount);
        origStr = m_ResourceSummaryList.GetItemText(nItem, 4);
        if (origStr.Compare(tmpStr))
            m_ResourceSummaryList.SetItemText (nItem, 4, tmpStr.GetBuffer(1024));

        totalCount = 0;
        for (j = 0; j < jobIDCount; j++)
        {
            totalCount += (long)m_TotalJobDoneArray[j];
        }
        tmpStr.Format ("%ld", totalCount);
        origStr = m_ResourceSummaryList.GetItemText(nItem, 5);
        if (origStr.Compare(tmpStr))
            m_ResourceSummaryList.SetItemText (nItem, 5, tmpStr.GetBuffer(1024));
    }
    if (!itemCount)
        m_ResourceSummaryList.SortItems(CompareFunc, (LPARAM) this);
    m_ResourceSummaryList.SetRedraw(TRUE);
    UpdateWindow();
    return 0;
}

int CResourceSummary::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    int rc;
    CResourceSummary * pSummary = (CResourceSummary *) lParamSort;
    int jobIDCount = pSummary->m_JobIDArray.GetSize();
    int index1 = (int) lParam1;
    int index2 = (int) lParam2;

    if (index1 >= jobIDCount)
    {
        if (index2 >= jobIDCount)
        {
            if (index2 > index1)
                rc = -1;
            else
                rc = 1;
        }
        else
        {
            rc = 1;
        }
    }
    else
    if (index2 >= jobIDCount)
    {
        if (index1 >= jobIDCount)
        {
            if (index2 > index1)
                rc = -1;
            else
                rc = 1;
        }
        else
        {
            rc = -1;
        }
    }
    else
    {
        rc = pSummary->m_JobIDArray[index1].CompareNoCase(pSummary->m_JobIDArray[index2]);
    }

    return rc;
}
