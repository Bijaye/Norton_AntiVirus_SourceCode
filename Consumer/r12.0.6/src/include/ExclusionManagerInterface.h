/*
 * This file contains the external exclusions interface that will
 * be exported via SymInterface.
 */

#pragma once

#ifndef __N32EXCLU_exclusionmanagerinterface_h__
#define __N32EXCLU_exclusionmanagerinterface_h__

#include "syminterface.h"
#include "n32exclu.h"
#include "ExclusionInterface.h"

namespace NavExclusions
{

class IExclusionManager: public ISymBase
{
public:
    // Create and add a FilesystemExclusion with an encapsulated path.
    virtual ExclusionId addExclusionFS(const cc::IString* pStrFilePath, 
                                       time_t tExpiry,
                                       IExclusion::ExclusionState excludeWhich
                                      ) = 0;

    // Create and add a Filesystem exclusion with an ANSI or OEM path.
    // For ANSI paths, set isOem to false; for OEM paths, set isOem to true.
    virtual ExclusionId addExclusionFS(const char* pszFilePath,
                                       time_t tExpiry,
                                       IExclusion::ExclusionState excludeWhich,
                                       bool bIsOem) = 0;

    // Create and add a VID-GUID exclusion. Set myVid to _INVALID_VID (zero)
    // or myGuid to NULL if you don't want to specify either of those. At
    // least one of the two must be specified.
    virtual ExclusionId addExclusionAnomaly(const ULONG ulVid,
                                            const cc::IString* pStrGuid,
                                            time_t tExpiry,
                                            const cc::IString* pStrThreatName,
                                        IExclusion::ExclusionState excludeWhich)
                                             = 0;

    // Find the ID of a FilesystemExclusion with a unicode path.
    virtual ExclusionId findExclusionFS(const cc::IString* pStrUnicodePath) = 0;

    // Find the ID of a Filesystem exclusion with an ANSI or OEM path.
    // For ANSI paths, set isOem to false; for OEM paths, set isOem to true.
    virtual ExclusionId findExclusionFS(const char* pszFilePath,
                                        bool bIsOem) = 0;

    // Find the ID of a VID-GUID exclusion from a VID and/or GUID.
    virtual ExclusionId findExclusionAnomaly(const ULONG ulVid,
                                             const cc::IString* pStrGuid) = 0;

    // The following two functions will return an ExclusionState enum
    // defining which exclusion types are set. These will be logical-ORed
    // together; at the moment, since there's only two, you can check
    // for equivalency with _ALL, _VIRAL and _NONVIRAL.
    // If the input path hits as a directory exclusion with INCLUDE_SUBDIRS
    // set, then the EXCLUDE_INCLUDE_SUBDIRS bit will be set in the return
    // bitvector.

    // Check if a unicode path is excluded.
    virtual IExclusion::ExclusionState isExcludedFS(
                                        const cc::IString* pStrUnicodePath, 
                                       IExclusion::ExclusionState requiredState)
                                       = 0;

    // Check if an ANSI or OEM path is excluded.
    // For ANSI paths, set isOem to false; for OEM paths, set isOem to true.
    virtual IExclusion::ExclusionState isExcludedFS(const char* pszFilePath,
                                                    bool bIsOem,
                                  IExclusion::ExclusionState requiredState) = 0;
    
    // Since anomalies and VIDs imply viral or nonviral threats,
    // we don't need to check for category matches.
    // Hence, this function will always return _NONE or _ALL.

    // Check if a raw VID is excluded.
    virtual IExclusion::ExclusionState isExcludedAnomaly(const ULONG ulVid,
                                                   const cc::IString* pStrGuid,
                                       IExclusion::ExclusionState requiredState)
                                                    = 0;

    // Retrieve a given exclusion object. Returns a copy, not the original.
    virtual ExResult getExclusion(const ExclusionId exclusionId,
                                      IExclusion* &pExclusion) = 0;

    // Replace (swap out data) of a given exclusion object.
    // This will fail if the exclusions are of different types!
    // You may omit whichExclusion if replacement has its ID properly
    // set.
    virtual ExResult replaceExclusion(const ExclusionId whichExclusion, 
                                    IExclusion* replacement) = 0;

    // Destroy a given exclusion.
    virtual bool removeExclusion(const ExclusionId exclusionId) = 0;

    // Flush to CC Settings. Don't do this a lot.
    virtual ExResult saveState() = 0;

    // Discard current state and restore this object to the state saved 
    // in CC Settings. Definitely don't do this a lot.
    virtual ExResult reloadState() = 0;

    virtual ExResult initialize(bool bCommitOnTeardown) = 0;

    // Override is meaningful if this is a non-temporary
    // exclusion manager. In that case, if Override is set,
    // then the exclusion manager will NOT dump its state
    // out to CC Settings before tearing everything down.
    virtual ExResult uninitialize(bool bForceCommit) = 0;
    virtual bool isAutoCommit() = 0;
    virtual ExResult setAutoCommit(bool bAutoCommit) = 0;

    // Exclusion enumeration methods.
    // GetNext* returns the appropriate data for the next VALID exclusion
    // object in the deque, returning _INVALID_ID or False if
    // there are no more objects before the end.
    // Reset resets the enumeration, preparing it for another GetNext* call.
    virtual ExclusionId enumGetNextID() = 0;
    virtual ExResult enumGetNextExclusionObj(IExclusion* &pExclusion) = 0;
    virtual ExResult enumReset() = 0;   // Always returns Success... for now.

    virtual ExResult saveToFile(cc::IString* pFileName) = 0;
    virtual ExResult resetToDefaults(IExclusion::ExclusionState requiredState,
                                     IExclusion::ExclusionType requiredType) = 0;
    virtual ExResult loadFromFile(ccLib::CString* strFile,
                                  IExclusion::ExclusionState requiredState,
                                  IExclusion::ExclusionType requiredType) = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_IExclusionManager, 0x25e8e0c2, 0x5246, 0x4770, 0x8c,
                        0x61, 0x59, 0x40, 0xb, 0x23, 0xff, 0xb6);

typedef CSymPtr<IExclusionManager> IExclusionManagerPtr;
typedef CSymQIPtr<IExclusionManager, &IID_IExclusionManager> 
    IExclusionManagerQIPtr;

}

#endif