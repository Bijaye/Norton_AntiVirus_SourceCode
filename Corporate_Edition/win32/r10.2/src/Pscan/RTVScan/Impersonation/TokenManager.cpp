// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.
#include "savassert.h"
#include "tokenmanager.h"
#include "dprintf.h"


CTokenManager CTokenManager::g_objTokenManager;


CTokenManager& CTokenManager::GetInstance(void)
{
    return g_objTokenManager;
}

/**
 * DWORD AddToken(const DWORD dwKey, CAccessToken& objToken);
 * 
 * adds a token to the token manager
 *
 * @return ERROR_SUCCESS on success
 * @return windows error code on failure
 *
**/
DWORD CTokenManager::AddToken(const DWORD dwKey, CAccessToken& objToken)
{
    CGuard objGuard(m_objLock);
    if( CAccessToken::TOKEN_INVALID == objToken.GetTokenType() )
    {
        SAVASSERT(FALSE);
        return ERROR_INVALID_PARAMETER;
    }
    
    dprintfTag1(DEBUGIMPERSONATION, "Token Manager: Adding Tokens; Total token Count(%d)\n", m_TokenMap.size());

    //return an error
    DWORD dwRet = ERROR_INVALID_PARAMETER;
    if( m_TokenMap.find(dwKey) == m_TokenMap.end() )
    {
        //add the key to the map
        try
        {
            m_TokenMap[dwKey] = objToken;
            dwRet = ERROR_SUCCESS;
            dprintfTag3(DEBUGIMPERSONATION, "Token Manager: Added token for user(%s):session(%d):key(%d)\n", objToken.GetUserName(), objToken.GetSessionId(), dwKey);
        }
        catch(std::bad_alloc&)
        {
            dwRet = ERROR_NOT_ENOUGH_MEMORY;
            dprintfTag3(DEBUGIMPERSONATION, "Token Manager: Failed to add token for user(%s):session(%d):key(%d)\n", objToken.GetUserName(), objToken.GetSessionId(), dwKey);
        }
    }
    else
    {
        dprintfTag3(DEBUGIMPERSONATION, "Token Manager: Failed to add token for user(%s):session(%d):key(%d)\n", objToken.GetUserName(), objToken.GetSessionId(), dwKey);
        SAVASSERT(0);
    }

    return dwRet;
}

/**
 * DWORD GetToken(const DWORD dwKey, CAccessToken& objToken);
 *
 * gets an access token from the map
 * @return ERROR_SUCCESS if there was a token and it was successfully returned
 * 
**/
DWORD CTokenManager::GetToken(const DWORD dwKey, CAccessToken& objToken)
{
    CGuard objGuard(m_objLock);
    //return an error
    DWORD dwRet = ERROR_INVALID_PARAMETER;
    if( m_TokenMap.find(dwKey) != m_TokenMap.end() )
    {
        objToken = m_TokenMap[dwKey];
        dprintfTag3(DEBUGIMPERSONATION, "Token Manager: Getting token for user(%s):session(%d):key(%d)\n", objToken.GetUserName(), objToken.GetSessionId(), dwKey);
        dwRet = ERROR_SUCCESS;
    }
    else
    {
        dprintfTag1(DEBUGIMPERSONATION, "Token Manager: Failed to get token for key(%d)\n", dwKey);
		SAVASSERT(!"Token Manager failed to get a token.  This is ok if acting from risk/scan history or manual add to quarantine.");
    }

    return dwRet;
}

/**
 * DWORD RemoveAPTokens(const DWORD dwSessionId)
 *
 * removes all AP tokens from a specific session from the map
 *
 * @return the number of Tokens removed
 *
**/
DWORD CTokenManager::RemoveAPTokens(const DWORD dwSessionId)
{
    CGuard objGuard(m_objLock);
    dprintfTag2(DEBUGIMPERSONATION, "Token Manager: Removing AP access tokens for Session Id(%d), Current token count(%d)\n", dwSessionId, m_TokenMap.size());
    DWORD dwCount = 0; //number of items removed
    std::map<DWORD,CAccessToken>::iterator iter = m_TokenMap.begin();
    while( iter != m_TokenMap.end() )
    {
        DWORD dwIndex = (*iter).first;
        if( (*iter).second.GetTokenType() == CAccessToken::TOKEN_AUTO_PROTECT &&
            (*iter).second.GetSessionId() == dwSessionId )
        {
            dprintfTag3(DEBUGIMPERSONATION, "Token Manager: Removing token for scan id(%d), for session(%d), for user(%s)\n",
                                            (*iter).first, dwSessionId, (*iter).second.GetUserName());
            m_TokenMap.erase(dwIndex);
            dwCount++;
        }
        iter = m_TokenMap.upper_bound(dwIndex);
    }

    dprintfTag2(DEBUGIMPERSONATION, "Token Manager: Total token Count(%d), Tokens Removed(%d)\n", m_TokenMap.size(), dwCount);
    return dwCount;
}

/**
 * DWORD RemoveOnDemandTokens(const DWORD dwSessionId)
 *
 * removes all On Demand tokens from a specific session from the map
 *
 * @return the number of Tokens removed
 *
**/
DWORD CTokenManager::RemoveOnDemandTokens(const DWORD dwScanId)
{
    CGuard objGuard(m_objLock);
	// TODO: Do we need to do this for the manual scan case?
	// Simplify if we can conclude that the token map can ever only have 1 item.
    dprintfTag2(DEBUGIMPERSONATION, "Token Manager: Removing On demand access tokens for Scan Id(%d), Current token count(%d)\n", dwScanId, m_TokenMap.size());
    DWORD dwCount = 0; //number of items removed

    std::map<DWORD,CAccessToken>::iterator iter = m_TokenMap.find(dwScanId);
    if( iter != m_TokenMap.end() )
    {
        dprintfTag4(DEBUGIMPERSONATION, "Token Manager: Removing token for scan id(%d), Type(%d), Session(%d), for user(%s)\n", (*iter).first, (*iter).second.GetTokenType(), (*iter).second.GetSessionId(), (*iter).second.GetUserName());
        
        m_TokenMap.erase(iter);
    }
    else
        dprintfTag1(DEBUGIMPERSONATION, "Token Manager: Failed to remove token for scan id(%d)\n",
                                        dwScanId);

    dprintfTag2(DEBUGIMPERSONATION, "Token Manager: Total token Count(%d), Tokens Removed(%d)\n", m_TokenMap.size(), dwCount);
    return dwCount;
}