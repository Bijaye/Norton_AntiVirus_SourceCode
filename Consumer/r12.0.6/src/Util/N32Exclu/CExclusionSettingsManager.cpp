// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////

/***
 * Implementation file for the Settings manager wrapper class.
 * Has helper functions for serializing and deserializing exclusions.
 ***/

#ifndef __N32EXCLU_cexclusionsettingsmanager_cpp__
#define __N32EXCLU_cexclusionsettingsmanager_cpp__

#include "CExclusionSettingsManager.h"
#include "ExclusionInterface.h"
#include "CAnomalyExclusion.h"
#include "CFileSystemExclusion.h"
#include "ccSymMemoryStreamImpl.h"
#include "ccSymFileStreamImpl.h"
#include "ccSettingsInterface.h"
#include "ccSingleLock.h"
#include "ccMessageLock.h"
#include "ccEvent.h"
#include "ccSymKeyValueCollectionImpl.h"

namespace NavExclusions
{
    const DWORD CExclusionSettingsManager::m_dwLoadWaitMax = 180000;    // 3 min
    const DWORD CExclusionSettingsManager::m_dwLoadWaitInterval = 1000;  // 1 sec

    CExclusionSettingsManager::CExclusionSettingsManager():
        m_bInitialized(false)
    {
    }

    CExclusionSettingsManager::~CExclusionSettingsManager()
    {
        m_bInitialized = false;
    }

    bool CExclusionSettingsManager::isInitialized() 
    { return this->m_bInitialized; }

    ExResult CExclusionSettingsManager::initialize()
    {
        if(m_bInitialized)
        {
            CCTRACEE(_T("CExclusionSettingsManager::initialize - Already "\
                        "initialized!"));
            return NavExclusions::Fail;
        }

        m_bInitialized = this->Load(EXCLUSIONS_SETTINGS_KEY);
        this->m_pEnumValues = NULL;
        this->m_dwEnumCurrent = 0;
        if(m_bInitialized)
        {
            CCTRACEI(_T("CExclusionSettingsManager::initialize - Init OK!"));
            return NavExclusions::Success;
        }
        else
        {
            CCTRACEE(_T("CExclusionSettingsManager::initialize - Could not "\
                        "initialize CC Settings manager class."));
            return NavExclusions::NotInitialized;
        }
    }

    ExResult CExclusionSettingsManager::uninitialize()
    {
        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionSettingsManager::uninitialize - Not "\
                        "initialized!"));
            return NavExclusions::NotInitialized;
        }

        this->enumUnload();
        m_bInitialized = false;
        if(this->Commit(EXCLUSIONS_SETTINGS_KEY))
        {
            this->m_pSettings = NULL;
            this->m_pSettingsManager = NULL;
        }
        else
        {
            CCTRACEW(_T("CExclusionSettingsManager::uninitialize - "\
                        "Error during commit!"));
            return NavExclusions::Fail;
        }

