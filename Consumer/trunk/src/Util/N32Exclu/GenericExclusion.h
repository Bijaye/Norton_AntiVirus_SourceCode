////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/* This file is a horrific abomination. You should not abuse the
 * preprocessor like this.
 */

#ifndef __MY_CLASS
#define __MY_CLASS IExclusion
#endif

#ifndef __N32EXCLU_IMPLEMENTATION_
virtual ExResult isDeleted();
virtual ExResult isExpired();
virtual ExResult isActive();
virtual ExResult isMatch(const void* pDatum1, const void* pDatum2);

virtual ExResult setActive(bool active);
virtual ExResult setExpiryStamp(time_t expiry);
virtual ExResult setExclusionGuid(const cc::IString* newGuid);
virtual ExResult setID(const ULONGLONG newID);
virtual ExResult setDisplayName(const cc::IString* newDisplayName);
virtual ExResult setState(const ExclusionState newState);
virtual ExResult setUserData(const cc::IKeyValueCollection* pUserData);
virtual ExResult setStateFlag(const IExclusion::ExclusionState state, bool bTrue);

virtual const ExclusionType getExclusionType();
virtual ExResult getExclusionGuid(cc::IString*& pStrGuid);
virtual const ULONGLONG getID();
virtual ExResult getDisplayName(cc::IString*& pStrName);
virtual ExResult getUserData(cc::IKeyValueCollection* &pUserData);
virtual ExResult getStateFlag(const IExclusion::ExclusionState state);
virtual time_t getExpiryStamp();

virtual ExResult isValidForMatch();
virtual ExResult setDeleted(bool deleted);
virtual ExResult setExpired();

virtual bool isInitialized();

virtual __MY_INTERFACE* clone();

virtual IExclusion::ExclusionState getState();

virtual ExResult Save(cc::IStream* pStream);

protected:
    virtual ExResult isFlagSet(BVFlags flag);
    virtual ExResult setFlag(BVFlags flag, bool value);
    virtual ExResult setType(const ExclusionType newType);

    ExResult getStringFromBag(size_t position, cc::IString*& pStr);
    bool    m_bInitialized;
    cc::IKeyValueCollectionPtr m_spProperties;
private:
    virtual bool initMe();
    virtual bool uninitMe();
    bool areCommonItemsIdentical(IExclusion* ptr);
#else
bool __MY_CLASS::initMe()
{
    // Attempt to initialize the properties
    m_bInitialized = false;
    this->m_spProperties.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
    if(m_spProperties)
    {
        m_bInitialized = true;
        m_spProperties->SetAutoConvert(false);
        this->setType(IExclusion::ExclusionType::INVALID);
    }
    else
        CCTRACEW(_T(__MY_CLASSNAME)\
        _T("::initMe - Could not initialize ")\
        _T("key-value collection!"));

    return m_bInitialized;
}

bool __MY_CLASS::uninitMe()
{
    if(this->isInitialized())
    {
        if(this->m_spProperties)
        {
            this->m_spProperties.Release();
            this->m_bInitialized = false;
        }
    }
    return !this->m_bInitialized;
}

ExResult __MY_CLASS::isDeleted()
{
    return this->isFlagSet(BVFlags::DELETED);
}

ExResult __MY_CLASS::isExpired()
{
    time_t expiryStamp;
    if(!this->isInitialized()) 
    { 
        CCTRACEE(_T(__MY_CLASSNAME)\
            _T("::isExpired - Not initialized."));
        return NavExclusions::NotInitialized; 
    }

    if((expiryStamp = this->getExpiryStamp()) == 0)
        return NavExclusions::False;

    if(expiryStamp == 0 || expiryStamp > time(NULL))
        return NavExclusions::False;
    else
    {
        // Expire this exclusion, deleting if necessary
        if(!this->isDeleted())
            this->setExpired(); // Deletes and updates vars

        return NavExclusions::Success;
    }
}

ExResult __MY_CLASS::isActive()
{
    ExResult er = this->isFlagSet(BVFlags::DISABLED);
    if(er == NavExclusions::False)
        return NavExclusions::Success;
    else if(er == NavExclusions::Success)
        return NavExclusions::False;
    else
        return er;
}

