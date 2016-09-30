// CompressedInfection.cpp: implementation of the CCompressedInfection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hammertime.h"
#include "navscan.h"
#include "CompressedInfection.h"

#pragma warning (disable: 4786)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCompressedInfection::CCompressedInfection(int iFileNum, int iCompressionDepth) :
	m_lUserData(0)
{
	ostringstream ost;
	ost << "Compressed File" << iFileNum;
	m_sFileName = ost.str();	
	
	m_iCompressionDepth = iCompressionDepth;
	// Allocate the vector for the components
	for(int i = 0; i < m_iCompressionDepth; i++)
	{
		if(i == 0)
			m_vComponents.push_back("Infection");
		else
		{
			ostringstream ost2;
			ost2 << "Level" << (i);
			m_vComponents.push_back(ost2.str());	
		}
	}
	m_iStatus = IScanInfection::INFECTED;

    // The default is to set this to the virus infection category
     m_plCategories = new unsigned long[1];
    m_plCategories[0] = SCAN_THREAT_TYPE_VIRUS;
    m_lNumCategories = 1;
    m_lUserData = 0;
}

CCompressedInfection::~CCompressedInfection()
{
    if(m_plCategories)
        delete [] m_plCategories;
}

const char* CCompressedInfection::GetVirusName()
{
	return "Compressed Bogus";
}

unsigned long CCompressedInfection::GetVirusID()
{
	return 0;	
}

int CCompressedInfection::GetStatus()
{
	return m_iStatus;	
}

void CCompressedInfection::SetUserData( long lData )
{
	m_lUserData = lData;	
}

long CCompressedInfection::GetUserData()
{
	return m_lUserData;	
}

int CCompressedInfection::GetComponentCount()
{
	return m_iCompressionDepth;		
}

const char* CCompressedInfection::GetComponent( int iComponent )
{
	return m_vComponents[iComponent].c_str();
}

const char * CCompressedInfection::GetTempFileName()
{
	return m_sFileName.c_str();
}

unsigned long CCompressedInfection::GetDecCharSet()
{
	// Return Char set for US ANSII OEM
	return 0x96;
}

unsigned long CCompressedInfection::GetCanDeleteAncestorDepth()
{
    return true;
}

unsigned long CCompressedInfection::GetComponentCharSet( int iComponent )
{
    // Return Char set for US ANSII OEM
	return 0x96;
}

unsigned long CCompressedInfection::GetCategoryCount()
{
    return m_lNumCategories;
}

const unsigned long* CCompressedInfection::GetCategories()
{
    return static_cast<const unsigned long*>(m_plCategories);
}

bool CCompressedInfection::IsCategoryType( unsigned long ulCategory )
{
    for( long i=0; i<m_lNumCategories; i++ )
    {
        if( m_plCategories[i] == ulCategory )
            return true;
    }
    return false;
}

bool CCompressedInfection::IsPackedExecutable()
{
    return false;
}