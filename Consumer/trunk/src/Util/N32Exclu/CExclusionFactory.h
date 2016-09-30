////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/***
 * Interface header file for Exclusion Factory; this class provides a
 * unified interface for constructing IExclusions and their
 * subclasses.
 *  NOTE: Whenever a create...() call is made, memory is allocated!
 * You pass it back in to the DestroyExclusion() method to destroy it,
 * or the ID reuse subsystem will not work, causing lots and lots of
 * wasted memory.
 *  Exception: If you're destroying a CEM (e.g. on a 'cancel' operation),
 * then go ahead and nuke them yourself.
 ***/

#ifndef __N32EXCLU_cexclusionfactory_h__
#define __N32EXCLU_cexclusionfactory_h__

#include "n32exclu.h"
#include "ExclusionInterface.h"
#include "CAnomalyExclusion.h"
#include "CFileSystemExclusion.h"

#include <list>

namespace NavExclusions
{

class CExclusionFactory
{
public:

    CExclusionFactory();
    virtual ~CExclusionFactory();

    /**
     * Use this function to destroy an exclusion. Or use delete.
     **/
    virtual ExResult destroyExclusion(IExclusion* &pExclusion);

    /***
     * This is the primary function for creating exclusions.
     * Arguments:
     *  pExclusion - An IExclusion pointer pointing to null. Used as output.
     *  eType - Indicates the type of Exclusion, e.g. ExclusionType::ANOMALY
     *  bActive - Whether or not the exclusion is active by default
     *  eState - Viral/Nonviral/Both; only matters for filesystem exclusions
     *  threatName - String for the displayable threat name; only for anomalies
     *  pDatum1 - A pointer to the first data item.
     *              For filesystem exclusions, this is a pointer to a
     *                const ccLib::CStringW object indicating the path.
     *              For anomaly exclusions, this is a pointer to a
     *                const ccLib::CStringW object indicating the GUID -OR-
     *                NULL for no GUID.
     *  pDatum2 - A pointer to the second data item.
     *              For filesystem exclusions, this is ignored.
     *              For anomaly exclusions, this is a pointer to a const
     *                  ULONGLONG, indicating the VID, or NULL for no VID.
     *
     *  Returns a pointer to a freshly-allocated IExclusion subclass in
     * the pExclusion argument and returns an ExResult with a success/fail code.
     * This object must subsequently be freed with delete or a call
     * to CExclusionFactory::destroyExclusion(...).
     * Returns NULL in pExclusion in the event of an error.
     ***/
    virtual ExResult createExclusion(IExclusion* &pExclusion,
                             IExclusion::ExclusionType eType,
                             time_t expiryStamp,
                             bool bActive,
                             IExclusion::ExclusionState eState,
                             const cc::IString* threatName,
                             const void* pDatum1,
                             const void* pDatum2);

    /***
    * This function 'rebuilds' an exclusion from a raw KVC.
    ***/
    virtual ExResult rebuildExclusion(IExclusion* &pExclusion, 
        cc::IKeyValueCollection* pData);

    /***
     * THESE FUNCTIONS ARE ONLY FOR USE WHEN RESTORING THE STATE OF THE
     * CEXCLUSIONMANAGER. DO NOT CALL THEM ELSEWHERE OR THINGS WILL
     * BREAK IN HORRIBLE WAYS; IT WILL CAUSE THE EXCLUSIONS LIBRARY TO SACRIFICE
     * YOUR FIRSTBORN BY THE LIGHT OF THE FULL MOON, AWAKENING DEAD CTHULHU
     * AND HERALDING THE DOOM OF CIVILIZATION AS WE KNOW IT.
     ***/
    virtual ExResult assignId(IExclusion* pExclusion, ULONGLONG newId);
    virtual ExResult dumpIdStack();
    virtual ExResult resetTopId();

protected:
    /***
     * These specialized functions return a specific type of Exclusion
     * and only require the appropriate arguments. See above or CPP file for 
     * argument definitions.
     ***/
    ExResult createFSExclusion(IFileSystemExclusion* &pExclusion,
                               const cc::IString* lpPath,
                               time_t expiryStamp,
                               bool bActive,
                               IExclusion::ExclusionState eState);

    ExResult createAnomalyExclusion(IAnomalyExclusion* &pExclusion,
                                    const cc::IString* lpGUID,
                                    const ULONG ulVid,
                                    time_t expiryStamp,
                                    bool bActive,
                                    const cc::IString* lpThreatName,
                                    IExclusion::ExclusionState eState);

private:
    std::list<ULONGLONG> myUnusedIds;
    ULONGLONG myMaxId;

    ULONGLONG getFreeId();
}; // class CExclusionFactory

} // namespace NavExclusions

#endif