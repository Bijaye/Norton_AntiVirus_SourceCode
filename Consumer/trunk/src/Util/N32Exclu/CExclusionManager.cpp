////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * This file contains the external exclusions interface that will
 * be exported via SymInterface.
 */
#pragma once

#ifndef __N32EXCLU_cexclusionmanager_cpp__
#define __N32EXCLU_cexclusionmanager_cpp__

#include "ExclusionInterface.h"
#include "CExclusionManager.h"
#include "CAnomalyExclusion.h"
#include "CFileSystemExclusion.h"
#include "CAnomalyExclusionSet.h"
#include "CFileSystemExclusionSet.h"
#include "ExclusionManagerInterface.h"
#include "ccSymStringImpl.h"
#include "GlobalEvent.h"
#include "ccSymFileStreamImpl.h"
#include "ExclusionFileLoader.h"
#include "ccSymInstalledApps.h"

namespace NavExclusions
{
    CExclusionManager::CExclusionManager()
    {
        m_bInitialized = false;
        m_bAutoCommit = false;
        if(this->m_CriticalSection.Create() != TRUE)
        {
            CCTRACEE(_T("CExclusionManager::CExclusionManager - ")\
                        _T("Could not create critical section."));
            // TODO: Throw an exception or something?
        }
    }

    CExclusionManager::~CExclusionManager()
    {
        if(m_bInitialized)
            this->uninitialize();
        this->m_CriticalSection.Destroy();
    }

    /*** PRECONDITION: Construction of Critical Section must have succeeded. */
    ExResult CExclusionManager::initialize(bool bCommitOnTeardown)
    {
        CCTRACEI(_T("CExclusionManager::initialize - Starting init"));
        assert(this->m_CriticalSection.IsCriticalSection());
        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        // Don't wait for the settings manager in safe mode because it's not running
        if( GetSystemMetrics(SM_CLEANBOOT) != 0)
        {
            CCTRACEW(_T("CExclusionManager::initialize - The system is in safe mode, ")\
                        _T("exclusions cannot be saved. The only exclusions in the ")\
                        _T("system will be those in the defaults file."));
        }

        if(this->m_bInitialized)
        {
            CCTRACEI(_T("CExclusionManager::initialize - Already initialized."));
            return NavExclusions::Success;
        }

        m_bAutoCommit = bCommitOnTeardown;
        this->m_Factory.resetTopId();
        this->m_Factory.dumpIdStack();
        this->m_bInitialized = true;

        ExResult resReloadState = this->reloadState();
        if( NavExclusions::Success != resReloadState )
        {
            CCTRACEE(_T("CExclusionManager::initialize - ReloadState() failed with, ")\
                        _T("error %d"), resReloadState);
            this->m_bInitialized = false;
            return resReloadState;
        }

        CCTRACEI(_T("CExclusionManager::initialize - Init OK!"));
        return NavExclusions::Success;
    }

    ExResult CExclusionManager::uninitialize(bool bForceCommit)
    {
        CCTRACEI(_T("CExclusionManager::uninitialize - Uninitializing..."));
        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEI(_T("CExclusionManager::uninitialize - Already uninitialized."));
            return NavExclusions::Success;
        }

        if(this->m_bAutoCommit || bForceCommit)
            this->saveState();

