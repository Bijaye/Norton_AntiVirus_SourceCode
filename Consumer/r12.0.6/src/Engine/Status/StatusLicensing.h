#pragma once

#include "StatusItem.h"

class CStatusLicensing : public CStatusItem  
{
public:
	CStatusLicensing();
	virtual ~CStatusLicensing();

protected:
	void MakeStatus();
};
