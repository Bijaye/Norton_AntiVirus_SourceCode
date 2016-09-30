///////////////////////////////////////////////////////////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
// Options Reset Utility - 4 October 2006 - Ian Dundore
//
///////////////////////////////////////////////////////////////////////////////

#include <atlbase.h>
#include <atlconv.h>
#include <atlcom.h>
#include "stdlib.h"

#define INITIIDS
#include "NAVOptions_h.h"
#include "NAVOptions_i.c"

#include "ccLib.h"
#include "ccLibDllLink.h"
#include "ccLibDll.h"
#include "ccSymDelayLoader.h"
#include "ccCoInitialize.h"
#include "ccString.h"

int main(int argc, wchar_t** argv)
{
    HRESULT hr;

    wprintf(L"starting up COM... ");

    // Initialize COM Safely.
    ccLib::CCoInitialize coInit;

    hr = coInit.Initialize(ccLib::CCoInitialize::eSTAModel);
    wprintf(L"OK (hr = 0x%X)\n", hr);

    CComPtr<INAVOptions> spNAVOptions;
    if (SUCCEEDED(hr = spNAVOptions.CoCreateInstance(CLSID_NAVOptions, NULL, CLSCTX_INPROC_SERVER)))
    {
        // Do da bidness.

        /* This would reset ALL options to defaults 
        if(FAILED(hr = spNAVOptions->Default()))
            CCTRCTXE1(_T("Failed to set NAV Options defaults. HR = 0x%X"), hr);
        */

        /* Reset AP Exclusions to defaults */
        wprintf(L"Setting AP Exclusion defaults.\n");
        if(FAILED(hr = spNAVOptions->SetAPExclusionDefaults()))
        {
            // Trace or something...
            wprintf(L"ERROR\n");
        }

        /* Reset Manual Scan and Security Risk exclusion lists to default */
        wprintf(L"Setting Path Exclusion defaults.\n");
        if(FAILED(hr = spNAVOptions->SetPathExclusionDefaults()))
        {
            // Trace or something...
            wprintf(L"ERROR\n");
        }

        wprintf(L"Setting Anomaly Exclusion defaults.\n");
        if(FAILED(hr = spNAVOptions->SetAnomalyExclusionDefaults()))
        {
            // Trace or something...
            wprintf(L"ERROR\n");
        }

        /* Modify Auto-Protect behavior to automatically delete Security Risks
         * This sets the AP response mode to 'Autodelete' for lowrisk threats.
         * Medium/high-risk threats are ALWAYS automatically removed. */
        wprintf(L"Setting NAVAP_ResponseMode to 0.\n");
        if(FAILED(hr = spNAVOptions->Put(CComBSTR(_T("NAVAP:ResponseMode")), CComVariant(0))))
        {
            // Trace or something...
            wprintf(L"ERROR\n");
        }

        /* Modify Manual Scanner behavior to automatically delete Security Risks
         * This sets the manual scan response mode to 'Autodelete' for lowrisk threats.
         * Medium/high-risk threats are ALWAYS automatically removed. */
        wprintf(L"Setting SCANNER_ResponseMode to 0.\n");
        if(FAILED(hr = spNAVOptions->Put(CComBSTR(_T("SCANNER:ResponseMode")), CComVariant(0))))
        {
            // Trace or something...
            wprintf(L"ERROR\n");
        }

        wprintf(L"Saving changes... ");
        if(FAILED(hr= spNAVOptions->Save()))
        {
            wprintf(L"ERROR\n");
        }

        // More stuff here..
        wprintf(L"OK\n");
    }
    else
    {
        wprintf(L"Creating NAVOptions object failed. HR = 0x%X\n", hr);
    }

    // Smart objects will autodestroy themselves, no need to delete them.
    wprintf(L"Shutting down\n");
    return 0;
}