#pragma once

class CHPPUtilityImpl : 
    public HPP::IHPPUtilityInterface,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    CHPPUtilityImpl(void);
    virtual ~CHPPUtilityImpl(void);

    // Interface map.
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(HPP::IID_HPPUtilityInterface, HPP::IHPPUtilityInterface)
    SYM_INTERFACE_MAP_END()                 

    virtual HRESULT SetHomePage (LPCSTR szNewHomePage, BOOL bCurrentUser);
    virtual HRESULT ResetOptions (void);
    virtual HRESULT PrepHPPForUninstall (void);

};
