#ifndef SAV_EMAILAPIMPL
#define SAV_EMAILAPIMPL

// CSavEmailAutoprotectImpl.cpp : Implementation of CCSavEmailAutoprotectImpl
#include "stdafx.h"
#include "ProtectionProvider.h"
#include "TrustUtil.h"


// Implements one of SAV's e-mail autoprotects.  Their implementations are practically identical with only
// changes in data values, so I've factored it out into a single common class that the COM objects can
// then instantiate and use.  Inerhitance by the AP component is not an option due to ATL_NO_VTABLE, and
// implementation as a template doesn't work well due to template type limitations.
// To use:
// 1.  Declare a CSavEmailAutoprotectImpl member variable of your autoprotect:
//          CSavEmailAutoprotectImp                     apImplementation;
// 2.  Declare a Config static member of your autoprotect (static not required but recommended):
//          static CSavEmailAutoprotectImpl::Config     config;
// 3.  Initialize the static config object
//          CSavEmailAutoprotectImpl::Config CSavAutoprotectExchange::config = 
//          {
//              CLSID_SavAutoprotectExchange,
//              0,
//              0,
//              _T("\\vpmsece4.dll"),
//              _T(szReg_Key_Storage_Exchange)
//          };
// 4.  Call the FinalConstruct member and pass in the config object before making any other calls (e.g., in your FinalConstruct)
//          apImplementation.FinalConstruct(config);
// 5.  In your ShowConfigureUI/ShowLogUI members, declare your CPropertyPage-derived class and pass that into the
//     corresponding call:
//          CSavEmailAutoprotectImpl call.
//          AFX_MANAGE_STATE(AfxGetStaticModuleState());
//          CExchangeApPage propertyPage;
//
//          return apImplementation.ShowConfigureUI(&propertyPage, parentWindowHandle);
class CSavEmailAutoprotectImpl
{
public:
    // Configuration information for this instance of CSavEmailAutoprotectImpl
    struct Config
    {
        CLSID           componentCLSID;
        UINT            displayNameID;
        UINT            descriptionID;
        LPCTSTR         storageFilename;
        LPCTSTR         storageName;
        CPropertyPage*  propertyPage;
    };

    // Initialize this object for use.  Do not call any other members until making this call.
    HRESULT FinalConstruct( Config newConfig );
    void FinalRelease( void );

    // IProtection
    HRESULT get_ID( GUID* id );
    HRESULT get_DisplayName( BSTR* shortName );
    HRESULT get_Description( BSTR* userDescription );
    HRESULT get_ProtectionStatus( ProtectionStatus* currStatus );
    HRESULT get_ProtectionStatusDescription( BSTR* statusDescription );
    HRESULT get_ProtectionStatusReadOnly( VARIANT_BOOL* isStatusReadOnly );
    HRESULT get_ProtectionConfiguration( VARIANT_BOOL* protectionEnabled );
    HRESULT put_ProtectionConfiguration( VARIANT_BOOL newStatus );
    HRESULT ShowConfigureUI( CPropertyPage* propertyPage, HWND parentWindowHandle );
    HRESULT ShowLogUI( CPropertyPage* propertyPage, HWND parentWindowHandle );

    // Returns S_OK if storage is installed, S_FALSE if not
    static bool IsStorageInstalled( LPCTSTR keyFilename, LPCTSTR storageName );
protected:    
    CTrustVerifier                  trustVerifier;
    Config                          config;
};

#endif // SAV_EMAILAPIMPL