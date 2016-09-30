// SymCorpUI.cpp : Implementation of DLL Exports, App object
#include "stdafx.h"
#include "resource.h"
#include "SymCorpUI.h"
#include "dlldatax.h"
#include "SymCorpUIApp.h"
#include "SymCorpUIDoc.h"
#include "SymCorpUIStatusView.h"
#include "MainFrm.h"
#include "SymCorpUIScansView.h"


// ** GLOBALS **
CSymCorpUIModule _AtlModule;
CSymCorpUIApp theApp;


// CSYMCORPUIAPP
BEGIN_MESSAGE_MAP(CSymCorpUIApp, CWinApp)
    // Standard app stuff
    ON_COMMAND(ID_APP_ABOUT, &CSymCorpUIApp::OnAppAbout)
    ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


BOOL CSymCorpUIApp::InitInstance()
{
    INITCOMMONCONTROLSEX            commonControlsInitArgs  = {0};
    Gdiplus::GdiplusStartupInput    gdiplusStartupArgs;
    Gdiplus::Status                 returnValStatus         = Gdiplus::GenericError;
    HRESULT                         returnValHR             = E_FAIL;
    BOOL returnValBOOL = FALSE;

#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, NULL))
		return FALSE;
#endif

    // MFC general initialization
    returnValBOOL = CWinApp::InitInstance();

    // Initialize common controls
	InitCommonControls();
    // NOTE:  On XP, if an app manifest specifies use of common controls but
    // this isn't call, any window creation will fail
    commonControlsInitArgs.dwSize = sizeof(commonControlsInitArgs);
    commonControlsInitArgs.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&commonControlsInitArgs);

    // Initialize OLE libraries
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }
    AfxEnableControlContainer();

    returnValStatus = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupArgs, NULL);
    returnValHR = GdiplusStatusToHresult(returnValStatus);
    if (FAILED(returnValHR))
        returnValBOOL = FALSE;

    // MFC document/view/frame association registrations
    // Status view
    statusViewTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CSymCorpUIDoc),
        RUNTIME_CLASS(CMainFrame),       // Main SDI frame window
        RUNTIME_CLASS(CSymCorpUIStatusView));
    if (statusViewTemplate != NULL)
        AddDocTemplate(statusViewTemplate);
    else
        returnValBOOL = FALSE;
    // Scan view
	scanViewTemplate = new CSingleDocTemplate(
		IDR_SYMCORPUISCANSVIEW_TMPL,
		RUNTIME_CLASS(CSymCorpUIDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CSymCorpUIScansView));
    if (scanViewTemplate != NULL)
        AddDocTemplate(scanViewTemplate);
    else
        returnValBOOL = FALSE;

    return returnValBOOL;
}

int CSymCorpUIApp::ExitInstance()
{
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return CWinApp::ExitInstance();
}

void CSymCorpUIApp::OnFileNew()
{
    // Eliminate MFC default behavior of popping a list showing all registered DocTemplates
    // This also requires clearing fields 2-6 of the string resource used for DocTemplate registration
    statusViewTemplate->OpenDocumentFile(NULL);
}

CSymCorpUIApp::CSymCorpUIApp() : gdiplusToken(NULL), statusViewTemplate(NULL), scanViewTemplate(NULL), scansViewFrame(NULL)
{
    // Nothing needed
}

// CABOUTDLG
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CSymCorpUIApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}


// EXPORTS
STDAPI DllCanUnloadNow(void)
// Used to determine whether the DLL can be unloaded by OLE
{
#ifdef _MERGE_PROXYSTUB
    HRESULT hr = PrxDllCanUnloadNow();
    if (hr != S_OK)
        return hr;
#endif
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _AtlModule.GetLockCount()==0) ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
// Returns a class factory to create an object of the requested type
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


STDAPI DllRegisterServer(void)
// DllRegisterServer - Adds entries to the system registry
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
#endif
	return hr;
}


STDAPI DllUnregisterServer(void)
// DllUnregisterServer - Removes entries from the system registry
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
    if (FAILED(hr))
        return hr;
    hr = PrxDllUnregisterServer();
#endif
	return hr;
}