        return NavExclusions::Success;
    }

    ExResult CExclusionSettingsManager::saveExclusion(IExclusion* pExclusion)
    {
        ExResult res;
        cc::IStringPtr spGuid;
        ccSym::CMemoryStreamImplPtr pMemStream;

        if(!this->isInitialized())
        {
            CCTRACEE(_T("CExclusionSettingsManager::saveExclusion - "\
                        "Not initialized."));
            return NavExclusions::NotInitialized;
        }

        if(!pExclusion)
        {
            CCTRACEE(_T("CExclusionSettingsManager::saveExclusion - "\
                        "Received a NULL pointer; can't save that!"));
            return NavExclusions::InvalidArgument;
        }

        if(pExclusion->isDeleted() == NavExclusions::Success ||
           pExclusion->isExpired() == NavExclusions::Success)
        {
            if(this->deleteExclusion(pExclusion) == NavExclusions::Success)
                return NavExclusions::False; // Indicate release of resources
            else
                return NavExclusions::Fail;
        }

        if(pExclusion->getExclusionGuid(spGuid) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionSettingsManager::saveExclusionAnomaly - "\
                "Couldn't retrieve GUID from exclusion."));
            return NavExclusions::Fail;
        }

        pMemStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
        if( pMemStream )
        {
            if((res = pExclusion->Save(pMemStream)) != NavExclusions::Success)
            {
                CCTRACEE(_T("CExclusionSettingsManager::saveExclusion - "\
                    "Error while saving to memory stream."));
                return res;
            }

            pMemStream->SetPosition(0);
            ccLib::CMemory mem = pMemStream->GetMemory();
            DWORD dwSize = (DWORD)mem.GetSize();
            CCTRACEI(_T("CExclusionSettingsManager::saveExclusion - Saving %d "\
                        "bytes of exclusion data to CC Settings."), dwSize);
            if(this->SetValue(spGuid->GetStringW(), (LPBYTE)mem.GetMemory(), 
                dwSize) != NavExclusions::Success)
            {
                CCTRACEE(_T("CExclusionSettingsManager::saveExclusion -"\
                    " Failed to save serialized data to ccSettings."));
                return NavExclusions::Fail;
            }
        }
        else
        {
            CCTRACEE(_T("CExclusionSettingsManager::saveExclusion - "\
                "Failed to create memory stream object."));
            return NavExclusions::OutOfMemory;
        }

        return NavExclusions::Success;
    }

    ExResult CExclusionSettingsManager::deleteExclusion(IExclusion* pExclusion)
    {
        cc::IStringPtr spGuid;
        if(!this->isInitialized())
        {
            CCTRACEE(_T("CExclusionSettingsManager::deleteExclusion - "\
                        "Not initialized."));
            return NavExclusions::NotInitialized;
        }

        if(!pExclusion)
        {
            CCTRACEE(_T("CExclusionSettingsManager::deleteExclusion - "\
                        "received a NULL pointer."));
            return NavExclusions::InvalidArgument;
        }

        if(pExclusion->getExclusionGuid(spGuid) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionSettingsManager::deleteExclusion - "\
                        "Couldn't retrieve GUID from exclusion."));
            return NavExclusions::Fail;
        }

        if(SYM_FAILED(this->m_pSettings->DeleteValue(spGuid->GetStringW())))
        {
            CCTRACEE(_T("CExclusionSettingsManager::deleteExclusion - "\
                        "CC Settings reported failure when trying to "\
                        "delete exclusion %s"), spGuid->GetStringA());
            return NavExclusions::Fail;
        }

        return NavExclusions::Success;
    }

    ExResult CExclusionSettingsManager::enumLoadExclusionData(cc::IKeyValueCollection* &pData)
    {
        DWORD dwCount = 0;
        if(!this->isInitialized())
        {
            CCTRACEE(_T("CExclusionSettingsManager::enumLoadExclusionData - Not "\
                        "Initialized."));
            return NavExclusions::NotInitialized;
        }
        
        if(pData != NULL)
        {
            CCTRACEW(_T("CExclusionSettingsManager::enumLoadExclusionData - "\
                        "Received a non-NULL KVC object. Autoreleasing."));
            pData->Release();
            pData = NULL;
        }

        if(this->m_pEnumValues == NULL)
        {
            CCTRACEE(_T("CExclusionSettingsManager::enumLoadExclusionData - "\
                        "Enumeration not initialized - call enumReset first."));
            return NavExclusions::NotInitialized;
        }

        if(SYM_FAILED(this->m_pEnumValues->GetCount(dwCount)))
        {
            CCTRACEE(_T("CExclusionSettingsManager::enumLoadExclusionData - "\
                "Could not retrieve # of objects in Value Enumeration."));
            return NavExclusions::Fail;
        }

        if(this->m_dwEnumCurrent >= dwCount)
        {
            CCTRACEI(_T("CExclusionSettingsManager::enumLoadExclusionData - "\
                        "End of enumeration reached."));
            return NavExclusions::False;    // End of enum indicator
        }

        CStringW strBuffer;
        DWORD dwSize, dwType, dwActualSize;
        SYMRESULT sr;
        dwSize = dwType = dwActualSize = 0;
        sr = this->m_pEnumValues->GetItem(this->m_dwEnumCurrent, (LPWSTR)NULL, 
                                            dwSize, dwType);
        if(SYM_FAILED(sr) && sr != SYM_SETTINGS_ERROR_BUFFER_TOO_SMALL)
        {
            CCTRACEE(_T("CExclusionSettingsManager::enumLoadExclusionData - "\
                        "Could not retrieve size of CC Settings value string."));
            return NavExclusions::InsufficientBuffer;
        }

        if(dwSize <= 0)
        {
            CCTRACEW(_T("CExclusionSettingsManager::enumLoadExclusionData - "\
                "Enumeration reported nonsensical (<=0) size. Skipping."));
            this->m_dwEnumCurrent++;
            return NavExclusions::Success;
        }

        dwSize += 1*sizeof(wchar_t); // Add 1 for null char.
        if(SYM_FAILED(this->m_pEnumValues->GetItem(this->m_dwEnumCurrent,
                                             strBuffer.GetBuffer((int)dwSize),
                                             dwSize, dwType)))
        {
            CCTRACEE(_T("CExclusionSettingsManager::enumLoadExclusionData - "\
                        "Failed to retrieve exclusion GUID key."));
            strBuffer.ReleaseBuffer();
            return NavExclusions::OutOfMemory;
        }
        strBuffer.ReleaseBuffer();

        dwSize = 0;
        sr = this->m_pSettings->GetBinary(strBuffer.GetString(), NULL, dwSize);
        if(SYM_FAILED(sr) && sr != SYM_SETTINGS_ERROR_BUFFER_TOO_SMALL)
        {
            CCTRACEE(_T("CExclusionSettingsManager::enumLoadExclusionData - "\
                        "Could not retrieve size of binary data when loading "\
                        "KVC."));
            return NavExclusions::InsufficientBuffer;
        }

        if(dwSize <= 0)
        {
            CCTRACEE(_T("CExclusionSettingsManager::enumLoadExclusionData - "\
                "Settings->GetBinary returned nonsensical (<=0) value. Skipping."));
            this->m_dwEnumCurrent++;
            return NavExclusions::Fail;
        }

        CCTRACEI(_T("CExclusionSettingsManager::enumLoadExclusionData() - "\
                 "Attempting to load %d bytes of data from settings for "\
                 "KVC object."), dwSize);
        ccSym::CMemoryStreamImplPtr pMemStream;
        pMemStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());

        if(pMemStream == NULL)
        {
            CCTRACEE(_T("CExclusionSettingsManager::enumLoadExclusionData - "\
                        "Could not allocate memory stream for deserialization."));
            return NavExclusions::OutOfMemory;
        }

        if(pMemStream->GetMemory().NewAlloc(dwSize) == NULL)
        {
            CCTRACEE(_T("CExclusionSettingsManager::enumLoadExclusionData - "\
                        "Could not allocate memory buffer for deserialization."));
            return NavExclusions::OutOfMemory;
        }

        if(SYM_FAILED(this->m_pSettings->GetBinary(strBuffer.GetString(), 
                                                   (LPVOID)pMemStream->GetMemory(), 
                                                   dwSize)))
        {
            CCTRACEE(_T("CExclusionSettingsManager::enumLoadExclusionData() - "\
                        "Failed to load exclusion data."));
            return NavExclusions::Fail;
        }

        // Create the collection
        pData = ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl();
        if( !pData )
        {
            CCTRACEE(_T("CExclusionSettingsManager::enumLoadExclusionData - "\
                        "failed to create new KVC object."));
            return NavExclusions::OutOfMemory;
        }

        cc::ISerializeQIPtr pSerialized = pData;
        if( !pSerialized )
        {
            CCTRACEE("CExclusionSettingsManager::enumLoadExclusionData - Unable to get serialize interface");
            pData->Release();
            pData = NULL;
            return NavExclusions::NoInterface;
        }

        // Load up the data
        if( !pSerialized->Load(pMemStream) )
        {
            CCTRACEE("CExclusionSettingsManager::enumLoadExclusionData - Unable to load from serialized data from memory stream");
            pData->Release();
            pData = NULL;
            return NavExclusions::Fail;
        }

        size_t nCount = pData->GetCount();
        CCTRACEI("CExclusionSettingsManager::enumLoadExclusionData - Loaded exclusion KVC data from settings. %d entries",nCount);
        this->m_dwEnumCurrent++;
        return NavExclusions::Success;
    }

    ExResult CExclusionSettingsManager::enumLoadReset()
    {
        if(!this->isInitialized())
        {
            CCTRACEE(_T("CExclusionSettingsManager::enumLoadReset - Not "\
                        "initialized!"));
            return NavExclusions::NotInitialized;
        }

        if(this->m_pEnumValues)
        {
            CCTRACEI(_T("CExclusionSettingsManager::enumLoadReset - "\
                        "Preexisting enumeration found; resetting."));
            if(!this->enumUnload())
            {
                CCTRACEE(_T("CExclusionSettingsManager::enumLoadReset - "\
                            "Could not unload enumeration."));
                return NavExclusions::Fail;
            }
        }

        if(SYM_FAILED(this->m_pSettings->EnumValues(&(this->m_pEnumValues.m_p))))
        {
            CCTRACEE(_T("CExclusionSettingsManager::enumLoadReset - "\
                        "Failed to load enumeration of values."));
            return NavExclusions::Fail;
        }

        this->m_dwEnumCurrent = 0;
        return NavExclusions::Success;
    }

    bool CExclusionSettingsManager::enumUnload()
    {
        if(!this->isInitialized())
            return false;

        this->m_dwEnumCurrent = 0;
        if(this->m_pEnumValues)    // Else, Already unloaded
        {
            this->m_pEnumValues.Release();
            this->m_pEnumValues = NULL;
        }
        return true;
    }

    ExResult CExclusionSettingsManager::SetValue(LPCWSTR lpszName, LPBYTE pBuf,
                                                DWORD dwSize)
	{
        bool bException = false;

        if( !this->m_bInitialized || this->m_pSettings == NULL )
        {
            CCTRACEE(_T("CExclusionSettingsManager::SetValue() - Not "
                        "initialized properly."));
            return NavExclusions::NotInitialized;
        }

		try
		{
			CString valueName;
            this->m_pSettings->PutBinary(lpszName, pBuf, dwSize);
		}
		catch (...)
		{
            bException = true;
		}

		CCTRACEI(_T("%s <- [%d]"), lpszName, dwSize);
        if(bException)
            return NavExclusions::Fail;
        else
            return NavExclusions::Success;
	}

    bool CExclusionSettingsManager::IsSettingsManagerActive(bool bSpinLock) 
    {
        BOOL bActive = ccSettings::CSettingsManagerHelper::IsSettingsManagerActive();

        // If it's on, or we aren't waiting, return now.
        if ( !bSpinLock || bActive )
        {
            return (bActive==TRUE?true:false);  // Stupid performance warning
        }

        ccLib::CEvent eventNothing;
        ccLib::CMessageLock msgLock (TRUE, TRUE);

        for ( DWORD dwWaitLoop = 0; 
                dwWaitLoop < m_dwLoadWaitMax; 
                dwWaitLoop += m_dwLoadWaitInterval )
        {
            eventNothing.Create ( NULL, FALSE, FALSE, NULL, FALSE);
            msgLock.Lock ( eventNothing, m_dwLoadWaitInterval );

            if (ccSettings::CSettingsManagerHelper::IsSettingsManagerActive())
                return true;
        }

        return false;
    }

    bool CExclusionSettingsManager::SettingsReady()
    {
        // Load ccSettings
        //
        if ( !this->m_pSettingsManager.m_p )
        {
		    if(!IsSettingsManagerActive(true))
			    return false;

            SYMRESULT symRes = ccSettings::CSettingsManagerHelper::Create(m_pSettingsManager.m_p);
            if (SYM_FAILED(symRes) ||
                m_pSettingsManager == NULL)
            {
                CCTRACEE(_T("CExclusionSettingsManager::SettingsReady() : "\
                            "ccSettings.Create() != SYM_OK, 0x%08X\n"), symRes);
                return false;
            }
        }

        return true;
    }

    bool CExclusionSettingsManager::Load (LPCWSTR lpszKey)
    {
        if ( !this->SettingsReady() )
            return false;

        // Create main key, if it doesn't exist
        SYMRESULT symRes = this->m_pSettingsManager->CreateSettings(lpszKey, 
                                                          &(this->m_pSettings));
        if (SYM_FAILED(symRes))
        {
            CCTRACEE(_T("CExclusionSettingsManager::Load() : "\
                        "pSettingsManager->GetSettings() != SYM_OK, 0x%08X\n"), 
                        symRes);
            return false;
        }

        return true;
    }

    bool CExclusionSettingsManager::Commit (LPCWSTR lpszKey)
    {
        if ( !this->SettingsReady() )
            return false;

        // Enum each key/value in the bag and store in Settings
        SYMRESULT symRes;

        // Create main key, if it's not already open
        if ( !m_pSettings.m_p )
        {
            symRes = m_pSettingsManager->CreateSettings(lpszKey, &m_pSettings);
            if (SYM_FAILED(symRes))
            {
                CCTRACEE(_T("CExclusionSettingsManager::Commit() : pSettingsManager->CreateSettings() != SYM_OK, 0x%08X\n"), symRes);
                return false;
            }
        }

        // Commit
        ccSettings::ISettingsManager3QIPtr pSettingsMgr3 (m_pSettingsManager);
		if (pSettingsMgr3)
            symRes = pSettingsMgr3->PutSettings(m_pSettings, true); // Don't send events!
        else
        {
            assert(false);
            symRes = m_pSettingsManager->PutSettings(m_pSettings); // Fall back to old interface
        }

        if (FAILED(symRes))
        {
            CCTRACEE(_T("CExclusionSettingsManager::Commit() : "\
                "pSettingsManager->PutSettings() != SYM_OK, 0x%08X\n"), symRes);
            return false;
        }

        return true;
    }
}

#endif
