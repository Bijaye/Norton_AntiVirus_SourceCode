#pragma once

#include "nscIConsoleType.h"

class CConsoleTypeNotInstalled: public ISymBaseImpl<CSymThreadSafeRefCount>,public nsc::IConsoleType
{
public:
	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(nsc::IID_nscIConsoleType,nsc::IConsoleType)
	SYM_INTERFACE_MAP_END()

public:
	virtual nsc::NSCRESULT GetType(nsc::e_SymConsoleType& type_out) const;
};
