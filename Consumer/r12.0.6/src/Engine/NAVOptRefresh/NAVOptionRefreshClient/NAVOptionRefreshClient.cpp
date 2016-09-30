// NAVOptionRefreshClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#if !defined(_NO_SIMON_INITGUID)
    #define SIMON_INITGUID
#endif

#include <SIMON.h>
#include "NAVOptRefresh.h"

void Start();
void Stop();

int main(int argc, char* argv[])
{
    HRESULT hr = S_OK;
    try
    {
        StahlSoft::HRX hrx;
        TCHAR szBuffer[_MAX_PATH*2] = {0};
        ::GetModuleFileName(NULL,szBuffer,sizeof(szBuffer));
        TCHAR * p = _tcsrchr(szBuffer,_T('\\'));
        hrx << (p?S_OK:E_POINTER);
        *p = _T('\0');
        _tcscat(szBuffer,_T("\\NAVOPTRF.DLL"));
        StahlSoft::CSmartModuleHandle smLib(::LoadLibrary(szBuffer));
        SIMON::CSimonPtr<INAVOptionRefreshEngine>   spEngine;
        
        hrx << SimonCreateInstanceByDLLInstance(smLib,CLSID_CNAVOptionRefreshEngine,IID_INAVOptionRefreshEngine,(void**)&spEngine);
        hrx << spEngine->Init();
        hrx << spEngine->Start();
        
        bool b= true;
        while(b)
        {
            Sleep(1000);
        }
        hrx << spEngine->Stop();
    }
    catch(_com_error& e)
    {
        hr = e.Error();
    }
    catch(...)
    {
        hr = E_UNEXPECTED;
    }
    return hr;
}