ExResult __MY_CLASS::setActive(bool active)
{
    return this->setFlag(BVFlags::DISABLED, !active);
}

ExResult __MY_CLASS::setDeleted(bool deleted)
{
    return this->setFlag(BVFlags::DELETED, deleted);
}

ExResult __MY_CLASS::setExpired()
{
    return this->setFlag(BVFlags::DELETED, true);
}

const IExclusion::ExclusionType __MY_CLASS::getExclusionType()
{
    ExclusionType eType;
    if(!this->isInitialized()) { return IExclusion::ExclusionType::INVALID; }

    if(!m_spProperties->GetValue(IExclusion::Property::EType, (ULONG&)eType))
        return IExclusion::ExclusionType::INVALID;
    else
        return eType;
}

ExResult __MY_CLASS::getExclusionGuid(cc::IString*& pStrGuid)
{
    if(pStrGuid != NULL)
    {
        CCTRACEI(_T(__MY_CLASSNAME)\
            _T("::getExclusionGuid received a ")\
            _T("preinitialized string - releasing."));
        pStrGuid->Release();
        pStrGuid = NULL;
    }

    return this->getStringFromBag((size_t)IExclusion::Property::EGUID,
        pStrGuid);
}

const ULONGLONG __MY_CLASS::getID()
{
    ULONGLONG eid;
    if(!this->isInitialized()) { return _INVALID_ID; }

    if(!m_spProperties->GetValue(IExclusion::Property::EID, eid))
        return _INVALID_ID;
    else
        return eid;
}

ExResult __MY_CLASS::isValidForMatch()
{
    ExResult res = this->isExpired();
    if(res == NavExclusions::Fail || res == NavExclusions::Success)
        return NavExclusions::False;

    res = this->isDeleted();
    if(res == NavExclusions::Success || res == NavExclusions::Fail)
        return NavExclusions::False;

    if(this->getExclusionType() == IExclusion::ExclusionType::INVALID)
        return NavExclusions::False;

    return this->isActive();
}

ExResult __MY_CLASS::isFlagSet(IExclusion::BVFlags flag)
{
    ExclusionBitvector bv;
    if(!this->isInitialized()) { return NavExclusions::NotInitialized; }

    if(!m_spProperties->GetValue(IExclusion::Property::EFlags, (ULONGLONG)bv))
        return NavExclusions::Fail;
    else if(bv & flag)
        return NavExclusions::Success;
    else
        return NavExclusions::False;
}

ExResult __MY_CLASS::setFlag(IExclusion::BVFlags flag, bool value)
{
    ExclusionBitvector bv;
    if(!this->isInitialized()) { return NavExclusions::NotInitialized; }

    if(!m_spProperties->GetValue(IExclusion::Property::EFlags, (ULONGLONG)bv))
    {
        bv = IExclusion::BVFlags::NONE;
    }

    if(value)
        bv = bv | flag; // Ensure that 'flag' is set to 1
    else
        bv = bv & (~flag);  // Ensure that 'flag' is set to 0

    if(!m_spProperties->SetValue(IExclusion::Property::EFlags, (ULONGLONG)bv))
        return NavExclusions::Fail;
    else
        return NavExclusions::Success;
}

// Returns a freshly-allocated IString* copy from the bag.
ExResult __MY_CLASS::getStringFromBag(size_t position, cc::IString*& pStr)
{
    ISymBasePtr pBase;

    if(!this->isInitialized()) 
    {
        CCTRACEE(_T(__MY_CLASSNAME)\
            _T("::getStringFromBag() - Not initialized."));
        return NavExclusions::NotInitialized; 
    }

    if(pStr != NULL)
    {
        CCTRACEI(_T(__MY_CLASSNAME)\
            _T("::getStringFromBag() - Received a ")\
            _T("preinitialized string, releasing."));
        pStr->Release();
        pStr = NULL;
    }

    if(!m_spProperties->GetExists(position))
    {
        CCTRACEI(_T(__MY_CLASSNAME)\
                _T("::getStringFromBag() - Item does not exist."));
        return NavExclusions::False;
    }

    if(!m_spProperties->GetValue(position, pBase))
    {
        CCTRACEE(_T(__MY_CLASSNAME)\
            _T("::getStringFromBag() - Couldn't get ")\
            _T("string out of m_spProperties."));
        return NavExclusions::Fail;
    }

    if(!pBase)
    {
        CCTRACEI(_T(__MY_CLASSNAME)\
            _T("::getStringFromBag() - Retrieved ")\
            _T("NULL pointer from bag."));
        return NavExclusions::False;
    }

    if( SYM_FAILED(pBase->QueryInterface(cc::IID_String, 
        reinterpret_cast<void**>(&pStr))) )
    {
        CCTRACEE(_T(__MY_CLASSNAME)\
            _T("::getStringFromBag() - Couldn't QI ")\
            _T("retrieved object to IID_String."));
        return NavExclusions::Fail;
    }

    return NavExclusions::Success;
}

