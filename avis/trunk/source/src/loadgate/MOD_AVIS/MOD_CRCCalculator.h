//
//	IBM AntiVirus Immune System
//
//	File Name:	CRCCalculator.h
//	Author:		Andy Klapper
//
//	This is the abstract class that represents all CRCCalculating classes.
//	It is to this interface that all clients of CRCCalculating services
//	work to.
//
//	$Log: /AVISdb/ImmuneSystem/FilterTestHarness/CRCCalculator.h $
//	
//	1     8/19/98 9:10p Andytk
//	Initial check in
//

#if !defined(AFX_RCCALCULATOR_H__D2432426_15D4_11D2_9056_947982000000__INCLUDED_)
#define AFX_RCCALCULATOR_H__D2432426_15D4_11D2_9056_947982000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>

#include "MOD_AVIS.h"

class /*AVISFILE_API*/ CRCCalculator  
{
protected:
	CRCCalculator();

public:
	virtual			~CRCCalculator();

	virtual bool	CRCCalculate(const uchar* buffer, uint bufferSize)	= 0;
///!!!	virtual bool	Done(std::string& crc)	= 0;
	virtual bool	Done (char *pCrcString) = 0;

///!!!	virtual const std::string&	Name(void)	= 0;
};

#endif // !defined(AFX_RCCALCULATOR_H__D2432426_15D4_11D2_9056_947982000000__INCLUDED_)
