// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SymInterface.h>
#include <NavWInterface.h>
#include <AVInterfaces.h>
#include <ccAlertInterface.h>

namespace avScanUI
{

//////////////////////////////////////////////////////////////////////////////////
// Interface IDs
//////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////
// AntiVirus ScanUI IID
// {368C04BF-2FAD-4611-9864-7400CA801C19}
SYM_DEFINE_INTERFACE_ID(IID_ScanUI,
                        0x368c04bf, 0x2fad, 0x4611, 0x98, 0x64, 0x74, 0x0, 0xca, 0x80, 0x1c, 0x19);

/////////////////////////////////////////
// AntiVirus Email ScanUI IID
// {7EEB8B88-194B-4217-9E42-5AC11231C832}
SYM_DEFINE_INTERFACE_ID(IID_EmailScanUI,
                        0x7eeb8b88, 0x194b, 0x4217, 0x9e, 0x42, 0x5a, 0xc1, 0x12, 0x31, 0xc8, 0x32);

/////////////////////////////////////////
// Misc Helper UIs IID
// {EED3FDA6-24AF-4e4d-9140-F33C4E903F4B}
SYM_DEFINE_INTERFACE_ID(IID_ScanUIMisc,
                        0xeed3fda6, 0x24af, 0x4e4d, 0x91, 0x40, 0xf3, 0x3c, 0x4e, 0x90, 0x3f, 0x4b);

/////////////////////////////////////////
// Single Instance helper IID
// {26526077-139B-48b1-98DC-58694CB1ECA0}
SYM_DEFINE_INTERFACE_ID(IID_SingleInstance,
                        0x26526077, 0x139b, 0x48b1, 0x98, 0xdc, 0x58, 0x69, 0x4c, 0xb1, 0xec, 0xa0);

//////////////////////////////////////////////////////////////////////////////////
// Object IDs
//////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////
// AntiVirus ScanUI Engine
// {A5CD792E-FD9C-4ef4-BEEE-3E72AE0E714B}
SYM_DEFINE_OBJECT_ID(SYMOBJECT_AVSCANUI,
                     0xa5cd792e, 0xfd9c, 0x4ef4, 0xbe, 0xee, 0x3e, 0x72, 0xae, 0xe, 0x71, 0x4b);

/////////////////////////////////////////
// AntiVirus Email ScanUI Engine
// {4F71A12B-1050-4880-9F4B-25F6E1B06767}
SYM_DEFINE_OBJECT_ID(SYMOBJECT_AVEMAILSCANUI,
                     0x4f71a12b, 0x1050, 0x4880, 0x9f, 0x4b, 0x25, 0xf6, 0xe1, 0xb0, 0x67, 0x67);

/////////////////////////////////////////
// Misc Helper UIs
// {32D8AB96-1893-4d5d-9CDD-56DDA0396F34}
SYM_DEFINE_OBJECT_ID(SYMOBJECT_SCANUIMISC,
                     0x32d8ab96, 0x1893, 0x4d5d, 0x9c, 0xdd, 0x56, 0xdd, 0xa0, 0x39, 0x6f, 0x34);

/////////////////////////////////////////
// Single Instance helper
// {31A31733-F08B-4c26-AF20-084691DCAE34}
SYM_DEFINE_INTERFACE_ID(SYMOBJECT_SINGLEINSTANCE,
                        0x31a31733, 0xf08b, 0x4c26, 0xaf, 0x20, 0x8, 0x46, 0x91, 0xdc, 0xae, 0x34);

//////////////////////////////////////////////////////////////////////////////////
// Forward decls
//////////////////////////////////////////////////////////////////////////////////
// none

//////////////////////////////////////////////////////////////////////////////////
// Interfaces
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
// Enumerations
//////////////////////////////////////////////////////////////////////////////////
enum eUIMode
{
    eUIMode_Normal = 0,
    eUIMode_NoResults,
    eUIMode_OnlyResults,
    eUIMode_None,
    eUIMode_OnlyInfections,
};

enum eScanResult
{
	SRESULT_OK = 0,
	SRESULT_ABORT,

	// Errors between 100 and 1000 have not had
	// a CED displayed for them ... one should be
	SRESULT_CED_NEEDED_FIRST = 100,
	SRESULT_ERROR_SCANUI = SRESULT_CED_NEEDED_FIRST,
	SRESULT_ERROR_CAV,
	SRESULT_ERROR_MEMORY,

