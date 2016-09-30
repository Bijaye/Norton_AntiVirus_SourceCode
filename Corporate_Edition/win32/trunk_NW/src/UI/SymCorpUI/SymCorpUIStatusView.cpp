// SymCorpUI-newView.cpp : implementation of the CSymCorpUIStatusView class
//

#include "stdafx.h"
#include "SymCorpUI.h"
#include "SymCorpUIDoc.h"
#include "SymCorpUIStatusView.h"
#include "SymSaferRegistry.h"
#include "ScanState.h"

// CSymCorpUIStatusView
BEGIN_MESSAGE_MAP(CSymCorpUIStatusView, CViewEx)
    // Command notifications
    ON_COMMAND(IDC_STATUSVIEW_ACTIONBOX_ACTIONBUTTON, OnActionFixButton)
    ON_COMMAND(IDC_STATUSVIEW_ACTIONBOX_UPCOMING_BUTTON, OnActionUpcomingButton)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CSymCorpUIStatusView, CViewEx)


// CSymCorpUIStatusView construction/destruction
CSymCorpUIStatusView::CSymCorpUIStatusView() : CViewEx(CSymCorpUIStatusView::IDD, COLOR_WHITE), providerBlocks(NULL)
{
    // Nothing for now
}

CSymCorpUIStatusView::~CSymCorpUIStatusView()
{
    ReleaseProviderBlocks(false);
}

void CSymCorpUIStatusView::OnDraw(CDC* /*pDC*/)
{
    CSymCorpUIDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // TODO: add draw code for native data here
    CStatusPTBlockDlgPtrList::iterator currPTblock;
    for (currPTblock = providerBlocks.begin(); currPTblock != providerBlocks.end(); currPTblock++)
    {
        (*currPTblock)->ShowWindow(TRUE);
    }
}

void CSymCorpUIStatusView::DoDataExchange( CDataExchange* pDX )
{
    CViewEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATUSVIEW_STATIC_TITLE, staticProtectionTechHeader);
    DDX_Control(pDX, IDC_STATUSVIEW_STATIC_DESCRIPTION, staticDescription);
    actionBox.DoDataExchange(pDX);
}

// CSymCorpUIStatusView message handlers
void CSymCorpUIStatusView::OnInitialUpdate( )
{
    // Configure statics
    staticProtectionTechHeader.fontInfo.LoadFromString(IDS_STATUS_VIEWHEADER_FONT);
    staticDescription.fontInfo.LoadFromString(IDS_STATUS_VIEWDESCRIPTION_FONT);

    // Action box
    DWORD startTime = GetTickCount();
    actionBox.OnInitialUpdate();

    // Create controls...
    CViewEx::OnInitialUpdate();
    DWORD timeB = GetTickCount();

    // CSymCorpUIStatusView, most visibly Action box, is flickering because in the dialog creation of CreateProviderBlocks,
    // the parent window seems to get invalidated and redrawn even though we're creating an invisible window.
    CreateProviderBlocks();
    DWORD stopTime = GetTickCount();
//    actionBox.SetMode(ActionBox::Mode_ActionRequested, ActionBox::BackgroundMode_Problem, _T("There are multiple problems"), _T("AntiVirus protection is disabled.\nFirewall protection is disabled.  Click Fix to resolve this."));
//    actionBox.SetActionRequestedButton(true, _T("Enable"));
    actionBox.SetMode(ActionBox::Mode_Informational, ActionBox::BackgroundMode_Good, _T("\nYour computer is protected."), _T("It's all good, holmes."));
    actionBox.SetActionRequestedButton(false, _T(""));
}

HRESULT CSymCorpUIStatusView::CreateProviderBlocks( void )
// Creates the provider block listings on the status dialog
{
    CSymCorpUIDoc*                                              currDocument                = NULL;
    ProtectionProviderLib::ProtectionProviderList::iterator     currProtectionProvider;
    CStatusPTBlockDlg*                                          newProviderBlockDialog      = NULL;
    CRect                                                       blockRect;
    int                                                         blockVerticalPosition       = 0;

    ReleaseProviderBlocks(false);

    blockVerticalPosition = defaultSize.Height();
    currDocument = GetDocument();
    if (currDocument != NULL)
    {
        for (currProtectionProvider = currDocument->protectionProviders.begin(); currProtectionProvider != currDocument->protectionProviders.end(); currProtectionProvider++)
        {
            try
            {
                newProviderBlockDialog = new CStatusPTBlockDlg;
            }
            catch (std::bad_alloc&)
            {
                newProviderBlockDialog = NULL;
            }
            if (newProviderBlockDialog != NULL)
            {
                newProviderBlockDialog->protectionProvider = *currProtectionProvider;
                if (newProviderBlockDialog->Create(CStatusPTBlockDlg::IDD))
                {
                    newProviderBlockDialog->SetParent(this);
                    if (blockRect.IsRectEmpty())
                        newProviderBlockDialog->GetClientRect(blockRect);
                    newProviderBlockDialog->SetWindowPos(&wndTop, 0, blockVerticalPosition, 0, 0, SWP_NOSIZE | SWP_HIDEWINDOW);
                    newProviderBlockDialog->RefreshDisplay();
                    blockVerticalPosition += blockRect.Height();
                    providerBlocks.push_back(newProviderBlockDialog);
                }
                else
                {
                    delete newProviderBlockDialog;
                    newProviderBlockDialog = NULL;
                }
            }
        }
    }

    return S_OK;
}

HRESULT CSymCorpUIStatusView::ReleaseProviderBlocks( bool updateDisplay )
// Releases the provider block dialogs
{
    HRESULT                             returnValHR     = S_FALSE;
    CStatusPTBlockDlgPtrList::iterator  currProviderBlock;

    if (providerBlocks.size() != 0)
        returnValHR = S_OK;
    for (currProviderBlock = providerBlocks.begin(); currProviderBlock != providerBlocks.end(); currProviderBlock++)
        delete (*currProviderBlock);
    providerBlocks.clear();

    if (updateDisplay)
        Invalidate();
    return returnValHR;
}

void CSymCorpUIStatusView::OnActionFixButton()
{
    // *** DML TEMP DEV
    // ***
    MessageBox(_T("OnActionFixButton"), _T("Caption"));
}

void CSymCorpUIStatusView::OnActionUpcomingButton()
{
    MessageBox(_T("OnActionUpcomingButton"), _T("Caption"));
}

void CSymCorpUIStatusView::OnUpdate( CView* sender, LPARAM hintParam, CObject* hintObject )
{
    // Can't create PTBlockDlg's until we have a window, so don't
    if (!initialUpdateRan)
        return;

    //CreateProviderBlocks();

//    actionBox.SetMode(ActionBox::Mode_ActionRequested, ActionBox::BackgroundMode_Problem, _T("There are multiple problems"), _T("AntiVirus protection is disabled.\nFirewall protection is disabled.  Click Fix to resolve this."));
//    actionBox.SetActionRequestedButton(true, _T("Enable"));
//    actionBox.SetInformationalUpcomingText(_T("What's Next?"), _T("Next scan schedule for 2006 March 27 at 9:12P"), true, _T("Reschedule"));

//    actionBox.SetMode(ActionBox::Mode_Informational, ActionBox::BackgroundMode_Good, _T("Your computer is protected.  Really long german words go here"), _T("A description of the alert can go here to clarify what the title means.  And yet even more description may spill past in foreign languages!  And even longer lines will wrap like this one hopefully does..."));
//    actionBox.SetInformationalUpcomingText(_T("What's Next?"), _T("Next scan schedule for 2006 March 27 at 9:12P"), true, _T("Reschedule"));
}