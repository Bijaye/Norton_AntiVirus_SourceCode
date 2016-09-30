// SavAutoprotectNotes.cpp : Implementation of CSavAutoprotectNotes
#include "stdafx.h"
#include "SavAutoprotectNotes.h"
#include "NotesApPage.h"


// CSavAutoprotectNotes
// ** STATIC DATA INITIALIZATION
CSavEmailAutoprotectImpl::Config CSavAutoprotectNotes::config = 
{
    CLSID_SavAutoprotectExchange,
    IDS_AP_NOTES_DISPLAYNAME,
    IDS_AP_NOTES_DESCRIPTION,
    _T("\\notesext.dll"),
    _T(szReg_Key_Storage_Notes)
};
 

// CSavAutoprotectNotes
HRESULT CSavAutoprotectNotes::FinalConstruct()
{
    apImplementation.FinalConstruct(config);
    return S_OK;
}

void CSavAutoprotectNotes::FinalRelease()
{
    apImplementation.FinalRelease();
}

// IPROTECTION_AUTOPROTECT
HRESULT CSavAutoprotectNotes::get_ID( GUID* id )
{
    return apImplementation.get_ID(id);
}

HRESULT CSavAutoprotectNotes::get_DisplayName( BSTR* shortName )
{
    return apImplementation.get_DisplayName(shortName);
}

HRESULT CSavAutoprotectNotes::get_Description( BSTR* userDescription )
{
    return apImplementation.get_Description(userDescription);
}

HRESULT CSavAutoprotectNotes::get_ProtectionStatus( ProtectionStatus* currStatus )
{
    return apImplementation.get_ProtectionStatus(currStatus);
}

HRESULT CSavAutoprotectNotes::get_ProtectionStatusDescription( BSTR* statusDescription )
{
    return apImplementation.get_ProtectionStatusDescription(statusDescription);
}

HRESULT CSavAutoprotectNotes::get_ProtectionStatusReadOnly( VARIANT_BOOL* isStatusReadOnly )
{
    return apImplementation.get_ProtectionStatusReadOnly(isStatusReadOnly);
}

HRESULT CSavAutoprotectNotes::get_ProtectionConfiguration( VARIANT_BOOL* protectionEnabled )
{
    return apImplementation.get_ProtectionConfiguration(protectionEnabled);
}

HRESULT CSavAutoprotectNotes::put_ProtectionConfiguration( VARIANT_BOOL newStatus )
{
    return apImplementation.put_ProtectionConfiguration(newStatus);
}

HRESULT CSavAutoprotectNotes::ShowConfigureUI( HWND parentWindowHandle )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CNotesApPage propertyPage;

    return apImplementation.ShowConfigureUI(&propertyPage, parentWindowHandle);
}

HRESULT CSavAutoprotectNotes::ShowLogUI( HWND parentWindowHandle )
{
    return apImplementation.ShowLogUI(NULL, parentWindowHandle);
}

// ** CLASS STATICS **
bool CSavAutoprotectNotes::IsStorageInstalled( void )
// Returns S_OK if Exchange protection is installed, S_FALSE if not
{
    return CSavEmailAutoprotectImpl::IsStorageInstalled(config.storageFilename, config.storageName);
}