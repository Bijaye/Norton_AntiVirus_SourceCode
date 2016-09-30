#include "TrustUtil.h"
#include "ccVerifyTrustLoader.h"
#include "SymSaferStrings.h"
#include "SymSaferRegistry.h"
#include "StrSafe.h"


// Constructor-destructor
CTrustVerifier::CTrustVerifier() : initialized(false), mode(VerifyMode_Direct), 
    winTrustHandle(NULL),
    crypt32Handle(NULL),
    winVerifyTrust(NULL),
    pfnPDfSD(NULL),
    pfnGPSfC(NULL),
    pWTHelperGetProvCertFromChain(NULL),
    pCertGetNameStringW(NULL),
    ccTrustVerifier(NULL)
{
    // Nothing for now
}

CTrustVerifier::~CTrustVerifier()
{
    Shutdown();
}

HRESULT CTrustVerifier::Initialize( VerifyMode newMode )
{
    HRESULT     returnValHR     = E_FAIL;
    
    // Validate state
    if (initialized)
        return E_ABORT;

    if (newMode == VerifyMode_Direct)
    {
        // WinVerifyTrust is loaded dynamically because it may not be available on this OS - SDK docs note it may be
        // removed from the OS in future versions
        winTrustHandle = LoadLibrary(_T("wintrust.dll"));
        if (winTrustHandle != NULL)
        {
            crypt32Handle = LoadLibrary(_T("crypt32.dll"));
            if (crypt32Handle != NULL)
            {
                winVerifyTrust                  = (WinVerifyTrustFP) GetProcAddress(winTrustHandle, "WinVerifyTrust");
                pfnPDfSD                        = (WTHelperProvDataFromStateDataFP) GetProcAddress(winTrustHandle, "WTHelperProvDataFromStateData");
                pfnGPSfC                        = (WTHelperGetProvSignerFromChainFP) GetProcAddress(winTrustHandle, "WTHelperGetProvSignerFromChain");
                pWTHelperGetProvCertFromChain   = (WTHelperGetProvCertFromChainFP) GetProcAddress(winTrustHandle, "WTHelperGetProvCertFromChain");
                pCertGetNameStringW             = (CertGetNameStringAFP) GetProcAddress(crypt32Handle, "CertGetNameStringW");
                if ((winVerifyTrust != NULL) && (pfnPDfSD != NULL) && (pfnGPSfC != NULL) && (pWTHelperGetProvCertFromChain != NULL) && (pCertGetNameStringW != NULL))
                    returnValHR = S_OK;
                else
                    returnValHR = HRESULT_FROM_WIN32(GetLastError());
            }
            else
            {
                returnValHR = HRESULT_FROM_WIN32(GetLastError());
            }
        }
        else
        {
            returnValHR = HRESULT_FROM_WIN32(GetLastError());
        }

    }
    else // VerifyMode_CommonClient
    {
        returnValHR = CreateTrustVerifier(&ccVerifyTrustLoader, &ccTrustVerifier);
    }
    
    // Cleanup and return
    if (SUCCEEDED(returnValHR))
    {
		mode = newMode;
        initialized = true;
    }
	else
    {
        Shutdown();
    }
    return returnValHR;
}

HRESULT CTrustVerifier::Shutdown( void )
{
    // Direct mode shutdown
    if (winTrustHandle != NULL)
    {
        FreeLibrary(winTrustHandle);
        winTrustHandle = NULL;
    }
    if (crypt32Handle != NULL)
    {
        FreeLibrary(crypt32Handle);
        crypt32Handle = NULL;
    }
    winVerifyTrust                  = NULL;
    pfnPDfSD                        = NULL;
    pfnGPSfC                        = NULL;
    pWTHelperGetProvCertFromChain   = NULL;
    pCertGetNameStringW             = NULL;

    // Common client mode shutdown
	if (ccTrustVerifier != NULL)
    {
	    ccTrustVerifier->Release();
        ccTrustVerifier = NULL;
    }
    
    initialized = false;
    return S_OK;
}

HRESULT CTrustVerifier::IsFileTrusted( LPCTSTR filename )
// Returns S_OK if filename is trusted, S_FALSE if not, else the error code of the failure
{
	HRESULT		returnValHR		= E_FAIL;

    // Validate state
    if (!initialized)
        return E_ABORT;

#ifdef PROTECTIONPROVIDER_TRUSTUTIL_DEBUGNOCHECK
    return S_OK;
#endif
    if (mode == VerifyMode_Direct)
        returnValHR = VerifyFileSignedByCompany(filename, _T("Symantec "));        
    else // VerifyMode_CommonClient
        returnValHR = CCVerifyFileTrusted(filename);

	return returnValHR;
}

