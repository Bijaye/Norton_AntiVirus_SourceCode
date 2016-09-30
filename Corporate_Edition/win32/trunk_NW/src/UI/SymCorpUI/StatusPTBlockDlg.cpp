// StatusPTBlockDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SymCorpUI.h"
#include "StatusPTBlockDlg.h"
#define PROTECTIONPROVIDER_HELPERTYPES_WITHNAMESPACE
#import "ProtectionProvider.tlb" raw_interfaces_only exclude("wireHWND", "_RemotableHandle", "__MIDL_IWinTypes_0009")
#include "ProtectionProvider.h"
#include "util.h"


// CStatusPTBlockDlg dialog
IMPLEMENT_DYNAMIC(CStatusPTBlockDlg, CDialog)

BEGIN_MESSAGE_MAP(CStatusPTBlockDlg, CDialog)
    // Command handlers
    ON_COMMAND(IDB_STATUS_PTBLOCK_OPTIONS, OnBtnOptions)

    // Everything else
    ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CStatusPTBlockDlg::CStatusPTBlockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStatusPTBlockDlg::IDD, pParent), protectionProvider(NULL), splashBitmapHandle(NULL)
{
    // Create the brush for the background color.
    backgroundBrush.CreateSolidBrush(COLOR_WHITE);
    // Load background
    LoadImageResource(AfxGetResourceHandle(), IDR_STATUS_PT_BACKGROUND, _T("BINARY"), &backgroundImage);
}

CStatusPTBlockDlg::~CStatusPTBlockDlg()
{
    if (static_cast<HBRUSH>(backgroundBrush) != NULL)
        backgroundBrush.DeleteObject();
    // graphicCtrl owns splashBitmapHandle
}

void CStatusPTBlockDlg::DoDataExchange(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_STATUS_PTBLOCK_GRAPHIC, graphicCtrl);
    DDX_Control(pDX, IDC_STATUS_PTBLOCK_ONOFFGRAPHIC, onoffGraphicCtrl);
    DDX_Control(pDX, IDC_STATUS_PTBLOCK_NAME, nameCtrl);
    DDX_Control(pDX, IDC_STATUS_PTBLOCK_DESCRIPTION, descriptionCtrl);
    DDX_Control(pDX, IDC_STATUS_PTBLOCK_DEFSNAME, defsNameCtrl);
    DDX_Control(pDX, IDC_STATUS_PTBLOCK_DEFSINFO, defsDateRevCtrl);
    DDX_Control(pDX, IDC_STATUS_PTBLOCK_ONOFF, onOffCtrl);
    DDX_Control(pDX, IDB_STATUS_PTBLOCK_OPTIONS, optionsCtrl);
	CDialog::DoDataExchange(pDX);
}

HBRUSH CStatusPTBlockDlg::OnCtlColor( CDC* drawDC, CWnd* thisWindow, UINT controlCode )
{
    // Specify dialog background color to use
    switch (controlCode)
    {
    case CTLCOLOR_BTN:
    case CTLCOLOR_STATIC:
        drawDC->SetBkMode(TRANSPARENT);
        return static_cast<HBRUSH>(backgroundBrush.GetSafeHandle());
    case CTLCOLOR_DLG:
        return static_cast<HBRUSH>(backgroundBrush.GetSafeHandle());
    }

    return CDialog::OnCtlColor(drawDC, thisWindow, controlCode);
}

BOOL CStatusPTBlockDlg::OnEraseBkgnd( CDC* paintDC )
{
    // Tile version
	CRect clientRect;
	GetClientRect(clientRect);
    int currYposition = 0;

    for (currYposition = 0; currYposition < clientRect.Height(); currYposition += backgroundImage.GetHeight())
        backgroundImage.StretchBlt(paintDC->GetSafeHdc(), 0, currYposition, backgroundImage.GetWidth(), backgroundImage.GetHeight(), SRCCOPY);
    // Stretch version (for reference only)
//	backgroundImage.StretchBlt(paintDC->GetSafeHdc(), 0, 0, clientRect.Width(), clientRect.Height(), SRCCOPY);

    return TRUE;
}

BOOL CStatusPTBlockDlg::OnInitDialog()
{
    nameCtrl.fontInfo.LoadFromString(IDS_STATUS_NAME_FONT);
    //onoffGraphicCtrl handled separately
    descriptionCtrl.fontInfo.LoadFromString(IDS_STATUS_DESCRIPTION_FONT);
    defsNameCtrl.fontInfo.LoadFromString(IDS_STATUS_DEFSNAME_FONT);
    defsDateRevCtrl.fontInfo.LoadFromString(IDS_STATUS_DEFSINFO_FONT);
    optionsCtrl.defaultFont.LoadFromString(IDS_STATUS_OPTIONS_FONT);
    optionsCtrl.LoadImages(0, 0, IDR_STATUS_PTBLOCK_OPTIONSBTN_PRESSED, IDR_STATUS_PTBLOCK_OPTIONSBTN_PRESSED, CButtonEx::ScaleMode_StretchToFit);
    // On-off display
    onFont.LoadFromString(IDS_STATUS_ONOFF_ON_FONT);
    offFont.LoadFromString(IDS_STATUS_ONOFF_OFF_FONT);
    errorFont.LoadFromString(IDS_STATUS_ONOFF_ERROR_FONT);
    onText.LoadString(IDS_STATUS_ONOFF_ON);
    offText.LoadString(IDS_STATUS_ONOFF_OFF);
    errorText.LoadString(IDS_STATUS_ONOFF_ERROR);

    // Create controls
    CDialog::OnInitDialog();

    RefreshDisplay();

    return FALSE;
}

