// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation, All rights reserved.

#pragma once

#include <windows.h>
#include <string>
#include <map>
#include "IWtsSessionListener.h"
#include "wtssessionevent.h"
#include "accesstoken.h"
#include "guard.h"

class CStorageListener: public IWtsSessionListener
{
    class CSessionInfo
    {
    public:
        CSessionInfo():m_bActive(false){}
        ~CSessionInfo(){}
        CSessionInfo(CAccessToken& objToken, const bool bActive):m_objToken(objToken), m_bActive(bActive){};
        CSessionInfo(const CSessionInfo& objRhs){ *this = objRhs; }
        CSessionInfo& operator=(const CSessionInfo& objRhs)
        { 
            m_bActive = objRhs.m_bActive;
            m_objToken = objRhs.m_objToken;
            return *this;
        }

        void SetActive(){ m_bActive = true; }
        void SetInActive(){ m_bActive = false; }

        const char* GetUserName(void)const{ return m_objToken.GetUserName(); }
        HANDLE GetToken(void)const{ return m_objToken; }

    private:
        CAccessToken    m_objToken;
        bool            m_bActive; //is this session active
    };


public:
    static CStorageListener& GetInstance();
    ~CStorageListener(void);

    //session change notification
    virtual void SessionChange(const CWtsSessionEvent& objEvent);

    /** GetUserName from active session **/
    const char* GetUserName()const{ return m_strUserName.c_str(); }

    /** returns false if the user is in the process of logging out **/
    bool GetIsUserConnected()const{ return m_bUserConnected; }

    /** returns if a user is logged in.  This is different from GetIsUserConnected.
    Users can be logged on without being connected. **/
    bool GetIsUserLoggedIn();

    /** Gets the access token from the class 
     *
     * @returns If the user is logged in the access token
     * @returns NULL if the user is logged out
    **/
    HANDLE GetToken(void);
    
protected:
    CStorageListener(void);
    CStorageListener(const CStorageListener&);
    void operator=(const CStorageListener&);

    /** void ProcessLogon(const CWtsSessionEvent& objEvent) **/
    void ProcessLogon(const CWtsSessionEvent& objEvent);

    /** void ProcessLogout() **/
    void ProcessLogout(const DWORD dwSessionId);

    /** Set no active logged on user **/
    void SetNoActiveUser();

    /** Sets the active session **/
    void SetSessionActive(const DWORD dwSessionId);

    /** Sets the session inactive **/
    void SetSessionInActive(const DWORD dwSessionId);

    /** returns the number of logged in users **/
    DWORD GetLoggedInUserCount();

private:
    static CStorageListener         g_objStorageListenerInstance;   // Guarded by m_objLock
    std::map<DWORD,CSessionInfo>    m_mapSessionInfo;               // Guarded by m_mapLock
    std::string                     m_strUserName;
    DWORD                           m_dwActiveSessionId;// Key to currently active user session. Session will not be listed as active if no user logged in.
    bool                            m_bUserConnected;
    bool                            m_bInitialized;
    CLock                           m_objLock;          // Access to g_objStorageListenerInstance
    CLock                           m_mapLock;          // Access to m_mapSessionInfo
};
