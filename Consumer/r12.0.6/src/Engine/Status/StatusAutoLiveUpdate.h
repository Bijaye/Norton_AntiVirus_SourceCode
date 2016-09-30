#pragma once

#include "StatusItem.h"

class CStatusAutoLiveUpdate : public CStatusItem  
{
public:
	CStatusAutoLiveUpdate();
	virtual ~CStatusAutoLiveUpdate();
   
protected:
    void    MakeStatus();
};
