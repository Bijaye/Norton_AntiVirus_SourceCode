//
//	IBM AntiVirus Immune System
//
//	File Name:	CRCCalculator.cpp
//	Author:		Andy Klapper
//
//	This abstract class defines the interface that all CRCCalculator
//	clients work with.  Sub classes of this class provide specific
//	implementations that perform the work of CRCCalculation.
//
//	$Log: /AVISdb/ImmuneSystem/FilterTestHarness/CRCCalculator.cpp $
//
//1     8/19/98 9:10p Andytk
//Initial check in
//

#include "MOD_stdafx.h"
#include "MOD_AVISFile.h"
#include "MOD_CRCCalculator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRCCalculator::CRCCalculator()
{

}

CRCCalculator::~CRCCalculator()
{

}
