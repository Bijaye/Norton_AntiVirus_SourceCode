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
 * Declaration: CAnomalyExclusionSet
 ***/
#ifndef __N32EXCLU_canomalyexclusionset_h__
#define __N32EXCLU_canomalyexclusionset_h__

#include "n32exclu.h"
#include "ExclusionInterface.h"
#include <list>

namespace NavExclusions
{

class CAnomalyExclusionSet
{
public:
    CAnomalyExclusionSet();
    ~CAnomalyExclusionSet();
    IExclusion::ExclusionState isExcluded(const cc::IString* pStrGuid, 
        const ULONG ulVid, IExclusion::ExclusionState requiredState);
    ExclusionId findExclusionId(const cc::IString* pStrGuid, const ULONG ulVid);
    const IAnomalyExclusion* getExclusion(const cc::IString* pStrGuid, 
                                          const ULONG ulVid);
    const IAnomalyExclusion* getExclusion(ExclusionId eid);
    ExResult addExclusion(IAnomalyExclusion* pExclusion);
    ExResult removeExclusion(const cc::IString* pStrGuid, const ULONG ulVid);
    ExResult removeExclusion(ExclusionId eid);

    ExResult nuke();
    ExResult isEmpty();

    ExclusionId findDuplicate(IAnomalyExclusion* pAE);

private:
    std::list<IAnomalyExclusion*> m_lstExclusions;
    IAnomalyExclusion* findExclusion(const cc::IString* pStrGuid, 
                                          const ULONG ulVid);
    IAnomalyExclusion* findExclusion(ExclusionId eid);
};

}

#endif