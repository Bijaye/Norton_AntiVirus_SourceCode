/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

/*
    ScopeExtData.cpp

    Extend the scope menu when running as an extension to Q Console.

    Written by: Jim Hill
*/


#include "stdafx.h"             
#include "resource.h"
#include <atlsnap.h>
#include "AvisConsole.h"
#include "ScopeExtData.h"   
  


static const GUID CScopeExtDataGUID_NODETYPE = 
{ 0x26140c87, 0xb554, 0x11d2, { 0x8f, 0x45, 0x30, 0x78, 0x30, 0x2c, 0x20, 0x30 } };
const GUID*    CScopeExtData::m_NODETYPE = &CScopeExtDataGUID_NODETYPE;
const OLECHAR* CScopeExtData::m_SZNODETYPE = OLESTR("26140C87-B554-11D2-8F45-3078302C2030");
const OLECHAR* CScopeExtData::m_SZDISPLAY_NAME = OLESTR("Symantec AntiVirus Avis Console");
const CLSID*   CScopeExtData::m_SNAPIN_CLASSID  = &CLSID_AvisCon;


int g_ScopeExtDataClassTotalCount = 0;


HRESULT CScopeExtData::CreatePropertyPages(LPPROPERTYSHEETCALLBACK lpProvider,
    long handle, 
	IUnknown* pUnk,
	DATA_OBJECT_TYPES type)
{
    HRESULT hr = S_OK;
    TCHAR   szServerName[MAX_PATH + 10];

    AFX_MANAGE_STATE(AfxGetStaticModuleState());	
    memset( szServerName, 0, sizeof(szServerName));

    fWidePrintString("CScopeExtData::CreatePropertyPages  called. ObjectID= %d TotalCount= %d", 
                                       m_iExtDataObjectID, g_ScopeExtDataClassTotalCount );


    // IF ICEPACK NOT INSTALLED, THEN DON'T EXTEND ANYTHING 1/1/00
    if( !IsAvisIcePackSupported(m_iExtDataObjectID) )
		return S_OK;


    // add pages
    try
    {
 		fWidePrintString(" CServerConfigData   New  size=%d 'AvisServer Config Data' ObjectID= %d",sizeof(CServerConfigData), m_iExtDataObjectID );
        // ADD NEW CServerConfigData  Object   CServerConfigData*
        m_pSConfigData = new CServerConfigData;
 		if(m_pSConfigData==NULL) 
            return E_OUTOFMEMORY;
 
        m_pSConfigData->m_pSnapin = this;
        hr = m_pSConfigData->Initialize( m_pDataObject, szServerName, m_iExtDataObjectID );
        if( FAILED( hr ) )
        {
            m_pSConfigData = NULL;
            m_pSConfigData->~CServerConfigData();
            return(hr);
        }
        
        // READ IN ALL CONFIG DATA FROM SERVER
        hr = m_pSConfigData->ReadConfigDataFromServer();
        if( FAILED(hr) )    
        {
            m_pSConfigData = NULL;
            m_pSConfigData->~CServerConfigData();
            return(hr);
        }
        
        //--------------------------------------------------------------
		fWidePrintString("ScopeExtData  CAvisServerConfig2::New  size=%d 'AVIS Web Communications' Prop page ObjectID= %d",sizeof(CAvisServerConfig2), m_iExtDataObjectID );
		//  "AVIS Web Communications"
		m_pAvisServerConfig2 = new CAvisServerConfig2;
		if(m_pAvisServerConfig2==NULL) 
            return E_OUTOFMEMORY;
		
        // Save off notification handle.
        m_pAvisServerConfig2->m_pSnapin = this;
        // SAVE OFF TO USE WITH HOOK CALLBACK. ONLY THIS PAGE 
        m_pAvisServerConfig2->m_lParam = (LPARAM) this;  // THIS TELLS THE NEW CALLBACK FUNCTION, WHICH PAGE
        m_pAvisServerConfig2->Initialize( m_pSConfigData, m_iExtDataObjectID );
        
        // Add to MMC property sheet.
        m_pAvisServerConfig2->m_psp.dwFlags |= PSP_HASHELP;  // ADD HELP BUTTON TO PROPSHEET
        MMCPropPageCallback( &m_pAvisServerConfig2->m_psp );
        // HOOK THE CALLBACK FOR THIS PAGE ONLY TO DETECT WHEN THE PROP SHEET IS CLOSING
        m_pAvisServerConfig2->HookPropPageCallback();        
        HPROPSHEETPAGE hp2 = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pAvisServerConfig2->m_psp );
        hr = lpProvider->AddPage( hp2 );
        ++m_bPropPage;

//        //--------------------------------------------------------------
//		  fWidePrintString("ScopeExtData  CAServerFirewallConfig2::New  size=%d 'AVIS Firewall' Prop page ObjectID= %d",sizeof(CCustomerInfo), m_iExtDataObjectID );
//        //  "AVIS Firewall"  8/23/00  jhill
        m_pAServerFirewallConfig2 = new  CAServerFirewallConfig2;
		if(m_pAServerFirewallConfig2==NULL) 
            return E_OUTOFMEMORY;
		
        // Save off notification handle.
        m_pAServerFirewallConfig2->m_pSnapin = this;
        m_pAServerFirewallConfig2->Initialize( m_pSConfigData, m_iExtDataObjectID );

        // Add to MMC property sheet.
        m_pAServerFirewallConfig2->m_psp.dwFlags |= PSP_HASHELP;  // ADD HELP BUTTON TO PROPSHEET
        MMCPropPageCallback( &m_pAServerFirewallConfig2->m_psp );
        HPROPSHEETPAGE hp5 = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pAServerFirewallConfig2->m_psp );
        hr = lpProvider->AddPage( hp5 );
        ++m_bPropPage;
    
     
        //--------------------------------------------------------------
		fWidePrintString("ScopeExtData  CAvisServerConfig::New  size=%d 'Sample Policy' Prop page ObjectID= %d",sizeof(CAvisServerConfig), m_iExtDataObjectID );
		//  "AVIS Policies"
		m_pAvisServerConfig = new CAvisServerConfig;
		if(m_pAvisServerConfig==NULL) 
            return E_OUTOFMEMORY;
		
        // Save off notification handle.
        m_pAvisServerConfig->m_pSnapin = this;
        m_pAvisServerConfig->Initialize( m_pSConfigData, m_iExtDataObjectID );
		
        // Add to MMC property sheet.
        m_pAvisServerConfig->m_psp.dwFlags |= PSP_HASHELP;  // ADD HELP BUTTON TO PROPSHEET
        MMCPropPageCallback( &m_pAvisServerConfig->m_psp );
        HPROPSHEETPAGE hp = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pAvisServerConfig->m_psp );
        hr = lpProvider->AddPage( hp );
        ++m_bPropPage;

        //--------------------------------------------------------------
		fWidePrintString("ScopeExtData  CAvisServerDefPolicyConfig::New  size=%d 'Avis Definition Policies' Prop page ObjectID= %d",sizeof(CAvisServerConfig), m_iExtDataObjectID );
		//  "AVIS DEF Policies"   12/11/99
		m_pAvisServerDefPolicyConfig = new CAvisServerDefPolicyConfig;
		if(m_pAvisServerDefPolicyConfig==NULL) 
            return E_OUTOFMEMORY;
		
        // Save off notification handle.
        m_pAvisServerDefPolicyConfig->m_pSnapin = this;
        m_pAvisServerDefPolicyConfig->Initialize( m_pSConfigData, m_iExtDataObjectID );
		
        // Add to MMC property sheet.
        m_pAvisServerDefPolicyConfig->m_psp.dwFlags |= PSP_HASHELP;  // ADD HELP BUTTON TO PROPSHEET
        MMCPropPageCallback( &m_pAvisServerDefPolicyConfig->m_psp );
        HPROPSHEETPAGE hp4 = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pAvisServerDefPolicyConfig->m_psp );
        hr = lpProvider->AddPage( hp4 );
        ++m_bPropPage;


        //--------------------------------------------------------------
		//  "Install Definitions"   12/16/99
		m_pInstallDefinitions = new CInstallDefinitions;
		if(m_pInstallDefinitions==NULL) 
            return E_OUTOFMEMORY;
		
        // Save off notification handle.
        m_pInstallDefinitions->m_pSnapin = this;
        m_pInstallDefinitions->Initialize( m_pSConfigData, m_iExtDataObjectID );
		
        // Add to MMC property sheet.
        m_pInstallDefinitions->m_psp.dwFlags |= PSP_HASHELP;  // ADD HELP BUTTON TO PROPSHEET
        MMCPropPageCallback( &m_pInstallDefinitions->m_psp );
        HPROPSHEETPAGE hp8 = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pInstallDefinitions->m_psp );
        hr = lpProvider->AddPage( hp8 );
        ++m_bPropPage;


        //--------------------------------------------------------------
		fWidePrintString("ScopeExtData  CCustomerInfo::New  size=%d 'AVIS Customer Info' Prop page ObjectID= %d",sizeof(CCustomerInfo), m_iExtDataObjectID );
        //  "AVIS Customer Info"
        m_pAvisCustomerInfo = new  CCustomerInfo;
		if(m_pAvisCustomerInfo==NULL) 
            return E_OUTOFMEMORY;
		
        // Save off notification handle.
        m_pAvisCustomerInfo->m_pSnapin = this;
        m_pAvisCustomerInfo->Initialize( m_pSConfigData, m_iExtDataObjectID );

        // Add to MMC property sheet.
        m_pAvisCustomerInfo->m_psp.dwFlags |= PSP_HASHELP;  // ADD HELP BUTTON TO PROPSHEET
        MMCPropPageCallback( &m_pAvisCustomerInfo->m_psp );
        HPROPSHEETPAGE hp3 = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pAvisCustomerInfo->m_psp );
        hr = lpProvider->AddPage( hp3 );
        ++m_bPropPage;


        //--------------------------------------------------------------
		fWidePrintString("ScopeExtData  CAlertingSetConfig::New  size=%d 'Alerting' Prop page ObjectID= %d",sizeof(CAvisServerConfig), m_iExtDataObjectID );
		//  "Alerting"   12/11/99
        m_pAlertingSetConfig = new CAlertingSetConfig;
		if(m_pAlertingSetConfig==NULL) 
            return E_OUTOFMEMORY;

        // Save off notification handle.
        m_pAlertingSetConfig->m_pSnapin = this;
        m_pAlertingSetConfig->Initialize( m_pSConfigData, m_iExtDataObjectID );
		
        // Add to MMC property sheet.
        m_pAlertingSetConfig->m_psp.dwFlags |= PSP_HASHELP;  // ADD HELP BUTTON TO PROPSHEET
        MMCPropPageCallback( &m_pAlertingSetConfig->m_psp );
        HPROPSHEETPAGE hp9 = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pAlertingSetConfig->m_psp );
        hr = lpProvider->AddPage( hp9 );
        ++m_bPropPage;

        //--------------------------------------------------------------
		fWidePrintString("ScopeExtData  CServerGeneralError::New  size=%d 'Attention' Prop page ObjectID= %d",sizeof(CAvisServerConfig), m_iExtDataObjectID );
		//  "Attention"   12/11/99
		m_pServerGeneralError = new CServerGeneralError;
		if(m_pServerGeneralError==NULL) 
            return E_OUTOFMEMORY;
		
        // Save off notification handle.
        m_pServerGeneralError->m_pSnapin = this;
        m_pServerGeneralError->Initialize( m_pSConfigData, m_iExtDataObjectID );
		
        // Add to MMC property sheet.
        m_pServerGeneralError->m_psp.dwFlags |= PSP_HASHELP;  // ADD HELP BUTTON TO PROPSHEET
        MMCPropPageCallback( &m_pServerGeneralError->m_psp );
        HPROPSHEETPAGE hp7 = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pServerGeneralError->m_psp );
        hr = lpProvider->AddPage( hp7 );
        ++m_bPropPage;

        //--------------------------------------------------------------


        // NOW SAVE THE PAGE PTRS TO CAvisServerConfig PAGE
        m_pAvisServerConfig2->m_pAvisServerConfig2         = m_pAvisServerConfig2;
        m_pAvisServerConfig2->m_pAServerFirewallConfig2    = m_pAServerFirewallConfig2;
        m_pAvisServerConfig2->m_pAvisServerConfig          = m_pAvisServerConfig; 
        m_pAvisServerConfig2->m_pAvisServerDefPolicyConfig = m_pAvisServerDefPolicyConfig; 
        m_pAvisServerConfig2->m_pInstallDefinitions        = m_pInstallDefinitions;
        m_pAvisServerConfig2->m_pAvisCustomerInfo          = m_pAvisCustomerInfo; 
        m_pAvisServerConfig2->m_pServerGeneralError        = m_pServerGeneralError;
        m_pAvisServerConfig2->m_pAlertingSetConfig         = m_pAlertingSetConfig;


    }
    catch( _com_error e )
    {
        hr = e.Error();
    }
    catch(...)
    {
        hr = E_UNEXPECTED;
    }
	return hr;     // E_UNEXPECTED
}










