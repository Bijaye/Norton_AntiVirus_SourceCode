////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifndef __N32EXCLU_exclusionfileloader_cpp__
#define __N32EXCLU_exclusionfileloader_cpp__

#pragma warning( disable : 4290 )

#include "ExclusionFileLoader.h"
#include "ccMemoryInterface.h"
#include "ccSymMemoryStreamImpl.h"
#include "ccSymFileStreamImpl.h"
#include "ccSymKeyValueCollectionImpl.h"
#include "ccMessageLock.h"
#include "ccEvent.h"

namespace NavExclusions
{
    const DWORD CExclusionFileLoader::m_dwLoadWaitMax = 180000;    // 3 min
    const DWORD CExclusionFileLoader::m_dwLoadWaitInterval = 1000;  // 1 sec

    CExclusionFileLoader::CExclusionFileLoader(): m_bInitialized(false)
    {
    }

    CExclusionFileLoader::~CExclusionFileLoader()
    {
        if(m_bInitialized)
            this->uninitialize();

        m_bInitialized = false;
    }

    bool CExclusionFileLoader::isInitialized() 
    { return this->m_bInitialized; }

    ExResult CExclusionFileLoader::initialize(LPCTSTR pszFileName)
    {
        if(m_bInitialized)
        {
            CCTRACEE(_T("CExclusionFileLoader::initialize - Already ")\
                _T("initialized!"));
            return NavExclusions::Fail;
        }

        this->m_spFileStream = ccSym::CFileStreamImpl::CreateFileStreamImpl();
        if(!this->m_spFileStream)
        {
            CCTRACEE(_T("CExclusionFileLoader::initialize - Cannot create ")\
                        _T("File Stream."));
            return NavExclusions::OutOfMemory;
        }

        if(this->m_spFileStream->GetFile().Open(pszFileName, GENERIC_READ, 
                                                 NULL, NULL, OPEN_EXISTING, 
                                                 0, NULL) != TRUE)
        {
            CCTRACEE(_T("CExclusionFileLoader::initialize - Cannot open ")\
                        _T("the named file (%s)."), pszFileName);
            return NavExclusions::InvalidPath;
        }

        m_bInitialized = this->Load(EXCLUSIONS_SETTINGS_KEY);
        if(m_bInitialized)
        {
            CCTRACEI(_T("CExclusionFileLoader::initialize - Init OK!"));
            return NavExclusions::Success;
        }
        else
        {
            CCTRACEE(_T("CExclusionFileLoader::initialize - Could not ")\
                _T("initialize CC Settings."));
            if( GetSystemMetrics(SM_CLEANBOOT) != 0)
            {
                CCTRACEW(_T("CExclusionFileLoader::Initialize - Ignoring ccSettings ")\
                            _T("failure to load due to safe mode."));
                m_bInitialized = true;
                return NavExclusions::Success;
            }
            else
                return NavExclusions::NotInitialized;
        }
    }

    ExResult CExclusionFileLoader::uninitialize()
    {
        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionFileLoader::uninitialize - Not ")\
                _T("initialized!"));
            return NavExclusions::NotInitialized;
        }

        m_bInitialized = false;
        if(this->Commit(EXCLUSIONS_SETTINGS_KEY))
        {
            this->m_pSettings = NULL;
            this->m_pSettingsManager = NULL;
        }
        else
        {
            if(GetSystemMetrics(SM_CLEANBOOT) != 0)
            {
                CCTRACEW(_T("CExclusionFileLoader::uninitialize - Ignoring ")\
                            _T("commit failure due to Safe Mode."));
            }
            else
            {
                CCTRACEE(_T("CExclusionFileLoader::uninitialize - ")\
                    _T("Error during commit!"));
                return NavExclusions::Fail;
            }
        }

        if(this->m_spFileStream->GetFile().Close() != TRUE)
        {
            CCTRACEW(_T("CExclusionFileLoader::uninitialize - Error while ")\
                        _T("closing file."));
        }

        this->m_spFileStream.Release();

