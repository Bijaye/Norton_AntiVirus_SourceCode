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

#ifndef __N32EXCLU_cexclusionmanager_h__
#define __N32EXCLU_cexclusionmanager_h__

#include "syminterface.h"
#include "n32exclu.h"
#include "CAnomalyExclusionSet.h"
#include "CFileSystemExclusionSet.h"
#include "ExclusionInterface.h"
#include "ExclusionManagerInterface.h"
#include "CExclusionFactory.h"
#include "CExclusionSettingsManager.h"
#include "ccSingleLock.h"

#include <deque>

namespace NavExclusions
{

class CExclusionManager: public IExclusionManager,
                         public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(IID_IExclusionManager, IExclusionManager)
    SYM_INTERFACE_MAP_END()

    CExclusionManager();
    ~CExclusionManager();

    // Create and add a FilesystemExclusion with an encapsulated path.
    virtual ExclusionId addExclusionFS(const cc::IString* pStrFilePath, 
                                       time_t tExpiry,
                                       IExclusion::ExclusionState excludeWhich);

    // Create and add a Filesystem exclusion with an ANSI or OEM path.
    // For ANSI paths, set isOem to false; for OEM paths, set isOem to true.
    virtual ExclusionId addExclusionFS(const char* pszFilePath,
                                        time_t tExpiry,
                                        IExclusion::ExclusionState excludeWhich,
                                        bool bIsOem);

    // Create and add a VID-GUID exclusion. Set myVid to _INVALID_VID (zero)
    // or myGuid to NULL if you don't want to specify either of those. At
    // least one of the two must be specified.
    virtual ExclusionId addExclusionAnomaly(const ULONG ulVid,
                                            const cc::IString* pStrGuid,
                                            time_t tExpiry,
                                            const cc::IString* pStrThreatName,
                                       IExclusion::ExclusionState excludeWhich);

    // Find the ID of a FilesystemExclusion with a unicode path.
    virtual ExclusionId findExclusionFS(const cc::IString* pStrUnicodePath);

    // Find the ID of a Filesystem exclusion with an ANSI or OEM path.
    // For ANSI paths, set isOem to false; for OEM paths, set isOem to true.
    virtual ExclusionId findExclusionFS(const char* pszFilePath,
                                        bool bIsOem);

    // Find the ID of a VID-GUID exclusion from a VID and/or GUID.
    virtual ExclusionId findExclusionAnomaly(const ULONG ulVid,
                                             const cc::IString* pStrGuid);

    // The following two functions will return an ExclusionState enum
    // defining which exclusion types are set. These will be logical-ORed
    // together; at the moment, since there's only two, you can check
    // for equivalency with _ALL, _VIRAL and _NONVIRAL.
    // If the input path hits as a directory exclusion with INCLUDE_SUBDIRS
    // set, then the EXCLUDE_INCLUDE_SUBDIRS bit will be set in the return
    // bitvector.

    // Check if a unicode path is excluded.
    virtual IExclusion::ExclusionState isExcludedFS(const cc::IString* pStrUnicodePath, 
                                                    IExclusion::ExclusionState requiredState);

    // Check if an ANSI or OEM path is excluded.
    // For ANSI paths, set isOem to false; for OEM paths, set isOem to true.
    virtual IExclusion::ExclusionState isExcludedFS(const char* pszFilePath,
                        bool bIsOem, IExclusion::ExclusionState requiredState);
    
    // Since anomalies and VIDs imply viral or nonviral threats,
    // we don't need to check for category matches.
    // Hence, this function will always return _NONE or _ALL.

    // Check if a raw VID is excluded.
    virtual IExclusion::ExclusionState isExcludedAnomaly(const ULONG ulVid,
                                                    const cc::IString* pStrGuid, 
                                      IExclusion::ExclusionState requiredState);

    // Destroy a given exclusion.
    virtual bool removeExclusion(const ExclusionId exclusionId);

    // Retrieve a given exclusion object. Returns a copy, not the original.
    virtual ExResult getExclusion(const ExclusionId exclusionId,
                                    IExclusion* &pExclusion);

    // Replace (swap out data) of a given exclusion object.
    // This will fail if the exclusions are of different types!
    // You may omit whichExclusion if replacement has its ID properly
    // set.
    virtual ExResult replaceExclusion(const ExclusionId whichExclusion, 
                                    IExclusion* replacement);

    // Flush to CC Settings. Don't do this a lot.
    virtual ExResult saveState();

    // Restore this object to the state saved in CC Settings.
    // Definitely don't do this a lot.
    virtual ExResult reloadState();

    virtual ExResult initialize(bool bCommitOnTeardown = true);

    // Override is meaningful if this is a non-temporary
    // exclusion manager. In that case, if Override is set,
    // then the exclusion manager will NOT dump its state
    // out to CC Settings before tearing everything down.
    virtual ExResult uninitialize(bool bForceCommit = false);

    virtual bool isAutoCommit();
    virtual ExResult setAutoCommit(bool bAutoCommit);

    // Exclusion enumeration methods.
    // GetNext* returns the appropriate data for the next VALID exclusion
    // object in the deque, returning _INVALID_ID or False if
    // there are no more objects.
    // Reset resets the enumeration, starting from the beginning,
    // returning False if there are no VALID exclusion objects in the
    // deque at all.
    virtual ExclusionId enumGetNextID();    // 
    virtual ExResult enumGetNextExclusionObj(IExclusion* &pExclusion);
    virtual ExResult enumReset();

    virtual ExResult saveToFile(cc::IString* pFileName);

    // Reload a defaults file into a CEM instead of directly to CC Settings.
    virtual ExResult resetToDefaults(IExclusion::ExclusionState requiredState,
                                     IExclusion::ExclusionType requiredType);

    virtual ExResult loadFromFile(ccLib::CString* strFile,
        IExclusion::ExclusionState requiredState,
        IExclusion::ExclusionType requiredType);

private:
    bool m_bInitialized;
    bool m_bAutoCommit;
    std::deque<IExclusion*> m_dqExclusions;
    CAnomalyExclusionSet m_setAE;
    CFileSystemExclusionSet m_setFSE;
    CExclusionFactory m_Factory;
    CExclusionSettingsManager m_Settings;
    std::deque<IExclusion*>::iterator m_itEnumPos;
    ccLib::CCriticalSection m_CriticalSection;

    bool nukeDeque();
    bool putInDeque(size_t pos, IExclusion* ptr);
    bool mbcsToUnicode(cc::IString* &pStrOut, const char* pszMBCS, bool bIsOem);
    bool sendChangeEvent();
    bool mergeExclusions(ExclusionId targetExclusion, time_t tExpiry,
                         IExclusion::ExclusionState excludeWhich);
    bool lopOffBackslash(cc::IString*& spStr);

    // Clear out the state.
    virtual ExResult clearState();

    // Enumerator helper function. Seeks to the next non-NULL and valid
    // exclusion object in the deque. If it hits the end, it halts.
    inline void enumSeekToNext();
};

}

#endif