BOOL CStatusPTBlockDlg::PreTranslateMessage(MSG* pMsg)
{
    // Ignore the Escape key.  If we don't do this, then the last item in the PT block
    // display will mysteriously get a WINDOWPOSCHANGING with SW_HIDE (??)
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
    {
        // Discard this message
        return TRUE;
    }
    return CDialog::PreTranslateMessage(pMsg);
}

HRESULT CStatusPTBlockDlg::SetProtectionStatusDisplay( ProtectionProviderLib::ProtectionStatus status )
{
    if (status == ProtectionProviderLib::ProtectionStatus_On)
    {
        onoffGraphicCtrl.LoadImage(IDR_STATUS_PT_ON, CStaticEx::ScaleMode_FitKeepAspectRatio);
        onOffCtrl.fontInfo = onFont;
        onOffCtrl.SetWindowText(onText);
    }
    else if (status == ProtectionProviderLib::ProtectionStatus_Off)
    {
        onoffGraphicCtrl.LoadImage(IDR_STATUS_PT_OFF, CStaticEx::ScaleMode_FitKeepAspectRatio);
        onOffCtrl.fontInfo = offFont;
        onOffCtrl.SetWindowText(offText);
    }
    else if (status == ProtectionProviderLib::ProtectionStatus_Error)
    {
        onoffGraphicCtrl.LoadImage(IDR_STATUS_PT_OFF, CStaticEx::ScaleMode_FitKeepAspectRatio);
        onOffCtrl.fontInfo = errorFont;
        onOffCtrl.SetWindowText(errorText);
    }
    return S_OK;
}

HRESULT CStatusPTBlockDlg::RefreshDisplay( void )
{
    CComBSTR        name;
    CComBSTR        description;
    ProtectionProviderLib::ProtectionStatus     protectionStatus = ProtectionProviderLib::ProtectionStatus_Off;
    CComBSTR        defsDescription;
    CComBSTR        defsDateRev;
    ProtectionProviderLib::IProtectionPtr                   providerProtectionPtr;
    ProtectionProviderLib::IProtectionPtr                   primaryAP;
    ProtectionProviderLib::IProtection_DefinitionInfoPtr    providerDefInfoPtr;
    ProtectionProviderLib::_SYSTEMTIME      defsDate                    = {0};
    unsigned int    defsRevisionNo              = 0;
    CString         dateFormatString;
    CString         dateRevFormat;
    CString         dateString;
    CString         finalDateRevString;
    HRESULT         returnValHR                 = E_FAIL;
    HRESULT         returnValHRb                = E_FAIL;

    providerProtectionPtr = protectionProvider;
    if (providerProtectionPtr != NULL)
    {
        providerProtectionPtr->get_DisplayName(&name);
        providerProtectionPtr->get_Description(&description);
        providerProtectionPtr->get_ProtectionStatus(&protectionStatus);
        // If status is ON, override with primary autoprotect status, if present
        if (protectionStatus == ProtectionProviderLib::ProtectionStatus_On)
        {
            protectionProvider->get_PrimaryAutoprotect(&primaryAP);
            if (primaryAP != NULL)
                primaryAP->get_ProtectionStatus(&protectionStatus);
        }
        SetProtectionStatusDisplay(protectionStatus);
        nameCtrl.SetWindowText(name);
        descriptionCtrl.SetWindowText(description);
        returnValHR = protectionProvider->get_SplashGraphic((void**) &splashBitmapHandle);
        if (FAILED(returnValHR))
            splashBitmapHandle = LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_STATUS_PTBLOCK_DEFAULTSPLASH));
        graphicCtrl.SetBitmap(splashBitmapHandle, CStaticEx::ScaleMode_FitKeepAspectRatio);
    }

    providerDefInfoPtr = protectionProvider;
    if (providerDefInfoPtr != NULL)
    {
        providerDefInfoPtr->get_ShortDescription(&defsDescription);

        returnValHR = providerDefInfoPtr->get_Date(&defsDate);
        returnValHRb = providerDefInfoPtr->get_RevisionNo(&defsRevisionNo);
        if ((returnValHR == S_OK) && (returnValHRb == S_OK))
        {
            // Date and revision
            dateFormatString.LoadString(IDS_DEFSDATEFORMAT);
            dateRevFormat.LoadString(IDS_STATUS_DEFSDATEREVFORMAT);
            GetDateFormat(NULL, NULL, (SYSTEMTIME*) &defsDate, (LPCTSTR) dateFormatString, dateString.GetBuffer(50), 50);
            dateString.ReleaseBuffer();
            finalDateRevString.FormatMessage(dateRevFormat, (LPCTSTR) dateString, defsRevisionNo);
        }
        else if ((returnValHR == S_OK) && (returnValHRb == S_FALSE))
        {
            // Date only
            GetDateFormat(NULL, DATE_LONGDATE, (SYSTEMTIME*) &defsDate, NULL, finalDateRevString.GetBuffer(50), 50);
            finalDateRevString.ReleaseBuffer();
        }
        else if ((returnValHR == S_FALSE) && (returnValHRb == S_OK))
        {
            // Revision only
            finalDateRevString.FormatMessage(_T("%1!d!"), defsRevisionNo);
        }
        else if ((returnValHR == S_FALSE) && (returnValHRb == S_FALSE))
        {
            // Neither date or revision
        }
        else
        {
            // Error - show nothing
        }
    }
    defsNameCtrl.SetWindowText(defsDescription);
    defsDateRevCtrl.SetWindowText(finalDateRevString);
    return S_OK;
}

