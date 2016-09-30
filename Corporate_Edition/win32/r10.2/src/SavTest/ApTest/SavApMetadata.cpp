// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\savapmetadata.h"

SavApMetadata::SavApMetadata(void):
m_infection( INFECTION_CLEAN ),
m_virus( VIRUS_MACRO )
{
}

SavApMetadata::SavApMetadata( INFECTIONTYPE infection, VIRUSTYPE virus ):
m_infection( infection ),
m_virus( virus )
{
}

SavApMetadata::~SavApMetadata(void)
{
}

DWORD SavApMetadata::Parse( LPCTSTR name )
{
	DWORD ret = ERROR_SUCCESS;

	return ret;
}