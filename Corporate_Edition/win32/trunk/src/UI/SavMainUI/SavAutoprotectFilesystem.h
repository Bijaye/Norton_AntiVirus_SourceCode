// SavAutoprotectFilesystem.h : Declaration of the CSavAutoprotectFilesystem
#pragma once
#include "resource.h"       // main symbols
#include "SavMainUI.h"
#include "ProtectionProvider.h"
#include "TrustUtil.h"


// CSavAutoprotectFilesystem
class ATL_NO_VTABLE CSavAutoprotectFilesystem :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSavAutoprotectFilesystem, &CLSID_SavAutoprotectFilesystem>,
    public IDispatchImpl<IProtection, &__uuidof(IProtection), &LIBID_ProtectionProviderLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
    // Install COM Registration
    DECLARE_REGISTRY_RESOURCEID(IDR_SAVAUTOPROTECTFILESYSTEM)

    // ATL support
    BEGIN_COM_MAP(CSavAutoprotectFilesystem)
        COM_INTERFACE_ENTRY(IProtection)
        COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()

    // Constructor-destructor
    CSavAutoprotectFilesystem();
	DECLARE_PROTECT_FINAL_CONSTRUCT();
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

private:
    CTrustVerifier                                  trustVerifier;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(SavAutoprotectFilesystem), CSavAutoprotectFilesystem)
