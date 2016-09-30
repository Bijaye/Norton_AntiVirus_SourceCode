// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// SavProduct.h
//
// Declaration of CSavProductPlugin -- ccApp product plug-in for SAV Corporate Edition.
// Responsible for loading our functionality plug-in's (at this time, ccEmailProxy).
//***************************************************************************

#ifndef __SAV_PRODUCT_H
#define __SAV_PRODUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSavProductPlugin :  
    public ICCAppProductPlugin,
    public ISymBaseImpl< CSymThreadSafeRefCount >
{
public:
    CSavProductPlugin();
    virtual ~CSavProductPlugin();

    SYM_INTERFACE_MAP_BEGIN()                
        SYM_INTERFACE_ENTRY( IID_CCAppProductPlugin, ICCAppProductPlugin ) 
    SYM_INTERFACE_MAP_END()                  

    //
    // ICCAppProductPlugin methods
    //
    virtual SYMRESULT VerifyProduct();
    virtual unsigned int GetPluginDLLCount();
    virtual const char * GetPluginDLLPath( unsigned int uIndex );
    virtual const char * GetProductName();

protected:
    bool buildPluginList();

private:
    // The list of plugins
    vector<string>  m_vPlugins;
    
    string  m_strPath;
    string  m_strCCPath;

    // Static arrays of plugin dll names.

    static  const char *    m_sSAVPluginDllNames[];
    static  const char *    m_sCommonPluginDllNames[];
};

#endif // __SAV_PRODUCT_H
