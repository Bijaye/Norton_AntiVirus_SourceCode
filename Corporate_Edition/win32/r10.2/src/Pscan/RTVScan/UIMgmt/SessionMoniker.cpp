// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#include "sessionmoniker.h"

const OLECHAR* CSessionMoniker::s_olechMonikerFormat = L"Session:%d!clsid:%s";

CSessionMoniker::CSessionMoniker(const DWORD dwSessionID) : m_pMoniker(NULL),
                                                          m_pBindCtx(NULL),
                                                          m_dwSessionID(dwSessionID)
{
    memset(m_olechMoniker, 0, sizeof(m_olechMoniker));
}

CSessionMoniker::~CSessionMoniker(void)
{
        if( m_pMoniker )
        {
            m_pMoniker->Release();
            m_pMoniker = NULL;
        }

        if( m_pBindCtx )
        {
            m_pBindCtx->Release();
            m_pBindCtx = NULL;
        }
}

/** DWORD FormatStringMoniker(const CLSID CLSIDobjToInstantiate)
  * 
  * formats the moniker string to be used by create moniker for
  * creating the correct session
  *
  **/
DWORD CSessionMoniker::FormatStringMoniker(const CLSID CLSIDobjToInstantiate)
{
    LPOLESTR pwsCLSID = NULL;
    DWORD dwRet = StringFromCLSID(CLSIDobjToInstantiate, &pwsCLSID);
    if( SUCCEEDED(dwRet) )
    {
        swprintf(m_olechMoniker, s_olechMonikerFormat, m_dwSessionID, pwsCLSID);
        CoTaskMemFree(pwsCLSID);
    }

    return dwRet;
}

/** CreateMoniker(const CLSID CLSIDobjToInstantiate);
  * 
  * creates the session moniker for the session specified by m_dwSessionId
  *
  **/
DWORD CSessionMoniker::CreateMoniker(const CLSID CLSIDobjToInstantiate)
{
    DWORD dwRet = ERROR_INVALID_DATA;
    if ( S_OK == FormatStringMoniker(CLSIDobjToInstantiate) && 
         S_OK == CreateBindCtx(NULL, &m_pBindCtx) )
    {
        ULONG ulParsed = 0;
		dwRet = MkParseDisplayNameEx(m_pBindCtx, m_olechMoniker, &ulParsed, &m_pMoniker);
    }
    
    return dwRet;
}

/** DWORD BindToObject(void** objFactory)
  *
  * Finds the object and puts it into a running state
  *
  **/
DWORD CSessionMoniker::BindToObject(void** objFactory)
{
    return m_pMoniker->BindToObject(m_pBindCtx, NULL, IID_IClassFactory, objFactory);
}