        this->m_setAE.nuke();
        this->m_setFSE.nuke();
        this->nukeDeque();
        this->m_Factory.resetTopId();
        this->m_Factory.dumpIdStack();
        this->m_bInitialized = false;
        CCTRACEI(_T("CExclusionManager::uninitialize - Uninit OK!"));
        return NavExclusions::Success;
    }

    ExResult CExclusionManager::clearState()
    {
        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        CCTRACEI(_T("CExclusionManager::clearState - Clearing..."));
        if(!this->m_bInitialized)
            return NavExclusions::NotInitialized;

        this->m_setAE.nuke();
        this->m_setFSE.nuke();
        this->nukeDeque();
        this->m_Factory.resetTopId();
        this->m_Factory.dumpIdStack();

        CCTRACEI(_T("CExclusionManager::clearState - Cleared."));
        return NavExclusions::Success;
    }

    ExResult CExclusionManager::saveState()
    {
        std::deque<IExclusion*>::iterator it;
        bool bAnyFailures = false;

        CCTRACEI(_T("CExclusionManager::saveState - Starting."));

        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::saveState - Manager not initialized."));
            return NavExclusions::NotInitialized;
        }

        if(GetSystemMetrics(SM_CLEANBOOT) != 0)
        {   // Safe mode. Reload from file.
            CCTRACEW(_T("CExclusionManager::saveState - Not allowed in safe mode."));
            return NavExclusions::Success;
        }

        if(this->m_Settings.initialize() != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionManager::saveState - Settings Manager ")\
                        _T("could not be initialized."));
            return NavExclusions::NotInitialized;
        }

        for(it = this->m_dqExclusions.begin();
            it != this->m_dqExclusions.end();
            it++)
        {
            if(!(*it)) { continue; } // Skip nulls
            else
            {
                if(this->m_Settings.saveExclusion((*it)) >= NavExclusions::Fail)
                {
                    CCTRACEW(_T("CExclusionManager::saveState - Settings ")\
                                _T("manager reported a failure to properly save ")\
                                _T("or delete an exclusion."));
                    bAnyFailures = true;
                }
            }
        }

        // Commit changes
        if(this->m_Settings.uninitialize() != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionManager::saveState - Could not commit and ")\
                        _T("uninitialize Settings Manager."));
            return NavExclusions::Fail;
        }

        CCTRACEI(_T("CExclusionManager::saveState - Saved, sending change event."));

        // Send event for AP Svc and any other listeners if we succeeded
        if(!bAnyFailures && this->sendChangeEvent())
            return NavExclusions::Success;
        else
            return NavExclusions::Fail;
    }

    ExResult CExclusionManager::reloadState()
    {
        IExclusionPtr pExclusion;
        cc::IKeyValueCollectionPtr pData;
        ExResult res;
        IExclusion::ExclusionType eType;
        ExclusionId myId;
        CCTRACEI(_T("CExclusionManager::reloadState - Starting."));

        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::reloadState - Manager not initialized."));
            return NavExclusions::NotInitialized;
        }

        if(GetSystemMetrics(SM_CLEANBOOT) != 0)
        {   // Safe mode. Reload from file.
            CCTRACEW(_T("CExclusionManager::reloadState - SAFE MODE, loading defaults from file."));
            return this->resetToDefaults((IExclusion::ExclusionState)0,(IExclusion::ExclusionType)0);
        }

        if(this->m_Settings.initialize() != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionManager::reloadState - Settings Manager ")\
                _T("could not be initialized."));
            return NavExclusions::NotInitialized;
        }

        if(this->m_Settings.enumLoadReset() != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionManager::reloadState - Could not reset ")\
                        _T("settings enumeration."));
            this->m_Settings.uninitialize();
            return NavExclusions::Fail;
        }

        /* Eliminate current state */
        if(this->clearState() != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionManager::reloadState - Could not reset ")\
                        _T("internal state with clearState()!"));
            return NavExclusions::Fail;
        }

        pData = NULL;

        while(1)
        {
            res = this->m_Settings.enumLoadExclusionData(pData);
            if(res == NavExclusions::False)
            {   // Nothing left
                pData = NULL;
                break;
            }
            else if(res >= NavExclusions::Fail)
            {
                pData = NULL;
                break;
            }
            else
            {   // Success!
                res = this->m_Factory.rebuildExclusion(pExclusion, pData);
                if(res != NavExclusions::Success)
                {
                    pData = NULL;
                    pExclusion = NULL;
                    break;
                }

                myId = pExclusion->getID();
                if(myId == _INVALID_ID)
                {
                    CCTRACEW(_T("CExclusionManager::reloadState - Received an ")\
                                _T("exclusion with a bad ID. Releasing."));
                    this->m_Factory.destroyExclusion(pExclusion);
                    pExclusion = NULL;
                    pData = NULL;
                    continue;
                }

                ExclusionId dupId;
                eType = pExclusion->getExclusionType();
                if(eType == IExclusion::ExclusionType::ANOMALY)
                {
                    IAnomalyExclusionQIPtr qi = pExclusion;
                    cc::IStringPtr spGuid;
                    ULONG ulVid;

                    if(!qi)
                    {
                        CCTRACEW(_T("CExclusionManager::reloadState - Couldn't ")\
                                    _T("QI to Anomaly exclusion; ignoring."));
                        res = NavExclusions::NoInterface;
                        this->m_Factory.destroyExclusion(pExclusion);
                        pExclusion = NULL;
                        pData = NULL;
                        qi = NULL;
                        continue;
                    }

                    ulVid = qi->getVid();
                    if(qi->getAnomalyGuid(spGuid) >= NavExclusions::Fail)
                    {
                        CCTRACEW(_T("CExclusionManager::reloadState - Failure ")\
                                    _T("when retrieving GUID from anomaly exclusion. "));
                        continue;
                    }

                    dupId = this->m_setAE.findExclusionId(spGuid, ulVid);
                    if(dupId != _INVALID_ID)
                    {
                        if(!this->removeExclusion(dupId))
                        {
                            CCTRACEW(_T("CExclusionManager::reloadState - ")\
                                        _T("Couldn't remove duplicate AE!"));
                            continue;
                        }
                    }

                    if(!this->putInDeque((size_t)myId, pExclusion))
                    {
                        CCTRACEE(_T("CExclusionManager::reloadState - Error ")\
                            _T("while placing exclusion in deque!"));
                        return NavExclusions::OutOfMemory;
                    }

                    res = this->m_setAE.addExclusion(qi);
                    if(res != NavExclusions::Success)
                    {
                        CCTRACEE(_T("CExclusionManager::reloadState - Error while ")\
                                    _T("adding item to AE set. ExResult = %lu"), res);
                        return res;
                    }
                    pExclusion->AddRef();
                }
                else if(eType == IExclusion::ExclusionType::FILESYSTEM)
                {
                    IFileSystemExclusionQIPtr qi = pExclusion;
                    cc::IStringPtr spPath;

                    if(!qi)
                    {
                        CCTRACEW(_T("CExclusionManager::reloadState - Couldn't ")\
                            _T("QI to FS exclusion; ignoring."));
                        res = NavExclusions::NoInterface;
                        this->m_Factory.destroyExclusion(pExclusion);
                        pExclusion = NULL;
                        pData = NULL;
                        qi = NULL;
                        continue;
                    }

                    if(qi->getFSPath(spPath) >= NavExclusions::Fail)
                    {
                        CCTRACEW(_T("CExclusionManager::reloadState - Failure ")\
                            _T("when retrieving path from FS exclusion. "));
                        continue;
                    }

                    dupId = this->m_setFSE.findExclusionId(spPath);
                    if(dupId != _INVALID_ID)
                    {
                        if(!this->removeExclusion(dupId))
                        {
                            CCTRACEW(_T("CExclusionManager::reloadState - ")\
                                _T("Couldn't remove duplicate FSE!"));
                            continue;
                        }
                    }

                    if(!this->putInDeque((size_t)myId, pExclusion))
                    {
                        CCTRACEE(_T("CExclusionManager::reloadState - Error ")\
                            _T("while placing exclusion in deque!"));
                        return NavExclusions::OutOfMemory;
                    }

                    res = this->m_setFSE.addExclusion(qi);
                    if(res != NavExclusions::Success)
                    {
                        CCTRACEE(_T("CExclusionManager::reloadState - Error while ")\
                            _T("adding item to FSE set. ExResult = %lu"), res);
                        return res;
                    }
                    pExclusion->AddRef();
                }
                else
                {
                    CCTRACEE(_T("CExclusionManager::reloadState - Received an ")\
                        _T("exclusion with an invalid type. Releasing."));
                    this->m_Factory.destroyExclusion(pExclusion);
                }
                pExclusion.Release();
                pExclusion = NULL;
                pData.Release();
                pData = NULL;
            }   // End Else: enumLoadExclusionData == Success
        } // End while(1)

        this->m_Settings.uninitialize();
        CCTRACEI(_T("CExclusionManager::reloadState - Reload loop completed."));

        if(res >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CExclusionManager::reloadState - Encountered an ")\
                        _T("error while loading KVC objects."));
            return res;
        }
        else
            return NavExclusions::Success;
    }

    ExResult CExclusionManager::replaceExclusion(const ExclusionId whichExclusion,
                                                 IExclusion* replacement)
    {
        IExclusion* existingExclusion;
        IExclusion::ExclusionType eType;
        IExclusion::ExclusionState eState;
        ExclusionId dupId;
        CCTRACEI(_T("CExclusionManager::replaceExclusion - Starting."));

        if(!replacement || 
            replacement->isDeleted() != NavExclusions::False || 
            replacement->isExpired() != NavExclusions::False)
        {
            CCTRACEE(_T("CExclusionManager::replaceExclusion - Cannot insert a ")\
                        _T("NULL, deleted or expired exclusion using replace."));
            return NavExclusions::InvalidArgument;
        }

        eState = replacement->getState();
        if(!(eState & IExclusion::ExclusionState::EXCLUDE_BOTH))
        {   // If we haven't got VIRAL or NONVIRAL set (or both), invalid
            CCTRACEE(_T("CExclusionManager::replaceExclusion - Cannot insert ")\
                        _T("an exclusion without settings either the VIRAL ")\
                        _T("or NONVIRAL state flags."));
            return NavExclusions::InvalidArgument;
        }

        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::replaceExclusion - Not initialized."));
            return NavExclusions::NotInitialized;
        }

        if(whichExclusion <= _INVALID_ID || whichExclusion >= this->m_dqExclusions.size())
        {
            CCTRACEE(_T("CExclusionManager::replaceExclusion - Received an ")\
                        _T("invalid exclusion ID."));
            return NavExclusions::InvalidArgument;
        }

        existingExclusion = (IExclusion*) this->m_dqExclusions.at((size_t)whichExclusion);
        if(!existingExclusion)
        {
            CCTRACEI(_T("CExclusionManager::replaceExclusion - Couldn't find ")\
                        _T("an exclusion at the indicated position."));
            return NavExclusions::False;
        }
        else if(existingExclusion->isDeleted() == NavExclusions::Success ||
            existingExclusion->isExpired() == NavExclusions::Success)
        {
            CCTRACEI(_T("CExclusionManager::replaceExclusion - Found ")\
                        _T("a deleted/expired exclusion at the indicated ")\
                        _T("position. Replacement is illegal."));
            return NavExclusions::False;
        }

        eType = existingExclusion->getExclusionType();

        if(eType != replacement->getExclusionType())
        {
            CCTRACEE(_T("CExclusionManager::replaceExclusion - Existing exclusion ")\
                        _T("is not the same type as its replacement!"));
            return NavExclusions::Fail;
        }

        if(eType == IExclusion::ExclusionType::ANOMALY)
        {
            IAnomalyExclusionPtr spAE;

            spAE.Attach((IAnomalyExclusion*)replacement->clone());
            if(!spAE)
            {
                CCTRACEE(_T("CExclusionManager::replaceExclusion - Could not ")\
                            _T("duplicate input exclusion for internal storage."));
                return NavExclusions::OutOfMemory;
            }

            dupId = this->m_setAE.findDuplicate(spAE);
            if(dupId != _INVALID_ID && dupId != spAE->getID())
            {
                if(!this->mergeExclusions(dupId, 
                                          replacement->getExpiryStamp(),
                                          replacement->getState()))
                {
                    CCTRACEE(_T("CExclusionManager::replaceExclusion - Cannot ")\
                                _T("add duplicate exclusions via replace!"));
                    return NavExclusions::Fail;
                }
                else
                {
                    if(!this->removeExclusion(whichExclusion))
                    {
                        CCTRACEE(_T("CExclusionManager::replaceExclusion - Could not ")\
                            _T("remove duplicate exclusion when trying to ")\
                            _T("perform a merge."));
                        return NavExclusions::Fail;
                    }
                    CCTRACEI(_T("CExclusionManager::replaceExclusion - Found ")\
                                _T("dupe; merged."));
                    return NavExclusions::Success;
                }
            }

            if(this->m_setAE.removeExclusion(whichExclusion) != NavExclusions::Success)
            {
                CCTRACEW(_T("CExclusionManager::replaceExclusion - Existing ")\
                    _T("exclusion not found in AE set or could not be ")\
                    _T("removed! Type-sets may be desynchronized from the ")\
                    _T("manager!"));
            }

            if(!this->putInDeque((size_t)whichExclusion, spAE))
            {
                CCTRACEE(_T("CExclusionManager::replaceExclusion - Could not ")\
                            _T("place new item into the deque! Out of memory?"));
                return NavExclusions::OutOfMemory;
            }

            spAE->AddRef();
            if(this->m_setAE.addExclusion(spAE) != NavExclusions::Success)
            {
                CCTRACEE(_T("CExclusionManager::replaceExclusion - Could not ")\
                            _T("add exclusion to the type-set! Set has ")\
                            _T("desynchronized from the manager."));
                return NavExclusions::Fail;
            }
            spAE.Release();

            // Not using the factory as we're not releasing an ID,
            // just replacing an object.
            existingExclusion->Release();
                            
        }
        else if(eType == IExclusion::ExclusionType::FILESYSTEM)
        {
            IFileSystemExclusionPtr spFSE;

            spFSE.Attach((IFileSystemExclusion*)replacement->clone());
            if(!spFSE)
            {
                CCTRACEE(_T("CExclusionManager::replaceExclusion - Could not ")\
                            _T("duplicate input exclusion for internal storage."));
                return NavExclusions::OutOfMemory;
            }

            dupId = this->m_setFSE.findDuplicate(spFSE);
            if(dupId != _INVALID_ID && dupId != spFSE->getID())
            {
                if(!this->mergeExclusions(dupId, 
                    replacement->getExpiryStamp(),
                    replacement->getState()))
                {
                    CCTRACEE(_T("CExclusionManager::replaceExclusion - Cannot ")\
                        _T("add duplicate exclusions via replace!"));
                    return NavExclusions::Fail;
                }
                else
                {
                    if(!this->removeExclusion(whichExclusion))
                    {
                        CCTRACEE(_T("CExclusionManager::replaceExclusion - Could not ")\
                                    _T("remove duplicate exclusion when trying to ")\
                                    _T("perform a merge."));
                        return NavExclusions::Fail;
                    }
                    CCTRACEI(_T("CExclusionManager::replaceExclusion - Found ")\
                        _T("dupe; merged."));
                    return NavExclusions::Success;
                }
            }

            if(this->m_setFSE.removeExclusion(whichExclusion) != NavExclusions::Success)
            {
                CCTRACEW(_T("CExclusionManager::replaceExclusion - Existing ")\
                            _T("exclusion not found in FSE set or could not be ")\
                            _T("removed! Type-sets may be desynchronized from the ")\
                            _T("manager!"));
            }

            if(!this->putInDeque((size_t)whichExclusion, spFSE))
            {
                CCTRACEE(_T("CExclusionManager::replaceExclusion - Could not ")\
                    _T("place new item into the deque! Out of memory?"));
                return NavExclusions::OutOfMemory;
            }

            spFSE->AddRef();
            if(this->m_setFSE.addExclusion(spFSE) != NavExclusions::Success)
            {
                CCTRACEE(_T("CExclusionManager::replaceExclusion - Could not ")\
                    _T("add exclusion to the type-set! Set has ")\
                    _T("desynchronized from the manager."));
                return NavExclusions::Fail;
            }
            spFSE.Release();

            // Not using the factory as we're not releasing an ID,
            // just replacing an object.
            existingExclusion->Release();
        }
        else
        {
            CCTRACEE(_T("CExclusionManager::replaceExclusion - Incoming ")\
                        _T("exclusion is Invalid!"));
            return NavExclusions::InvalidArgument;
        }

        CCTRACEI(_T("CExclusionManager::replaceExclusion - Replace OK!"));

        return NavExclusions::Success;
    }

    ExclusionId CExclusionManager::addExclusionAnomaly(const ULONG ulVid,
                                                    const cc::IString* pStrGuid,
                                                    time_t tExpiry,
                                              const cc::IString* pStrThreatName,
                                        IExclusion::ExclusionState excludeWhich)
    {
        IAnomalyExclusion* pAE = NULL;
        ExclusionId myId = _INVALID_ID;
        ExResult res;

        CCTRACEI(_T("CExclusionManager::addExclusionAnomaly - Starting."));

        if((excludeWhich & IExclusion::ExclusionState::EXCLUDE_BOTH) == 0)
        {
            CCTRACEE(_T("CExclusionManager::addExclusionAnomaly - Passed in a ")\
                        _T("State lacking viral/nonviral flags - invalid argument."));
            return _INVALID_ID;
        }

        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::addExclusionAnomaly - Not initialized."));
            return _INVALID_ID;
        }

        myId = this->findExclusionAnomaly(ulVid, pStrGuid);
        if(myId != _INVALID_ID)
        {
            CCTRACEI(_T("CExclusionManager::addExclusionAnomaly - Merging duplicates."));

            if(this->mergeExclusions(myId, tExpiry, excludeWhich))
            {
                CCTRACEI(_T("CExclusionManager::addExclusionAnomaly - Merge OK!"));
                return myId;
            }

            CCTRACEI(_T("CExclusionManager::addExclusionAnomaly - Merge not possible, adding instead."));
        }

        res = this->m_Factory.createExclusion((IExclusion*&)pAE, 
                                             IExclusion::ExclusionType::ANOMALY,
                                             tExpiry, true, excludeWhich, 
                                             pStrThreatName, pStrGuid, &ulVid);
        if(res == NavExclusions::Success)
        {
            myId = pAE->getID();
            if(myId <= _INVALID_ID)
            {
                CCTRACEE(_T("CExclusionManager::addExclusionAnomaly - ")\
                            _T("CExclusionFactory returned an object with an ")\
                            _T("invalid Exclusion ID."));
                this->m_Factory.destroyExclusion((IExclusion*&)pAE);
                return _INVALID_ID;
            }

            if(!this->putInDeque((size_t)myId, pAE))
            {
                CCTRACEE(_T("CExclusionManager::addExclusionAnomaly - ")\
                            _T("Could not place new exclusion into the deque. ")\
                            _T("Probably out of memory."));
                this->m_Factory.destroyExclusion((IExclusion*&)pAE);
                return _INVALID_ID;
            }

            res = this->m_setAE.addExclusion(pAE);
            if(res != NavExclusions::Success)
            {
                CCTRACEE(_T("CExclusionManager::addExclusionAnomaly - ")\
                            _T("Could not add exclusion to type-set."));
                this->m_Factory.destroyExclusion((IExclusion*&)pAE);
                return _INVALID_ID;
            }

            CCTRACEI(_T("CExclusionManager::addExclusionAnomaly - Add OK!"));
            return myId;
        }

        CCTRACEE(_T("CExclusionManager::addExclusionAnomaly - Factory failed ")\
                    _T("to create an Anomaly exclusion."));
        if(pAE != NULL) { this->m_Factory.destroyExclusion((IExclusion*&)pAE); }
        return _INVALID_ID;
    }

    ExclusionId CExclusionManager::addExclusionFS(const char* pszFilePath,
                                                  time_t tExpiry,
                                        IExclusion::ExclusionState excludeWhich,
                                                  bool bIsOem)
    {
        cc::IStringPtr spString;
        ExclusionId myId;

        CCTRACEI(_T("CExclusionManager::addExclusionFS (CSTR) - Starting."));

        if(!pszFilePath)
        {
            CCTRACEE(_T("CExclusionManager::addExclusionFS - Received a NULL ")\
                _T("character string; invalid argument."));
            return _INVALID_ID;
        }

        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::addExclusionFS - Not initialized."));
            return _INVALID_ID;
        }

        if(!this->mbcsToUnicode(spString, pszFilePath, bIsOem))
        {
            CCTRACEE(_T("CExclusionManager::addExclusionFS - Could not ")\
                        _T("convert MBCS string to Unicode."));
            return _INVALID_ID;
        }

        if(!spString)
        {
            CCTRACEE(_T("CExclusionManager::addExclusionFS - Invalid unicode ")\
                        _T("string returned from mbcsToUnicode."));
            return _INVALID_ID;
        }

        // Let the other function do the rest of the work.
        myId = this->addExclusionFS(spString, tExpiry, excludeWhich);
        spString.Release();
        CCTRACEI(_T("CExclusionManager::addExclusionFS (CSTR) - Finished."));

        return myId;
    }

    ExclusionId CExclusionManager::addExclusionFS(const cc::IString* pStrUnicodePath,
                                                  time_t tExpiry,
                                        IExclusion::ExclusionState excludeWhich)
    {
        cc::IStringPtr spStr;
        IFileSystemExclusion* pFSE = NULL;
        ExclusionId myId = _INVALID_ID;
        ExResult res;

        CCTRACEI(_T("CExclusionManager::addExclusionFS (ISTR) - Starting."));

        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::addExclusionFS - Not initialized."));
            return _INVALID_ID;
        }

        if((excludeWhich & IExclusion::ExclusionState::EXCLUDE_BOTH) == 0)
        {
            CCTRACEE(_T("CExclusionManager::addExclusionFS - Passed in a State ")\
                        _T("lacking viral/nonviral flags - invalid argument."));
            return _INVALID_ID;
        }

        if(!pStrUnicodePath)
        {
            CCTRACEE(_T("CExclusionManager::addExclusionFS - Passed in a null path."));
            return _INVALID_ID;
        }
        else
        {
            spStr = ccSym::CStringImpl::CreateStringImpl(pStrUnicodePath->GetStringW());
            if(!this->lopOffBackslash(spStr))
            {
                CCTRACEE(_T("CExclusionManager::addExclusionFS - Cannot remove backslash."));
                return _INVALID_ID;
            }
        }

        myId = this->findExclusionFS(spStr);
        if(myId != _INVALID_ID)
        {
            CCTRACEI(_T("CExclusionManager::addExclusionFS (ISTR) - Dupe found, merging."));
            if(this->mergeExclusions(myId, tExpiry, excludeWhich))
            {
                CCTRACEI(_T("CExclusionManager::addExclusionFS (ISTR) - Done."));
                return myId;
            }
            CCTRACEI(_T("CExclusionManager::addExclusionFS (ISTR) - Merge not possible, adding."));
        }

        res = this->m_Factory.createExclusion((IExclusion*&)pFSE, 
                                             IExclusion::ExclusionType::FILESYSTEM,
                                             tExpiry, true,
                                             excludeWhich,
                                             NULL, spStr, NULL);

        if(res == NavExclusions::Success)
        {
            myId = pFSE->getID();
            if(myId <= _INVALID_ID)
            {
                CCTRACEE(_T("CExclusionManager::addExclusionFS - ")\
                            _T("CExclusionFactory returned an object with an ")\
                            _T("invalid Exclusion ID."));
                this->m_Factory.destroyExclusion((IExclusion*&)pFSE);
                return _INVALID_ID;
            }

            if(!this->putInDeque((size_t)myId, pFSE))
            {
                CCTRACEE(_T("CExclusionManager::addExclusionFS - ")\
                    _T("Could not place new exclusion into the deque. ")\
                    _T("Probably out of memory."));
                this->m_Factory.destroyExclusion((IExclusion*&)pFSE);
                return _INVALID_ID;
            }

            res = this->m_setFSE.addExclusion(pFSE);
            if(res != NavExclusions::Success)
            {
                CCTRACEE(_T("CExclusionManager::addExclusionFS - ")\
                    _T("Could not add exclusion to type-set."));
                this->m_Factory.destroyExclusion((IExclusion*&)pFSE);
                return _INVALID_ID;
            }

            CCTRACEI(_T("CExclusionManager::addExclusionFS (ISTR) - Add OK!"));
            return myId;
        }

        CCTRACEE(_T("CExclusionManager::addExclusionFS - Factory failed ")\
                    _T("to create a FileSystem exclusion."));
        if(pFSE != NULL) { this->m_Factory.destroyExclusion((IExclusion*&)pFSE); }
        return _INVALID_ID;
    }

    ExclusionId CExclusionManager::findExclusionAnomaly(const ULONG ulVid,
                                                        const cc::IString* pStrGuid)
    {
        CCTRACEI(_T("CExclusionManager::findExclusionAnomaly - Starting."));

        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);
        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::findExclusionAnomaly - Not initialized."));
            return _INVALID_ID;
        }

        CCTRACEI(_T("CExclusionManager::findExclusionAnomaly - Passing into AE typeset."));

        return this->m_setAE.findExclusionId(pStrGuid, ulVid);
    }

    ExclusionId CExclusionManager::findExclusionFS(const char* pszFilePath,
                                                bool bIsOem)
    {
        cc::IStringPtr spString;
        CCTRACEI(_T("CExclusionManager::findExclusionFS (CSTR) - Starting."));

        if(!pszFilePath)
        {
            CCTRACEE(_T("CExclusionManager::findExclusionFS - Received a NULL ")\
                _T("character string; invalid argument."));
            return _INVALID_ID;
        }

        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::findExclusionFS - Not initialized."));
            return _INVALID_ID;
        }

        if(!this->mbcsToUnicode(spString, pszFilePath, bIsOem))
        {
            CCTRACEE(_T("CExclusionManager::findExclusionFS - Could not ")\
                        _T("convert MBCS string to Unicode."));
            return _INVALID_ID;
        }

        if(!spString)
        {
            CCTRACEE(_T("CExclusionManager::findExclusionFS - Invalid unicode ")\
                        _T("string returned from mbcsToUnicode."));
            return _INVALID_ID;
        }

        CCTRACEI(_T("CExclusionManager::findExclusionFS (CSTR) - Conversion OK."));

        // Let the other function do the rest of the work.
        return this->findExclusionFS(spString);
    }

    ExclusionId CExclusionManager::findExclusionFS(const cc::IString* pStrFilePath)
    {
        cc::IStringPtr spStr;
        CCTRACEI(_T("CExclusionManager::findExclusionFS (ISTR) - Starting."));
        
        if(!pStrFilePath)
        {
            CCTRACEE(_T("CExclusionManager::findExclusionFS - NULL path is not valid for match."));
            return _INVALID_ID;
        }

        spStr = ccSym::CStringImpl::CreateStringImpl(pStrFilePath->GetStringW());
        if(!this->lopOffBackslash(spStr))
        {
            CCTRACEE(_T("CExclusionManager::findExclusionFS - Could not lop off backslash."));
            return _INVALID_ID;
        }

        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::findExclusionFS - Not initialized."));
            return _INVALID_ID;
        }

        CCTRACEI(_T("CExclusionManager::findExclusionFS (ISTR) - Passing into typeset."));

        return this->m_setFSE.findExclusionId(spStr);
    }

    IExclusion::ExclusionState CExclusionManager::isExcludedAnomaly(
                                                    const ULONG ulVid,
                                                    const cc::IString* pStrGuid, 
                                                    IExclusion::ExclusionState requiredState)
    {
        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::isExcludedAnomaly - Not initialized."));
            return IExclusion::ExclusionState::NOT_EXCLUDED;
        }

        return this->m_setAE.isExcluded(pStrGuid, ulVid, requiredState);
    }

    IExclusion::ExclusionState CExclusionManager::isExcludedFS(
                                                        const char* pszFilePath,
                                                        bool bIsOem, 
                                                        IExclusion::ExclusionState requiredState)
    {
        cc::IStringPtr spString;
        if(!pszFilePath)
        {
            CCTRACEE(_T("CExclusionManager::isdExcludedFS - Received a NULL ")\
                _T("character string; invalid argument."));
            return IExclusion::ExclusionState::NOT_EXCLUDED;
        }

        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::isExcludedFS - Not initialized."));
            return IExclusion::ExclusionState::NOT_EXCLUDED;
        }

        if(!this->mbcsToUnicode(spString, pszFilePath, bIsOem))
        {
            CCTRACEE(_T("CExclusionManager::isExcludedFS - Could not ")\
                        _T("convert MBCS string to Unicode."));
            return IExclusion::ExclusionState::NOT_EXCLUDED;
        }

        if(!spString)
        {
            CCTRACEE(_T("CExclusionManager::isExcludedFS - Invalid unicode ")\
                        _T("string returned from mbcsToUnicode."));
            return IExclusion::ExclusionState::NOT_EXCLUDED;
        }

        // Let the other function do the rest of the work.
        return this->isExcludedFS(spString, requiredState);
    }

    IExclusion::ExclusionState CExclusionManager::isExcludedFS(
        const cc::IString* pStrUnicodePath, 
        IExclusion::ExclusionState requiredState)
    {
        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::isExcludedFS - Not initialized."));
            return IExclusion::ExclusionState::NOT_EXCLUDED;
        }

        return this->m_setFSE.isExcluded(pStrUnicodePath, requiredState);
    }

    bool CExclusionManager::removeExclusion(const ExclusionId exclusionId)
    {
        IExclusion *ptr;
        IExclusion::ExclusionType eType;

        CCTRACEI(_T("CExclusionManager::removeExclusion - Starting."));
        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::removeExclusion - Not initialized."));
            return false;
        }

        if(exclusionId >= this->m_dqExclusions.size())
        {
            CCTRACEE(_T("CExclusionManager::removeExclusion - Invalid ID ")\
                        _T("received as argument."));
            return false;
        }

        CCTRACEI(_T("CExclusionManager::removeExclusion - Retrieving exclusion with ID %d"), exclusionId);

        ptr = this->m_dqExclusions[(const size_t)exclusionId];
        if(!ptr)
        {
            CCTRACEW(_T("CExclusionManager::removeExclusion - ID received ")\
                        _T("references empty slot."));
            return false;
        }

        if(ptr->isDeleted() == NavExclusions::Success || 
            ptr->isExpired() == NavExclusions::Success)
        {
            CCTRACEI(_T("CExclusionManager::removeExclusion - ID received ")\
                        _T("references an exclusion which has already been ")\
                        _T("removed/deleted."));
        }

        eType = ptr->getExclusionType();
        if(eType == IExclusion::ExclusionType::ANOMALY)
        {
            if(this->m_setAE.removeExclusion(exclusionId) != NavExclusions::Success)
            {
                CCTRACEW(_T("CExclusionManager::removeExclusion - Error while ")\
                            _T("removing anomaly exclusion from typeset."));
            }
        }
        else if(eType == IExclusion::ExclusionType::FILESYSTEM)
        {
            if(this->m_setFSE.removeExclusion(exclusionId) != NavExclusions::Success)
            {
                CCTRACEW(_T("CExclusionManager::removeExclusion - Error while ")\
                    _T("removing FS exclusion from typeset."));
            }
        }

        if(ptr->setDeleted(true) != NavExclusions::Success)
        {
            CCTRACEW(_T("CExclusionManager::removeExclusion - Setting ")\
                        _T("deletion flag did not succeed; exclusion ")\
                        _T("might not be deleted."));
        }

        CCTRACEI(_T("CExclusionManager::removeExclusion - Remove complete."));

        return true;
    }

    ExResult CExclusionManager::getExclusion(const ExclusionId exclusionId,
                                                IExclusion* &pExclusion)
    {
        CCTRACEI(_T("CExclusionManager::getExclusion - Starting!"));

        if(pExclusion != NULL)
        {
            CCTRACEW(_T("CExclusionManager::getExclusion - Output exclusion ")\
                        _T("argument was not set to NULL; autoreleasing."));
            pExclusion->Release();
            pExclusion = NULL;
        }

        if(exclusionId <= _INVALID_ID)
        {
            CCTRACEE(_T("CExclusionManager::getExclusion - Received an ")\
                        _T("invalid Exclusion ID."));
            return NavExclusions::InvalidArgument;
        }

        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::getExclusion - Not initialized."));
            return NavExclusions::NotInitialized;
        }

        if(exclusionId >= this->m_dqExclusions.size())
        {
            CCTRACEE(_T("CExclusionManager::getExclusion - Received an ")\
                _T("invalid Exclusion ID."));
            return NavExclusions::InvalidArgument;
        }

        pExclusion = this->m_dqExclusions.at((size_t)exclusionId);
        if(!pExclusion)
        {
            CCTRACEW(_T("CExclusionManager::getExclusion - Found a NULL cell ")\
                        _T("at the requested ID address."));
            return NavExclusions::False;
        }
        else if(pExclusion->isDeleted() == NavExclusions::Success || 
                pExclusion->isExpired() == NavExclusions::Success)
        {
            CCTRACEI(_T("CExclusionManager::getExclusion - Found a deleted or ")\
                        _T("expired exclusion at the requested ID address."));
            return NavExclusions::False;
        } 
        else
        {
            CCTRACEI(_T("CExclusionManager::getExclusion - Cloning and returning."));

            pExclusion = pExclusion->clone();
            if(pExclusion)
                return NavExclusions::Success;
            else
                return NavExclusions::Fail;
        }
    }

    bool CExclusionManager::isAutoCommit() 
    { 
        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);
        return this->m_bAutoCommit; 
    }

    ExResult CExclusionManager::setAutoCommit(bool bAutoCommit)
    {
        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::setAutoCommit - Not initialized."));
            return NavExclusions::NotInitialized;
        }

        this->m_bAutoCommit = bAutoCommit;
        return NavExclusions::Success;
    }

    // WARNING: Returns a copy!
    ExResult CExclusionManager::enumGetNextExclusionObj(IExclusion* &pExclusion)
    {
        ExclusionId myId;
        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::enumGetNextExclusionObj - Not initialized."));
            return NavExclusions::NotInitialized;
        }

        if(pExclusion != NULL)
        {
            CCTRACEW(_T("CExclusionManager::enumGetNextExclusionObj - ")\
                        _T("Received a non-NULL exclusion object. Autoreleasing."));
            pExclusion->Release();
            pExclusion = NULL;
        }

        myId = this->enumGetNextID();
        if(myId != _INVALID_ID)
        {
            return this->getExclusion(myId, pExclusion);
        }
        else
            return NavExclusions::False;
    }

    ExclusionId CExclusionManager::enumGetNextID()
    {
        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

        CCTRACEI(_T("CExclusionManager::enumGetNextID - Starting."));

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::enumGetNextID - Not initialized."));
            return _INVALID_ID;
        }

        this->enumSeekToNext();
        if(this->m_itEnumPos == this->m_dqExclusions.end())
        {
            CCTRACEI(_T("CExclusionManager::enumGetNextID - End of enumeration reached."));
            return _INVALID_ID;
        }
        CCTRACEI(_T("CExclusionManager::enumGetNextID - Returning item %d."),(*(this->m_itEnumPos))->getID());
        return (*(this->m_itEnumPos))->getID();
    }

    ExResult CExclusionManager::enumReset()
    {
        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);
        CCTRACEI(_T("CExclusionManager::enumReset - Resetting enumeration."));

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::enumReset - Not initialized."));
            return NavExclusions::NotInitialized;
        }

        this->m_itEnumPos = this->m_dqExclusions.begin();
        CCTRACEI(_T("CExclusionManager::enumReset - Reset OK!"));
        return NavExclusions::Success;
    }

    ExResult CExclusionManager::saveToFile(cc::IString* pFileName)
    {
        ccSym::CFileStreamImplPtr spFileStream;
        std::deque<IExclusion*>::iterator it;
        ExResult res;
        CCTRACEI(_T("CExclusionManager::saveToFile - Starting."));

        if(!pFileName || pFileName->GetLength() <= 0)
        {
            CCTRACEE(_T("CExclusionManager::saveToFile - NULL or zero-length argument."));
            return NavExclusions::InvalidArgument;
        }
        
        spFileStream = ccSym::CFileStreamImpl::CreateFileStreamImpl();
        if(!spFileStream)
        {
            CCTRACEE(_T("CExclusionManager::saveToFile - Cannot create ")\
                _T("File Stream."));
            return NavExclusions::OutOfMemory;
        }

        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);

