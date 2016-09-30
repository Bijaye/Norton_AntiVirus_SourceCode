////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AvInterfaces.h"
#include "AllNAVEvents.h"

namespace AvProd
{
    namespace AvModuleLogging
    {
        //*************************************************************************
        // AutoProtect Client Activities Interface
        //*************************************************************************
        class IAutoProtectClientActivities : public ISymBase
        {
        public:
            virtual HRESULT Initialize() throw() = 0;
            virtual HRESULT OnAutoProtectRemediationComplete(AVModule::IAVMapDwordData* pActivityData) throw() = 0;
            virtual HRESULT OnAutoProtectThreatBlocked(AVModule::IAVMapDwordData* pActivityData) throw() = 0;
        };

        // {C7020720-3E6D-4d67-BA44-2BB01EC64C15}
        SYM_DEFINE_INTERFACE_ID(IID_AutoProtectClientActivities, 
            0xc7020720, 0x3e6d, 0x4d67, 0xba, 0x44, 0x2b, 0xb0, 0x1e, 0xc6, 0x4c, 0x15);
        
        TYPEDEF_SYM_POINTERS(IAutoProtectClientActivities, IID_AutoProtectClientActivities);

        //*************************************************************************
        // Manual Scan Client Activities Interface
        //*************************************************************************
        class IManualScanClientActivities : public ISymBase
        {
        public:
            virtual HRESULT Initialize() throw() = 0;
            
            virtual HRESULT SetScanResultByActivityData(AVModule::IAVMapStrData* pResults) throw() = 0;
            virtual HRESULT SetScanResultByProperty(DWORD dwPropertyId, DWORD dwValue) throw() = 0;
            virtual HRESULT SetScanResultByCategory(DWORD dwPropertyId, DWORD dwCategoryId, DWORD dwValue) throw() = 0; 
            virtual HRESULT SetEraserScanResults(DWORD dwPropertyId, DWORD dwValue) throw() = 0; // See ccEraserInterface.h Detection types
			virtual HRESULT SetScanType(DWORD dwScanType) throw() = 0;  // see AvDefines.h (eScanType)
            virtual HRESULT SetScanType(LPCWSTR wszScanType) throw() = 0;
			virtual HRESULT SetScanResult(DWORD dwScanResult) throw() = 0; // see AvDefines.h 
            virtual HRESULT SetScanInstanceId(REFGUID guidScanInstance) throw() = 0;
			virtual HRESULT Commit() throw() = 0;
			
			virtual HRESULT LogManualScanResults(REFGUID guidScanInstance) throw() = 0;
        };

        // {79FC21FE-6AF8-4784-930B-E691273C52BF}
        SYM_DEFINE_INTERFACE_ID(IID_ManualScanClientActivities, 
            0x79fc21fe, 0x6af8, 0x4784, 0x93, 0xb, 0xe6, 0x91, 0x27, 0x3c, 0x52, 0xbf);
        
        TYPEDEF_SYM_POINTERS(IManualScanClientActivities, IID_ManualScanClientActivities);

        //*************************************************************************
        // Email scan Client Activities Interface
        //*************************************************************************
        class IEmailScanClientActivities : public ISymBase
        {
        public:
            virtual HRESULT Initialize() throw() = 0;
            virtual HRESULT OnEmailThreatsDetected(AVModule::IAVMapStrData* pActivityData) throw() = 0;
            virtual HRESULT OnOEHDetection(AVModule::IAVMapStrData* pOEHInfo, DWORD dwOEHAction) throw() = 0;
            virtual HRESULT OnEmailScanComplete(AVModule::IAVMapStrData* pActivityData) throw() = 0;
			virtual HRESULT LogEmailScanResults(REFGUID guidScanInstance) throw() = 0;
        };

        // {FAA06FFD-499A-41ba-9CFB-2D76AD85E076}
        SYM_DEFINE_INTERFACE_ID(IID_EmailScanClientActivities, 
            0xfaa06ffd, 0x499a, 0x41ba, 0x9c, 0xfb, 0x2d, 0x76, 0xad, 0x85, 0xe0, 0x76);

        TYPEDEF_SYM_POINTERS(IEmailScanClientActivities, IID_EmailScanClientActivities);

        //*************************************************************************
        // Threat Data Formatting Interface
        //*************************************************************************
        class IThreatDataEz : public ISymBase
        {
        public:
            virtual HRESULT GetCategoriesString(AVModule::IAVMapDwordData* pThreatTrackInfo, cc::IString*& pCategoriesString) throw() = 0;
        };

        // {93EA2ACA-4AA3-4bcb-A6D7-939433CE41D8}
        SYM_DEFINE_INTERFACE_ID(IID_ThreatDataEz, 
            0x93ea2aca, 0x4aa3, 0x4bcb, 0xa6, 0xd7, 0x93, 0x94, 0x33, 0xce, 0x41, 0xd8);

        TYPEDEF_SYM_POINTERS(IThreatDataEz, IID_ThreatDataEz);

    }

}