HRESULT CTrustVerifier::IsCallerProcessTrusted( void )
// Returns S_OK if the calling process is trusted, S_FALSE if not, else the error code of the failure
{
    TCHAR           filenameBuffer[2*MAX_PATH+1]        = _T("");
    DWORD           returnValDW                         = ERROR_OUT_OF_PAPER;
    HRESULT         returnValHR                         = E_FAIL;

    // Validate state
    if (!initialized)
        return E_ABORT;

    returnValDW = GetModuleFileName(NULL, filenameBuffer, sizeof(filenameBuffer)/sizeof(filenameBuffer[0]));
    if (returnValDW != 0)
        returnValHR = IsFileTrusted(filenameBuffer);
    else
        returnValHR = HRESULT_FROM_WIN32(GetLastError());

    return returnValHR;
}

HRESULT CTrustVerifier::TrustVerifyComponentList( CLSIDList* componentList )
// Checks the DLLs backing the InProc32 objects of all objects identified by CLSID in *componentList.  If the files have valid
// Symantec digital signatures they are left in the list, otherwise they are filtered out of it.
// Returns S_OK on success, else the error code of the failure
{
    CLSIDList                   filteredComponentIDs;
    CLSIDList::iterator         currComponentID;
    std::wstring                currComponentFilename;
    HRESULT                     returnValHR = E_FAIL;

    // Validate state
    if (!initialized)
        return E_ABORT;
    // Validate parameters
    if (componentList == NULL)
        return E_POINTER;

    // Filter the list
    for (currComponentID = componentList->begin(); (currComponentID != componentList->end()) && (returnValHR != E_OUTOFMEMORY); currComponentID++)
    {
        returnValHR = GetFileBackingObject(*currComponentID, &currComponentFilename);
        if (SUCCEEDED(returnValHR))
        {
            returnValHR = IsFileTrusted(currComponentFilename.c_str());
            if (returnValHR == S_OK)
            {
                try
                {
                    filteredComponentIDs.push_back(*currComponentID);
                }
                catch (std::bad_alloc&)
                {
                    returnValHR = E_OUTOFMEMORY;
                }
            }
        }
    }

    // Return result
    try
    {
        *componentList = filteredComponentIDs;
        returnValHR = S_OK;
    }
    catch (std::bad_alloc&)
    {
        returnValHR = E_OUTOFMEMORY;
    }
    return returnValHR;
}

HRESULT CTrustVerifier::GetFileBackingObject( CLSID componentID, std::wstring* filename )
// Sets *filename equal to the full pathname of the file implementing componentID
// Returns S_OK on success, else the error code of the failure
{
    LPOLESTR    componentIDStr              = NULL;
    TCHAR       filenameTC[2*MAX_PATH+1]    = {_T("")};
    long        valueDataSize               = 0;
    HKEY        keyHandle                   = NULL;
    TCHAR       componentPath[100]          = {_T("")};
    DWORD       returnValDW                 = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR                 = E_FAIL;

    // Validate parameters
    if (filename == NULL)
        return E_POINTER;
    if (componentID == GUID_NULL)
        return E_INVALIDARG;

    returnValHR = StringFromCLSID(componentID, &componentIDStr);
    if (SUCCEEDED(returnValHR))
    {
        StringCchPrintf(componentPath, sizeof(componentPath)/sizeof(componentPath[0]), _T("CLSID\\%s\\InProcServer32"), componentIDStr);
//        sssnprintf(componentPath, sizeof(componentPath)/sizeof(componentPath[0]), _T("CLSID\\%s\\InProcServer32"), componentIDStr);
        returnValDW = RegOpenKeyEx(HKEY_CLASSES_ROOT, componentPath, NULL, KEY_READ, &keyHandle);
        if (returnValHR == ERROR_SUCCESS)
        {
            valueDataSize = sizeof(filenameTC)/sizeof(filenameTC[0]);
            returnValDW = SymSaferRegQueryValue(keyHandle, NULL, filenameTC, &valueDataSize);

            if (returnValDW == ERROR_SUCCESS)
            {
                try
                {
                    *filename = filenameTC;
                    returnValHR = S_OK;
                }
                catch (std::bad_alloc&)
                {
                    returnValHR = E_OUTOFMEMORY;
                }
            }
            else
            {
                returnValHR = HRESULT_FROM_WIN32(returnValDW);
            }
            RegCloseKey(keyHandle);
            keyHandle = NULL;
        }
        else
        {
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
        }
        CoTaskMemFree(componentIDStr);
        componentIDStr = NULL;
    }
    return returnValHR;
}

