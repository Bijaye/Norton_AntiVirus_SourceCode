#pragma once

#include "StatusItem.h"

// For Spyware status item
//
class CStatusSpyware : public CStatusItem  
{
public:
	CStatusSpyware();
	virtual ~CStatusSpyware();

protected:
    void    MakeStatus();
};