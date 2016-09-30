// SymCorpUIApp.cpp : Implementation of DLL Exports, App object

#ifndef SYMCORPUIAPP
#define SYMCORPUIAPP

#include "stdafx.h"
#include "resource.h"
#include "SymCorpUI.h"
#include "dlldatax.h"
#include <atlbase.h>


class CSymCorpUIModule : public CAtlDllModuleT< CSymCorpUIModule >
{
public :
    DECLARE_LIBID(LIBID_SymCorpUILib)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SYMCORPUI, "{1EECA411-1010-4F68-8BE6-AB96C4E08769}")
};

class CSymCorpUIApp : public CWinApp
{
public:
    CSymCorpUIApp();
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    virtual void OnFileNew();
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
private:
    ULONG_PTR                       gdiplusToken;
    CSingleDocTemplate*             statusViewTemplate;
    CSingleDocTemplate*             scanViewTemplate;
    CFrameWnd*                      scansViewFrame;
};

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

#endif // SYMCORPUIAPP