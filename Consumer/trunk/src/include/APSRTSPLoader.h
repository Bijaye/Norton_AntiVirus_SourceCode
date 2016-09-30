////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// This is the NAV Version of the AP 'srtsp32.dll' loader, needed for such things as the
// AP Exclusions SymInterface. We provide our own because AP doesn't, for some reason.

// CC includes
#include <ccSymInstalledApps.h>
#include <ccStringConvert.h>
#include <ccSymInterfaceLoader.h>
#include "SrtExclusionsInterface.h"

namespace SRTSP
{
	class CSRTSPPathProvider
	{
	public:
		static bool GetPath(LPTSTR szPath, size_t &nSize) throw()
		{
			ccLib::CString strSRTSPDir;
			if( !ccSym::CInstalledApps::GetInstAppsDirectory(_T("SRTSP"), strSRTSPDir) )
			{
				return false;
			}
			return ccLib::CStringConvert::Copy(strSRTSPDir, szPath, nSize);
		}
	};

	/*
	Based on CC's CSymInterfaceDLLHelper.  Modified to allow DLL name to be
	specified without requiring a .cpp file for module name definition (by
	overriding GetDLLName in derived classes).
	*/
	template
		<
			class TPathProvider, 
			class TInterface, 
				const SYMOBJECT_ID* pOID, 
				const SYMINTERFACE_ID* pIID,
			class TLoader
		>
		class CSRTSPInterfaceHelperWithCustomLoader :
			public CSymInterfacePathHelper
			<
			TPathProvider, 
			TLoader, 
			TInterface, 
			pOID, 
			pIID
			>
		{
		public:
			typedef CSymInterfacePathHelper
				<
				TPathProvider,
				TLoader, 
				TInterface, 
				pOID, 
				pIID
				> TBase;

		public:
			CSRTSPInterfaceHelperWithCustomLoader(void) throw() {}
			virtual ~CSRTSPInterfaceHelperWithCustomLoader() throw() {}

		private:
			CSRTSPInterfaceHelperWithCustomLoader(
				const CSRTSPInterfaceHelperWithCustomLoader&) throw();
			CSRTSPInterfaceHelperWithCustomLoader& operator =(
				const CSRTSPInterfaceHelperWithCustomLoader&) throw();

		public:
			SYMRESULT Create(TInterface** ppInterface) throw()
			{
				return CreateObject(ppInterface);
			}
			SYMRESULT CreateObject(TInterface** ppInterface) throw()
			{
				if (m_hMod == NULL)
				{
					SYMRESULT symRes = Initialize();
					if (SYM_FAILED(symRes))
					{
						return symRes;
					}
				}
				return TBase::CreateObject(ppInterface);
			}
			SYMRESULT CreateObject(TInterface*& pInterface) throw()
			{
				return CreateObject(&pInterface);
			}
			SYMRESULT Initialize() throw()
			{
				return TBase::Initialize(GetDLLName());
			}
			virtual LPCTSTR GetDLLName(void) throw() = 0;

			// Since SRTSP requires the DLL to be unloaded and re-loaded when a
			// reload event is recieved, this interface helper exposes that functionality.
			// The client must release all objects obtained via this interface loader
			// in order for this to work.
			void Uninitialize() throw()
			{
				CCTRACEI(L"Explicitly unloading Srtsp32.dll, via client request. CLIENT NEEDS TO ENSURE ALL OBJECTS ARE RELEASED.");
				TLoader::clear();
			}
		};

		// ISrtContol loader
		class SRTSPLoader_ISrtExclusions :
			public CSRTSPInterfaceHelperWithCustomLoader
			<
			CSRTSPPathProvider,
			ISrtExclusions,
			&OBJID_SrtExclusions,
			&IID_SrtExclusions,
			::cc::CSymInterfaceTrustedCacheLoader>
		{
		public:
			LPCTSTR GetDLLName(void) throw() { return _T("Srtsp32.dll"); }
		};

}