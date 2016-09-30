////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// BootInfection.cpp: implementation of the CBootInfection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hammertime.h"
#include "navscan.h"
#include "BootInfection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBootInfection::CBootInfection(char cLetter)
{
	m_cLetter = cLetter;
	m_iStatus = IScanInfection::INFECTED;

    // br infections will always just be viruses
    m_plCategories = new unsigned long[1];
    m_plCategories[0] = SCAN_THREAT_TYPE_VIRUS;
    m_lNumCategories = 1;
    m_lUserData = 0;
}

CBootInfection::~CBootInfection()
{
    if(m_plCategories)
        delete [] m_plCategories;
}


const char* CBootInfection::GetVirusName()
{
	return "Boot Bogus";
}

unsigned long CBootInfection::GetVirusID()
{
	return 0;	
}

int CBootInfection::GetStatus()
{
	return m_iStatus;	
}

void CBootInfection::SetUserData( long lData )
{
	m_lUserData = lData;	
}

long CBootInfection::GetUserData()
{
	return m_lUserData;	
}

char CBootInfection::GetDriveLetter()
{
	return m_cLetter;

}

unsigned long CBootInfection::GetCategoryCount()
{
    return m_lNumCategories;
}

const unsigned long* CBootInfection::GetCategories()
{
    return static_cast<const unsigned long*>(m_plCategories);
}

bool CBootInfection::IsCategoryType( unsigned long ulCategory )
{
    for( long i=0; i<m_lNumCategories; i++ )
    {
        if( m_plCategories[i] == ulCategory )
            return true;
    }
    return false;
}