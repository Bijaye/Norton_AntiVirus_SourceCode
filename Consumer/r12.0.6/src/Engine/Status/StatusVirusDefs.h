#pragma once

#include "StatusItem.h"

class CStatusVirusDefs : public CStatusItem  
{
public:
	CStatusVirusDefs();
	virtual ~CStatusVirusDefs();

protected:
    void        MakeStatus();
};