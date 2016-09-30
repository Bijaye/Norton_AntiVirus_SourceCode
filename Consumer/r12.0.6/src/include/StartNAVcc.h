// StartNAVcc.h
//
//////////////////////////////////////////////////////////////////////

#pragma once

namespace NAVToolbox
{
class CStartNAVcc
{
public:
	// Return values
	// true - if we start up CC or do not need to
	// false - if we fail
	bool StartCC(bool bSetRunKey = false);

private:
    bool doCCStart(bool bSetRunKey);
    bool ValidateRunKey();
};

}