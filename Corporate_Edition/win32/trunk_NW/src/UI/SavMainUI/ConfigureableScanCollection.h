// ConfigureableScanCollection.h : Declaration of the CConfigureableScanCollection

#pragma once
#include "resource.h"       // main symbols
#define PROTECTIONPROVIDER_HELPERTYPES_NONAMESPACE
#include "ProtectionProvider.h"
#include "SavMainUI.h"
#include "Util.h"
#include "TrustUtil.h"
#include "SavConfigureableScan.h"

typedef std::list<IProtection_ConfigureableScan*> ConfigureableScanList;
typedef CopyInterfaceFixed<IProtection_ConfigureableScan> ConfigureableScanCopyPolicy;
typedef CComEnumOnSTL< IEnumProtection_ConfigureableScan, &IID_IEnumProtection_ConfigureableScan, IProtection_ConfigureableScan*, ConfigureableScanCopyPolicy, ConfigureableScanList >          CComEnumConfigureableScanOnList;
typedef ICollectionOnSTLImpl< IProtection_ConfigureableScan_Container, ConfigureableScanList, IProtection_ConfigureableScan*, ConfigureableScanCopyPolicy, CComEnumConfigureableScanOnList >    CCollectionOnSTLConfigureableScanList;

// CConfigureableScanCollection
class ATL_NO_VTABLE CConfigureableScanCollection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CConfigureableScanCollection, &CLSID_ConfigureableScanCollection>,
	public IDispatchImpl<CCollectionOnSTLConfigureableScanList, &IID_IProtection_ConfigureableScan_Container, &LIBID_ProtectionProviderLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
    // Install COM Registration
    DECLARE_REGISTRY_RESOURCEID(IDR_CONFIGUREABLESCANCOLLECTION)

    // ATL support
    DECLARE_NOT_AGGREGATABLE(CConfigureableScanCollection)
    BEGIN_COM_MAP(CConfigureableScanCollection)
	    COM_INTERFACE_ENTRY(IProtection_ConfigureableScan_Container)
	    COM_INTERFACE_ENTRY2(IDispatch, IProtection_ConfigureableScan_Container)
    END_COM_MAP()

    // IProtection_ConfigureableScan_Container Methods not already implemented by ICollectionOnSTLImpl
    STDMETHOD(get_Item)( GUID id, IProtection_ConfigureableScan** scan );
    // Why isn't ICollectionOnSTLImpl implementing this member?
    STDMETHOD(get__NewEnum)( IEnumProtection_ConfigureableScan** enumerator );
    // Deletes an existing scan from the collection
    STDMETHOD(Delete)( GUID id );
    // Returns S_OK on success, S_FALSE if no user cancelled, else the error code of the failure
    STDMETHOD(ShowAddWizard)( HWND parentWindowHandle, IProtection_ConfigureableScan** newScan );

    // Private APIs
    // Add all current user scans to the collection
    HRESULT AddCurrentUserScans( void );
    // Add all administrator-defined (local machine) scans to the collection
    HRESULT AddAdminScans( void );
    // Adds the scans from the specified parent key
    HRESULT AddScansFromKey( HKEY hiveHandle, LPCTSTR keyPath, bool isReadOnly );

    // Constructor-destructor
    CConfigureableScanCollection();
	DECLARE_PROTECT_FINAL_CONSTRUCT()
    HRESULT FinalConstruct();
    void FinalRelease();
private:
    CTrustVerifier                                  trustVerifier;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(ConfigureableScanCollection), CConfigureableScanCollection)