HRESULT CTrustVerifier::CreateTrustVerifier( ccVerifyTrust::ccVerifyTrust_IVerifyTrust* verifyTrustLoader, ccVerifyTrust::IVerifyTrust** trustVerifier )
// Creates the ccVerifyTrust object and configures it to use cached verifies
// Returns S_OK on success, else the error code of the failure
{
    ccVerifyTrust::IVerifyTrustPtr              verifyTrustPtr;
    ccVerifyTrust::ICachedVerifyTrustQIPtr      cachedVerifyTrustPtr;
    HRESULT                                     returnValHR             = E_FAIL;

    // Validate parameters
	if (verifyTrustLoader == NULL)
		return E_POINTER;
    if (trustVerifier == NULL)
        return E_POINTER;

    // Load and create the IVerifyTrust interface.
    if (SYM_SUCCEEDED(verifyTrustLoader->CreateObject(verifyTrustPtr.m_p)))
    {
        if (verifyTrustPtr->Create(true) == ccVerifyTrust::eNoError)
        {
            cachedVerifyTrustPtr = verifyTrustPtr;
            if (cachedVerifyTrustPtr != NULL)
                cachedVerifyTrustPtr->SetCacheEnabled(true);
            *trustVerifier = verifyTrustPtr.Detach();
            returnValHR = S_OK;
        }
    }

    return returnValHR;
}

HRESULT CTrustVerifier::CCVerifyFileTrusted( LPCTSTR filename )
// Returns S_OK if filename is trusted, S_FALSE if not, else the error code of the failure
{
    DWORD                                       fileAttributes          = 0;
    ccVerifyTrust::ErrorType                    returnValCCVT;
    HRESULT                                     returnValHR             = E_FAIL;

    // Validate parameters
    if (filename == NULL)
        return E_POINTER;

    // Load and create the IVerifyTrust interface.
    fileAttributes = GetFileAttributes(filename);
    if ((fileAttributes != INVALID_FILE_ATTRIBUTES) && ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0))
    {
        returnValCCVT = ccTrustVerifier->VerifyFile(filename, ccVerifyTrust::eSymantecSignature);
        returnValHR = CCVerifyTrustErrorToHRESULT(returnValCCVT);
        if (returnValHR == SEC_E_UNTRUSTED_ROOT)
            returnValHR = S_FALSE;
    }
    else
    {
        returnValHR = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    return returnValHR;
}

HRESULT CTrustVerifier::CCVerifyTrustErrorToHRESULT( ccVerifyTrust::ErrorType errorCode )
// Translates a ccCverifyTrust error code to an HRESULT and returns the result
{
    HRESULT         returnValHR = E_FAIL;

    switch (errorCode)
    {
    case ccVerifyTrust::eUnknownError:
        returnValHR = E_FAIL;
        break;
    case ccVerifyTrust::eNoError:
        returnValHR = S_OK;
        break;
    case ccVerifyTrust::eFatalError:
        returnValHR = E_FAIL;
        break;
    case ccVerifyTrust::eVerifyError:
        returnValHR = ERROR_IPSEC_IKE_INVALID_SIGNATURE;
        break;
    case ccVerifyTrust::eTrustSystemError:
        returnValHR = SEC_E_CANNOT_INSTALL;
        break;
    case ccVerifyTrust::eBadParameterError:
        returnValHR = E_INVALIDARG;
        break;
    case ccVerifyTrust::eOutOfMemoryError:
        returnValHR = E_OUTOFMEMORY;
        break;

    // New types...
    case ccVerifyTrust::eUnknownFileTypeError:
        returnValHR = HRESULT_FROM_WIN32(ERROR_UNSUPPORTED_TYPE);
        break;
    case ccVerifyTrust::eSignatureNotDecodedError:
        returnValHR = CRYPT_E_BAD_ENCODE;
        break;
    case ccVerifyTrust::eFileOpenError:
        returnValHR = HRESULT_FROM_WIN32(ERROR_OPEN_FAILED);
        break;
    case ccVerifyTrust::eFileReadError:
        returnValHR = HRESULT_FROM_WIN32(ERROR_READ_FAULT);
        break;
    case ccVerifyTrust::eDigestVerificationError:
        returnValHR = TRUST_E_BAD_DIGEST;
        break;
    case ccVerifyTrust::eCertificateUnreadableError:
        returnValHR = E_FAIL;
        break;
    case ccVerifyTrust::eCertificateInvalidError:
        returnValHR = CERTSRV_E_INVALID_CA_CERTIFICATE;
        break;
    case ccVerifyTrust::eCertificateNotTrustedError:
        returnValHR = SEC_E_UNTRUSTED_ROOT;
        break;
    case ccVerifyTrust::eCertificateRevocationCheckError:
        returnValHR = CRYPT_E_REVOKED;
        break;
    case ccVerifyTrust::eSingleSignerCertNotFoundError:
        returnValHR = CERT_E_UNTRUSTEDROOT;
        break;
    case ccVerifyTrust::eSingleSignerCertChainingError:
        returnValHR = CERT_E_CHAINING;
        break;
    case ccVerifyTrust::eNoCertificateForSignerError:
        returnValHR = SEC_E_UNTRUSTED_ROOT;
        break;
    case ccVerifyTrust::eSetupError:
        returnValHR = E_FAIL;
        break;
    }

    return returnValHR;
}

