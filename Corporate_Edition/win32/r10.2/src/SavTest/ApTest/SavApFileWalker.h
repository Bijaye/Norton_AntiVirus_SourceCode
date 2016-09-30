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

#include "SavApTestFile.h"
#include <vector>

/**
 * SavApFileWalker maintains a sequence of file actions to be taken on the contents of a group of
 * directories.  For each directory, he sequence of actions is performed on each file before
 * moving on to the next directory.
 */
class SavApFileWalker
{
public:
	SavApFileWalker(void);
	virtual ~SavApFileWalker(void);

	virtual DWORD AddFileAction( FILEACTION action );
	virtual void AddTestFiles( TestFileVector* testFiles ) { m_testFileVectorPtr = testFiles; };
	virtual DWORD Execute();

protected:
	virtual DWORD RunActions( SavApTestFile& file );

	//The sequence of actions
	FileActionVector m_actions;

	//Pointer to the list of test files
	TestFileVector* m_testFileVectorPtr;

	// Declared but not implemented to prevent callers from using default implementations
	SavApFileWalker(const SavApFileWalker& oRhs);
	SavApFileWalker& operator= (const SavApFileWalker&);
};
