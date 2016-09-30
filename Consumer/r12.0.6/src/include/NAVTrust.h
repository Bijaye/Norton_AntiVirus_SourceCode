#pragma once

#include "ccVerifyTrustInterface.h"
#include "ccVerifyTrustLoader.h"

namespace NAVToolbox
{
    ///////////////////////////////////////////////////////////////////////////////
    //
    // Return code for methods.
    //

    typedef unsigned long NAVTRUSTSTATUS;

    ///////////////////////////////////////////////////////////////////////////////
    //
    // Return codes for calls into NAVTrust functions
    //
    //
    // NAVTRUST_OK: The COM server associated the the CLSID/ProgID is signed by
    //              Symantec.
    // NAVTRUST_ERROR_NOT_SIGNED: The COM server associated with the CLSID/ProgID 
    //                            is not signed by Symantec.
    // NAVTRUST_ERROR_INVALID_PARAM: Bad parameter passed into the function. File/CLSID/ProgID not found.
    // NAVTRUST_ERROR_NO_REG_ACCESS: User cannot access the registry so the server
    //                               cannot be found to be verified.
    // NAVTRUST_ERROR_CCNOTHANDLED: The error is known to be a custom ccTrust error
    //                              but the NAVTrust function did not have any special
    //                              handling implemented for it.
    // NAVTRUST_ERROR_OUTOFMEMORY: Uh, dude there's no more memory.
    // NAVTRUST_ERROR_NO_TRUST_VERIFIER: The ccVerifyTrust mechanism isn't there
    //                                   or couldn't be loaded
    // NAVTRUST_ERROR_UNKNOWN: Don't know what happened.
    //
    enum eTrustReturnCodes
    {
	    NAVTRUST_OK                       = 0,
        NAVTRUST_ERROR_NO_TRUST_VERIFIER,
        NAVTRUST_ERROR_NOT_SIGNED,
        NAVTRUST_ERROR_INVALID_PARAM,
	    NAVTRUST_ERROR_NO_REG_ACCESS,
        NAVTRUST_ERROR_CCNOTHANDLED,
        NAVTRUST_ERROR_OUTOFMEMORY,
	    NAVTRUST_ERROR_UNKNOWN
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    // IsSymantecComServer - Verifies that a Com Server is signed by Symantec 
    // using the CLSID or ProgID of the server.
    //
    ///////////////////////////////////////////////////////////////////////////////
    NAVTRUSTSTATUS IsSymantecComServer(REFCLSID rclsid);

    NAVTRUSTSTATUS IsSymantecComServer(LPCSTR lpszProgID);

    NAVTRUSTSTATUS IsSymantecSignedImage(LPCSTR lpcFileName);
    NAVTRUSTSTATUS IsSymantecSignedImage(LPWSTR lpcFileName);   // Wide char version.

    NAVTRUSTSTATUS IsSymantecApplication();
} // end namespace NAVToolbox
