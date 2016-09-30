////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Infection.cpp: implementation of the CInfection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hammertime.h"
#include "navscan.h"
#include "ccgseinterface.h"
#include "Infection.h"
#include "commonuiinterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInfection::CInfection( int iFileNum, unsigned long* plCats, long lCatCount, bool bCanDelete, bool bCanSERepair) :
	m_lUserData(0)
{
	ostringstream ost;
	ost << "File" << iFileNum;
	m_sFileName = ost.str();
	m_iStatus = IScanInfection::INFECTED;
    m_bCanDelete = bCanDelete;
    m_bCanSERepair = bCanSERepair;

    if( NULL == plCats )
    {
        // The default is to set this to the virus infection category
        m_plCategories = new unsigned long[1];
        m_plCategories[0] = SCAN_THREAT_TYPE_VIRUS;
        m_lNumCategories = 1;
    }
    else
    {
        m_lNumCategories = lCatCount;
        m_plCategories = new unsigned long[m_lNumCategories];
        for( int i=0; i<m_lNumCategories; i++ )
        {
            m_plCategories[i] = plCats[i];

            // Set this as an exlcude page item if it falls into the category
            // (not deleteable and a non-viral threat)
            if( plCats[i] >= SCAN_THREAT_TYPE_SECURITY_RISK && !m_bCanDelete )
                m_lUserData = InfectionStatus_ExcludePageItem;
        }
    }
}

CInfection::~CInfection()
{
    if(m_plCategories)
        delete [] m_plCategories;
}

const char* CInfection::GetVirusName()
{
	return "Bogus";
}

unsigned long CInfection::GetVirusID()
{
	return rand();
}

int CInfection::GetStatus()
{
	return m_iStatus;	
}

void CInfection::SetUserData( long lData )
{
	m_lUserData = lData;	
}

long CInfection::GetUserData()
{
	return m_lUserData;	
}

const char * CInfection::GetLongFileName()
{
	return m_sFileName.c_str();							
}

const char * CInfection::GetShortFileName()
{
	return m_sFileName.c_str();		
}

unsigned long CInfection::GetCategoryCount()
{
    return m_lNumCategories;
}

const unsigned long* CInfection::GetCategories()
{
    return static_cast<const unsigned long*>(m_plCategories);
}

bool CInfection::IsCategoryType( unsigned long ulCategory )
{
    for( long i=0; i<m_lNumCategories; i++ )
    {
        if( m_plCategories[i] == ulCategory )
            return true;
    }
    return false;
}

bool CInfection::CanDelete()
{
    return m_bCanDelete;
}

bool CInfection::CanRepair()
{
    return true;
}

bool CInfection::CanSideEffectRepair()
{
    return m_bCanSERepair;
}

CSideEffectInfection::CSideEffectInfection( int iFileNum, unsigned long* plCats, long lCatCount, bool bCanDelete, bool bCanSERepair, int nSECount)
    : CInfection(iFileNum, plCats, lCatCount, bCanDelete, bCanSERepair),
    m_nSECount(nSECount)
{
}

int CSideEffectInfection::GetSideEffectsCount()
{
    return m_nSECount;
}

SCANSTATUS CSideEffectInfection::GetSideEffect(int iSideEffect, ISideEffect** ppSideEffect)
{
    return SCAN_OK;
}

CSideEffectInfection::~CSideEffectInfection()
{
}