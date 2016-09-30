////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/***
 * Implementation file for Exclusion Factory; this class provides a
 * unified interface for constructing IExclusions and their
 * subclasses.
 *  NOTE: Whenever a create...() call is made, memory is allocated!
 * You _MUST_ pass it back to the DestroyExclusion() method for deletion,
 * otherwise EIDs will never be reused and this can lead to lots of
 * wasted memory.
 *  The exception to this rule is if you're destroying a CExclusionManager
 * wholesale - at that point, go ahead and delete them however you like.
 ***/

#ifndef __N32EXCLU_cexclusionfactory_cpp__
#define __N32EXCLU_cexclusionfactory_cpp__

#define CEF_RELEASE(pE) this->destroyExclusion((IExclusion*&)pE); pE = NULL;

// Common Client headers
#include "n32exclu.h"
#include "SymInterface.h"
#include "ccString.h"
#include "ccStringInterface.h"
#include "ccSymStringImpl.h"

#include "CExclusionFactory.h"

namespace NavExclusions
{
    CExclusionFactory::CExclusionFactory()
    {
        this->resetTopId();
        this->dumpIdStack();
    }

    CExclusionFactory::~CExclusionFactory() {}

    /*** ONLY FOR USE BY CEXCLUSION MANAGER ***/
    ExResult CExclusionFactory::resetTopId()
    {
        this->myMaxId = _INVALID_ID;
        return NavExclusions::Success;
    }

    /** Dump in this case means 'make empty' **/
    ExResult CExclusionFactory::dumpIdStack()
    {
        while(!(this->myUnusedIds.empty()))
            this->myUnusedIds.pop_back();
        return NavExclusions::Success;
    }

    /* When resetting the factory, reset the top ID, dump the stack
     * and then assign the new IDs using this method.
     * This will ensure that the MaxID is properly captured.
     */
    ExResult CExclusionFactory::assignId(IExclusion *pExclusion, 
                                         ULONGLONG newId)
    {
        if(!pExclusion)
        {
            CCTRACEE(_T("CExclusionFactory::assignId - Null parameter ")\
                        _T("received."));
            return NavExclusions::InvalidArgument;
        }

        if(pExclusion->setID(newId) == NavExclusions::Success)
        {
            if(newId > this->myMaxId) { this->myMaxId = newId; }
            return NavExclusions::Success;
        }
        else
        {
            CCTRACEW(_T("CExclusionFactory::assignID - Could not assign ID ")\
                        _T("to exclusion; internal error likely."));
            return NavExclusions::Fail;
        }
    }
    /*** END ONLY FOR USE BY CEXCLUSIONMANAGER ***/ 

    ExResult CExclusionFactory::destroyExclusion(IExclusion* &pExclusion)
    {
        ULONGLONG exclusionId;
        if(!pExclusion)
        {
            CCTRACEE(_T("CExclusionFactory::destroyExclusion - Invalid ")\
                        _T("argument received."));
            return NavExclusions::InvalidArgument;
        }

        exclusionId = pExclusion->getID();
        if(exclusionId <= _INVALID_ID)
        {
            CCTRACEW(_T("CExclusionFactory::destroyExclusion - Could not ")\
                        _T("retrieve exclusion ID or invalid exclusion ID. ")\
                        _T("Internal error likely; the state of the exclusion ID ")\
                        _T("space may no longer be known."));
            return NavExclusions::Fail;
        }

        // If this is the top value, just decrement the MaxID value.
        // We don't need to close gaps since the assignment function
        // will always go to the list-stack first.
        if(exclusionId == this->myMaxId)
            this->myMaxId--;
        else // Add unused ID to the 'unused' stack
            this->myUnusedIds.push_front(exclusionId);

        // Perform the release
        pExclusion->Release();
        pExclusion = NULL;

        return NavExclusions::Success;
    }