void CStatusPTBlockDlg::OnBtnOptions()
{
    CMenu                                   menuLoader;
    CMenu*                                  optionsMenu             = NULL;
    CRect                                   controlScreenRect;
    ProtectionProviderLib::IProtectionPtr   protectionProviderControl;
    ProtectionProviderLib::IProtectionPtr   primaryAutoprotect;
    ProtectionProviderLib::ProtectionStatus primaryAutoprotectProtectionStatus  = ProtectionProviderLib::ProtectionStatus_Off;
    DWORD                                   returnValDW                         = 0;

    // TODO:  Interrogate protection provider for extensions, add here, and handle

    // Initialize
    protectionProviderControl = protectionProvider;
    optionsCtrl.GetWindowRect(controlScreenRect);
    menuLoader.LoadMenu(IDR_STATUS_OPTIONSMENU);
    optionsMenu = menuLoader.GetSubMenu(0);
    if (optionsMenu != NULL)
    {
        // If there is a primary autoprotect, configure the turn off/turn on realtime protection option
        protectionProvider->get_PrimaryAutoprotect(&primaryAutoprotect);
        if (primaryAutoprotect != NULL)
        {
            primaryAutoprotect->get_ProtectionStatus(&primaryAutoprotectProtectionStatus);
            if (primaryAutoprotectProtectionStatus == ProtectionProviderLib::ProtectionStatus_On)
            {
                optionsMenu->RemoveMenu(ID_STATUS_OPTIONS_REALTIMEPROTECTION_TURNON, MF_BYCOMMAND);
            }
            else if (primaryAutoprotectProtectionStatus == ProtectionProviderLib::ProtectionStatus_Off)
            {
                optionsMenu->RemoveMenu(ID_STATUS_OPTIONS_REALTIMEPROTECTION_TURNOFF, MF_BYCOMMAND);
            }
            else // Error
            {
                optionsMenu->RemoveMenu(ID_STATUS_OPTIONS_REALTIMEPROTECTION_TURNON, MF_BYCOMMAND);
                optionsMenu->RemoveMenu(ID_STATUS_OPTIONS_REALTIMEPROTECTION_TURNOFF, MF_BYCOMMAND);
            }
        }
        else
        {
            optionsMenu->RemoveMenu(ID_STATUS_OPTIONS_REALTIMEPROTECTION_TURNON, MF_BYCOMMAND);
            optionsMenu->RemoveMenu(ID_STATUS_OPTIONS_REALTIMEPROTECTION_TURNOFF, MF_BYCOMMAND);
        }

        // Get and handle the menu selection
        returnValDW = optionsMenu->TrackPopupMenu(TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_CENTERALIGN, controlScreenRect.CenterPoint().x, controlScreenRect.CenterPoint().y, this);
        switch (returnValDW)
        {
        case ID_STATUS_OPTIONS_CHANGESETTINGS:
            protectionProviderControl->ShowConfigureUI((wireHWND) GetSafeHwnd());
            break;
        case ID_STATUS_OPTIONS_VIEWLOGS:
            protectionProviderControl->ShowLogUI((wireHWND) GetSafeHwnd());
            break;
        case ID_STATUS_OPTIONS_REALTIMEPROTECTION_TURNOFF:
            primaryAutoprotect->put_ProtectionConfiguration(FALSE);
            RefreshDisplay();
            break;
        case ID_STATUS_OPTIONS_REALTIMEPROTECTION_TURNON:
            primaryAutoprotect->put_ProtectionConfiguration(TRUE);
            RefreshDisplay();
            break;
        case 0:
            break;
        default:
            // This is where extension options would be handled - IDD_SOMEBASEVAL else+0, +1, etc.
            MessageBox(_T("Something else selected"));
            break;
        }
    }
}