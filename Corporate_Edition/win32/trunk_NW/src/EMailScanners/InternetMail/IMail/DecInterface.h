// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#pragma once

///////////////////////////////////
//
// class CDecInterface
//
// Interface into creating decomposer objects
#include "dapi.h"
#include "dec2.h"
#include "decfs.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "textfile.h"

class CDecInterface
{
public:
    //default constructor and destructor
	CDecInterface(void);
	~CDecInterface(void);

	///////////////////////////////////////////////////////////
    //
    // DECRESULT CDecomposerSDK::NewObject(const char* szName, const char* szDataFile)
    //
    // parameters:
    //          szName
    //          szDataFile
    //          szTempDir
    //          **ppIDecObject
    //
    // this is used for Internet E-mail while we are integrating the ccDec
    // this is a new interface that doesn't use dec2.dll to create a new decomposer object
    //
	DECRESULT NewObject(const char *szName, 
						const char *szDataFile, 
						const char* szTempDir,
						IDecContainerObjectEx **ppIDecObject);

	/////////////////////////////////////////////////////////////////////////////
    // DECRESULT CDecInterface::DeleteFileObject()
    //
    // delete decomposer file object
    // pulled from decomposer object
    //
    // parameters: IDecObject
    //
    // returns: DECRESULT
	DECRESULT DeleteObject(IDecContainerObjectEx *pObject);

    /////////////////////////////////////////////////////////////////////////////
    //
    // DECRESULT CDecInterface::NewDecomposer()
    //
    // Creates a new decomposer object
    // 
    // Parameters:
    //			IDecomposer
    //
    // Returns:
    //			DECRESULT
	static DECRESULT NewDecomposer();

    /////////////////////////////////////////////////////////////////////////////
    //
    // DECRESULT CDecInterface::DeleteDecomposer()
    //
    // deletes a decomposer object
    //
    // parameters: 
    //			IDecomposer
    //
    // Returns:
    //		DECRESULT
	static DECRESULT DeleteDecomposer();
	
    //////////////////////////////////////////////////////////////////////////
    //
    // bool CDecInterface::CheckFileSystem() const
    //
    // returns:
    //		TRUE if filesystem is initialized
    //		FALSE if filesystem isn't
	bool CheckFileSystem()const;

    ///////////////////////////////////////////////////
    //
    // IDecFiles* operator->()
    //
    // returns a pointer to the IDecFiles Interface
	IDecFiles* operator->(void) { return s_pIDecFiles; }

    //////////////////////////////////////////////////////////////
    //
    // int CDecInterface::NewIOService()
    //
    // parameters:
    //          None
    //
    // return: 
    //          DECRESULT
	static int NewIOService();

    /////////////////////////////////////////////////////
    //
    // void CDecInterface::DeleteIOService()
    //
    // parameters:
    //          None
	static void DeleteIOService();

    //////////////////////////////////////////////////
    //
    // void CDecInterface::GetDecIO()
    //
    // parameters:
    //          None
    IDecIO* GetDecIO()const;

    ///////////////////////////////////////////
    //
    // IDecomposerEx* CDecInterface::GetDecomposer() const
    //
    // parameters:
    //          None
    //
    // returns:
    //          Decomposer object
	IDecomposerEx* GetDecomposer() const;

private:
    /////////////////////////////////////////////////////////////////////////////
    //
    // DECRESULT CDecInterface::decNewDecomposer()
    //
    // Creates a new decomposer object
    // 
    // Parameters:
    //			IDecomposer
    //
    // Returns:
    //			DECRESULT
	//moved from DAPI to decomposersdk for Internet E-Mail scanning
	//we need to use the dec2text Mime Engine for I-EMAIL it is currently eaiser
	//to maintain the decomposer interface
	DECRESULT decNewFileObject(const char *szName, DWORD dwCharset, const char *szDataFile, IDecContainerObjectEx **ppObject);

	//decomposer file system 
	static IDecFiles*	s_pIDecFiles;
	static IDecIO*      s_pIDecIO;
	static IDecomposerEx* s_pDecomposer;

	static CTextEngine s_cTextEngine;
};

