////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/***
 * Interface header file for Anomaly exclusion object. Primarily provides
 *  driver functions for manipulating guids/vids and implements proper
 *  matching of similar objects.
 ***/
#pragma once

#ifndef __N32EXCLU_canomalyexclusion_h__
#define __N32EXCLU_canomalyexclusion_h__

#define __MY_INTERFACE IAnomalyExclusion
#define __MY_CLASS CAnomalyExclusion
#define __MY_CLASSNAME "CAnomalyExclusion"

#include <time.h>
#include "n32exclu.h"
#include "ExclusionInterface.h"

namespace NavExclusions
{

class CAnomalyExclusion: public IAnomalyExclusion,
                         public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    CAnomalyExclusion();
    CAnomalyExclusion(cc::IKeyValueCollection* rhs);
    virtual ~CAnomalyExclusion();

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(IID_IExclusion, IExclusion)
        SYM_INTERFACE_ENTRY(IID_IAnomalyExclusion, IAnomalyExclusion)
    SYM_INTERFACE_MAP_END()

    enum Property
    {
        AnomalyGuid = IExclusion::Property::Last_Prop + 1,
        Vid,
        Last_Prop = 2000
    };

    virtual NavExclusions::ExResult setAnomalyGuid(const cc::IString* guid);
    virtual ExResult getAnomalyGuid(cc::IString*& pStrAnomalyGuid);
    virtual NavExclusions::ExResult setVid(const ULONG vid);
    virtual const ULONG getVid();
    virtual ExResult isExactMatch(const cc::IString* pStrGuid, const ULONG ulVid);

protected:
    // VID/GUID internal methods
    virtual NavExclusions::ExResult isGuidMatch(const cc::IString* rhsGuid);
    virtual NavExclusions::ExResult isVidMatch(const ULONG rhsVid);

    virtual ExResult hasGuid();
    virtual ExResult hasVid();

/** MACRO THIS **/
#include "GenericExclusion.h"
/** END MACRO THIS **/
};

#undef __MY_INTERFACE
#undef __MY_CLASS
#undef __MY_CLASSNAME

}
#endif