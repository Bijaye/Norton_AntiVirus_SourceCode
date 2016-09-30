#pragma once

#include "FeatureContainerImpl.h"
#include "nscLibCActionsCollection.h"
#include "nscLibCAction.h"
#include "ccModule.h"
#include "lm.h"

// {31BC2611-72F6-4cec-B0F3-366F49E05652}
//SYM_DEFINE_OBJECT_ID(CLSID_CAdminCheck, 
//		0x31bc2611, 0x72f6, 0x4cec, 0xb0, 0xf3, 0x36, 0x6f, 0x49, 0xe0, 0x56, 0x52);
//
//// {DBF21199-007C-470b-91C5-71EA5E614FB2}
//DEFINE_GUID(CallbackGUID, 
//			0xdbf21199, 0x7c, 0x470b, 0x91, 0xc5, 0x71, 0xea, 0x5e, 0x61, 0x4f, 0xb2);

class CAdminCheckFeature :
	public CFeatureContainerImpl
{
public:
	CAdminCheckFeature(void);
	virtual ~CAdminCheckFeature(void);

// override of IFeature
public:
	virtual nsc::NSCRESULT GetHealth(const nsc::IContext* context_in, const nsc::IHealth*& health_out) const throw();
	virtual nsc::NSCRESULT GetActions(const nsc::IContext* context_in, const nsc::IActionsCollection*& actions_out) const throw();

// overrid of IClientCallback
public:
	nsc::NSCRESULT Execute(HWND window_in, DWORD cookie_in) const throw();

private:
	nsc::NSCRESULT AllUserAccessActions(const nsc::IActionsCollection*& actions_out) const;
	bool IsAllUserAccessAction(GUID& ActionGuid) const;
	
// internal methods for creating the client actions
protected:
	//nsc::NSCRESULT AddClientAction(nscLib::CActionsCollection*& pCollection, CString sActionName, CString sActionDesc, DWORD dwPolicyID, const GUID& guid_in) const throw();
	//nsc::NSCRESULT CreateAction(nscLib::CAction **ppAction, CString sActionName, CString sActionDesc, const GUID& guid_in) const throw();
	//nsc::NSCRESULT CreateClientAction(nscLib::CAction **ppAction, CString sActionName, CString sActionDesc, DWORD dwPolicyID, const GUID& guid_in) const throw();

	//bool IsAdmin(const nsc::IContext* context_in) const throw();

// NetApi.dll helper
//protected:
//	class CNetAPI : public ccLib::CModule
//	{
//	public:
//		CNetAPI();
//		virtual ~CNetAPI();
//
//		bool Create() throw();
//		void Destroy() throw();
//
//		CCMODULE_DECLARE_PROC_MAP();
//		CCMODULE_DECLARE_PROC(NET_API_STATUS, __stdcall, NetUserGetLocalGroups, (LPCWSTR servername, LPCWSTR username, DWORD level, DWORD flags, LPBYTE *bufptr, DWORD prefmaxlen, LPDWORD entriesread, LPDWORD totalentries));
//		CCMODULE_DECLARE_PROC(NET_API_STATUS, __stdcall, NetApiBufferFree, (LPVOID Buffer));
//	};
};