ExResult __MY_CLASS::setExpiryStamp(time_t expiry)
{
    if(!this->isInitialized()) { return NavExclusions::NotInitialized; }

    if(!m_spProperties->SetValue(IExclusion::Property::EExpiryStamp, (LONG)expiry))
        return NavExclusions::Fail;
    else
        return NavExclusions::Success;
}

ExResult __MY_CLASS::setType(const IExclusion::ExclusionType newType)
{
    if(!this->isInitialized()) { return NavExclusions::NotInitialized; }

    if(m_spProperties->SetValue(IExclusion::Property::EType, 
        (ULONG)newType))
        return NavExclusions::Success;
    else
        return NavExclusions::Fail;
}

ExResult __MY_CLASS::setExclusionGuid(const cc::IString* newGuid)
{
    if(!this->isInitialized()) { return NavExclusions::NotInitialized; }

    if(m_spProperties->SetValue(IExclusion::Property::EGUID, newGuid))
        return NavExclusions::Success;
    else
        return NavExclusions::Fail;        
}

ExResult __MY_CLASS::setDisplayName(const cc::IString* newDisplayName)
{
    if(!this->isInitialized()) { return NavExclusions::NotInitialized; }

    if(m_spProperties->SetValue(IExclusion::Property::EThreatName,
        newDisplayName))
        return NavExclusions::Success;
    else
        return NavExclusions::Fail;
}

ExResult __MY_CLASS::setID(const ULONGLONG newID)
{
    if(!this->isInitialized()) { return NavExclusions::NotInitialized; }

    if(m_spProperties->SetValue(IExclusion::Property::EID, newID))
        return NavExclusions::Success;
    else
        return NavExclusions::Fail;
}

ExResult __MY_CLASS::setState(const ExclusionState state)
{
    ULONG s;
    if(!this->isInitialized()) { return NavExclusions::NotInitialized; }

    // Only allow bits that exist to be set.
    s = state & IExclusion::ExclusionState::EXCLUDE_ALL;

    if(!m_spProperties->SetValue(IExclusion::Property::EState, s))
        return NavExclusions::Fail;
    else
        return NavExclusions::Success;
}

bool __MY_CLASS::isInitialized() { return this->m_bInitialized; }

IExclusion::ExclusionState __MY_CLASS::getState()
{
    ULONG s;
    if(!this->isInitialized()) { IExclusion::ExclusionState::NOT_EXCLUDED; }

    if(!m_spProperties->GetValue(IExclusion::Property::EState, s))
    {   // Nothing set? Then everything is off.
        s = IExclusion::ExclusionState::NOT_EXCLUDED;
    }

    return (IExclusion::ExclusionState)s;
}

ExResult __MY_CLASS::setUserData(const cc::IKeyValueCollection* pUserData)
{
    if(!this->isInitialized()) { return NavExclusions::NotInitialized; }

    if(m_spProperties->SetValue(IExclusion::Property::UserData,
        pUserData))
        return NavExclusions::Success;
    else
        return NavExclusions::Fail;
}

