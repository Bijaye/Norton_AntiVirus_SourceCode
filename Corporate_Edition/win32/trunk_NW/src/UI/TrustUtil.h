#ifndef PROTECTIONPROVIDER_TRUSTUTIL
#define PROTECTIONPROVIDER_TRUSTUTIL

#include "stdafx.h"
#include "util.h"
#include "ccVerifyTrustTypes.h"
#include "ccVerifyTrustLoader.h"
#pragma warning( push )
#pragma warning( disable : 4115 )
#include "wincrypt.h"
#include "wintrust.h"
#include "softpub.h"
#pragma warning( pop )
#include <list>
#include <comcat.h>


// To disable trust checking, #define PROTECTIONPROVIDER_TRUSTUTIL_DEBUGNOCHECK in your app.
class CTrustVerifier
{
public:
    // ** DATA TYPES **
    typedef enum {VerifyMode_Direct, VerifyMode_CommonClient} VerifyMode;

    // Constructor-destructor
    CTrustVerifier();
    virtual ~CTrustVerifier();
    
    // Startup-shutdown
    HRESULT Initialize( VerifyMode newMode );
    HRESULT Shutdown( void );

    // Returns S_OK if filename is trusted, S_FALSE if not, else the error code of the failure
    HRESULT IsFileTrusted( LPCTSTR filename );
    // Returns S_OK if the calling process is trusted, S_FALSE if not, else the error code of the failure
    HRESULT IsCallerProcessTrusted( void );
    // Checks the DLLs backing the InProc32 objects of all objects identified by CLSID in *componentList.  If the files have valid
    // Symantec digital signatures they are left in the list, otherwise they are filtered out of it.
    // Returns S_OK on success, else the error code of the failure
    HRESULT TrustVerifyComponentList( CLSIDList* componentList );
private:
    // ** DATA TYPES **
    typedef HRESULT (WINAPI *WinVerifyTrustFP)(HWND, GUID*, WINTRUST_DATA*);
    typedef CRYPT_PROVIDER_DATA *(WINAPI *WTHelperProvDataFromStateDataFP)(HANDLE) ;
    typedef CRYPT_PROVIDER_SGNR *(WINAPI *WTHelperGetProvSignerFromChainFP)(HANDLE, DWORD, BOOL, DWORD);
    typedef CRYPT_PROVIDER_CERT *(WINAPI *WTHelperGetProvCertFromChainFP)(CRYPT_PROVIDER_SGNR *pSgnr, DWORD idxCert);
    typedef DWORD (WINAPI *CertGetNameStringAFP)(PCCERT_CONTEXT, DWORD, DWORD, void *, LPWSTR, DWORD);
    
    // ** FUNCTIONS **
    // Sets *filename equal to the full pathname of the file implementing componentID
    // Returns S_OK on success, else the error code of the failure
    HRESULT GetFileBackingObject( CLSID componentID, std::wstring* filename );
    // Creates the ccVerifyTrust object and configures it to use cached verifies
    // Returns S_OK on success, else the error code of the failure
    HRESULT CreateTrustVerifier( ccVerifyTrust::ccVerifyTrust_IVerifyTrust* verifyTrustLoader, ccVerifyTrust::IVerifyTrust** trustVerifier );
    // Returns S_OK if the specified file is trusted, S_FALSE if not, else the error code of the failure
    HRESULT CCVerifyFileTrusted( LPCTSTR filename );
    // Translates a ccCverifyTrust error code to an HRESULT and returns the result
    HRESULT CCVerifyTrustErrorToHRESULT( ccVerifyTrust::ErrorType errorCode );
    // Verifies trust using WinTrust - no CC!!
    HRESULT VerifyFileSignedByCompany( LPCTSTR filename, LPCTSTR companyNameKeyword );
    
    // ** DATA MEMBERS **
    bool                                    initialized;
    VerifyMode                              mode;                   // What logic mode to use to verify trust
    
    // Direct mode members
    HMODULE                                 winTrustHandle;
    HMODULE                                 crypt32Handle;
    WinVerifyTrustFP                        winVerifyTrust;
    WTHelperProvDataFromStateDataFP         pfnPDfSD;
    WTHelperGetProvSignerFromChainFP        pfnGPSfC;
    WTHelperGetProvCertFromChainFP          pWTHelperGetProvCertFromChain;
    CertGetNameStringAFP                    pCertGetNameStringW;

    // Common client mode members
    ccVerifyTrust::ccVerifyTrust_IVerifyTrust   ccVerifyTrustLoader;
    // Note that trustVerifier's lifetime is bound by verifyTrustLoader's, overriding ref counts!
    ccVerifyTrust::IVerifyTrust*                ccTrustVerifier;
};

#endif // PROTECTIONPROVIDER_TRUSTUTIL