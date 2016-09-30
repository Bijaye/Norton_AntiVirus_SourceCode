////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// MBRInfection.cpp: implementation of the CMBRInfection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hammertime.h"
#include "navscan.h"
#include "MBRInfection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMBRInfection::CMBRInfection(char cNumber, long lOffset)
{
	m_cNumber = cNumber;
	m_lOffset = lOffset;
	m_iStatus = IScanInfection::INFECTED;

    // mbr infections will always just be viruses
    m_plCategories = new unsigned long[1];
    m_plCategories[0] = SCAN_THREAT_TYPE_VIRUS;
    m_lNumCategories = 1;
    m_lUserData = 0;
}

CMBRInfection::~CMBRInfection()
{
    if(m_plCategories)
        delete [] m_plCategories;
}



const char* CMBRInfection::GetVirusName()
{
	return "MBR Bogus";
}

unsigned long CMBRInfection::GetVirusID()
{
	return 0;	
}

int CMBRInfection::GetStatus()
{
	return m_iStatus;	
}

void CMBRInfection::SetUserData( long lData )
{
	m_lUserData = lData;	
}

long CMBRInfection::GetUserData()
{
	return m_lUserData;	
}

unsigned char CMBRInfection::GetDriveNumber()
{
	return m_cNumber;
}
		
unsigned long CMBRInfection::GetMBROffset()
{
	return m_lOffset;
}

unsigned long CMBRInfection::GetCategoryCount()
{
    return m_lNumCategories;
}

const unsigned long* CMBRInfection::GetCategories()
{
    return static_cast<const unsigned long*>(m_plCategories);
}

bool CMBRInfection::IsCategoryType( unsigned long ulCategory )
{
    for( long i=0; i<m_lNumCategories; i++ )
    {
        if( m_plCategories[i] == ulCategory )
            return true;
    }
    return false;
}