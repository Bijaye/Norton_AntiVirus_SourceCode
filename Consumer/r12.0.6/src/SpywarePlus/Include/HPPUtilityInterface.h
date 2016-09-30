#pragma once

namespace HPP
{

//
// Home Page Protection Utility Class ID
//
// {59A2775B-BDE7-484f-AA7A-9F94347A5FE3}
SYM_DEFINE_INTERFACE_ID(CLSID_HPPUtility, 
            0x59a2775b, 0xbde7, 0x484f, 0xaa, 0x7a, 0x9f, 0x94, 0x34, 0x7a, 0x5f, 0xe3);


class IHPPUtilityInterface : public ISymBase
{
public:
    virtual HRESULT SetHomePage (LPCSTR szNewHomePage, BOOL bCurrentUser) = 0;
    virtual HRESULT ResetOptions (void) = 0;
    virtual HRESULT PrepHPPForUninstall (void) = 0;

};

// {593A9EAB-80F3-42b8-B1FB-0EE96E2E7720}
SYM_DEFINE_INTERFACE_ID(IID_HPPUtilityInterface, 
                        0x593a9eab, 0x80f3, 0x42b8, 0xb1, 0xfb, 0xe, 0xe9, 0x6e, 0x2e, 0x77, 0x20);

typedef CSymPtr<IHPPUtilityInterface> IHPPUtilityInterfacePtr;
typedef CSymQIPtr<IHPPUtilityInterface, &IID_HPPUtilityInterface> IHPPUtilityInterfaceQIPtr;

} // end namespace HPP