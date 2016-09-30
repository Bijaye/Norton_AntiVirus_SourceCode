#pragma once

#include "StatusItem.h"

// For AP status item
//
class CStatusAutoProtect : public CStatusItem  
{
public:
	CStatusAutoProtect();
	virtual ~CStatusAutoProtect();

protected:
    void    MakeStatus();
};
