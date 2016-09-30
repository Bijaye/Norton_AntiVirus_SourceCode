// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

#pragma once
#include <comdef.h>

#include "sessionmoniker.h"
#include "VirusFoundCOMCallback.h"
#include "savui_h.h"


class CVirusFoundProxy
{
private:
    _COM_SMARTPTR_TYPEDEF(IVirusFoundCOMAdapter, __uuidof(IVirusFoundCOMAdapter));
    _COM_SMARTPTR_TYPEDEF(IVirusFoundCOMCallback, __uuidof(IVirusFoundCOMCallback));

public:
    /** default constructor **/
    CVirusFoundProxy():m_dwSessionId(0){}
    CVirusFoundProxy(LPCTSTR pcTitle,
                     const DWORD dwSessionId = 0) : m_dwSessionId(dwSessionId),
                                                    m_strTitle(pcTitle)
    {
    }
    /** default destructor **/
    ~CVirusFoundProxy()
    {
    }

    CVirusFoundProxy& operator=(const CVirusFoundProxy& objRhs);
    CVirusFoundProxy(const CVirusFoundProxy& objRhs);

    DWORD CreateDlg();
    DWORD AddMessage(LPCTSTR pcLogline, LPCTSTR pcDescription);

private:
    const DWORD				  m_dwSessionId;
	std::string				  m_strTitle;
    IClassFactoryPtr		  m_IClassFactoryPtr;
    IVirusFoundCOMAdapterPtr  m_pCOMAdapter;
    IVirusFoundCOMCallbackPtr m_pCOMCallback;
};
