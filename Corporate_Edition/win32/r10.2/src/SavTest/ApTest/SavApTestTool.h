// /////////////////////////////////////////////////////////////////////////////////////////////////
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

#include <vector>
#include "ITestTool.h"
#include "SavApFileWalker.h"
#include "SavApSettingsAdapter.h"
#include "ApTestToolSettings.h"
#include "SavApTestFile.h"
#include "Checksum.h"

class SavApTestTool : public ITestTool
{
public:
	SavApTestTool(void);
	virtual ~SavApTestTool(void);

	virtual SYMTESTRESULT Configure( LPCSTR testData, SavApSettingsAdapter* settings );
	virtual SYMTESTRESULT Configure();
	virtual SYMTESTRESULT Execute();
	virtual SYMTESTRESULT VerifyResults();
	virtual DWORD EnumerateFiles( LPCTSTR directory );
	virtual bool FileIsClean( SavApTestFile& file );
	virtual bool ApIsEnabled();
	virtual bool FileIsIncluded( SavApTestFile& file );
	virtual bool FileIsExcluded( SavApTestFile& file );
	virtual bool NetworkScanIsEnabled();
	virtual bool FileIsNetwork( SavApTestFile& file );
	virtual bool ApTriggerOnRead();
	virtual bool AttemptedToModifyFile( SavApTestFile& file );
	virtual bool FileIsUnchanged( SavApTestFile& file );
	virtual bool FileIsMacro( SavApTestFile& file );
	virtual SYMTESTRESULT VerifyAction( LPCTSTR action );
	virtual void EnableAP( bool enable );

protected:
	//TODO: A Reference to a real TestData instance for walking files and reading in metadata
	LPCTSTR m_testData;
	TestFileVector m_testFiles;
	SavApSettingsAdapter* m_productSettings;
	ApTestToolSettings* m_toolSettings;

	// Declared but not implemented to prevent callers from using default implementations
	SavApTestTool(const SavApTestTool& oRhs);
	SavApTestTool& operator= (const SavApTestTool&);
};
