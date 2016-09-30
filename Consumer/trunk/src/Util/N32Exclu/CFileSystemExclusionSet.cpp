////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/***
 * Implementation: CFileSystemExclusionSet
 ***/
#ifndef __N32EXCLU_cfilesystemexclusionset_cpp__
#define __N32EXCLU_cfilesystemexclusionset_cpp__

#include "n32exclu.h"
#include "CFileSystemExclusionSet.h"

namespace NavExclusions
{
    CFileSystemExclusionSet::CFileSystemExclusionSet()
    {}

    CFileSystemExclusionSet::~CFileSystemExclusionSet()
    {
        while(!this->m_lstExclusions.empty())
        {
            this->m_lstExclusions.front()->Release();
            this->m_lstExclusions.pop_front();
        }
    }

    ExResult CFileSystemExclusionSet::addExclusion(IFileSystemExclusion* pExclusion)
    {
        cc::IStringPtr spStrPath;

        if(!pExclusion)
        {
            CCTRACEE(_T("CFileSystemExclusionSet::addExclusion - Received a null ")\
                        _T("pointer as an argument."));
            return NavExclusions::InvalidArgument;
        }

        if(pExclusion->getExclusionType() != IExclusion::ExclusionType::FILESYSTEM)
        {
            CCTRACEE(_T("CFileSystemExclusionSet::addExclusion - Received a ")\
                        _T("non-FS exclusion as an argument."));
            return NavExclusions::InvalidArgument;
        }

        if(!pExclusion->isInitialized())
        {
            CCTRACEE(_T("CFileSystemExclusionSet::addExclusion - Received an ")\
                        _T("uninitialized exclusion object."));
            return NavExclusions::InvalidArgument;        
        }

        if(pExclusion->getFSPath(spStrPath) >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CFileSystemExclusionSet::addExclusion - Failure when ")\
                        _T("trying to retrieve FS path."));
            return NavExclusions::Fail;
        }

        // Check for duplicates
        if(this->findExclusionId(spStrPath) != _INVALID_ID)
        {
            CCTRACEW(_T("CFileSystemExclusionSet::addExclusion - Rejecting add ")\
                        _T("of duplicate exclusion."));
            return NavExclusions::False;
        }
             
        spStrPath.Release();

        // Add exclusion
        this->m_lstExclusions.push_back(pExclusion);
        if(this->m_lstExclusions.back() == pExclusion)
        {
            pExclusion->AddRef();
            return NavExclusions::Success;
        }
        else
        {   // This should never happen
            CCTRACEE(_T("CFileSystemExclusion::addExclusion - back != argument ")\
                        _T("after a push_back()! Weirdness in STL List?"));
            return NavExclusions::Fail;
        }
    }

    /*** Search Methods - Very much the same with different returns/calls. ***/
    const IFileSystemExclusion* CFileSystemExclusionSet::getExclusion(ExclusionId eid)
    {
        return this->findExclusion(eid);
    }

    const IFileSystemExclusion* CFileSystemExclusionSet::getExclusion(const cc::IString* pStrPath)
    {
        return this->findExclusion(pStrPath);
    }

    ExclusionId CFileSystemExclusionSet::findExclusionId(const cc::IString* pStrPath)
    {
        IFileSystemExclusion* pAE = this->findExclusion(pStrPath);
        if(pAE)
            return pAE->getID();
        else
            return _INVALID_ID;
    }

    ExResult CFileSystemExclusionSet::removeExclusion(ExclusionId eid)
    {
        if(eid <= _INVALID_ID) { return NavExclusions::InvalidArgument; }

        IFileSystemExclusion* pAE = NULL;
        std::list<IFileSystemExclusion*>::const_iterator it;
        for(it = this->m_lstExclusions.begin(); 
            it != this->m_lstExclusions.end(); it++)
        {
            if((*it)->getID() == eid)
            {
                pAE = (*it);
                break;
            }
        }

        if(!pAE)
        {
            CCTRACEW(_T("CFileSystemExclusion::removeExclusion - Couldn't find ")\
                    _T("an exclusion with the supplied EID."));
            return NavExclusions::Fail;
        }

        // Remove from list and release our refcount to it.
        this->m_lstExclusions.remove(pAE);
        pAE->Release();

        return NavExclusions::Success;
    }

