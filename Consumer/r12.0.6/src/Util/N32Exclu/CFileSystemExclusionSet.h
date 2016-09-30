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
#ifndef __N32EXCLU_cfilesystemexclusionset_h__
#define __N32EXCLU_cfilesystemexclusionset_h__

#include "n32exclu.h"
#include "ExclusionInterface.h"
#include <list>

namespace NavExclusions
{

class CFileSystemExclusionSet
{
public:
    CFileSystemExclusionSet();
    ~CFileSystemExclusionSet();
    IExclusion::ExclusionState isExcluded(const cc::IString* pStrPath, 
        IExclusion::ExclusionState requiredState);
    ExclusionId findExclusionId(const cc::IString* pStrPath);
    const IFileSystemExclusion* getExclusion(const cc::IString* pStrPath);
    const IFileSystemExclusion* getExclusion(ExclusionId eid);
    ExResult addExclusion(IFileSystemExclusion* pExclusion);
    ExResult removeExclusion(const cc::IString* pStrPath);
    ExResult removeExclusion(ExclusionId eid);

    ExResult nuke();
    ExResult isEmpty();

    ExclusionId findDuplicate(IFileSystemExclusion* pFSE);

private:
    std::list<IFileSystemExclusion*> m_lstExclusions;
    IFileSystemExclusion* findExclusion(const cc::IString* pStrPath);
    IFileSystemExclusion* findExclusion(ExclusionId eid);
};

}

#endif