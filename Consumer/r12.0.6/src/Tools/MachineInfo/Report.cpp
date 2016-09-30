// Report.cpp: implementation of the CReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MachineInfo.h"
#include "Report.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReport::CReport()
{
    m_bIsAvailable = false;
}

CReport::~CReport()
{

}

bool CReport::IsAvailable()
{
    return m_bIsAvailable;
}
