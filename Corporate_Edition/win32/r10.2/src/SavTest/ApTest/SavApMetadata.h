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
#pragma once

enum INFECTIONTYPE
{
	INFECTION_CLEAN,
	INFECTION_REPAIRABLE,
	INFECTION_UNREPAIRABLE,
	INFECTION_DIR
};

enum VIRUSTYPE
{
	VIRUS_MACRO,
	VIRUS_NONMACRO
};

/**
 * SavApMetadata encapsulates the data contained in one sample file's associated metadata file.
 * Allowing default implementations of Assignment Operator and Copy Constructor
 */
class SavApMetadata
{
public:
	SavApMetadata(void);
	SavApMetadata( INFECTIONTYPE infection, VIRUSTYPE virus );
	virtual ~SavApMetadata(void);

	virtual INFECTIONTYPE GetInfectionType() { return m_infection; }
	virtual VIRUSTYPE GetVirusType() { return m_virus; }
	virtual DWORD Parse( LPCTSTR name );

protected:
	FILE* m_file;
	INFECTIONTYPE m_infection;
	VIRUSTYPE m_virus;
};
