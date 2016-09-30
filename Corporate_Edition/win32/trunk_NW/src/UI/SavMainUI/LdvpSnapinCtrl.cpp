#include "stdafx.h"
#include "LDVPSnapInCtrl.h"
#include "SuperAssertEx.h"
#include "util.h"
#include <afxpriv.h>


// ** CONSTANTS **
const CLSID CLdvpSnapinCtrl::FilesystemApSnapinCLSID         = { 0x536604C2, 0xB82E, 0x11D1, { 0x82, 0x52, 0x00, 0xA0, 0xC9, 0x5C, 0x07, 0x56 } };
const CLSID CLdvpSnapinCtrl::ExchangeApSnapinCLSID           = { 0xFF1C1AB8, 0xC27D, 0x11D1, { 0x82, 0x63, 0x00, 0xA0, 0xC9, 0x5C, 0x07, 0x56 } };
const CLSID CLdvpSnapinCtrl::NotesApSnapinCLSID              = { 0x2707AAC5, 0xC268, 0x11D1, { 0x82, 0x63, 0x00, 0xA0, 0xC9, 0x5C, 0x07, 0x56 } };
const CLSID CLdvpSnapinCtrl::InternetMailApSnapinCLSID       = { 0x72E2440E, 0xEBEA, 0x49E6, { 0xA1, 0x85, 0x1B, 0xE0, 0x3F, 0x72, 0x3E, 0x28 } };

// ** MFC Support **
IMPLEMENT_DYNCREATE(CLdvpSnapinCtrl, CWnd)

BEGIN_MESSAGE_MAP(CLdvpSnapinCtrl, CWnd)
END_MESSAGE_MAP()

CLdvpSnapinCtrl::CLdvpSnapinCtrl() : snapinID(CLdvpSnapinCtrl::SavSnapin_Filesystem), configObject(NULL), configPath(NULL)
{
    // Nothing for now...
}

CLdvpSnapinCtrl::~CLdvpSnapinCtrl()
{
    // Nothing for now
}

void CLdvpSnapinCtrl::SetSavSnapinID( SavSnapin newSnapinID )
{
    LPCTSTR         storageName     = NULL;
    DWORD           returnValDW     = ERROR_OUT_OF_PAPER;
    
    switch (newSnapinID)
    {
        case SavSnapin_Filesystem:
            configPath = _T(szReg_Key_Storage_File);
            break;
        case SavSnapin_Exchange:
            configPath = _T(szReg_Key_Storage_Exchange);
            break;
        case SavSnapin_Notes:
            configPath = _T(szReg_Key_Storage_Notes);
            break;
        case SavSnapin_InternetEmail:
            configPath = _T(szReg_Key_Storage_InternetMail);
            break;
        default:
            SAVASSERT("Unknown snapin ID for CLdvpSnapinCtrl!");
            configPath = _T(szReg_Key_Storage_File);
            break;
    }
    snapinID = newSnapinID;
}

CLdvpSnapinCtrl::SavSnapin CLdvpSnapinCtrl::GetSnapinID( void )
{
    return snapinID;
}

CLSID const& CLdvpSnapinCtrl::GetClsid()
{
    switch (snapinID)
    {
        case SavSnapin_Filesystem:
            return FilesystemApSnapinCLSID;
        case SavSnapin_Exchange:
            return ExchangeApSnapinCLSID;
        case SavSnapin_Notes:
            return NotesApSnapinCLSID;
        case SavSnapin_InternetEmail:
            return InternetMailApSnapinCLSID;
        default:
            SAVASSERT("Unknown snapin ID for CLdvpSnapinCtrl!");
            return FilesystemApSnapinCLSID;
    }
}

HRESULT CLdvpSnapinCtrl::CreationSetup( void )
{
    _bstr_t     stringConverter;
    DWORD       returnValDW     = ERROR_OUT_OF_PAPER;

    if (SUCCEEDED(CoCreateLDVPObject(CLSID_CliProxy, IID_IGenericConfig, (void**)&configObject)))
    {
        stringConverter = configPath;
        returnValDW = configObject->Open(NULL, HKEY_VP_STORAGE_REALTIME, (char*) stringConverter, GC_MODEL_SINGLE);

        if (returnValDW == ERROR_SUCCESS)
        {
            SetConfigInterface(configObject);
            Load();
            configObject->Release();
        }
        return HRESULT_FROM_WIN32(returnValDW);
    }
    else
    {
        return E_FAIL;
    }
}

BOOL CLdvpSnapinCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext )
{ 
    if (CreationSetup() == ERROR_SUCCESS)
        return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
    else
        return FALSE;
}

BOOL CLdvpSnapinCtrl::Create( LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CFile* pPersist, BOOL bStorage, BSTR bstrLicKey )
{ 
    if (CreationSetup() == ERROR_SUCCESS)
        return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID, pPersist, bStorage, bstrLicKey); 
    else
        return FALSE;
}

void CLdvpSnapinCtrl::PreSubclassWindow( )
{
    CreationSetup();
}

signed char CLdvpSnapinCtrl::Load()
{
    signed char result;
    InvokeHelper(0x5, DISPATCH_METHOD, VT_I1, (void*)&result, NULL);
    return result;
}

signed char CLdvpSnapinCtrl::Store()
{
    signed char result;
    InvokeHelper(0x6, DISPATCH_METHOD, VT_I1, (void*)&result, NULL);
    return result;
}

long CLdvpSnapinCtrl::GetDlgHeight()
{
    long result;
    InvokeHelper(0x7, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
    return result;
}

void CLdvpSnapinCtrl::AboutBox()
{
    InvokeHelper(DISPID_ABOUTBOX, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

// Properties
long CLdvpSnapinCtrl::GetMode()
{
    long result;
    GetProperty(0x1, VT_I4, (void*)&result);
    return result;
}

void CLdvpSnapinCtrl::SetMode( long newMode )
{
    SetProperty(0x1, VT_I4, newMode);
}

long CLdvpSnapinCtrl::GetType()
{
    long result;
    GetProperty(0x2, VT_I4, (void*)&result);
    return result;
}

void CLdvpSnapinCtrl::SetType( long newType )
{
    SetProperty(0x2, VT_I4, newType);
}

long CLdvpSnapinCtrl::GetConfiguration()
{
    long result;
    GetProperty(0x3, VT_I4, (void*)&result);
    return result;
}

void CLdvpSnapinCtrl::SetConfiguration( long newConfig )
{
    SetProperty(0x3, VT_I4, newConfig);
}

LPUNKNOWN CLdvpSnapinCtrl::GetConfigInterface()
{
    LPUNKNOWN result;
    GetProperty(0x4, VT_UNKNOWN, (void*)&result);
    return result;
}

void CLdvpSnapinCtrl::SetConfigInterface( LPUNKNOWN newConfigObject )
{
    SetProperty(0x4, VT_UNKNOWN, newConfigObject);
}