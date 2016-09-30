#pragma once

#include "StatusItem.h"
#include "NavSettings.h"

class CStatusEmail : public CStatusItem  
{
public:
	CStatusEmail();
	virtual ~CStatusEmail();

protected:
    void    MakeStatus();
};
