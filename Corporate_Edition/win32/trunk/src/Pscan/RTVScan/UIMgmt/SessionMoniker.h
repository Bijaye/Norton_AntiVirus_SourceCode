// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.


#pragma once


#include "windows.h"
#include <string>

/** class CSessionMoniker
  * 
  * Class is used to bind to a COM server in a specific session
  * This causes the launching of a process in another session
  **/
class CSessionMoniker
{
private:
    /** Formating for the moniker string **/
    static const OLECHAR* s_olechMonikerFormat;

public:
    /** default constructor and destructor **/
    CSessionMoniker(const DWORD dwSessionID);
    virtual ~CSessionMoniker(void);

    /** CreateMoniker(const CLSID CLSIDobjToInstantiate);
      * 
      * creates the session moniker for the session specified by m_dwSessionId
      *
      **/
    DWORD CreateMoniker(const CLSID CLSIDobjToInstantiate);

    /** DWORD BindToObject(void** objFactory)
      *
      * Finds the object and puts it into a running state
      *
      **/
    DWORD BindToObject(void** objFactory);

protected:
    const CSessionMoniker(const CSessionMoniker&);
    void operator =(const CSessionMoniker&);
protected:

    /** DWORD FormatStringMoniker(const CLSID CLSIDobjToInstantiate)
      * 
      * formats the moniker string to be used by create moniker for
      * creating the correct session
      *
      **/
    DWORD FormatStringMoniker(const CLSID CLSIDobjToInstantiate);

private:
    IMoniker*       m_pMoniker;
    IBindCtx*       m_pBindCtx;
    OLECHAR         m_olechMoniker[128];
    const DWORD     m_dwSessionID;
};