ExResult __MY_CLASS::getUserData(cc::IKeyValueCollection*& pUserData)
{
    ISymBasePtr pBase;

    if(!this->isInitialized()) 
    {
        CCTRACEE(_T(__MY_CLASSNAME)\
              _T("::getUserData() - Not initialized."));
        return NavExclusions::NotInitialized; 
    }

    if(pUserData != NULL)
    {
        CCTRACEI(_T(__MY_CLASSNAME)\
            _T("::getUserData() - Received a ")\
            _T("preinitialized object, releasing."));
        pUserData->Release();
        pUserData = NULL;
    }

    if(!m_spProperties->GetValue(IExclusion::Property::UserData, pBase))
    {
        CCTRACEI(_T(__MY_CLASSNAME)\
            _T("::getUserData() - Couldn't get ")\
            _T("UserData out of m_spProperties. Probably hasn't ")\
            _T("been created yet."));
        return NavExclusions::False;
    }

    if(!pBase)
    {
        CCTRACEI(_T(__MY_CLASSNAME)\
            _T("::getUserData() - Retrieved ")\
            _T("NULL pointer from bag. Might not have been ")\
            _T("created/initialized yet."));
        return NavExclusions::False;
    }

    if( SYM_FAILED(pBase->QueryInterface(cc::IID_KeyValueCollection, 
        reinterpret_cast<void**>(&pUserData))) )
    {
        CCTRACEE(_T(__MY_CLASSNAME)\
            _T("::getUserData() - Couldn't QI ")\
            _T("retrieved object to IID_KVC."));
        return NavExclusions::NoInterface;
    }

    return NavExclusions::Success;
}

bool __MY_CLASS::areCommonItemsIdentical(IExclusion* ptr)
{
    if(!ptr) return false;
    if(!this->isInitialized()) return false;

    // EID
    ULONGLONG myId;
    myId = this->getID();
    if(myId > _INVALID_ID && myId != ptr->getID())
        return false;

    // Etype
    IExclusion::ExclusionType eType = this->getExclusionType();
    if(eType != IExclusion::ExclusionType::INVALID 
        && eType != ptr->getExclusionType())
        return false;

    // EGUID
    cc::IStringPtr myGuid, ptrGuid;
    if(this->getExclusionGuid(myGuid) != NavExclusions::Success ||
        ptr->getExclusionGuid(ptrGuid) != NavExclusions::Success)
        return false;
    else if(wcscmp(myGuid->GetStringW(), ptrGuid->GetStringW()) != 0)
        return false;

    return true;
}

ExResult __MY_CLASS::getStateFlag(const IExclusion::ExclusionState state)
{
    ULONG currentState;

    if(!this->isInitialized())
        return NavExclusions::NotInitialized;

    currentState = this->getState();
    if((currentState & state))
        return NavExclusions::Success;
    else
        return NavExclusions::False;
}

ExResult __MY_CLASS::setStateFlag(const IExclusion::ExclusionState state, bool bTrue)
{
    ULONG currentState;

    if(!this->isInitialized())
        return NavExclusions::NotInitialized;

    currentState = this->getState();
    if(bTrue)
    {
        currentState |= state;
    }
    else
    {
        currentState &= ~(state);
    }

    return this->setState((IExclusion::ExclusionState)currentState);
}

ExResult __MY_CLASS::Save(cc::IStream* pStream)
{
    if(!this->isInitialized())
    {
        CCTRACEE(_T(__MY_CLASSNAME)\
            _T("::Save() - Not initialized."));

        return NavExclusions::NotInitialized;
    }

    if(!pStream)
    {
        CCTRACEE(_T(__MY_CLASSNAME)\
            _T("::Save() - Can't save to NULL stream!"));
        return NavExclusions::InvalidArgument;
    }

    cc::ISerializeQIPtr spQI = this->m_spProperties;
    if(!spQI)
    {
        CCTRACEE(_T(__MY_CLASSNAME)\
            _T("::Save() - Couldn't QI properties collection ")\
            _T("to ISerialize."));

        return NavExclusions::NoInterface;
    }

    if(!spQI->Save(pStream))
    {
        CCTRACEE(_T(__MY_CLASSNAME)\
            _T("::Save() - Error while saving."));
        return NavExclusions::Fail;
    }

    return NavExclusions::Success;
}

time_t __MY_CLASS::getExpiryStamp()
{
    time_t expiry;

    if(!this->isInitialized()) { return 0; }

    if(!m_spProperties->GetValue(IExclusion::Property::EExpiryStamp, (LONG)expiry))
        return 0;
    else
        return expiry;
}

#endif
