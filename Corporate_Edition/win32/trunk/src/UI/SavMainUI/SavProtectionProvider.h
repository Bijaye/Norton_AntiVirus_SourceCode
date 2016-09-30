// SavProtectionProvider.h : Declaration of the CSavProtectionProvider

#pragma once
#include "resource.h"       // main symbols
#define PROTECTIONPROVIDER_HELPERTYPES_NONAMESPACE
#include "ProtectionProvider.h"
#include "SavMainUI.h"
#include "TrustUtil.h"

// CSavProtectionProvider
class ATL_NO_VTABLE CSavProtectionProvider :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CSavProtectionProvider, &CLSID_SavProtectionProvider>,
    public ISupportErrorInfo,
    public IDispatchImpl<ISavProtectionProvider, &IID_ISavProtectionProvider, &LIBID_SavMainUILib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public IDispatchImpl<IProtection, &__uuidof(IProtection), &LIBID_ProtectionProviderLib, /* wMajor = */ 1, /* wMinor = */ 0>,
    public IDispatchImpl<IProtection_Container, &__uuidof(IProtection_Container), &LIBID_ProtectionProviderLib, /* wMajor = */ 1, /* wMinor = */ 0>,
    public IDispatchImpl<IProtection_DefinitionInfo, &__uuidof(IProtection_DefinitionInfo), &LIBID_ProtectionProviderLib, /* wMajor = */ 1, /* wMinor = */ 0>,
    public IDispatchImpl<IProtection_Provider, &__uuidof(IProtection_Provider), &LIBID_ProtectionProviderLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
    // Install COM Registration
    DECLARE_REGISTRY_RESOURCEID(IDR_SAVPROTECTIONPROVIDER)
    BEGIN_CATEGORY_MAP(CSavProtectionProvider)
        IMPLEMENTED_CATEGORY(CATID_ProtectionProvider)
    END_CATEGORY_MAP()
    
    // ATL support
    BEGIN_COM_MAP(CSavProtectionProvider)
        COM_INTERFACE_ENTRY(ISavProtectionProvider)
        COM_INTERFACE_ENTRY2(IDispatch, IProtection)
        COM_INTERFACE_ENTRY(ISupportErrorInfo)
        COM_INTERFACE_ENTRY(IProtection)
        COM_INTERFACE_ENTRY(IProtection_Container)
        COM_INTERFACE_ENTRY(IProtection_DefinitionInfo)
        COM_INTERFACE_ENTRY(IProtection_Provider)
    END_COM_MAP()

    // Constructor-destructor
    CSavProtectionProvider();
    DECLARE_PROTECT_FINAL_CONSTRUCT()
    HRESULT FinalConstruct();
    void FinalRelease();

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    // IProtection_Provider Methods
    STDMETHOD(get_ID)( GUID* id );
    STDMETHOD(get_SplashGraphic)( HGDIOBJ* bitmapHandle );

    STDMETHOD(get_Installed)( VARIANT_BOOL* isInstalled );
    STDMETHOD(get_Autoprotects)( IProtection_Container** autoprotects );
    STDMETHOD(get_PrimaryAutoprotect)( IProtection** primaryAutoprotect );
    STDMETHOD(get_ConfigureableScans)( IProtection_ConfigureableScan_Container** scans );
    STDMETHOD(get_AdministrativeExceptions)( IExceptionItem_Container** administrativeExceptions );
    STDMETHOD(get_LocalExceptions)( IExceptionItem_Container** localExceptions );
    STDMETHOD(put_LocalExceptions)( IExceptionItem_Container* newLocalExceptions );
        
    // IProtection Methods
    STDMETHOD(get_DisplayName)( BSTR* shortDisplayName );
    STDMETHOD(get_Description)( BSTR* userDescription );
    STDMETHOD(get_ProtectionStatus)( ProtectionStatus* currStatus );
    STDMETHOD(get_ProtectionStatusDescription)( BSTR* statusDescription );
    STDMETHOD(get_ProtectionStatusReadOnly)( VARIANT_BOOL* isReadOnly );
    STDMETHOD(get_ProtectionConfiguration)( VARIANT_BOOL* protectionEnabled );
    STDMETHOD(put_ProtectionConfiguration)( VARIANT_BOOL newStatus );
    STDMETHOD(ShowConfigureUI)( HWND parentWindowHandle );
    STDMETHOD(ShowLogUI)( HWND parentWindowHandle );

    // IProtection_DefinitionInfo Methods
    STDMETHOD(get_Date)( SYSTEMTIME* defsDate );
    STDMETHOD(get_RevisionNo)( unsigned int* revisionNo );
    STDMETHOD(get_ShortDescription)( BSTR* shortDescription );

    // IProtection_Container Methods (for autoprotects)
    STDMETHOD(get_ProtectionCount)( long* noItems );
    STDMETHOD(get_Item)( GUID itemID, IProtection** autoprotect );
    STDMETHOD(get__NewEnum)( IEnumProtection** enumerator );

    // ISavProtectionProvider
    STDMETHOD(RunQuickScan)( HWND parentWindow, DWORD* savErrorCode );
    STDMETHOD(RunFullScan)( HWND parentWindow, DWORD* savErrorCode );

private:
    // ** DATA MEMBERS **
    CTrustVerifier                                  trustVerifier;
    ProtectionList                                  autoprotects;           // List of all autoprotects supported by this protection technology
    CComPtr<IProtection>                            apFilesystem;
    CComPtr<IProtection>                            apInternetEmail;
    CComPtr<IProtection>                            apNotes;
    CComPtr<IProtection>                            apExchange;
};

OBJECT_ENTRY_AUTO(__uuidof(SavProtectionProvider), CSavProtectionProvider)