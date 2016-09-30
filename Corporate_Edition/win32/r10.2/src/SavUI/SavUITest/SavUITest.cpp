// SavUITest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <comdef.h>
#include "SavUITest.h"
#include "sessionmoniker.h"
#include "OSUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "savui_h.h"
// The one and only application object

_COM_SMARTPTR_TYPEDEF(IVirusFoundCOMAdapter, __uuidof(IVirusFoundCOMAdapter));
_COM_SMARTPTR_TYPEDEF(IResultsViewCOMAdapter, __uuidof(IResultsViewCOMAdapter));

CWinApp theApp;

using namespace std;

const CLSID CLSID_VirusFoundCOMAdapter = {0xA1987DB8,0x9F0D,0x47D1,0x80,0xC9,0xDF,0xCE,0x76,0x26,0x08,0x41};
const IID IID_IVirusFoundCOMAdapter	   = {0x11D8A19E,0xB7BA,0x46EE,0xBC,0xB7,0x4A,0x8A,0x74,0x1C,0xAC,0x2F};

const CLSID CLSID_ResultsViewCOMAdapter	= {0x84AC6BE7,0x8CF2,0x4e67,0xA8,0x0E,0x32,0xAC,0xD3,0xD7,0xC3,0x81};
const IID IID_IResultsViewCOMAdapter	= {0x646C8A12,0x4A6B,0x425c,0x97,0xF4,0xC2,0x2D,0xDA,0x93,0xB7,0x44};

_bstr_t pszLogLine = "2401110A0E05,5,1,2,IAN-XPSP2,Test,EICAR Test String,C:\\Documents and Settings\\Test\\Local Settings\\Temporary Internet Files\\Content.IE5\\GXAVSLUB\\eicar[1].com,17,17,19,256,574640196,"",0,,0,201	4	11	1	65536	0	0	0	0	0	0,228327424,11101,0,1,0,0,0,0,,0,2,4,228327424,,{D6A03612-C79A-46D3-8C4B-9E63ECC8AFF1},,,,MSHOME,00:0C:29:55:AF:20,10.1.0.9,,,,,,,,,,,,,,,,0,,,0,";
_bstr_t pszDescription = "Scan type:   Scan\r\nEvent:  \r\nEICAR Test String\r\nFile:  C:\\Documents and Settings\\Test\\Local Settings\\Temporary Internet Files\\Content.IE5\\GXAVSLUB\\eicar[1].com\r\nLocation:  C:\\Documents and Settings\\Test\\Local Settings\\Temporary Internet Files\\Content.IE5\\GXAVSLUB\r\nComputer:  IAN-XPSP2\r\nUser:  Test\r\nAction taken:  \r\nDate found: Friday, February 17, 2006  10:14:05 AM";

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	// To get rid of unused parameter warnings.
	argc; argv; envp;
	// Initialize.
	int nRetCode = 0;

    CoInitialize(NULL);

#if 0
    ::MessageBox(NULL, "TEST", "TEST", MB_OK);
    IVirusFoundCOMAdapterPtr VirusFoundCOMAdapterPtr = NULL;
    HRESULT hr = VirusFoundCOMAdapterPtr.CreateInstance(CLSID_VirusFoundCOMAdapter);
    if( VirusFoundCOMAdapterPtr != NULL )
    {
        CComVariant vLogLine;
        CComVariant vDescription;
        //    vLogLine.bstrVal = pszLogLine;
        //    vDescription.bstrVal = pszDescription;
        VirusFoundCOMAdapterPtr->AddMessage(pszLogLine, pszDescription);
    }
#endif 
//#if 0
    ::MessageBox(NULL, "TEST", "TEST", MB_OK);

	DWORD dwSessionId = IsWindowsVista() ? 1 : 0;

    {
        IClassFactoryPtr objFactoryPtr = NULL;
        IVirusFoundCOMAdapterPtr VirusFoundCOMAdapterPtr = NULL;

		CSessionMoniker objSessionMoniker(dwSessionId);
        DWORD dwRet = objSessionMoniker.CreateMoniker(CLSID_VirusFoundCOMAdapter);
        if( SUCCEEDED(dwRet) )
        {
            dwRet = objSessionMoniker.BindToObject((void**)&objFactoryPtr);
            if( ERROR_SUCCESS == dwRet )
                dwRet = objFactoryPtr->CreateInstance(NULL, IID_IVirusFoundCOMAdapter, (void**)&VirusFoundCOMAdapterPtr);
        }

        if( VirusFoundCOMAdapterPtr != NULL )
        {
            
        //    vLogLine.bstrVal = pszLogLine;
        //    vDescription.bstrVal = pszDescription;
            CComBSTR title = "test";
            VirusFoundCOMAdapterPtr->CreateDlg(dwSessionId, title);
            VirusFoundCOMAdapterPtr->AddMessage(pszLogLine, pszDescription);
            //Sleep(60000);
        }
    }

    {
        IClassFactoryPtr objFactoryPtr = NULL;
        IResultsViewCOMAdapterPtr ResultsViewCOMAdapterPtr = NULL;

        CSessionMoniker objSessionMoniker(dwSessionId);
        DWORD dwRet = objSessionMoniker.CreateMoniker(CLSID_ResultsViewCOMAdapter);
        if( SUCCEEDED(dwRet) )
        {
            dwRet = objSessionMoniker.BindToObject((void**)&objFactoryPtr);
            if( ERROR_SUCCESS == dwRet )
                dwRet = objFactoryPtr->CreateInstance(NULL, IID_IResultsViewCOMAdapter, (void**)&ResultsViewCOMAdapterPtr);
        }

        if( ResultsViewCOMAdapterPtr != NULL )
        {
            
        //    vLogLine.bstrVal = pszLogLine;
        //    vDescription.bstrVal = pszDescription;
            CComBSTR title = "test scan results";
            ResultsViewCOMAdapterPtr->CreateDlg(dwSessionId, title);
            //ResultsViewCOMAdapterPtr->AddMessage(pszLogLine, pszDescription);
            //Sleep(60000);
        }
    }
//#endif
    CoUninitialize();
    

	return nRetCode;
}