#ifdef _UNICODE
        if(spFileStream->GetFile().Open(pFileName->GetStringW(), GENERIC_READ|GENERIC_WRITE, 
#else
        if(spFileStream->GetFile().Open(pFileName->GetStringA(), GENERIC_READ|GENERIC_WRITE, 
#endif
            NULL, NULL, CREATE_ALWAYS, 0, NULL) != TRUE)
        {
            CCTRACEE(_T("CExclusionManager::saveToFile - Cannot open ")\
                _T("the named file (%s)."), pFileName->GetStringA());
            return NavExclusions::InvalidPath;
        }

        if(!this->m_bInitialized)
        {
            CCTRACEE(_T("CExclusionManager::saveToFile - Not initialized."));
            return NavExclusions::NotInitialized;
        }

        for(it = this->m_dqExclusions.begin();
            it != this->m_dqExclusions.end();
            it++)
        {
            if(!(*it)) { continue; } // Skip nulls
            else if((*it)->isValidForMatch() != NavExclusions::Success)
            {
                CCTRACEI(_T("CExclusionManager::saveToFile - Skipping expired/deleted exclusion."));
                continue;   // Skip deleted/expired
            }
            else if((res = (*it)->Save(spFileStream)) != NavExclusions::Success)
            {
                CCTRACEE(_T("CExclusionManager::saveToFile - Error while ")\
                    _T("serializing exclusion to file stream."));
                return res;
            }
            else
            {
                CCTRACEI(_T("CExclusionManager::saveToFile - Saved an exclusion to file."));
            }
        }

        if(spFileStream->GetFile().Close() != TRUE)
        {
            CCTRACEW(_T("CExclusionManager::saveToFile - Error occured while ")\
                        _T("closing file stream; file may be corrupt!"));
        }

        CCTRACEI(_T("CExclusionManager::saveToFile - Finished."));

        return NavExclusions::Success;
    }

    /************** PRIVATE FUNCTIONS FROM HERE ON *****************
     *** NOT NATIVELY THREAD SAFE. DO NOT EXPOSE THESE WITHOUT ***
                     *** ADDING LOCKING! ***/
    bool CExclusionManager::putInDeque(size_t pos, IExclusion* ptr)
    {
        // Inflate array to proper size with NULLs
        // This will throw an exception if out of memory.
        try
        {
            while(pos >= this->m_dqExclusions.size())
                this->m_dqExclusions.push_back(NULL);
        }
        catch(...)
        {
            CCTRACEE(_T("CExclusionManager::putInDeque - Caught an exception ")\
                        _T("while expanding deque; assuming out of memory."));
            return false;
        }

        this->m_dqExclusions[pos] = ptr;
        return true;
    }

    bool CExclusionManager::mbcsToUnicode(cc::IString* &pStrOut, 
        const char* pszMBCS, bool bIsOem)
    {
        size_t strLen, bufLen;
        CStringW cswBuffer;
        cc::IStringPtr spString;

        if(pStrOut != NULL)
        {
            CCTRACEW(_T("CExclusionManager::mbcsToUnicode - Received a ")\
                _T("non-NULL output pointer; releasing."));
            pStrOut->Release();
            pStrOut = NULL;
        }

        if(!pszMBCS)
        {
            CCTRACEE(_T("CExclusionManager::mbcsToUnicode - Received a NULL ")\
                _T("character string; invalid argument."));
            return false;
        }

        strLen = strlen(pszMBCS)+1;
        bufLen = MultiByteToWideChar((bIsOem?CP_OEMCP:CP_ACP), 0, pszMBCS,
            (int)strLen, cswBuffer.GetBuffer((int)strLen), (int)strLen);
        cswBuffer.ReleaseBuffer();
        if(bufLen == 0)
        {
            CCTRACEE(_T("CExclusionManager::mbcsToUnicode - Failed to convert ")\
                _T("multibyte character string to Unicode."));
            return false;
        }

        spString.Attach(ccSym::CStringImpl::CreateStringImpl(
            cswBuffer.GetString()));
        if(!spString)
        {
            CCTRACEE(_T("CExclusionManager::mbcsToUnicode - Failed to create ")\
                _T("IString copy of Unicode file path."));
            return false;
        }

        // Let the other function do the rest of the work.
        pStrOut = spString;
        pStrOut->AddRef();
        spString.Release();
        return true;
    }

    inline void CExclusionManager::enumSeekToNext()
    {
        while(this->m_itEnumPos != this->m_dqExclusions.end())
        {
            this->m_itEnumPos++;
            if(this->m_itEnumPos != this->m_dqExclusions.end() &&
               *(this->m_itEnumPos) && 
               (*(this->m_itEnumPos))->isValidForMatch() == NavExclusions::Success)
            {
                return;
            }
        }
    }

    bool CExclusionManager::nukeDeque()
    {
        std::deque<IExclusion*>::iterator it;
        for(it = this->m_dqExclusions.begin();
            it != this->m_dqExclusions.end();
            it++)
        {
            if((*it))
            {
                (*it)->Release();
                (*it) = NULL;
            }
        }
        this->m_dqExclusions.clear();
        return true;
    }

    inline bool CExclusionManager::sendChangeEvent()
    {
        // Tell the world that the exclusions have changed.
        CGlobalEvent event;
        event.CreateW(SYM_REFRESH_NAV_EXCLUSIONS, FALSE );
        if(::PulseEvent ( event ))
            return true;
        else
            return false;
    }

    bool CExclusionManager::mergeExclusions(ExclusionId targetExclusion, 
                                            time_t tExpiry,
                                            IExclusion::ExclusionState excludeWhich)
    {
        if(targetExclusion >= this->m_dqExclusions.size() ||
            targetExclusion <= _INVALID_ID)
        {
            CCTRACEE(_T("CExclusionManager::mergeExclusions - Received an ")\
                        _T("invalid ID for merge."));
            return false;
        }

        // Get the item; we're merging generic info, so no QI needed
        time_t myExpiry;
        IExclusion::ExclusionState eState;
        IExclusionPtr pEx;

        // Locate it in the deque
        pEx = this->m_dqExclusions.at((std::deque<IExclusion*>::size_type)targetExclusion);

        if(pEx != NULL && pEx->isDeleted() != NavExclusions::Success
            && pEx->isExpired() != NavExclusions::Success)
        {
            // Exists and valid! Integrate new info.
            myExpiry = pEx->getExpiryStamp();

            if(tExpiry == 0 || (myExpiry != 0 && myExpiry < tExpiry))
                pEx->setExpiryStamp(tExpiry);

            eState = pEx->getState();
            if((eState | excludeWhich) != eState)
                pEx->setState((IExclusion::ExclusionState)(eState | excludeWhich));

            pEx = NULL;
            CCTRACEI(_T("CExclusionManager::mergeExclusions - Merge OK!"));

            return true;
        }

        CCTRACEI(_T("CExclusionManager::mergeExclusion - Merge not possible."));
        return false;
    }

    bool CExclusionManager::lopOffBackslash(cc::IString*& spStr)
    {
        if(!spStr)
            return false;

        CStringW csBuffer(spStr->GetStringW());

        // If we have a trailing backslash, delete it
        if(csBuffer[csBuffer.GetLength()-1] == '\\')
        {
            csBuffer.Delete(csBuffer.GetLength()-1,1);
            spStr->Release();
            spStr = ccSym::CStringImpl::CreateStringImpl(csBuffer.GetString());
        }

        return true;
    }

    ExResult CExclusionManager::resetToDefaults(IExclusion::ExclusionState requiredState,
                                                IExclusion::ExclusionType requiredType)
    {
        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);
        if(!this->m_bInitialized)
        {
            return NavExclusions::NotInitialized;
        }

        // Attempt to open file
        ccLib::CString strNavPath;
        ccSym::CInstalledApps::GetNAVDirectory(strNavPath);

        if(strNavPath.GetLength() > 0 && strNavPath[strNavPath.GetLength()-1] != '\\')
            strNavPath += '\\';

        strNavPath += ccLib::CString(EXCLUSIONS_DEFAULTS_FILENAME_TCHAR);

        return this->loadFromFile(&strNavPath, requiredState, requiredType);
    }

    ExResult CExclusionManager::loadFromFile(ccLib::CString* strFile,
                                             IExclusion::ExclusionState requiredState,
                                             IExclusion::ExclusionType requiredType)
    {
        ccLib::CSingleLock cLock(&this->m_CriticalSection, INFINITE, FALSE);
        if(!this->m_bInitialized)
        {
            return NavExclusions::NotInitialized;
        }

        IExclusion::ExclusionType eType;
        CExclusionFileLoader efl;

        if(!strFile)
        {
            CCTRACEE(_T("CExclusionManager::loadFromFile - Received NULL path."));
            return NavExclusions::InvalidPath;
        }

        if(efl.initialize(strFile->GetString()) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionManager::loadFromFile - Could not open ")\
                        _T("defaults file (%s)"), strFile->GetString());
            return NavExclusions::InvalidPath;
        }

        // Remove all relevant exclusions
        std::deque<IExclusion*>::iterator it;
        if((requiredState & IExclusion::ExclusionState::EXCLUDE_BOTH)
            == IExclusion::ExclusionState::EXCLUDE_BOTH
            || requiredState == 0)
        {   // If we're removing both V and NV _OR_ not filtering, just nuke everything.
            for(it = this->m_dqExclusions.begin(); it != this->m_dqExclusions.end(); it++)
            {
                if(*it != NULL)
                {   // MUST set items to deleted in order to actually remove them.
                    (*it)->setDeleted(true);
                }
            }
        }
        else
        {   // Otherwise, perform fancy filtering footwork.

            // Loop through all exclusions.
            for(it = this->m_dqExclusions.begin(); 
                it != this->m_dqExclusions.end(); it++)
            {
                if(*it != NULL)
                {   // If it's not a dead slot...
                    eType = (*it)->getExclusionType();
                    if( ((*it)->getState() & requiredState)
                        && eType == requiredType)
                    {   // If it passes the filters...

                        // Unset the filtered states
                        (*it)->setStateFlag(requiredState, false);

                        // See if we still have a V/NV flag set.
                        if(((*it)->getState() & IExclusion::ExclusionState::EXCLUDE_BOTH) == 0)
                        {   // It was monostate, most likely. Delete it forcibly.
                            (*it)->setDeleted(true);

                            // Remove from typesets
                            if(eType == IExclusion::ExclusionType::ANOMALY)
                            {
                                this->m_setAE.removeExclusion((*it)->getID());
                            }
                            else if(eType == IExclusion::ExclusionType::FILESYSTEM)
                            {
                                this->m_setFSE.removeExclusion((*it)->getID());
                            }
                        } // Else: it still has v or nv set, leave it be.
                    }   // Else, doesn't pass filters, leave it be.
                } // Else, it's null, don't deref a null pointer. :)
            } // End: FOR each exclusion...
        } // End else - we no longer have all sorts of dupe-causing nastiness.

        // Load from file
        cc::IKeyValueCollectionPtr spKVC = NULL;
        IExclusionPtr spExclusion;
        ExResult res;
        ULONG item;
        ExclusionId myId, dupId;

        // Loop through all exclusions in file
        CCTRACEI(_T("CExclusionManager::loadFromFile - Loading exclusions..."));
        while((res = efl.LoadNextItem(spKVC)) != NavExclusions::False)
        {
            if(res >= NavExclusions::Fail)
            {
                CCTRACEW(_T("CExclusionManager::loadFromFile - Caught an ")\
                    _T("error while loading an item. Terminating."));
                break;
            }

            // Validate KVC and run through filters
            if(!spKVC)
            {
                CCTRACEW(_T("CExclusionManager::loadFromFile - Null KVC in Loop"));
                continue;
            }

            if(!spKVC->GetValue(IExclusion::Property::EType, item))
            {
                CCTRACEW(_T("CExclusionManager::loadFromFile - Can't get Etype, skipping"));
                spKVC.Release();
                spKVC = NULL;
                continue;
            }

            if(requiredType != IExclusion::ExclusionType::INVALID && 
                (IExclusion::ExclusionType)item != requiredType)
            {
                CCTRACEI(_T("CExclusionManager::loadFromFile - EType doesn't match, skipping"));
                spKVC.Release();
                spKVC = NULL;
                continue;
            }

            if(!spKVC->GetValue(IExclusion::Property::EState, item))
            {
                CCTRACEW(_T("CExclusionManager::loadFromFile - Can't get EState, skipping"));
                spKVC.Release();
                spKVC = NULL;
                continue;
            }

            if(requiredState != IExclusion::ExclusionState::NOT_EXCLUDED 
                && (item & (ULONG)requiredState) != (ULONG)requiredState)
            {
                CCTRACEI(_T("CExclusionManager::loadFromFile - EState doesn't match, skipping"));
                spKVC.Release();
                spKVC = NULL;
                continue;
            }

            // Recreate exclusion object from KVC
            res = this->m_Factory.rebuildExclusion(spExclusion, spKVC);
            if(res != NavExclusions::Success)
            {
                CCTRACEE(_T("CExclusionManager::loadFromFile - Factory ")\
                            _T("failed to rebuild exclusion. Skipping."));
                spExclusion.Release();
                spKVC.Release();
                spKVC = NULL;
                spExclusion = NULL;
                continue;
            }

            // Validate new exclusion object
            myId = spExclusion->getID();
            if(myId == _INVALID_ID)
            {
                CCTRACEW(_T("CExclusionManager::loadFromFile - Received an ")\
                    _T("exclusion with a bad ID. Releasing."));
                this->m_Factory.destroyExclusion(spExclusion);
                spExclusion = NULL;
                spKVC = NULL;
                continue;
            }

            // Add to deque and typeset, requires QueryInterface.
            eType = spExclusion->getExclusionType();
            if(eType == IExclusion::ExclusionType::ANOMALY)
            {
                IAnomalyExclusionQIPtr qi = spExclusion;
                ULONG ulVid;
                cc::IStringPtr spGuid;

                if(!qi)
                {
                    CCTRACEW(_T("CExclusionManager::loadFromFile - Couldn't ")\
                        _T("QI to Anomaly exclusion; ignoring."));
                    this->m_Factory.destroyExclusion(spExclusion);
                    spExclusion = NULL;
                    spKVC = NULL;
                    continue;
                }

                ulVid = qi->getVid();
                if(qi->getAnomalyGuid(spGuid) >= NavExclusions::Fail)
                {
                    CCTRACEW(_T("CExclusionManager::reloadState - Failure ")\
                        _T("when retrieving GUID from anomaly exclusion. "));
                    spExclusion = NULL;
                    spKVC = NULL;
                    continue;
                }

                dupId = this->m_setAE.findExclusionId(spGuid, ulVid);
                if(dupId != _INVALID_ID)
                {
                    if(!this->mergeExclusions(dupId, qi->getExpiryStamp(), qi->getState()))
                    {
                        CCTRACEW(_T("CExclusionManager::loadFromFile - Error ")\
                                    _T("while merging exclusion."));
                    }
                    // Else, merge ok!
                    spExclusion = NULL;
                    spKVC = NULL;
                    continue;
                }

                if(!this->putInDeque((size_t)myId, spExclusion))
                {
                    CCTRACEE(_T("CExclusionManager::loadFromFile - Error ")\
                        _T("while placing exclusion in deque!"));
                    this->m_Factory.destroyExclusion(spExclusion);
                    spExclusion = NULL;
                    spKVC = NULL;
                    return NavExclusions::OutOfMemory;
                }

                res = this->m_setAE.addExclusion(qi);
                if(res != NavExclusions::Success)
                {
                    CCTRACEE(_T("CExclusionManager::loadFromFile - Error while ")\
                        _T("adding item to AE set. ExResult = %lu"), res);
                    this->m_Factory.destroyExclusion(spExclusion);
                    spExclusion = NULL;
                    spKVC = NULL;
                    return res;
                }
                spExclusion->AddRef();
            }
            else if(eType == IExclusion::ExclusionType::FILESYSTEM)
            {
                IFileSystemExclusionQIPtr qi = spExclusion;
                cc::IStringPtr spPath;

                if(!qi)
                {
                    CCTRACEW(_T("CExclusionManager::loadFromFile - Couldn't ")\
                        _T("QI to FS exclusion; ignoring."));
                    this->m_Factory.destroyExclusion(spExclusion);
                    spExclusion = NULL;
                    spKVC = NULL;
                    continue;
                }

                if(qi->getFSPath(spPath) >= NavExclusions::Fail)
                {
                    CCTRACEW(_T("CExclusionManager::loadFromFile - Failure ")\
                        _T("when retrieving path from FS exclusion. "));
                    spExclusion = NULL;
                    spKVC = NULL;
                    continue;
                }

                dupId = this->m_setFSE.findExclusionId(spPath);
                if(dupId != _INVALID_ID)
                {
                    if(!this->mergeExclusions(dupId, qi->getExpiryStamp(), qi->getState()))
                    {
                        CCTRACEW(_T("CExclusionManager::loadFromFile - ")\
                            _T("Couldn't merge duplicate exclusions!"));
                    }
                    // Else, merge ok!
                    spExclusion = NULL;
                    spKVC = NULL;
                    continue;
                }

                if(!this->putInDeque((size_t)myId, spExclusion))
                {
                    CCTRACEE(_T("CExclusionManager::loadFromFile - Error ")\
                        _T("while placing exclusion in deque!"));
                    this->m_Factory.destroyExclusion(spExclusion);
                    spExclusion = NULL;
                    spKVC = NULL;
                    return NavExclusions::OutOfMemory;
                }

                res = this->m_setFSE.addExclusion(qi);
                if(res != NavExclusions::Success)
                {
                    CCTRACEE(_T("CExclusionManager::loadFromFile - Error while ")\
                        _T("adding item to FSE set. ExResult = %lu"), res);
                    this->m_Factory.destroyExclusion(spExclusion);
                    spExclusion = NULL;
                    spKVC = NULL;
                    return res;
                }
                spExclusion->AddRef();
            }
            else
            {
                CCTRACEE(_T("CExclusionManager::loadFromFile - Received an ")\
                    _T("exclusion with an invalid type. Releasing."));
                this->m_Factory.destroyExclusion(spExclusion);
            }

            // Done! Release stuff for next round.
            spExclusion.Release();
            spKVC.Release();
        }
        CCTRACEI(_T("CExclusionManager::loadFromFile - Load loop complete."));

        if(efl.uninitialize() != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionManager::loadFromFile - Error while ")\
                        _T("closing EFL."));
            return NavExclusions::Fail;
        }

        if(res >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CExclusionManager::loadFromFile - Error while ")\
                _T("loading an exclusion KVC, result = %d"), res);
            return res;
        }
        else
        {
            return NavExclusions::Success;
        }
    }
}

#endif