//
//	IBM AntiVirus Immune System
//
//	File Name:	UnScrambler.h
//	Author:		Andy Klapper
//
//	This class defines the generic interface to all UnScrambler classes.
//
//	$Log: /AVISdb/ImmuneSystem/FilterTestHarness/UnScrambler.h $
//	 
//	1     8/19/98 9:10p Andytk
//	Initial check in
//

#if !defined(AFX_UnScrambler_H__D2432424_15D4_11D2_9056_947982000000__INCLUDED_)
#define AFX_UnScrambler_H__D2432424_15D4_11D2_9056_947982000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>

#include "AVIS.h"
#include "AVISFile.h"

class AVISFILE_API UnScrambler  
{
protected:
	UnScrambler();

public:
	virtual				~UnScrambler();

	virtual bool		UnScramble(uchar* buffer, uint bufferSize)	= 0;
	virtual const std::string&	Name(void)	= 0;
};

#endif // !defined(AFX_UnScrambler_H__D2432424_15D4_11D2_9056_947982000000__INCLUDED_)