    /***
     * This function manufactures a CFileSystemExclusion, which encapsulates
     * path-based exclusions.
     *
     * Arguments:
     *
     *  pExclusion - An IExclusion pointer pointing to NULL. Used as output.
     *  bActive - Whether or not the exclusion is active by default
     *  threatName - String for the displayable threat name
     *  lpPath - A ccLib::CStringW pointer, indicating a filesystem path or
     *          file name. Exactly what is valid is currently unclear.
     *
     *  Returns a pointer to a freshly-allocated CFileSystemExclusion object in
     * the pExclusion argument and returns an ExResult with a success/fail code.
     * This object must subsequently be freed with delete or a call
     * to CExclusionFactory::destroyExclusion(...).
     * Returns NULL in pExclusion in the event of an error.
     ***/
    ExResult CExclusionFactory::createFSExclusion(
                                            IFileSystemExclusion* &pExclusion,
                                            const cc::IString* lpPath,
                                            time_t expiryStamp,
                                            bool bActive,
                                            IExclusion::ExclusionState eState)
    {
        ULONGLONG myNewId;
        GUID myGuid;
        wchar_t guidBuf[39];
        cc::IStringPtr spScratchString;

        // Silly user, your tricks are for kids
        if(pExclusion != NULL)
        {
            CCTRACEI(_T("CExclusionFactory::createFSExclusion - Received a ")\
                        _T("non-NULL pointer. Autoreleasing."));
            pExclusion->Release();
            pExclusion = NULL;
        }

        // Ensure we have a path
        if(!lpPath || lpPath->GetLength() < 1)
        {
            CCTRACEE(_T("CExclusionFactory::createFSExclusion - Invalid path ")\
                        _T("(NULL or zero-length) received."));
            return NavExclusions::InvalidPath;
        }

        // Valid exclusion state?
        if(eState > IExclusion::ExclusionState::EXCLUDE_ALL ||
            eState <= IExclusion::ExclusionState::NOT_EXCLUDED)
        {
            CCTRACEE(_T("CExclusionFactory::createFSExclusion - Invalid ")\
                        _T("exclusion state received."));
            return NavExclusions::InvalidArgument;
        }

        // Allocate!
        CFileSystemExclusion* pFSExclusionTemp = new(std::nothrow) CFileSystemExclusion();

        //pExclusion = new(std::nothrow) CFileSystemExclusion();
        if(!pFSExclusionTemp)
        {
            CCTRACEE(_T("CExclusionFactory::createFSExclusion - Could not ")\
                        _T("allocate a new CFileSystemExclusion object."));
            return NavExclusions::OutOfMemory;
        }

        if( SYM_FAILED(pFSExclusionTemp->QueryInterface(IID_IFileSystemExclusion, (void**)&pExclusion)) || !pExclusion )
        {
            delete pFSExclusionTemp;
            pFSExclusionTemp = NULL;
            return NavExclusions::NoInterface;
        }

        // Store path
        if(pExclusion->setFSPath(lpPath) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createFSExclusion - Could not ")\
                        _T("assign FS path."));
            CEF_RELEASE(pExclusion)
            return NavExclusions::Fail;
        }

        // Store expiry stamp
        if(pExclusion->setExpiryStamp(expiryStamp) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createFSExclusion - Could not set ")\
                        _T("expiry stamp."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Store exclusion state (viral/nonviral) - passing in true
        // because eState input means 'turn these states on'.
        if(pExclusion->setState(eState) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createFSExclusion - Could not set ")\
                        _T("exclusion state (v/nv, subdirs)."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Store activity state
        if(pExclusion->setActive(bActive) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createFSExclusion - Could not set ")\
                        _T("Activation state."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Make a new GUID
        if(CoCreateGuid(&myGuid) != S_OK)
        {
            CCTRACEE(_T("CExclusionFactory::createFSExclusion - Could not ")\
                        _T("create a new GUID."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Stringify guid
        if(StringFromGUID2(myGuid, guidBuf, 39) != 39)
        {
            CCTRACEE(_T("CExclusionFactory::createFSExclusion - Couldn't ")\
                        _T("stringify GUID!"));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Move into IString
        spScratchString.Attach(ccSym::CStringImpl::CreateStringImpl(guidBuf));
        if(!spScratchString)
        {
            CCTRACEE(_T("CExclusionFactory::createFSExclusion - Couldn't ")\
                        _T("allocate scratch string for GUID."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::OutOfMemory;
        }

        // Set!
        if(pExclusion->setExclusionGuid(spScratchString) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createFSExclusion - Couldn't copy ")\
                        _T("GUID string into exclusion."));
            spScratchString.Release();
            CEF_RELEASE(pExclusion);
            return NavExclusions::False;
        }

        spScratchString.Release();

        // Retrieve a nonpersistent Exclusion ID
        myNewId = this->getFreeId();
        if(myNewId <= _INVALID_ID)
        {
            CCTRACEE(_T("CExclusionFactory::createFSExclusion - Internal Error")\
                        _T(" - Could not get a new EID."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Store the EID
        if(pExclusion->setID(myNewId) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createFSExclusion - Could not set ")\
                        _T("EID!"));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // whee
        return NavExclusions::Success;
    }

    /***
     * This function manufactures a CAnomalyExclusion, which encapsulates
     * VID+GUID exclusions.
     *
     * Arguments:
     *
     *  pExclusion - An IExclusion pointer pointing to null. Used as output.
     *  bActive - Whether or not the exclusion is active by default
     *  threatName - String for the displayable threat name
     * 
     * AT LEAST ONE OF THESE TWO ARGUMENTS MUST BE SET:
     *  lpGUID - A pointer to a CStringW object containing the GUID of the
     *           anomaly to be excluded.
     *  ulVid - An ULONGLONG containing the VID to be excluded.
     *
     *  Returns a pointer to a freshly-allocated CAnomalyExclusion object in
     * the pExclusion argument and returns an ExResult with a success/fail code.
     * This object must subsequently be freed with delete or a call
     * to CExclusionFactory::destroyExclusion(...).
     * Returns NULL in pExclusion in the event of an error.
     ***/
    ExResult CExclusionFactory::createAnomalyExclusion(
                                                IAnomalyExclusion* &pExclusion,
                                                const cc::IString* lpGUID,
                                                const ULONG ulVid,
                                                time_t expiryStamp,
                                                bool bActive,
                                                const cc::IString* lpThreatName,
                                              IExclusion::ExclusionState eState)
    {
        ULONGLONG myNewId;
        GUID myGuid;
        wchar_t guidBuf[39];
        cc::IStringPtr spScratchString;

        // Will not overwrite an existing exclusion.
        if(pExclusion != NULL)
        {
            CCTRACEI(_T("CExclusionFactory::createAnomalyExclusion - ")\
                        _T("Received a non-NULL exclusion pointer; autoreleasing."));
            pExclusion->Release();
            pExclusion = NULL;
        }

        // Check for an invalid VID -and- an invalid anomaly GUID.
        if((!lpGUID || lpGUID->GetLength() < 1) && ulVid <= _INVALID_VID)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Received ")\
                        _T("an invalid GUID string AND an invalid VID. One ")\
                        _T("of these must be valid."));
            return NavExclusions::InvalidArgument;
        }

        // Check for an invalid threat name, which is pretty much needed
        // for VID+GUID exclusions.
        if(!lpThreatName || lpThreatName->GetLength() < 1)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Received ")\
                        _T("an invalid Threat Name. Anomaly exclusions must have ")\
                        _T("a prespecified threat name."));
            return NavExclusions::InvalidArgument;
        }

        // Allocate!
        pExclusion = new(std::nothrow) CAnomalyExclusion();
        if(!pExclusion)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Could ")\
                        _T("not allocate a new CAnomalyExclusion object."));
            return NavExclusions::OutOfMemory;
        }
        pExclusion->AddRef();

        // Set the anomaly GUID - short circuits if no GUID passed in
        if(lpGUID && 
                pExclusion->setAnomalyGuid(lpGUID) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Unable ")\
                        _T("to set the Anomaly GUID string."));
            CEF_RELEASE(pExclusion)
            return NavExclusions::Fail;
        }

        // Set the VID - short circuits if invalid VID is passed in
        if(ulVid > _INVALID_VID &&
            pExclusion->setVid(ulVid) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Unable ")\
                        _T("to set the VID."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Set the threat name
        if(pExclusion->setDisplayName(lpThreatName) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Unable to ")\
                        _T("set the Threat Name."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Set the expiry stamp
        if(pExclusion->setExpiryStamp(expiryStamp) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Unable ")\
                        _T("to set the requested expiry stamp."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Store exclusion state (viral/nonviral) - passing in true
        // because eState input means 'turn these states on'.
        if(pExclusion->setState(eState) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Could not set ")\
                _T("exclusion state (v/nv, subdirs)."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Set inactive flag if needed
        if(pExclusion->setActive(bActive) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Unable ")\
                        _T("to set the Activation state."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Make a new GUID
        if(CoCreateGuid(&myGuid) != S_OK)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Could not ")\
                        _T("create a new GUID."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Stringify guid
        if(StringFromGUID2(myGuid, guidBuf, 39) != 39)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Couldn't ")\
                        _T("stringify GUID!"));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Move into IString
        spScratchString.Attach(ccSym::CStringImpl::CreateStringImpl(guidBuf));
        if(!spScratchString)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Couldn't ")\
                        _T("allocate scratch string for GUID."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::OutOfMemory;
        }

        // Set!
        if(pExclusion->setExclusionGuid(spScratchString) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Couldn't copy ")\
                        _T("GUID string into exclusion."));
            spScratchString.Release();
            CEF_RELEASE(pExclusion);
            return NavExclusions::False;
        }

        spScratchString.Release();

        // Retrieve a non-persistent exclusion ID
        myNewId = this->getFreeId();
        if(myNewId <= _INVALID_ID)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Unable ")\
                        _T("to retrieve a new EID."));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Set the ID
        if(pExclusion->setID(myNewId) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::createAnomalyExclusion - Unable to ")\
                        _T("properly set the EID!"));
            CEF_RELEASE(pExclusion);
            return NavExclusions::Fail;
        }

        // Yay, how exciting
        return NavExclusions::Success;
    }

    /***
     * This is the primary function for creating exclusions. It is mostly
     * a driver for the createFooExclusion(...) methods.
     *
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
    ExResult CExclusionFactory::createExclusion(
                                              IExclusion* &pExclusion,
                                              IExclusion::ExclusionType eType,
                                              time_t expiryStamp,
                                              bool bActive,
                                              IExclusion::ExclusionState eState,
                                              const cc::IString* threatName,
                                              const void* pDatum1,
                                              const void* pDatum2)
    {
        // Triage!
        if(pExclusion != NULL)
        {
            CCTRACEW(_T("CExclusionFactory::createExclusion - Received a ")\
                        _T("non-NULL exclusion pointer. Autoreleasing."));
            pExclusion->Release();
            pExclusion = NULL;
        }

        if(eType == IExclusion::ExclusionType::ANOMALY)
        {
            const cc::IString* lpGUID = (const cc::IString*)pDatum1;
            const ULONG* lpVid = (const ULONG*)pDatum2;
            return this->createAnomalyExclusion(
                                              (IAnomalyExclusion*&)pExclusion,
                                              lpGUID,
                                              (lpVid==NULL?_INVALID_VID:*lpVid),
                                              expiryStamp, bActive, threatName,
                                              eState);
        }
        else if(eType == IExclusion::ExclusionType::FILESYSTEM)
        {
            const cc::IString* lpPath = (const cc::IString*)pDatum1;
            return this->createFSExclusion((IFileSystemExclusion*&)pExclusion, 
                                           lpPath, expiryStamp, bActive, 
                                           eState);
        }
        else
        {
            return NavExclusions::InvalidArgument;
        }
    }

    /***
     * This function retrieves an unused Exclusion ID (nonpersistant ID).
     * In case of failure, it returns _INVALID_ID.
     * Unused IDs are preferentially fetched from the list-stack of
     * previously-released IDs; if no previously-released IDs are available,
     * then and only then does it 'create' a new one by incrementing the
     * maximum known ID.
     ***/
    ULONGLONG CExclusionFactory::getFreeId()
    {
        if(!this->myUnusedIds.empty())
        {
            ULONGLONG myId = (this->myUnusedIds.front());
            this->myUnusedIds.pop_front();
            if(myId > this->myMaxId) { this->myMaxId = myId; } // Just in case.
            return myId;
        }
        else
        {   // Increment maximum known ID and return.
            return ++(this->myMaxId);
        }
    }

    ExResult CExclusionFactory::rebuildExclusion(IExclusion* &pExclusion,
                                                 cc::IKeyValueCollection* pData)
    {
        ULONG buf;
        IExclusion::ExclusionType eType;
        ExclusionId myId;

        if(!pData)
        {
            CCTRACEE(_T("CExclusionFactory::rebuildExclusion - Received a ")\
                        _T("NULL pointer for the KVC."));
            return NavExclusions::InvalidArgument;
        }

        if(pExclusion != NULL)
        {
            CCTRACEW(_T("CExclusionFactory::rebuildExclusion - Received a ")\
                        _T("non-NULL Exclusion object. Autoreleasing."));
            pExclusion->Release();
            pExclusion = NULL;
        }

        if(!pData->GetValue(IExclusion::Property::EType, buf))
        {
            CCTRACEE(_T("CExclusionFactory::rebuildExclusion - Received a ")\
                        _T("KVC that does not include exclusion type. Error ")\
                        _T("during serialization/deserialization?"));
            return NavExclusions::Fail;
        }

        eType = (IExclusion::ExclusionType)buf;

        if(eType == IExclusion::ExclusionType::ANOMALY)
        {
            IAnomalyExclusion* pAE = new(std::nothrow) CAnomalyExclusion(pData);
            if(!pAE)
            {
                CCTRACEE(_T("CExclusionFactory::rebuildExclusion - Could not ")\
                            _T("allocate memory for a new CAnomalyExclusion."));
                return NavExclusions::OutOfMemory;
            }
            pAE->AddRef();

            IExclusionQIPtr ptr = pAE;
            if(!ptr)
            {
                CCTRACEE(_T("CExclusionFactory::rebuildExclusion - Could not ")\
                            _T("QI the exclusion object to an IExclusion."));
                pAE->Release();
                return NavExclusions::NoInterface;
            }

            pExclusion = ptr;
        }
        else if(eType == IExclusion::ExclusionType::FILESYSTEM)
        {
            IFileSystemExclusion* pFSE = new(std::nothrow) CFileSystemExclusion(pData);
            if(!pFSE)
            {
                CCTRACEE(_T("CExclusionFactory::rebuildExclusion - Could not ")\
                    _T("allocate memory for a new CFileSystemExclusion."));
                return NavExclusions::OutOfMemory;
            }
            pFSE->AddRef();

            IExclusionQIPtr ptr = pFSE;
            if(!ptr)
            {
                CCTRACEE(_T("CExclusionFactory::rebuildExclusion - Could not ")\
                    _T("QI the exclusion object to an IExclusion."));
                pFSE->Release();
                return NavExclusions::NoInterface;
            }

            pExclusion = ptr;
        }
        else
        {
            CCTRACEE(_T("CExclusionFactory::rebuildExclusion - Input KVC ")\
                        _T("has an invalid exclusion type set."));
            return NavExclusions::InvalidArgument;
        }

        // Assign an ID
        myId = this->getFreeId();
        if(this->assignId(pExclusion, myId) != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusionFactory::rebuildExclusion - Failed to ")\
                        _T("assign a valid ID to the exclusion."));
            pExclusion->Release();
            pExclusion = NULL;
            return NavExclusions::Fail;
        }

        return NavExclusions::Success;
    }
}

#endif