	// Errors above 1000 have CEDs displayed for
	// them in avScanUI prior to HandleScan returning
	SRESULT_CED_DONE_FIRST = 1000,
};

enum eRiskMode
{
    eRiskMode_Normal = 0,       // Default handling (show it as we get it)
    eRiskMode_IgnoreOnce,       // Ignore the risk once (move it to the details page as "ignored")
    eRiskMode_Exclude,          // Exclude the risk (shows on details page as "Excluded")
};

enum eRebootPromptResult
{
    eReboot_Now = 0,
    eReboot_Later,
    eReboot_Error
};

enum eProcTermResult
{
    eProcTerm_Yes = 0,
    eProcTerm_No,
    eProcTerm_YesToAll,
    eProcTerm_NoToAll,
    eProcTerm_Error,
};

///////////////////////////////////////
// IScanUI interface:
class IScanUI : public ISymBase
{
    // Any IScanUI derived object must implement AVModule::IAVProgressSink
public:
    // Init...
    virtual bool Initialize() throw() = 0;

    // Property get/sets
    virtual bool SetScanName(LPCWSTR pszName) throw() = 0;
    virtual void SetLowRiskMode(eRiskMode eMode) throw() = 0;
    virtual void SetMediumRiskMode(eRiskMode eMode) throw() = 0;
    virtual void SetHighRiskMode(eRiskMode eMode) throw() = 0;

    // HandleScan is a blocking call that will not return until the scan is
    // complete, aborted, or has failed...
    virtual eScanResult HandleScan(AVModule::IAVScanBase* pScanner, eUIMode eMode, NAVWRESULT& navWResult) throw() = 0;
};
typedef CSymPtr<IScanUI> IScanUIPtr;
typedef CSymQIPtr<IScanUI, &IID_ScanUI> IScanUIQIPtr;


///////////////////////////////////////
// IEmailScanUI interface:
class IEmailScanUI : public ISymBase
{
public:
    // Init...
    virtual HRESULT Initialize() throw() = 0;

    // DisplayThreatUI is a blocking call that will not return until the user has made a selection
    virtual HRESULT DisplayThreatUI(AVModule::IAVMapStrData* pEmailInfo, AVModule::IAVMapStrData* pReturnData) throw() = 0;
};
typedef CSymPtr<IEmailScanUI> IEmailScanUIPtr;
typedef CSymQIPtr<IEmailScanUI, &IID_EmailScanUI> IEmailScanUIQIPtr;

///////////////////////////////////////
// IScanUIMisc interface:
class IScanUIMisc : public ISymBase
{
public:
    // Init...
    virtual HRESULT Initialize() throw() = 0;

    // DisplayVIDDetails will display the threat matrix property sheet for a given VID
    virtual HRESULT DisplayVIDDetails(HWND hParentWnd, DWORD dwVID) throw() = 0;

    // DisplayThreatDetails will display the threat matrix property sheet and the details tab for a given threat ID
    virtual HRESULT DisplayThreatDetails(HWND hParentWnd, const GUID& idThreat) throw() = 0;

    // ProcessSingleThreat display a dialog and allow the user to choose an action for the specified threat,
    virtual HRESULT ProcessSingleThreat(HWND hParentWnd, const GUID& idThreat) throw() = 0;

    // QuarantineAdd will prompt the user to select an item, then it will add the user selected
    // item to quarantine and return the items new threat id in idThreat
    virtual HRESULT QuarantineAdd(HWND hParentWnd, GUID& idThreat) throw() = 0;

    // QuarantineRestore will restore the items the threat identified with idThreat to their original locations,
    // if an item fails to restore, the user will be prompted for an alternate location to restore the item to.
    virtual HRESULT QuarantineRestore(HWND hParentWnd, const GUID& idThreat) throw() = 0;

    // Prompt the user to reboot
    virtual eRebootPromptResult QueryUserForReboot(HWND hParentWnd) throw() = 0;

    // Prompt the user for a procterm operation
    virtual eProcTermResult QueryUserForProcTerm(HWND hParentWnd) throw() = 0;
};
typedef CSymPtr<IScanUIMisc> IScanUIMiscPtr;
typedef CSymQIPtr<IScanUIMisc, &IID_ScanUIMisc> IScanUIMiscQIPtr;

///////////////////////////////////////
// ISingleInstance interface:
class ISingleInstance : public ISymBase
{
public:
    virtual HRESULT RegisterSingleInstance(LPCSTR lpName, bool &bAlreadyExists, DWORD &dwOwnerProcess, DWORD &dwOwnerThread) = 0;
    virtual HRESULT AlreadyExists(LPCSTR lpName, bool &bAlreadyExists, DWORD &dwOwnerProcess, DWORD &dwOwnerThread) = 0;
};
typedef CSymPtr<ISingleInstance> ISingleInstancePtr;
typedef CSymQIPtr<ISingleInstance, &IID_SingleInstance> ISingleInstanceQIPtr;

};	// namespace
