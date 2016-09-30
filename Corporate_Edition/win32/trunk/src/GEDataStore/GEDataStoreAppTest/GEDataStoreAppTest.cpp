// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// GEDataStoreAppTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma warning (disable: 4786) // identifier was truncated to '255' characters in the debug information

#define INITIIDS

#define array_sizeof(x) (sizeof(x) / sizeof(x[0]))

//#define INVALID_ERROR_CHK

#include <iostream>
#include <crtdbg.h>
#include <string>
#include <vector>


#include "GEDataStoreUtils.h"

const char g_szPathToGEDataStoreDll[]        = "GEDataStore.dll";
HANDLE g_hEvent = NULL;

using namespace GlobalExceptionDataStore;


SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

static void print_usage()
{
    std::cout  << "GEDataStoreAppTest: Application API Test framework for the GEDataStore component.\n";
    std::cout  << std::endl;
}

static RESULT TestRiskBasedExceptions()
{
    RESULT					res = RTN_SUCCESS;
	SYMRESULT				symres = SYM_SUCCESS;
    IRiskGEDataStorePtr		ptrRiskGEDataStore;
    IRiskGEItemPtr			ptrRiskGEItem;
	
    // Create the Risk DataStore object.
    if (SYM_FAILED (symres = g_objRiskGEDataStoreLdr.CreateObject( &ptrRiskGEDataStore )))
	{
		ASSERT(FALSE); std::cout  << "ERROR: CreateObject() failed: 0x" << std::hex << symres << ".\n";
        return RTNERR_FAILED;
	}

	// Create a Risk Item	
	if (SYM_FAILED (symres = g_objRiskGEItemLdr.CreateObject( &ptrRiskGEItem )))
	{
		ASSERT(FALSE); std::cout  << "ERROR: CreateObject() failed: 0x" << std::hex << symres << ".\n";
		return RTNERR_FAILED;
	}

		DWORD dwPVID = 1111;
		GEACTION geaFirstBefore = GEA_QUARANTINE;
		GEACTION geaFirstAfter	= GEA_UNSET;
		GEACTION geaSecondBefore= GEA_LOGONLY;
		GEACTION geaSecondAfter	= GEA_UNSET;

		GEOWNER geoBefore = GEO_ADMIN;

		////////////////////////////////////
		// Test out Admin lists first.
		{
			// Set its values.
			if (FAILED (res = ptrRiskGEItem->SetPVID(dwPVID)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetPVID() failed: 0x" << std::hex << res << ".\n";
		        return res;
			}
			if (FAILED (res = ptrRiskGEItem->SetFirstAction(geaFirstBefore)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetFirstAction() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			if (FAILED (res = ptrRiskGEItem->SetSecondAction(geaSecondBefore)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetSecondAction() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			if (FAILED (res = ptrRiskGEItem->SetOwner(geoBefore)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetOwner() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			if (FAILED (res = ptrRiskGEItem->SetName(_T("PVID 1111"))))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			
			// Now, add it to the DataStore.
			if (FAILED (res = ptrRiskGEDataStore->SetAt( dwPVID, ptrRiskGEItem )))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
				return res;
			}

			// Add another one.
			dwPVID = 2222;
			if (FAILED (res = ptrRiskGEItem->SetPVID(dwPVID)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetPVID() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			if (FAILED (res = ptrRiskGEItem->SetName(_T("PVID 2222"))))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			if (FAILED (res = ptrRiskGEItem->SetOwner(GEO_ADMIN)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetProTech() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			// Now, add it to the DataStore.
			if (FAILED (res = ptrRiskGEDataStore->SetAt( dwPVID, ptrRiskGEItem )))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
		}

		//////////////////////////////////////////
		//  Now add some client global exceptions.
		{
			dwPVID = 3333;
			geoBefore = GEO_CLIENT;
			if (FAILED (res = ptrRiskGEItem->SetPVID(dwPVID)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetPVID() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			if (FAILED (res = ptrRiskGEItem->SetOwner(geoBefore)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetOwner() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			if (FAILED (res = ptrRiskGEItem->SetName(_T("PVID 3333"))))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			// Now, add it to the DataStore.
			if (FAILED (res = ptrRiskGEDataStore->SetAt( dwPVID, ptrRiskGEItem )))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
				return res;
			}

			////////////////////////
			// Add another one.
			dwPVID = 4444;
			if (FAILED (res = ptrRiskGEItem->SetPVID(dwPVID)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetPVID() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			if (FAILED (res = ptrRiskGEItem->SetName(_T("PVID 4444"))))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			if (FAILED (res = ptrRiskGEDataStore->SetAt( dwPVID, ptrRiskGEItem )))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
				return res;
			}

			////////////////////////
			// Add another one.
			dwPVID = 5555;
			if (FAILED (res = ptrRiskGEItem->SetPVID(dwPVID)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetPVID() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			if (FAILED (res = ptrRiskGEItem->SetName(_T("PVID 5555"))))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			// Now, add it to the DataStore.
			if (FAILED (res = ptrRiskGEDataStore->SetAt( dwPVID, ptrRiskGEItem )))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
		}

		// Since, we are reading and writing to the registry as fast as possible in this test app, we need to make sure that the watching
		// thread has had a chance to get notified and update the caches. Otherwise, we're only testing how fast threads swap.
		::WaitForSingleObject(g_hEvent, 5000); 


		//////////////////////////////////////////////////////
		// Try to find the first one added to the Admin list.
		{
			dwPVID = 1111;
			// Make sure we can read it from the datastore.
			IRiskGEItemPtr	ptrRiskGEItem2;
			if (FAILED(res = ptrRiskGEDataStore->Lookup( dwPVID, GEPT_RISK, ptrRiskGEItem2.m_p ))
				|| RTN_FALSE == res)
			{
				ASSERT(FALSE); std::cout  << "ERROR: Lookup() failed: 0x" << std::hex << res << ".\n";
				return (RTN_FALSE == res) ? RTNERR_FAILED : res;
			}

			// Verify we got something back.
			if (FAILED (res = ptrRiskGEItem2->GetFirstAction(&geaFirstAfter)) ||
				FAILED (res = ptrRiskGEItem2->GetSecondAction(&geaSecondAfter)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: GetFirstAction()/GetSecondAction(): 0x" << std::hex << res << ".\n";
				return res;
			}
			// Verify that what we set before is what we got back.
			if (geaFirstBefore != geaFirstAfter || geaSecondBefore != geaSecondAfter)
			{
				ASSERT(FALSE); std::cout  << "ERROR: First/Second actions don't match up after SetAt/Lookup.\n";
				return res;
			}
			// Check out the name.
			tstring::tstring tstr;
			if (FAILED (res = ptrRiskGEItem2->GetName(&tstr)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: GetName(): 0x" << std::hex << res << ".\n";
				return res;
			}
			if (tstr.empty() == true)
			{
				ASSERT(FALSE); std::cout  << "ERROR: GetName() - Name is empty!\n";
				return res;
			}

			
			//////////////////////////////////////////
			// Let's do the same thing for a client GE.
			dwPVID = 4444;
			IRiskGEItemPtr	ptrRiskGEItem3;
			if (FAILED (res = ptrRiskGEDataStore->Lookup( dwPVID, GEPT_RISK, ptrRiskGEItem3.m_p )) ||
				res == RTN_FALSE)
			{
				ASSERT(FALSE); std::cout  << "ERROR: Lookup() failed: 0x" << std::hex << res << ".\n";
				return (RTN_FALSE == res) ? RTNERR_FAILED : res;
			}

			// Verify we got something back.
			if (FAILED (res = ptrRiskGEItem3->GetFirstAction(&geaFirstAfter)) ||
				FAILED (res = ptrRiskGEItem3->GetSecondAction(&geaSecondAfter)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: GetFirstAction()/GetSecondAction(): 0x" << std::hex << res << ".\n";
				return res;
			}
			// Verify that what we set before is what we got back.
			if (geaFirstBefore != geaFirstAfter || geaSecondBefore != geaSecondAfter)
			{
				ASSERT(FALSE); std::cout  << "ERROR: First/Second actions don't match up after SetAt/Lookup.\n";
				return res;
			}
		}

		///////////////////////////////////////////////////
		// Enumerate the list.
		{
			IRiskGEItemPtr	ptrRiskGEItem4;
			DWORD dwNumItems;

			ptrRiskGEDataStore->InitList(GEPT_RISK);
			ptrRiskGEDataStore->GetListCount(&dwNumItems);

			for (DWORD dwIndex = 0; dwIndex < dwNumItems; dwIndex++)
			{
				if (ptrRiskGEItem4.m_p != NULL)
					ptrRiskGEItem4->Release();

				ptrRiskGEDataStore->GetItemInList(dwIndex, ptrRiskGEItem4.m_p);
				ptrRiskGEItem4->GetPVID(&dwPVID);
				dwPVID;
			}

		}

		///////////////////////////////////////////////////
		// Finally, let's try and remove one and then remove them all.
		{
			dwPVID = 1111;
			if (FAILED (res = ptrRiskGEDataStore->RemoveKey(dwPVID, GEO_ADMIN, GEPT_RISK)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: RemoveKey() failed: 0x" << std::hex << res << ".\n";
				return res;
			}

			// Since, we are reading and writing to the registry as fast as possible in this test app, we need to make sure that the watching
			// thread has had a chance to get notified and update the caches. Otherwise, we're only testing how fast threads swap.
			::WaitForSingleObject(g_hEvent, 5000); 

			// Let's verify it's gone by trying to find it.
			IRiskGEItemPtr	ptrRiskGEItem3;
			if (FAILED (res = ptrRiskGEDataStore->Lookup( dwPVID, GEPT_RISK, ptrRiskGEItem3.m_p )))
			{
				ASSERT(FALSE); std::cout  << "ERROR: Lookup() failed: 0x" << std::hex << res << ".\n";
				return res;
			}
			if (res != RTN_FALSE)
			{
				ASSERT(FALSE); std::cout  << "ERROR: We failed to remove this global exception: 0x" << std::hex << res << ".\n";
				return RTNERR_FAILED;
			}
		}

		/////////////////////////////////////////////////
		// Let's remove them all
		{
			if (FAILED (res = ptrRiskGEDataStore->RemoveAll(GEO_ADMIN, GEPT_RISK)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: RemoveAll() failed for Admin list: 0x" << std::hex << res << ".\n";
				return res;
			}
			if (FAILED (res = ptrRiskGEDataStore->RemoveAll(GEO_CLIENT, GEPT_RISK)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: RemoveAll() failed for Admin list: 0x" << std::hex << res << ".\n";
				return res;
			}
		}

	// Since, we are reading and writing to the registry as fast as possible in this test app, we need to make sure that the watching
	// thread has had a chance to get notified and update the caches. Otherwise, we're only testing how fast threads swap.
	::WaitForSingleObject(g_hEvent, 5000); 

	return RTN_SUCCESS;
}

static RESULT TestFileNameBasedExceptions()
{
    RESULT					res = RTN_SUCCESS;
	SYMRESULT				symres = SYM_SUCCESS;
    IFileNameGEDataStorePtr	ptrFileNameGEDataStore;
    IFileNameGEItemPtr		ptrFileNameGEItem;
	
    // Create the FileName DataStore object.
    if (SYM_FAILED (symres = g_objFileNameGEDataStoreLdr.CreateObject( &ptrFileNameGEDataStore )))
	{
		ASSERT(FALSE); std::cout  << "ERROR: CreateObject() failed: 0x" << std::hex << symres << ".\n";
        return RTNERR_FAILED;
	}

	// Create a Risk Item	
	if (SYM_FAILED (symres = g_objFileNameGEItemLdr.CreateObject( &ptrFileNameGEItem )))
	{
		ASSERT(FALSE); std::cout  << "ERROR: CreateObject() failed: 0x" << std::hex << symres << ".\n";
		return RTNERR_FAILED;
	}

		///////////////////////////////////////////
	// Add some exceptions to the DataStore
#ifdef INVALID_ERROR_CHK
		//////////////////////////////////////
		//Error Checking for FileName GE Setters
		{
			//Error Checking for Setting Invalid values!
			if (FAILED (res = ptrFileNameGEItem->SetName("")))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetFileNameName() failed: 0x" << std::hex << res << ".\n";
			}
			if (FAILED (res = ptrFileNameGEItem->SetOwner(GEO_UNSET )))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetFileNameOwner() failed: 0x" << std::hex << res << ".\n";
			}
			if (FAILED (res = ptrFileNameGEItem->SetFirstAction( GEA_UNSET )))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetFileNameFirstAction() failed: 0x" << std::hex << res << ".\n";
			}
			if (FAILED (res = ptrFileNameGEItem->SetSecondAction( GEA_LAST_ITEM )))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetFileNameSecondAction() failed: 0x" << std::hex << res << ".\n";
			}
			
			if (FAILED (res = ptrFileNameGEItem->SetProtectionTechnology(GEPT_UNSET)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetFileNameProtectionTechnology() failed: 0x" << std::hex << res << ".\n"; 
			}
		}
#endif

	ptrFileNameGEDataStore->Initialize( GEPT_HPP );

	tstring::tstring tstrFileName = "TestAdminFileName1";
	GEACTION geaFirstBefore = GEA_QUARANTINE;
	GEACTION geaFirstAfter	= GEA_UNSET;
	GEACTION geaSecondBefore= GEA_LOGONLY;
	GEACTION geaSecondAfter	= GEA_UNSET;

	GEOWNER geoBefore = GEO_ADMIN;

	////////////////////////////////////
	// Test out Admin lists first.
	{
		// Set its values.
		if (FAILED (res = ptrFileNameGEItem->SetProtectionTechnology( GEPT_HPP )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetProtectionTechnology() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileNameGEItem->SetFirstAction(geaFirstBefore)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetFirstAction() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileNameGEItem->SetSecondAction(geaSecondBefore)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetSecondAction() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileNameGEItem->SetOwner(geoBefore)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetOwner() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileNameGEItem->SetName(tstrFileName.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		
		// Now, add it to the DataStore.
		if (FAILED (res = ptrFileNameGEDataStore->SetAt( tstrFileName, ptrFileNameGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}

		// Add another one.
		tstrFileName = "TestAdminFileName2";
		if (FAILED (res = ptrFileNameGEItem->SetName(tstrFileName.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileNameGEItem->SetOwner(GEO_ADMIN)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetProTech() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		// Now, add it to the DataStore.
		if (FAILED (res = ptrFileNameGEDataStore->SetAt( tstrFileName, ptrFileNameGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
	}

	//////////////////////////////////////////
	//  Now add some client global exceptions.
	{
		tstrFileName = "TestAdminFileName3";
		geoBefore = GEO_CLIENT;
		if (FAILED (res = ptrFileNameGEItem->SetOwner(geoBefore)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetOwner() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileNameGEItem->SetName(tstrFileName.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		// Now, add it to the DataStore.
		if (FAILED (res = ptrFileNameGEDataStore->SetAt( tstrFileName, ptrFileNameGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}

		////////////////////////
		// Add another one.
		tstrFileName = "TestAdminFileName4";
		if (FAILED (res = ptrFileNameGEItem->SetName(tstrFileName.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileNameGEDataStore->SetAt( tstrFileName, ptrFileNameGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}

		////////////////////////
		// Add another one.
		tstrFileName = "TestAdminFileName5";
		if (FAILED (res = ptrFileNameGEItem->SetName(tstrFileName.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		// Now, add it to the DataStore.
		if (FAILED (res = ptrFileNameGEDataStore->SetAt( tstrFileName, ptrFileNameGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}

	}

	// Since, we are reading and writing to the registry as fast as possible in this test app, we need to make sure that the watching
	// thread has had a chance to get notified and update the caches. Otherwise, we're only testing how fast threads swap.
	::WaitForSingleObject(g_hEvent, 5000); 

	//////////////////////////////////////////////////////
	// Try to find the first one added to the Admin list.
	{
		tstrFileName = "TestAdminFileName1";
		// Make sure we can read it from the datastore.
		IFileNameGEItemPtr	ptrFileNameGEItem2;
		if (FAILED(res = ptrFileNameGEDataStore->Lookup( tstrFileName, ptrFileNameGEItem2 ))
			|| RTN_FALSE == res)
		{
			ASSERT(FALSE); std::cout  << "ERROR: Lookup() failed: 0x" << std::hex << res << ".\n";
			return (RTN_FALSE == res) ? RTNERR_FAILED : res;
		}

		// Verify we got something back.
		if (FAILED (res = ptrFileNameGEItem2->GetFirstAction(&geaFirstAfter)) ||
			FAILED (res = ptrFileNameGEItem2->GetSecondAction(&geaSecondAfter)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: GetFirstAction()/GetSecondAction(): 0x" << std::hex << res << ".\n";
			return res;
		}
		// Verify that what we set before is what we got back.
		if (geaFirstBefore != geaFirstAfter || geaSecondBefore != geaSecondAfter)
		{
			ASSERT(FALSE); std::cout  << "ERROR: First/Second actions don't match up after SetAt/Lookup.\n";
			return res;
		}
		// Check out the name.
		tstring::tstring tstr;
		if (FAILED (res = ptrFileNameGEItem2->GetName(&tstr)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: GetName(): 0x" << std::hex << res << ".\n";
			return res;
		}
		if (tstr.empty() == true)
		{
			ASSERT(FALSE); std::cout  << "ERROR: GetName() - Name is empty!\n";
			return res;
		}

		
		//////////////////////////////////////////
		// Let's do the same thing for a client GE.
		tstrFileName = "TestAdminFileName4";
		IFileNameGEItemPtr	ptrFileNameGEItem3;
		if (FAILED (res = ptrFileNameGEDataStore->Lookup( tstrFileName, ptrFileNameGEItem3 )) || res == RTN_FALSE)
		{
			ASSERT(FALSE); std::cout  << "ERROR: Lookup() failed: 0x" << std::hex << res << ".\n";
			return (RTN_FALSE == res) ? RTNERR_FAILED : res;
		}

		// Verify we got something back.
		if (FAILED (res = ptrFileNameGEItem3->GetFirstAction(&geaFirstAfter)) ||
			FAILED (res = ptrFileNameGEItem3->GetSecondAction(&geaSecondAfter)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: GetFirstAction()/GetSecondAction(): 0x" << std::hex << res << ".\n";
			return res;
		}
		// Verify that what we set before is what we got back.
		if (geaFirstBefore != geaFirstAfter || geaSecondBefore != geaSecondAfter)
		{
			ASSERT(FALSE); std::cout  << "ERROR: First/Second actions don't match up after SetAt/Lookup.\n";
			return res;
		}
	}

	///////////////////////////////////////////////////
	// Get the lookup Keys for the GE
	{
		std::vector<IFileNameGEItemPtr> vecFileNameGEKeys;
		ptrFileNameGEDataStore->GetExceptions( vecFileNameGEKeys );
		if ( !vecFileNameGEKeys.empty() )
		{
			RESULT res = RTN_SUCCESS;
			IFileNameGEItemPtr ptrFileNameGEItem = vecFileNameGEKeys[0];
			GEOWNER geo;
			if ( FAILED ( res = ptrFileNameGEItem->GetOwner( &geo ) ))
			{
				ASSERT(FALSE); std::cout  << "Error: Accessing Getowner from ptrFileNameGEItem!" << std::hex << res << std::endl;
				return res;
			}


		}
	}

	///////////////////////////////////////////////////
	// Finally, let's try and remove one and then remove them all.
	{
		tstrFileName = "TestAdminFileName4";
		if (FAILED (res = ptrFileNameGEDataStore->RemoveKey(tstrFileName, GEO_CLIENT, GEPT_HPP)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: RemoveKey() failed: 0x" << std::hex << res << ".\n";
			return res;
		}

		// Since, we are reading and writing to the registry as fast as possible in this test app, we need to make sure that the watching
		// thread has had a chance to get notified and update the caches. Otherwise, we're only testing how fast threads swap.
		::WaitForSingleObject(g_hEvent, 5000); 

		// Let's verify it's gone by trying to find it.
		IFileNameGEItemPtr	ptrFileNameGEItem3;
		if (FAILED (res = ptrFileNameGEDataStore->Lookup( tstrFileName, ptrFileNameGEItem3 )))
		{
			ASSERT(FALSE); std::cout    << "ERROR: Lookup() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (res != RTN_FALSE)
		{
			ASSERT(FALSE); std::cout  << "ERROR: We failed to remove this global exception: 0x" << std::hex << res << ".\n";
			return RTNERR_FAILED;
		}
	}

	/////////////////////////////////////////////////
	// Let's remove them all
	{
		if (FAILED (res = ptrFileNameGEDataStore->RemoveAll(GEO_ADMIN, GEPT_HPP)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: RemoveAll() failed for Admin list: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileNameGEDataStore->RemoveAll(GEO_CLIENT, GEPT_HPP)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: RemoveAll() failed for Admin list: 0x" << std::hex << res << ".\n";
			return res;
		}
	}

	// Since, we are reading and writing to the registry as fast as possible in this test app, we need to make sure that the watching
	// thread has had a chance to get notified and update the caches. Otherwise, we're only testing how fast threads swap.
	::WaitForSingleObject(g_hEvent, 5000); 

	return RTN_SUCCESS;
}

static RESULT TestFileHashBasedExceptions()
{
    RESULT					res = RTN_SUCCESS;
	SYMRESULT				symres = SYM_SUCCESS;
    IFileHashGEDataStorePtr	ptrFileHashGEDataStore;
    IFileHashGEItemPtr		ptrFileHashGEItem;
	
    // Create the FileHash DataStore object.
    if (SYM_FAILED (symres = g_objFileHashGEDataStoreLdr.CreateObject( &ptrFileHashGEDataStore )))
	{
		ASSERT(FALSE); std::cout  << "ERROR: CreateObject() failed: 0x" << std::hex << symres << ".\n";
        return RTNERR_FAILED;
	}
	ptrFileHashGEDataStore->Initialize( GEPT_HPP );

	// Create a FileHash Item	
	if (SYM_FAILED (symres = g_objFileHashGEItemLdr.CreateObject( &ptrFileHashGEItem )))
	{
		ASSERT(FALSE); std::cout  << "ERROR: CreateObject() failed: 0x" << std::hex << symres << ".\n";
		return RTNERR_FAILED;
	}



		///////////////////////////////////////////
	// Add some exceptions to the DataStore
#ifdef INVALID_ERROR_CHK
		//////////////////////////////////////
		//Error Checking for FileHash GE Setters
		{
			//Error Checking for Setting Invalid values!
			if (FAILED (res = ptrFileHashGEItem->SetName("")))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetFileHashName() failed: 0x" << std::hex << res << ".\n";
			}
			if (FAILED (res = ptrFileHashGEItem->SetOwner(GEO_UNSET )))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetFileHashOwner() failed: 0x" << std::hex << res << ".\n";
			}
			if (FAILED (res = ptrFileHashGEItem->SetFirstAction( GEA_UNSET )))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetFileHashFirstAction() failed: 0x" << std::hex << res << ".\n";
			}
			if (FAILED (res = ptrFileHashGEItem->SetSecondAction( GEA_LAST_ITEM )))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetFileHashSecondAction() failed: 0x" << std::hex << res << ".\n";
			}
			
			if (FAILED (res = ptrFileHashGEItem->SetProtectionTechnology(GEPT_UNSET)))
			{
				ASSERT(FALSE); std::cout  << "ERROR: SetFileHashProtectionTechnology() failed: 0x" << std::hex << res << ".\n"; 
			}
		}
#endif

	tstring::tstring tstrFileHash = "111111111111111111111111111";
	GEACTION geaFirstBefore = GEA_QUARANTINE;
	GEACTION geaFirstAfter	= GEA_UNSET;
	GEACTION geaSecondBefore= GEA_LOGONLY;
	GEACTION geaSecondAfter	= GEA_UNSET;

	GEOWNER geoBefore = GEO_ADMIN;

	////////////////////////////////////
	// Test out Admin lists first.
	{
		// Set its values.
		if (FAILED (res = ptrFileHashGEItem->SetProtectionTechnology( GEPT_HPP) ) )
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetProtectionTechnology() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileHashGEItem->SetFirstAction(geaFirstBefore)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetFirstAction() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileHashGEItem->SetSecondAction(geaSecondBefore)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetSecondAction() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileHashGEItem->SetOwner(geoBefore)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetOwner() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileHashGEItem->SetName(tstrFileHash.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileHashGEItem->SetFileSize( 10000 )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		
		// Now, add it to the DataStore.
		if (FAILED (res = ptrFileHashGEDataStore->SetAt( tstrFileHash, ptrFileHashGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}


		
		// Add another one.
		tstrFileHash = "2222222222222222222222222";
		if (FAILED (res = ptrFileHashGEItem->SetName(tstrFileHash.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileHashGEItem->SetOwner(GEO_ADMIN)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetProTech() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		// Now, add it to the DataStore.
		if (FAILED (res = ptrFileHashGEDataStore->SetAt( tstrFileHash, ptrFileHashGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}

		
	}


	//////////////////////////////////////////
	//  Now add some client global exceptions.
	{
		tstrFileHash = "33333333333333333333333333";
		geoBefore = GEO_CLIENT;
		if (FAILED (res = ptrFileHashGEItem->SetOwner(geoBefore)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetOwner() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileHashGEItem->SetName(tstrFileHash.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		// Now, add it to the DataStore.
		if (FAILED (res = ptrFileHashGEDataStore->SetAt( tstrFileHash, ptrFileHashGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}


		////////////////////////
		// Add another one.
		tstrFileHash = "444444444444444444444444444";
		if (FAILED (res = ptrFileHashGEItem->SetName(tstrFileHash.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileHashGEDataStore->SetAt( tstrFileHash, ptrFileHashGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}


		////////////////////////
		// Add another one.
		tstrFileHash = "5555555555555555555555555";
		if (FAILED (res = ptrFileHashGEItem->SetName(tstrFileHash.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		// Now, add it to the DataStore.
		if (FAILED (res = ptrFileHashGEDataStore->SetAt( tstrFileHash, ptrFileHashGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}

	}

	// Since, we are reading and writing to the registry as fast as possible in this test app, we need to make sure that the watching
	// thread has had a chance to get notified and update the caches. Otherwise, we're only testing how fast threads swap.
	::WaitForSingleObject(g_hEvent, 5000);

	//////////////////////////////////////////////////////
	// Try to find the first one added to the Admin list.
	{
		tstrFileHash = "111111111111111111111111111";
		// Make sure we can read it from the datastore.
		IFileHashGEItemPtr	ptrFileHashGEItem2;
		if (FAILED(res = ptrFileHashGEDataStore->Lookup( tstrFileHash, ptrFileHashGEItem2 ))
			|| RTN_FALSE == res)
		{
			ASSERT(FALSE); std::cout  << "ERROR: Lookup() failed: 0x" << std::hex << res << ".\n";
			return (RTN_FALSE == res) ? RTNERR_FAILED : res;
		}

		// Verify we got something back.
		if (FAILED (res = ptrFileHashGEItem2->GetFirstAction(&geaFirstAfter)) ||
			FAILED (res = ptrFileHashGEItem2->GetSecondAction(&geaSecondAfter)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: GetFirstAction()/GetSecondAction(): 0x" << std::hex << res << ".\n";
			return res;
		}
		// Verify that what we set before is what we got back.
		if (geaFirstBefore != geaFirstAfter || geaSecondBefore != geaSecondAfter)
		{
			ASSERT(FALSE); std::cout  << "ERROR: First/Second actions don't match up after SetAt/Lookup.\n";
			return res;
		}
		// Check out the name.
		tstring::tstring tstr;
		if (FAILED (res = ptrFileHashGEItem2->GetName(&tstr)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: GetName(): 0x" << std::hex << res << ".\n";
			return res;
		}
		if (tstr.empty() == true)
		{
			ASSERT(FALSE); std::cout  << "ERROR: GetName() - Name is empty!\n";
			return res;
		}


		//////////////////////////////////////////
		// Let's do the same thing for a client GE.
		tstrFileHash = "444444444444444444444444444";
		IFileHashGEItemPtr	ptrFileHashGEItem3;
		if (FAILED (res = ptrFileHashGEDataStore->Lookup( tstrFileHash, ptrFileHashGEItem3 )) ||
			res == RTN_FALSE)
		{
			ASSERT(FALSE); std::cout  << "ERROR: Lookup() failed: 0x" << std::hex << res << ".\n";
			return (RTN_FALSE == res) ? RTNERR_FAILED : res;
		}

		// Verify we got something back.
		if (FAILED (res = ptrFileHashGEItem3->GetFirstAction(&geaFirstAfter)) ||
			FAILED (res = ptrFileHashGEItem3->GetSecondAction(&geaSecondAfter)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: GetFirstAction()/GetSecondAction(): 0x" << std::hex << res << ".\n";
			return res;
		}
		// Verify that what we set before is what we got back.
		if (geaFirstBefore != geaFirstAfter || geaSecondBefore != geaSecondAfter)
		{
			ASSERT(FALSE); std::cout  << "ERROR: First/Second actions don't match up after SetAt/Lookup.\n";
			return res;
		}

	}

	///////////////////////////////////////////////////
	// Get Lookup Keys for the GE
	{
		std::vector<IFileHashGEItemPtr> vecFileHashGEKeys;
		ptrFileHashGEDataStore->GetExceptions( vecFileHashGEKeys );
		//SAVASSERT( vecFileHashGEKeys.size() != 0 );		
	}


	///////////////////////////////////////////////////
	// Finally, let's try and remove one and then remove them all.
	{
	
		IFileHashGEItemPtr ptrFileHashGEItem13;
		tstrFileHash = "444444444444444444444444444"; // This item is owned by the client
		if (FAILED (res = ptrFileHashGEDataStore->RemoveKey(tstrFileHash, GEO_CLIENT, GEPT_HPP)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: RemoveKey() failed: 0x" << std::hex << res << ".\n";
			return res;
		}

		// Since, we are reading and writing to the registry as fast as possible in this test app, we need to make sure that the watching
		// thread has had a chance to get notified and update the caches. Otherwise, we're only testing how fast threads swap.
		::WaitForSingleObject(g_hEvent, 5000); 

		if (FAILED (res = ptrFileHashGEDataStore->Lookup( tstrFileHash, ptrFileHashGEItem13 )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: Lookup() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (res != RTN_FALSE)
		{
			ASSERT(FALSE); std::cout  << "ERROR: We failed to remove this global exception: 0x" << std::hex << res << ".\n";
			return RTNERR_FAILED;
		}
		
	}

	/////////////////////////////////////////////////
	// Let's remove them all
	{
		if (FAILED (res = ptrFileHashGEDataStore->RemoveAll(GEO_ADMIN, GEPT_HPP)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: RemoveAll() failed for Admin list: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrFileHashGEDataStore->RemoveAll(GEO_CLIENT, GEPT_HPP)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: RemoveAll() failed for Admin list: 0x" << std::hex << res << ".\n";
			return res;
		}
	}

	// Since, we are reading and writing to the registry as fast as possible in this test app, we need to make sure that the watching
	// thread has had a chance to get notified and update the caches. Otherwise, we're only testing how fast threads swap.
	::WaitForSingleObject(g_hEvent, 5000);

	return RTN_SUCCESS;

}

static RESULT TestDirectoryBasedExceptions()
{
    RESULT						res = RTN_SUCCESS;
	SYMRESULT					symres = SYM_SUCCESS;
	IDirectoryGEDataStorePtr	ptrDirectoryGEDataStore;
	IDirectoryGEItemPtr			ptrDirectoryGEItem;
	
	// Create the Directory DataStore object.
    if (SYM_FAILED (symres = g_objDirectoryGEDataStoreLdr.CreateObject( &ptrDirectoryGEDataStore )))
	{
		ASSERT(FALSE); std::cout  << "ERROR: CreateObject() failed: 0x" << std::hex << symres << ".\n";
        return RTNERR_FAILED;
	}
	ptrDirectoryGEDataStore->Initialize( GEPT_HPP );

	// Create a Directory Item	
	if (SYM_FAILED (symres = g_objDirectoryGEItemLdr.CreateObject( &ptrDirectoryGEItem )))
	{
		ASSERT(FALSE); std::cout  << "ERROR: CreateObject() failed: 0x" << std::hex << symres << ".\n";
		return RTNERR_FAILED;
	}


	///////////////////////////////////////////
	// Add some exceptions to the DataStore
#ifdef INVALID_ERROR_CHK
	//////////////////////////////////////
	//Error Checking for Directory GE Setters
	{
		//Error Checking for Setting Invalid values!
		if (FAILED (res = ptrDirectoryGEItem->SetName("")))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetDirectoryName() failed: 0x" << std::hex << res << ".\n";
		}
		if (FAILED (res = ptrDirectoryGEItem->SetOwner(GEO_UNSET )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetDirectoryOwner() failed: 0x" << std::hex << res << ".\n";
		}
		if (FAILED (res = ptrDirectoryGEItem->SetFirstAction( GEA_UNSET )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetDirectoryFirstAction() failed: 0x" << std::hex << res << ".\n";
		}
		if (FAILED (res = ptrDirectoryGEItem->SetSecondAction( GEA_LAST_ITEM )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetDirectorySecondAction() failed: 0x" << std::hex << res << ".\n";
		}

		if (FAILED (res = ptrDirectoryGEItem->SetProtectionTechnology(GEPT_UNSET)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetDirectoryProtectionTechnology() failed: 0x" << std::hex << res << ".\n"; 
		}
	}
#endif

	tstring::tstring tstrDirectory = "C:\\Temp1";
	GEACTION geaFirstBefore = GEA_QUARANTINE;
	GEACTION geaFirstAfter	= GEA_UNSET;
	GEACTION geaSecondBefore= GEA_LOGONLY;
	GEACTION geaSecondAfter	= GEA_UNSET;

	GEOWNER geoBefore = GEO_ADMIN;

	////////////////////////////////////
	// Test out Admin lists first.
	{
		// Set its values.
		if (FAILED (res = ptrDirectoryGEItem->SetProtectionTechnology( GEPT_TP ) ) )
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetProtectionTechnology() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrDirectoryGEItem->SetFirstAction(geaFirstBefore)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetFirstAction() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrDirectoryGEItem->SetSecondAction(geaSecondBefore)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetSecondAction() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrDirectoryGEItem->SetOwner(geoBefore)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetOwner() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrDirectoryGEItem->SetName(tstrDirectory.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrDirectoryGEItem->SetExtensions(_T(".txt") )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetExtensions() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrDirectoryGEItem->SetIncludeSubDirs(true) ))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetSubDirectories() failed: 0x" << std::hex << res << ".\n";
			return res;
		}

		// Now, add it to the DataStore.
		if (FAILED (res = ptrDirectoryGEDataStore->SetAt( tstrDirectory, ptrDirectoryGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}

		// Add another one.
		tstrDirectory = "C:\\TEMP@2";
		if (FAILED (res = ptrDirectoryGEItem->SetName(tstrDirectory.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrDirectoryGEItem->SetOwner(GEO_ADMIN)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetProTech() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		// Now, add it to the DataStore.
		if (FAILED (res = ptrDirectoryGEDataStore->SetAt( tstrDirectory, ptrDirectoryGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
	}

	//////////////////////////////////////////
	//  Now add some client global exceptions.
	{
		tstrDirectory = "C:\\TEMP33";
		geoBefore = GEO_CLIENT;
		if (FAILED (res = ptrDirectoryGEItem->SetOwner(geoBefore)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetOwner() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrDirectoryGEItem->SetName(tstrDirectory.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		// Now, add it to the DataStore.
		if (FAILED (res = ptrDirectoryGEDataStore->SetAt( tstrDirectory, ptrDirectoryGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}


		////////////////////////
		// Add another one.
		tstrDirectory = "C:\\TEMP44";
		if (FAILED (res = ptrDirectoryGEItem->SetName(tstrDirectory.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrDirectoryGEDataStore->SetAt( tstrDirectory, ptrDirectoryGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}


		////////////////////////
		// Add another one.
		tstrDirectory = "C:\\TEMP44";
		if (FAILED (res = ptrDirectoryGEItem->SetName(tstrDirectory.c_str())))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetName() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		// Now, add it to the DataStore.
		if (FAILED (res = ptrDirectoryGEDataStore->SetAt( tstrDirectory, ptrDirectoryGEItem )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: SetAt() failed: 0x" << std::hex << res << ".\n";
			return res;
		}

	}


	// Since, we are reading and writing to the registry as fast as possible in this test app, we need to make sure that the watching
	// thread has had a chance to get notified and update the caches. Otherwise, we're only testing how fast threads swap.
	::WaitForSingleObject(g_hEvent, 5000); 

		//////////////////////////////////////////////////////
	// Try to find the first one added to the Admin list.
	{
		tstrDirectory = "C:\\Temp1";
		// Make sure we can read it from the datastore.
		IDirectoryGEItemPtr	ptrDirectoryGEItem2;
		if (FAILED(res = ptrDirectoryGEDataStore->Lookup( tstrDirectory, ptrDirectoryGEItem2 ))
			|| RTN_FALSE == res)
		{
			ASSERT(FALSE); std::cout  << "ERROR: Lookup() failed: 0x" << std::hex << res << ".\n";
			return (RTN_FALSE == res) ? RTNERR_FAILED : res;
		}

		// Verify we got something back.
		if (FAILED (res = ptrDirectoryGEItem2->GetFirstAction(&geaFirstAfter)) ||
			FAILED (res = ptrDirectoryGEItem2->GetSecondAction(&geaSecondAfter)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: GetFirstAction()/GetSecondAction(): 0x" << std::hex << res << ".\n";
			return res;
		}
		// Verify that what we set before is what we got back.
		if (geaFirstBefore != geaFirstAfter || geaSecondBefore != geaSecondAfter)
		{
			ASSERT(FALSE); std::cout  << "ERROR: First/Second actions don't match up after SetAt/Lookup.\n";
			return res;
		}
		// Check out the name.
		tstring::tstring tstr;
		if (FAILED (res = ptrDirectoryGEItem2->GetName(&tstr)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: GetName(): 0x" << std::hex << res << ".\n";
			return res;
		}
		if (tstr.empty() == true)
		{
			ASSERT(FALSE); std::cout  << "ERROR: GetName() - Name is empty!\n";
			return res;
		}


		// Since, we are reading and writing to the registry as fast as possible in this test app, we need to make sure that the watching
		// thread has had a chance to get notified and update the caches. Otherwise, we're only testing how fast threads swap.
		::WaitForSingleObject(g_hEvent, 5000); 

		//////////////////////////////////////////
		// Let's do the same thing for a client GE.
		tstrDirectory = "C:\\TEMP44";
		IDirectoryGEItemPtr	ptrDirectoryGEItem3;
		if (FAILED (res = ptrDirectoryGEDataStore->Lookup( tstrDirectory, ptrDirectoryGEItem3 )) ||
			res == RTN_FALSE)
		{
			ASSERT(FALSE); std::cout  << "ERROR: Lookup() failed: 0x" << std::hex << res << ".\n";
			return (RTN_FALSE == res) ? RTNERR_FAILED : res;
		}

		// Verify we got something back.
		if (FAILED (res = ptrDirectoryGEItem3->GetFirstAction(&geaFirstAfter)) ||
			FAILED (res = ptrDirectoryGEItem3->GetSecondAction(&geaSecondAfter)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: GetFirstAction()/GetSecondAction(): 0x" << std::hex << res << ".\n";
			return res;
		}
		// Verify that what we set before is what we got back.
		if (geaFirstBefore != geaFirstAfter || geaSecondBefore != geaSecondAfter)
		{
			ASSERT(FALSE); std::cout  << "ERROR: First/Second actions don't match up after SetAt/Lookup.\n";
			return res;
		}
	}

	///////////////////////////////////////////////////
	// Get Lookup Keys for the GE
	{
	//	std::vector<tstring::tstring> vecDirectoryGEKeys;
	//	ptrDirectoryGEDataStore->GetLookupKeys( &vecDirectoryGEKeys );
		//SAVASSERT( vecDirectoryGEKeys.size() != 0 );		
	}

	///////////////////////////////////////////////////
	// Finally, let's try and remove one and then remove them all.
	{
		tstrDirectory = "C:\\TEMP44"; // This item is owned by the client
		if (FAILED (res = ptrDirectoryGEDataStore->RemoveKey(tstrDirectory, GEO_CLIENT, GEPT_HPP)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: RemoveKey() failed: 0x" << std::hex << res << ".\n";
			return res;
		}

		// Since, we are reading and writing to the registry as fast as possible in this test app, we need to make sure that the watching
		// thread has had a chance to get notified and update the caches. Otherwise, we're only testing how fast threads swap.
		::WaitForSingleObject(g_hEvent, 5000); 

		// Let's verify it's gone by trying to find it.
		IDirectoryGEItemPtr	ptrDirectoryGEItem3;
		if (FAILED (res = ptrDirectoryGEDataStore->Lookup( tstrDirectory, ptrDirectoryGEItem3 )))
		{
			ASSERT(FALSE); std::cout  << "ERROR: Lookup() failed: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (res != RTN_FALSE)
		{
			ASSERT(FALSE); std::cout  << "ERROR: We failed to remove this global exception: 0x" << std::hex << res << ".\n";
			return RTNERR_FAILED;
		}
	}

	/////////////////////////////////////////////////
	// Let's remove them all
	{
		if (FAILED (res = ptrDirectoryGEDataStore->RemoveAll(GEO_ADMIN, GEPT_HPP)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: RemoveAll() failed for Admin list: 0x" << std::hex << res << ".\n";
			return res;
		}
		if (FAILED (res = ptrDirectoryGEDataStore->RemoveAll(GEO_CLIENT, GEPT_HPP)))
		{
			ASSERT(FALSE); std::cout  << "ERROR: RemoveAll() failed for Admin list: 0x" << std::hex << res << ".\n";
			return res;
		}
	}

	// Since, we are reading and writing to the registry as fast as possible in this test app, we need to make sure that the watching
	// thread has had a chance to get notified and update the caches. Otherwise, we're only testing how fast threads swap.
	::WaitForSingleObject(g_hEvent, 5000); 

	return RTN_SUCCESS;
}

static int app_test()
{
	RESULT res = RTN_SUCCESS;

	// Test out Risk Based global exceptions
/*	if (FAILED(res = TestRiskBasedExceptions()))
	{
		std::cout  << "ERROR: Risk Exceptions failed!\n";
		return 1;
	}
	ASSERT(RTN_FALSE != res);
*/

	// Test out FileName Based global exceptions
	if (FAILED(res = TestFileNameBasedExceptions()))
	{
		std::cout  << "ERROR: FileName Exceptions failed!\n";
		return 1;
	}
	ASSERT(RTN_FALSE != res);

	// Test out FileHash Based global exceptions
	if (FAILED(res = TestFileHashBasedExceptions()))
	{
		std::cout  << "ERROR: FileHash Exceptions failed!\n";
		return 1;
	}
	ASSERT(RTN_FALSE != res);

	// Test out Directory Based global exceptions
	if (FAILED(res = TestDirectoryBasedExceptions()))
	{
		std::cout  << "ERROR: Directory Exceptions failed!\n";
		return 1;
	}
	ASSERT(RTN_FALSE != res);

	return 0;
}

int main( int argc, char* argv[] )
{
    int ret = 0;
    int tmpFlag   = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
    tmpFlag      |= _CRTDBG_LEAK_CHECK_DF;
//  tmpFlag      |= _CRTDBG_CHECK_CRT_DF;

    _CrtSetDbgFlag( tmpFlag );


	// brian_schlatter -- I'm not sure we need this, but this is what ScsComms was doing and the
	// the reason is stated below.
	//
	// Get a second handle to the DLL so that we can ensure that it stays loaded
    // until process exit.  Otherwise, memory leaks reported by the C-run time will
    // throw exceptions, because information about where they were allocated is unmapped
    // when the DLL is unloaded.
	HMODULE hGEDataStore = LoadLibrary( g_szPathToGEDataStoreDll );
    if( hGEDataStore == NULL )
    {
        ASSERT(FALSE); std::cout  << "Failed to load GEDataStore.dll from: " << g_szPathToGEDataStoreDll << std::endl;
        return( -1 );
    }

	SYMRESULT symres = LoadGEDataStoreByHandle(hGEDataStore);

//    SYMRESULT symres = LoadGEDataStore();

    if (SYM_FAILED (symres))
        return 1;

	g_hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	ret = app_test();

	assert(ret == 0);

	::CloseHandle(g_hEvent);
	
	symres = UnLoadGEDataStore();
	if (SYM_FAILED (symres))
        return 1;

    return( ret );
}

