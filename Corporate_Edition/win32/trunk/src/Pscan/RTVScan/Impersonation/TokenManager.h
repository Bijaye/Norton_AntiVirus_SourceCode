// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

#include <map>
#include "accesstoken.h"
#include "guard.h"

#pragma once

/**
 * class CTokenManager
 *
 * purpose: Singleton class that manages access token for RTVScan impersonation
 *
**/
class CTokenManager
{
public:
    ~CTokenManager(){}

    /**
     * static CTokenManager& GetInstance(void);
     *
     * gets the singleton instance of the class
     *
    **/
    static CTokenManager& GetInstance(void);

    /**
     * DWORD AddToken(const DWORD dwKey, CAccessToken& objToken);
     * 
     * adds a token to the token manager
     *
     * @return ERROR_SUCCESS on success
     * @return windows error code on failure
     *
    **/
    DWORD AddToken(const DWORD dwKey, CAccessToken& objToken);

    /**
     * DWORD GetToken(const DWORD dwKey, CAccessToken& objToken);
     *
     * gets an access token from the map
     * @return ERROR_SUCCESS if there was a token and it was successfully returned
     * 
    **/
    DWORD GetToken(const DWORD dwKey, CAccessToken& objToken);

    /**
     * DWORD RemoveAPTokens(const DWORD dwSessionId)
     *
     * removes all AP tokens from a specific session from the map
     *
     * @return the number of Tokens removed
     *
    **/
    DWORD RemoveAPTokens(const DWORD dwSessionId);

    /**
     * DWORD RemoveOnDemandTokens(const DWORD dwSessionId)
     *
     * removes all On Demand tokens from a specific session from the map
     *
     * @return the number of Tokens removed
     *
    **/
	DWORD RemoveOnDemandTokens(const DWORD dwScanId);

protected:
    CTokenManager(){};
    CTokenManager(const CTokenManager&);
    void operator=(const CTokenManager&);

private:
    static CTokenManager            g_objTokenManager;
    std::map<DWORD,CAccessToken>    m_TokenMap;
    CLock                           m_objLock;
};