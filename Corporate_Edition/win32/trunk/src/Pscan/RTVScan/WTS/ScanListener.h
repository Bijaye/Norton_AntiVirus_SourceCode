// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation, All rights reserved.

#pragma once

#include <windows.h>
#include "IWtsSessionListener.h"
#include "wtssessionevent.h"
#include "guard.h"

class CScanListener: public IWtsSessionListener
{
public:
    static CScanListener& GetInstance();
    ~CScanListener(void);

    //session change notification
    virtual void SessionChange(const CWtsSessionEvent& objEvent);

    
    /** Gets the access token from the class 
     *
     * @returns If the user is logged in the access token
     * @returns NULL if the user is logged out
    **/
    HANDLE GetToken(void);
    
protected:
    CScanListener(void);
    CScanListener(const CScanListener&);
    void operator=(const CScanListener&);

    /** void ProcessLogon() **/
    void ProcessLogon();

private:
    static CScanListener            g_objScanListenerInstance;
    bool                            m_bInitialized;
    CLock                           m_objLock;
};
