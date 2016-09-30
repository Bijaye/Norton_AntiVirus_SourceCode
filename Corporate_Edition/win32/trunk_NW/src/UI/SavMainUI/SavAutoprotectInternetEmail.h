// SavAutoprotectInternetEmail.h : Declaration of the CSavAutoprotectInternetEmail
#pragma once
#include "resource.h"       // main symbols
#include "SavMainUI.h"
#include "TrustUtil.h"
#include "ProtectionProvider.h"
#include "SavEmailAutoprotectImpl.h"


// CSavAutoprotectInternetEmail
class ATL_NO_VTABLE CSavAutoprotectInternetEmail :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CSavAutoprotectInternetEmail, &CLSID_SavAutoprotectInternetEmail>,
    public IDispatchImpl<IProtection, &__uuidof(IProtection), &LIBID_ProtectionProviderLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
    // Install COM Registration
    DECLARE_REGISTRY_RESOURCEID(IDR_SAVAUTOPROTECTINTERNETEMAIL)

    // ATL support
    BEGIN_COM_MAP(CSavAutoprotectInternetEmail)
        COM_INTERFACE_ENTRY(IProtection)
        COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()

    // Constructor-destructor
    DECLARE_PROTECT_FINAL_CONSTRUCT()
    HRESULT FinalConstruct();
    void FinalRelease();

    // IProtection
    STDMETHOD(get_ID)( GUID* id );
    STDMETHOD(get_DisplayName)( BSTR* shortName );
    STDMETHOD(get_Description)( BSTR* userDescription );
    STDMETHOD(get_ProtectionStatus)( ProtectionStatus* currStatus );
    STDMETHOD(get_ProtectionStatusDescription)( BSTR* statusDescription );
    STDMETHOD(get_ProtectionStatusReadOnly)( VARIANT_BOOL* isStatusReadOnly );
    STDMETHOD(get_ProtectionConfiguration)( VARIANT_BOOL* protectionEnabled );
    STDMETHOD(put_ProtectionConfiguration)( VARIANT_BOOL newStatus );
    STDMETHOD(ShowConfigureUI)( HWND parentWindowHandle );
    STDMETHOD(ShowLogUI)( HWND parentWindowHandle );

    // Returns TRUE if Notes protection is installed, else FALSE
    static bool IsStorageInstalled( void );
private:
    CSavEmailAutoprotectImpl                    apImplementation;
    static CSavEmailAutoprotectImpl::Config     config;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(SavAutoprotectInternetEmail), CSavAutoprotectInternetEmail)
