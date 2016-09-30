// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

#include "stdafx.h"
#include <comdef.h>


#pragma once

/**
 * CGITManager manages the GlobalInterfaceTable com class
 *
 * this class allows you to register interfaces and retrieve the to be used on different
 * thread apartments
 *
**/
class CGITManager
{
private:
    _COM_SMARTPTR_TYPEDEF(IGlobalInterfaceTable, __uuidof(IGlobalInterfaceTable));
public:
    ~CGITManager(void);

    /**
     * static CGITManager& GetInstance()const;
     *
     * Gets the singleton object
     *
    **/
    static CGITManager& GetInstance();

    /**
     * DWORD RegisterInterface(IUknown* pUnk, const REFIID riid, DWORD& dwCookie);
     *
     * Registers an interface with the global interface table to allow for the 
     * use of the object in other apartments. The cookie returned is used to
     * retrieve the GIT later
     *
     **/
    DWORD RegisterInterface(IUnknown* pUnk, REFIID riid, DWORD& dwCookie);

    /**
     * DWORD RevokeInterface(const DWORD dwCookie);
     *
     * RevokeInterface removes an interface from the GIT
     *
    **/
    DWORD RevokeInterface(const DWORD dwCookie);

    /**
     * DWORD GetInterface(const DWORD dwCookie, const REFIID riid,void** pInterface);
     *
     * Get an interface stored in the GIT. 
     *
    **/
    DWORD GetInterface(const DWORD dwCookie, REFIID riid, void** pInterface);

private:
    CGITManager(const CGITManager&);
    CGITManager& operator=(const CGITManager&);
    CGITManager();

private:
    static CGITManager        s_objGITManager;
    IGlobalInterfaceTablePtr  m_pGITptr;
};