HRESULT CTrustVerifier::VerifyFileSignedByCompany( LPCTSTR filename, LPCTSTR companyNameKeyword )
// Verifies that filename is digitally signed by a company with companyNameKeyword in the certificate's company name.
// winTrustHandle is a handle to WinTrust.dll; crypt32Handle is a handle to Crypt32.dll.  If these are NULL, the DLLs are dynamically loaded.
// Returns S_OK if it is, TRUST_E_SUBJECT_NOT_TRUSTED if signature OK but companyNameKeyword not found, else the error code of the failure.
{
    DWORD                                   charactersToConvert             = 0;
    HANDLE                                  fileHandle                      = INVALID_HANDLE_VALUE;
    WINTRUST_DATA                           winTrustData;
    GUID                                    actionID                        = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    WINTRUST_FILE_INFO                      fileInfo;
    CRYPT_PROVIDER_DATA*                    providerData                    = NULL;
    CRYPT_PROVIDER_SGNR*                    providerSigner                  = NULL;
    CRYPT_PROVIDER_CERT*                    providerCertificate             = NULL;
    TCHAR                                   szCompany[MAX_PATH+1]           = _T("");
    BOOL                                    loadedWinTrust                  = FALSE;
    BOOL                                    loadedCrypt32                   = FALSE;
    HRESULT                                 returnVal                       = E_FAIL;
    
    // Validate parameters
    if (filename == NULL)
        return E_POINTER;
    if (companyNameKeyword == NULL)
        return E_POINTER;

    // These must be here to eliminate a superfluous 4701
    ZeroMemory(&winTrustData, sizeof(winTrustData));
    ZeroMemory(&fileInfo, sizeof(fileInfo));
    // On some DBCS machines, WinVerifyTrust couldn't handle the converted filename.
    // To resolve this, we open the file and specify the file handle in the VerifyTrust call
    fileHandle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        // Verify signature validity
        fileInfo.cbStruct               = sizeof(fileInfo);
        fileInfo.pcwszFilePath          = filename;
        fileInfo.hFile                  = fileHandle;

        winTrustData.cbStruct                   = sizeof(winTrustData);
        winTrustData.dwUIChoice                 = WTD_UI_NONE;
        winTrustData.fdwRevocationChecks        = WTD_REVOKE_NONE | WTD_REVOCATION_CHECK_NONE;
        winTrustData.dwUnionChoice              = WTD_CHOICE_FILE;
        winTrustData.pFile                      = &fileInfo;
        winTrustData.dwStateAction              = WTD_STATEACTION_VERIFY;

        returnVal = (*winVerifyTrust)((HWND)INVALID_HANDLE_VALUE, &actionID, &winTrustData);
    }
    if (returnVal == S_OK)
        providerData = (*pfnPDfSD)(winTrustData.hWVTStateData);

    // Get information on the certificate of the signer for this trust provider
    if (providerData != NULL)
        providerSigner = (*pfnGPSfC)(providerData,0,0,1);
    if (providerSigner != NULL)
        providerCertificate = pWTHelperGetProvCertFromChain(providerSigner, 0);
    if (providerCertificate != NULL)
    {
        // Verify the certifier is Symantec
        pCertGetNameStringW(providerCertificate->pCert, CERT_NAME_ATTR_TYPE, 0, NULL, szCompany, sizeof(szCompany));
        if (wcsstr(szCompany, companyNameKeyword) != NULL)
            returnVal = S_OK;
        else
            returnVal = S_FALSE;
    }

    // Shutdown and return
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(fileHandle);
        fileHandle = INVALID_HANDLE_VALUE;
    }
    
    return returnVal;
}
