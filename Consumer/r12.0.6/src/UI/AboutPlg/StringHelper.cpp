// StringHelper.cpp: implementation of the CStringHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StringHelper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStringHelper::CStringHelper( HINSTANCE hResourceInstance )
{
    _tcscpy ( m_szTemp, _T("\0"));
    m_hResourceInstance = hResourceInstance;
}

CStringHelper::~CStringHelper()
{

}

LPCTSTR CStringHelper::LoadString(UINT uID)
{
    ::LoadString ( m_hResourceInstance, uID, m_szTemp, sizeof (m_szTemp ));

    return m_szTemp;
}
