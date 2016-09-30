#ifndef _LISTNAVSERVER_H_
#define _LISTNAVSERVER_H_

#include "nsctop.h"
struct SServerData : public SERVERDATA
{
    SServerData()
    {
        _init();
    }

    void _init()
    {
        pi = 0;
        fPrimary = 0;
        iType = 0;
        iProtocol = 0;
        flags = 0;
        dwStatusFlags = 0;
        bstrName = NULL;
        bstrDomain = NULL;
        bstrAddress0 = NULL;
        bstrAddress1 = NULL;
        bstrNOS = NULL;
        bstrParent = NULL;
    }

    SServerData& operator=( const SServerData& rhs )
    {
        if (this == &rhs)
            return *this;

        _clear();
        _copy( &rhs );
        return *this;
    }

    SServerData& operator=( const SERVERDATA& rhs )
    {
        _clear();
        _copy( &rhs );
        return *this;
    }

    void _copy( const SERVERDATA *prhs )
    {
        bstrName = ::SysAllocString( prhs->bstrName );
        bstrDomain = ::SysAllocString( prhs->bstrDomain );
        bstrAddress0 = ::SysAllocString( prhs->bstrAddress0 );
        bstrAddress1 = ::SysAllocString( prhs->bstrAddress1 );
        bstrNOS = ::SysAllocString( prhs->bstrNOS );
        bstrParent = ::SysAllocString( prhs->bstrParent );

        iType = prhs->iType;
        iProtocol = prhs->iProtocol;
        fPrimary = prhs->fPrimary;
        flags = prhs->flags;
        dwStatusFlags = prhs->dwStatusFlags;

        pi = prhs->pi;
        pi->AddRef();
    }

    void _clear()
    {
        ::SysFreeString( bstrName );
        ::SysFreeString( bstrDomain );
        ::SysFreeString( bstrAddress0 );
        ::SysFreeString( bstrAddress1 );
        ::SysFreeString( bstrNOS );
        ::SysFreeString( bstrParent );
        if (pi)
            pi->Release();
        _init();
    }

    ~SServerData()
    {
        _clear();
    }
};

HRESULT ListNavServers(SServerData **pSServerData, long *pdwNoRecords);

#endif