    ExResult CFileSystemExclusionSet::removeExclusion(const cc::IString* pStrPath)
    {
        if(pStrPath == NULL) { return NavExclusions::InvalidArgument; }

        IFileSystemExclusion* pAE = NULL;
        std::list<IFileSystemExclusion*>::const_iterator it;
        for(it = this->m_lstExclusions.begin(); 
            it != this->m_lstExclusions.end(); it++)
        {
            if((*it)->isExactMatch(pStrPath) == NavExclusions::Success)
            {
                pAE = (*it);
                break;
            }
        }

        if(!pAE)
        {
            CCTRACEW(_T("CFileSystemExclusion::removeExclusion - Couldn't find ")\
                        _T("an exclusion with the supplied GUID and VID."));
            return NavExclusions::Fail;
        }

        // Remove from list and release our refcount to it.
        this->m_lstExclusions.remove(pAE);
        pAE->Release();

        return NavExclusions::Success;
    }

    IExclusion::ExclusionState CFileSystemExclusionSet::isExcluded(
        const cc::IString* pStrPath, 
        IExclusion::ExclusionState requiredState)
    {
        IExclusion::ExclusionState eState;
        if(!pStrPath) { return IExclusion::ExclusionState::NOT_EXCLUDED; }

        std::list<IFileSystemExclusion*>::iterator it;
        for(it = this->m_lstExclusions.begin();
            it != this->m_lstExclusions.end(); it++)
        {
            // isValidForMatch handles expiration, deletion, etc.
            if((*it)->isValidForMatch() == NavExclusions::Success
                && (*it)->isMatch((void*)pStrPath, (void*)&requiredState) == NavExclusions::Success)
            {
                eState = (*it)->getState();
                // If at least as restrictive as required...
                if((eState & requiredState) == requiredState)
                    return eState;
            }
        }

        return IExclusion::ExclusionState::NOT_EXCLUDED;
    }

    // Removes all contents. Yes, it is safe to do this multiple times
    // because STL IS GREAT.
    ExResult CFileSystemExclusionSet::nuke()
    {
        while(!this->m_lstExclusions.empty())
        {
            this->m_lstExclusions.front()->Release();
            this->m_lstExclusions.pop_front();
        }
        return NavExclusions::Success;
    }

    ExResult CFileSystemExclusionSet::isEmpty()
    {
        if(this->m_lstExclusions.empty()) { return NavExclusions::Success; }
        else { return NavExclusions::False; }
    }

    /*** Privates ***/
    IFileSystemExclusion* CFileSystemExclusionSet::findExclusion(ExclusionId eid)
    {
        if(eid <= _INVALID_ID) { return NULL; } // Yes, you are funny, Mr. User.

        std::list<IFileSystemExclusion*>::const_iterator it;
        for(it = this->m_lstExclusions.begin(); 
            it != this->m_lstExclusions.end(); it++)
        {
            if((*it)->getID() == eid
                && (*it)->isValidForMatch() == NavExclusions::Success)
            {
                return (*it);
            }
        }

        return NULL;
    }

    // We could do this by finding the ID, then calling getExclusion(ID),
    // but that would cost us O(2N) instead of O(N).
    IFileSystemExclusion* CFileSystemExclusionSet::findExclusion(const cc::IString* pStrPath)
    {
        if(pStrPath == NULL) 
        { return NULL; } // Yes, you are funny, Mr. User.

        std::list<IFileSystemExclusion*>::const_iterator it;
        for(it = this->m_lstExclusions.begin(); 
            it != this->m_lstExclusions.end(); it++)
        {
            if((*it)->isExactMatch(pStrPath) == NavExclusions::Success
                && (*it)->isValidForMatch() == NavExclusions::Success)
            {
                return (*it);
            }
        }

        return NULL;
    }

    ExclusionId CFileSystemExclusionSet::findDuplicate(IFileSystemExclusion* pFSE)
    {
        ExclusionId myId, thisId;
        cc::IStringPtr spPath;

        std::list<IFileSystemExclusion*>::const_iterator it;

        if(!pFSE || pFSE->getExclusionType() != IExclusion::ExclusionType::FILESYSTEM)
            return _INVALID_ID;

        myId = pFSE->getID();
        if(pFSE->getFSPath(spPath) >= NavExclusions::Fail)
        {   // Indicate error by returning the same ID
            return myId;
        }

        for(it = this->m_lstExclusions.begin(); 
            it != this->m_lstExclusions.end(); it++)
        {
            thisId = (*it)->getID();
            if(thisId != myId && 
                (*it)->isExactMatch(spPath) == NavExclusions::Success)
            {
                return thisId;
            }
        }

        return _INVALID_ID;
    }
}

#endif