        return NavExclusions::Success;
    }

    ExResult CExclusionFileLoader::SetValue(LPCWSTR lpszName, LPBYTE pBuf,
        DWORD dwSize)
    {
        bool bException = false;

        if( !this->m_bInitialized || this->m_pSettings == NULL )
        {
            CCTRACEE(_T("CExclusionFileLoader::SetValue() - Not ")\
                _T("initialized properly."));
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

    bool CExclusionFileLoader::IsSettingsManagerActive(bool bSpinLock) 
    {
        if( GetSystemMetrics(SM_CLEANBOOT) != 0 )
        {
            CCTRACEW(_T("CExclusionFileLoader::IsSettingsManagerActive - Returning FALSE due to SAFE MODE."));
            return false;
        }

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

    bool CExclusionFileLoader::SettingsReady()
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
                CCTRACEE(_T("CExclusionFileLoader::SettingsReady() : ")\
                    _T("ccSettings.Create() != SYM_OK, 0x%08X\n"), symRes);
                return false;
            }
        }

        return true;
    }

    bool CExclusionFileLoader::Load (LPCWSTR lpszKey)
    {
        if ( !this->SettingsReady() )
            return false;

        // Create main key, if it doesn't exist
        SYMRESULT symRes = this->m_pSettingsManager->CreateSettings(lpszKey, 
            &(this->m_pSettings));
        if (SYM_FAILED(symRes))
        {
            CCTRACEE(_T("CExclusionFileLoader::Load() : ")\
                _T("pSettingsManager->GetSettings() != SYM_OK, 0x%08X\n"), 
                symRes);
            return false;
        }

        return true;
    }

    bool CExclusionFileLoader::Commit (LPCWSTR lpszKey)
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
                CCTRACEE(_T("CExclusionFileLoader::Commit() : pSettingsManager->CreateSettings() != SYM_OK, 0x%08X\n"), symRes);
                return false;
            }
        }

        // Commit
        symRes = m_pSettingsManager->PutSettings(m_pSettings);
        if (FAILED(symRes))
        {
            CCTRACEE(_T("CExclusionFileLoader::Commit() : ")\
                _T("pSettingsManager->PutSettings() != SYM_OK, 0x%08X\n"), symRes);
            return false;
        }

        return true;
    }

    ExResult CExclusionFileLoader::LoadNextItem(cc::IKeyValueCollection*& pExclusion)
    {
        if(!this->isInitialized())
        {
            CCTRACEE(_T("CExclusionFileLoader::saveItem - Not initialized."));
            return NavExclusions::NotInitialized;
        }

        if(pExclusion != NULL)
        {
            CCTRACEW(_T("CExclusionFileLoader::LoadNextItem - Received a ")\
                        _T("non-NULL collection, autoreleasing."));
            pExclusion->Release();
            pExclusion = NULL;
        }

        pExclusion = ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl();
        if(!pExclusion)
        {
            CCTRACEE(_T("CExclusionFileLoader::LoadNextItem - Couldn't create KVC."));
            pExclusion = NULL;
            return NavExclusions::OutOfMemory;
        }

        cc::ISerializeQIPtr pSerialize = pExclusion;
        if(!pSerialize)
        {
            CCTRACEE(_T("CExclusionFileLoader::LoadNextItem - Couldn't QI KVC ")\
                        _T("to ISerialize."));
            pExclusion->Release();
            pExclusion = NULL;
            return NavExclusions::NoInterface;
        }

        if(!pSerialize->Load(this->m_spFileStream))
        {
            if(this->m_spFileStream->GetFile().GetPosition()
                == this->m_spFileStream->GetFile().GetLength())
            {
                CCTRACEI(_T("CExclusionFileLoader::LoadNextItem - EOF."));
                pExclusion->Release();
                pExclusion = NULL;
                return NavExclusions::False;
            }
            else
            {
                CCTRACEE(_T("CExclusionFileLoader::LoadNextItem - Failed to load."));
                pExclusion->Release();
                pExclusion = NULL;
                return NavExclusions::Fail;
            }
        }

        return NavExclusions::Success;
    }

    ExResult CExclusionFileLoader::saveItem(cc::IKeyValueCollection *pCollection)
    {
        cc::IStringPtr spGuid;
        if(!this->isInitialized())
        {
            CCTRACEE(_T("CExclusionFileLoader::saveItem - Not initialized."));
            return NavExclusions::NotInitialized;
        }

        if(!pCollection)
        {
            CCTRACEE(_T("CExclusionFileLoader::saveItem - NULL argument."));
            return NavExclusions::InvalidArgument;
        }

        if(!pCollection->GetValue(IExclusion::Property::EGUID, (ISymBase*&)spGuid))
        {
            CCTRACEE(_T("CExclusionFileLoader::saveItem - ")\
                _T("Couldn't retrieve GUID from collection."));
            return NavExclusions::Fail;
        }

        cc::ISerializeQIPtr pSerialize(pCollection);
        if( pSerialize )
        {
            ccSym::CMemoryStreamImplPtr pMemStream;
            pMemStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
            if( pMemStream )
            {
                pSerialize->Save(pMemStream);
                pMemStream->SetPosition(0);
                ccLib::CMemory mem = pMemStream->GetMemory();
                DWORD dwSize = (DWORD)mem.GetSize();
                CCTRACEI(_T("CExclusionFileLoader::saveItem() - Saving %d ")\
                            _T("bytes of exclusion data to CC Settings."), dwSize);
                if(this->SetValue(spGuid->GetStringW(), (LPBYTE)mem.GetMemory(), 
                    dwSize) != NavExclusions::Success)
                {
                    CCTRACEE(_T("CExclusionFileLoader::saveItem()")\
                        _T(" - Failed to save serialized data to ccSettings."));
                    return NavExclusions::Fail;
                }
            }
            else
            {
                CCTRACEE(_T("CExclusionFileLoader::saveItem() ")\
                    _T("- Failed to create memory stream to serialize ")\
                    _T("exclusion to."));
                return NavExclusions::OutOfMemory;
            }
        }
        else
        {
            CCTRACEE(_T("CExclusionFileLoader::saveItem() - Failed to ")\
                _T("QI the collection to ISerialize. Cannot save."));
            return NavExclusions::NoInterface;
        }
        return NavExclusions::Success;
    }

    // If you want all states, set to NOT_EXCLUDED.
    // If you want all types, set to INVALID.
    ExResult CExclusionFileLoader::LoadAndFilter(IExclusion::ExclusionState requiredState,
                                                 IExclusion::ExclusionType requiredType)
    {
        cc::IKeyValueCollectionPtr spKVC = NULL;
        cc::IStringPtr spGuid;
        ExResult res;
        ULONG item;

        if(!this->isInitialized())
        {
            CCTRACEE(_T("CExclusionFileLoader::LoadAndFilter - Not initialized"));
            return NavExclusions::NotInitialized;
        }

        if(!this->IsSettingsManagerActive(false))
        {
            CCTRACEE(_T("CExclusionFileLoader::LoadAndFilter - Settings Mgr offline."));
            return NavExclusions::NotInitialized;
        }

        while((res = this->LoadNextItem(spKVC)) != NavExclusions::False)
        {
            if(res >= NavExclusions::Fail)
            {
                CCTRACEW(_T("CExclusionFileLoader::LoadAndFilter - Caught an ")\
                            _T("error while loading an item. Terminating."));
                break;
            }
            if(!spKVC)
            {
                CCTRACEW(_T("CExclusionFileLoader::LoadAndFilter - Null KVC in Loop"));
                continue;
            }
   
            if(!spKVC->GetValue(IExclusion::Property::EType, item))
            {
                CCTRACEW(_T("CExclusionFileLoader::LoadAndFilter - Can't get Etype, skipping"));
                spKVC.Release();
                spKVC = NULL;
                continue;
            }

            if(requiredType != IExclusion::ExclusionType::INVALID && item != requiredType)
            {
                CCTRACEI(_T("CExclusionFileLoader::LoadAndFilter - EType doesn't match, skipping"));
                spKVC.Release();
                spKVC = NULL;
                continue;
            }

            if(!spKVC->GetValue(IExclusion::Property::EState, item))
            {
                CCTRACEW(_T("CExclusionFileLoader::LoadAndFilter - Can't get EState, skipping"));
                spKVC.Release();
                spKVC = NULL;
                continue;
            }

            if(requiredState != IExclusion::ExclusionState::NOT_EXCLUDED && 
                (item & requiredState) != requiredState)
            {
                CCTRACEI(_T("CExclusionFileLoader::LoadAndFilter - EState doesn't match, skipping"));
                spKVC.Release();
                spKVC = NULL;
                continue;
            }

            if((res = this->saveItem(spKVC)) != NavExclusions::Success)
            {
                CCTRACEW(_T("CExclusionFileLoader::LoadAndFilter - Error while ")\
                            _T("saving item, res = %d"), res);
                spKVC.Release();
                spKVC = NULL;
                continue;
            }

            spKVC.Release();
            spKVC = NULL;
        }

        if(res >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CExclusionFileLoader::LoadAndFilter - Error while ")\
                        _T("loading next exclusion, result = %d"), res);
            return res;
        }
        else
        {
            return NavExclusions::Success;
        }
    }
}

#endif
