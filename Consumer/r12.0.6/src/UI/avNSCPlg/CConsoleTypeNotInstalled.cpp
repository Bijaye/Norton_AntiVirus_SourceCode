#include "stdafx.h"
#include "CConsoleTypeNotInstalled.h"

nsc::NSCRESULT CConsoleTypeNotInstalled::GetType(nsc::e_SymConsoleType& type_out) const
{
	type_out = nsc::NOTINSTALLED_CONSOLETYPE;
	return nsc::NSC_SUCCESS;
}
