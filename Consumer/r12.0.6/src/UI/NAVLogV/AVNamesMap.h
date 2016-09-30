// AVFeatureNameMap.h: interface for the CAVFeatureNameMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVNAMESMAP_H__F1324C28_415B_4C56_936C_35B326D211AF__INCLUDED_)
#define AFX_AVNAMESMAP_H__F1324C28_415B_4C56_936C_35B326D211AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "ccModuleID.h"
#include "AllNAVEvents.h"

#include <map>

typedef ::std::map <long, UINT> mapAVNamesToResID;

class CAVFeatureNameMap  
{
public:
	CAVFeatureNameMap();
    virtual ~CAVFeatureNameMap(){};

    CString GetFeatureName(long lFeatureID);

protected:
    mapAVNamesToResID Names;
    CString m_csProdName;
};

class CAVActionTakenMap  
{
public:
	CAVActionTakenMap();
    virtual ~CAVActionTakenMap(){};

    mapAVNamesToResID Names;
};

class CAVActionAttemptedMap
{
public:
	CAVActionAttemptedMap();
    virtual ~CAVActionAttemptedMap(){};

    mapAVNamesToResID Names;
};

class CAVScanActionMap
{
public:
    CAVScanActionMap();
    virtual ~CAVScanActionMap(){};

    mapAVNamesToResID Names;
};
#endif // !defined(AFX_AVNAMESMAP_H__F1324C28_415B_4C56_936C_35B326D211AF__INCLUDED_)
