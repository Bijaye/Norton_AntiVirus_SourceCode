// VirusDefUpdateScheduleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SavMainUI.h"
#include "VirusDefUpdateScheduleDlg.h"


// CVirusDefUpdateScheduleDlg dialog

IMPLEMENT_DYNAMIC(CVirusDefUpdateScheduleDlg, CDialog)

CVirusDefUpdateScheduleDlg::CVirusDefUpdateScheduleDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CVirusDefUpdateScheduleDlg::IDD, pParent), configObject(NULL)
{
    // Nothing for now
}

CVirusDefUpdateScheduleDlg::~CVirusDefUpdateScheduleDlg()
{
    if (configObject != NULL)
    {
        configObject->Release();
        configObject = NULL;
    }
}

void CVirusDefUpdateScheduleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SCHEDULE, scheduleCtrl);
}


BEGIN_MESSAGE_MAP(CVirusDefUpdateScheduleDlg, CDialog)
END_MESSAGE_MAP()

BOOL CVirusDefUpdateScheduleDlg::OnInitDialog()
{
    IConfig*    configObjectConfig      = NULL;
    DWORD       returnValDW             = ERROR_OUT_OF_PAPER;

    // Create controls
    CDialog::OnInitDialog();

    // Load data into schedule control
    if (SUCCEEDED(CoCreateLDVPObject(CLSID_CliProxy, IID_IGenericConfig, (void**)&configObject)))
    {
        _bstr_t stringConverter;

        // Schedule control works with the szReg_Key_ScheduleKey subkey of what we pass it.
        stringConverter = _T(szReg_Key_PatternManager);
        returnValDW = configObject->Open(NULL, HKEY_VP_MAIN_ROOT, (char*) stringConverter, GC_MODEL_SINGLE);

        if (returnValDW == ERROR_SUCCESS)
        {
            // Following odd manual cast is required - passing configObject directly compiles
            // but the Store call will crash.
            configObjectConfig = configObject;
            configObjectConfig->AddRef();
            scheduleCtrl.Load(configObjectConfig);
            configObject->Release();
        }
    }
    return TRUE;
}

void CVirusDefUpdateScheduleDlg::OnOK()
{
    scheduleCtrl.Store(configObject);
    EndDialog(IDOK);
}