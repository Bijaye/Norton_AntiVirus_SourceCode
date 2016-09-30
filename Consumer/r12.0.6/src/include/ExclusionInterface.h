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
 * Interfaces: IExclusion, IFSExclusion, IAnomalyExclusion
 ***/
#ifndef __N32EXCLU_exclusioninterface_h__
#define __N32EXCLU_exclusioninterface_h__

#include "n32exclu.h"
#include "SymInterface.h"
#include "ccStreamInterface.h"

namespace NavExclusions
{

class IExclusion: public ISymBase
{
public:
    enum Property       // All datamembers encapsulated at the IExclusion level
    {
        EGUID = 0,      // GUID - exclusion GUID
        EID,            // ULONGLONG - temporary sequential identifier
                        //   DO NOT CHANGE THIS MANUALLY
                        //   LET THE SYSTEM DO IT OR FACE BREAKAGE!
        EType,          // ExclusionType - enum indicating object type
        EState,         // ExclusionState - enum indicating viral/nonviral
        EFlags,         // Bitvector indicating various boolean values
                        // See below (BVFlags) for details.
        EThreatName,    // IString* - threat name, if any; NULL for filesystems
        EExpiryStamp,   // time_t value indicating expiration.

        UserData,       // CKeyValueCollection for miscellany.
        Last_Prop = 1000
    };

    enum ExclusionType
    {
        INVALID = 0,
        FILESYSTEM,
        ANOMALY // VID-GUID
    };

    // Exclusion States
    enum ExclusionState {
        NOT_EXCLUDED = 0,
        EXCLUDE_VIRAL = 1,
        EXCLUDE_NONVIRAL = EXCLUDE_VIRAL << 1,
        EXCLUDE_BOTH = EXCLUDE_VIRAL | EXCLUDE_NONVIRAL,
        EXCLUDE_SUBDIRS_VIRAL = EXCLUDE_NONVIRAL << 1,
        EXCLUDE_SUBDIRS_NONVIRAL = EXCLUDE_SUBDIRS_VIRAL << 1,
        EXCLUDE_INCLUDE_SUBDIRS = EXCLUDE_SUBDIRS_NONVIRAL | EXCLUDE_SUBDIRS_VIRAL,
        EXCLUDE_ALL = EXCLUDE_VIRAL | EXCLUDE_NONVIRAL | EXCLUDE_INCLUDE_SUBDIRS
    };

    // Bitvector Flags
    enum BVFlags
    {
        NONE = 0,
        DELETED = 1,
        DISABLED = DELETED << 1,
        TEMPORARY = DISABLED << 1
    };

    virtual ExResult isDeleted() = 0;
    virtual ExResult isExpired() = 0;
    virtual ExResult isActive() = 0;
    virtual ExResult isMatch(const void* pDatum1, const void* pDatum2) = 0;

    virtual ExResult setActive(bool active) = 0;
    virtual ExResult setExpiryStamp(time_t expiry) = 0;
    virtual ExResult setExclusionGuid(const cc::IString* newGuid) = 0;
    virtual ExResult setID(const ULONGLONG newID) = 0;
    virtual ExResult setDisplayName(const cc::IString* newDisplayName) = 0;
    virtual ExResult setState(const ExclusionState newState) = 0;
    virtual ExResult setUserData(const cc::IKeyValueCollection* pUserData) = 0;
    virtual ExResult setStateFlag(const IExclusion::ExclusionState state, bool bTrue) = 0;

    virtual const ExclusionType getExclusionType() = 0;
    virtual ExResult getExclusionGuid(cc::IString*& pStrGuid) = 0;
    virtual const ULONGLONG getID() = 0;
    virtual ExResult getDisplayName(cc::IString*& pStrName) = 0;
    virtual ExResult getUserData(cc::IKeyValueCollection* &pUserData) = 0;
    virtual ExResult getStateFlag(const IExclusion::ExclusionState state) = 0;
    virtual time_t getExpiryStamp() = 0;

    virtual ExResult isValidForMatch() = 0;
    virtual ExResult setDeleted(bool deleted) = 0;
    virtual ExResult setExpired() = 0;

    virtual bool isInitialized() = 0;

    // Make a perfect duplicate of this object. Returns with Refcount 1.
    virtual IExclusion* clone() = 0;

    // Are these items completely identical?
//    virtual bool isIdentical(IExclusion* ptr) = 0;

    virtual ExResult Save(cc::IStream* pStream) = 0;

    virtual IExclusion::ExclusionState getState() = 0;
};
// {7E5C7101-FAD4-469b-A530-8058149A35A5}
SYM_DEFINE_INTERFACE_ID(IID_IExclusion, 0x7e5c7101, 0xfad4, 0x469b, 0xa5, 0x30,
                        0x80, 0x58, 0x14, 0x9a, 0x35, 0xa5);
typedef CSymPtr<IExclusion> IExclusionPtr;
typedef CSymQIPtr<IExclusion, &IID_IExclusion> IExclusionQIPtr;

class IFileSystemExclusion: public IExclusion
{
public:
    enum Property
    {
        FSPath = IExclusion::Property::Last_Prop + 1,
        Last_Prop = 2000
    };

    virtual NavExclusions::ExResult setFSPath(const cc::IString* myPath) = 0;
    virtual ExResult getFSPath(cc::IString*& pStrPath) = 0;

    virtual ExResult doesIncludeSubdirs(const IExclusion::ExclusionState requiredState) = 0;
    virtual ExResult isExactMatch(const cc::IString* pStrPath) = 0;
};
// {2F3ACF9C-7179-4087-925C-C5761F257212}
SYM_DEFINE_INTERFACE_ID(IID_IFileSystemExclusion, 0x2f3acf9c, 0x7179, 0x4087,
                        0x92, 0x5c, 0xc5, 0x76, 0x1f, 0x25, 0x72, 0x12);
typedef CSymPtr<IFileSystemExclusion> IFileSystemExclusionPtr;
typedef CSymQIPtr<IFileSystemExclusion, &IID_IFileSystemExclusion> IFileSystemExclusionQIPtr;


class IAnomalyExclusion: public IExclusion
{
public:
    enum Property
    {
        AnomalyGuid = IExclusion::Property::Last_Prop + 1,
        Vid,
        Last_Prop = 2000
    };

    virtual NavExclusions::ExResult setAnomalyGuid(const cc::IString* guid) = 0;
    virtual ExResult getAnomalyGuid(cc::IString*& pStrAnomalyGuid) = 0;
    virtual NavExclusions::ExResult setVid(const ULONG vid) = 0;
    virtual const ULONG getVid() = 0;
    virtual ExResult isExactMatch(const cc::IString* pStrGuid, const ULONG ulVid) = 0;
};
// {F2E7C4BC-08DE-47cc-88EF-E44FBEBA1546}
SYM_DEFINE_INTERFACE_ID(IID_IAnomalyExclusion, 0xf2e7c4bc, 0x8de, 0x47cc, 0x88,
                        0xef, 0xe4, 0x4f, 0xbe, 0xba, 0x15, 0x46);

typedef CSymPtr<IAnomalyExclusion> IAnomalyExclusionPtr;
typedef CSymQIPtr<IAnomalyExclusion, &IID_IAnomalyExclusion> IAnomalyExclusionQIPtr;

}

#endif
