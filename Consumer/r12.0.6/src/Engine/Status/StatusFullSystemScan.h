#pragma once

#include "StatusItem.h"

class CStatusFullSystemScan : public CStatusItem  
{
public:
	CStatusFullSystemScan();
	virtual ~CStatusFullSystemScan();

protected:
    void MakeStatus();
};
