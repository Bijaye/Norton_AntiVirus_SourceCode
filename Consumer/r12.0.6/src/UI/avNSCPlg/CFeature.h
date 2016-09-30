// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------

#include <map>

class CFeature : public ISymBaseImpl<CSymThreadSafeRefCount>,
        public nsc::IFeature, public nsc::IWrapper
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(nsc::IID_nscIFeature,nsc::IFeature)
        SYM_INTERFACE_ENTRY(nsc::IID_nscIWrapper,nsc::IWrapper)
    SYM_INTERFACE_MAP_END()

    // IWrapper 
    virtual nsc::NSCRESULT GetName(const nsc::IContext* context_in, 
                                                const cc::IString*& result_out) 
                                                const throw();
        
    virtual nsc::NSCRESULT GetGUID(const nsc::IContext* context_in, 
                                                GUID& guid_out) 
                                                const throw();


    virtual nsc::NSCRESULT GetDescription(const nsc::IContext* context_in, 
                                                const cc::IString*& description_out)
                                                const throw();

    
    virtual nsc::NSCRESULT GetHelpURI(              const nsc::IContext* context_in, 
                                                const cc::IString*& uri_out) 
                                                const throw();


    virtual nsc::NSCRESULT GetMonitorsCollection(   const nsc::IContext* context_in,
                                                const nsc::IMonitorsCollection*& monitorCollection_out)
                                                const throw();
                                                        

    virtual nsc::NSCRESULT GetPropertyBag(          const nsc::IContext* context_in, 
                                                const nsc::IPropertyBag*&  propertyBag_out)
                                                const throw();

    virtual nsc::NSCRESULT OnMarkedDirty() const throw();

    // IFeature
    virtual nsc::NSCRESULT GetStatus(   const nsc::IContext* context_in, 
                                    const nsc::IConsoleType*& status_out) 
                                    const throw();
    
    virtual nsc::NSCRESULT GetHealth(   const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw();

    virtual nsc::NSCRESULT GetActions(  const nsc::IContext* context_in, 
                                    const nsc::IActionsCollection*& actions_out)
                                    const throw();

    virtual nsc::NSCRESULT GetTTL(      const nsc::IContext* context_in, 
                                    int& ttl_out) 
                                    const throw();

protected:
    CFeature (DWORD dwNameID, GUID guid, DWORD dwDescriptionID, DWORD dwDescriptionIDNonAdmin, LPCTSTR sHelpURI) : 
        m_dwNameID(dwNameID), 
		m_guid(guid), 
		m_dwDescriptionID(dwDescriptionID), 
		m_dwDescriptionIDNonAdmin(dwDescriptionIDNonAdmin), 
		m_sHelpURI(sHelpURI)
		{};

	virtual ~CFeature();

    // Queries the status server for new status, if needed
    static bool getInternalStatus(const nsc::IContext* context_in, CEventData*& pReturnStatus);
    
    // Gets the local status data for the given session
    static CEventData* getLocalStatus (DWORD dwSessionID);

    DWORD       m_dwNameID;
    const GUID  m_guid;
    DWORD       m_dwDescriptionID;
	DWORD       m_dwDescriptionIDNonAdmin;
    LPCTSTR     m_sHelpURI;

    typedef std::map <DWORD /*session id*/, CEventData /*status data*/> mapSessionStatus;
    typedef mapSessionStatus::iterator iterSessionStatus;

    static mapSessionStatus        m_mapLocalStatus;
    static ccLib::CCriticalSection m_critStatus;
    
    // Is this the first time we've requested status?
    static bool                    m_bInit;  

private:
    // hide default methods
    CFeature(const CFeature&);
    CFeature& operator=(const CFeature&) throw();
};


//////////////////////////////////////////////////////////////////////////////
// Implemented NAV features

class CNAVLicenseFeature : public CFeature
{
public:
    CNAVLicenseFeature(): CFeature (
            IDS_PRODUCT_NAME_NAV, 
            CLSID_NAV_Feature_NAVLicense,
            IDS_PRODUCT_DESC_NAV,
			IDS_PRODUCT_DESC_NAV,
            URI_FEATURE_NAVLICENSE
        ) {};

	 virtual nsc::NSCRESULT GetName(const nsc::IContext* context_in, 
                                    const cc::IString*& result_out) 
                                    const throw();

	 virtual nsc::NSCRESULT GetHealth(const nsc::IContext* context_in, 
                                      const nsc::IHealth*& health_out) 
                                      const throw();

private:
    // hide default methods
    CNAVLicenseFeature(const CNAVLicenseFeature&);
    CNAVLicenseFeature& operator=(const CNAVLicenseFeature&) throw();
};

class CAutoProtectFeature : public CFeature
{
public:
    CAutoProtectFeature () : CFeature (
            IDS_FEATURE_NAME_AP, 
            CLSID_NAV_Feature_AutoProtect,
            IDS_FEATURE_DESC_AP,
			IDS_FEATURE_DESC_NONADMIN_AP,
            URI_FEATURE_AP
        ) {};

    virtual nsc::NSCRESULT GetStatus(   const nsc::IContext* context_in, 
                                    const nsc::IConsoleType*& status_out) 
                                    const throw();
    
    virtual nsc::NSCRESULT GetHealth(   const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw();

    virtual nsc::NSCRESULT GetActions(  const nsc::IContext* context_in, 
                                    const nsc::IActionsCollection*& actions_out)
                                    const throw();

private:
    // hide default methods
    CAutoProtectFeature(const CAutoProtectFeature&);
    CAutoProtectFeature& operator=(const CAutoProtectFeature&) throw();
};

class CEmailScanningFeature : public CFeature
{
public:
    CEmailScanningFeature () : CFeature (
            IDS_FEATURE_NAME_EMAIL_SCANNING, 
            CLSID_NAV_Feature_EmailScanning, 
            IDS_FEATURE_DESC_EMAIL_SCANNING,
			IDS_FEATURE_DESC_NONADMIN_EMAIL_SCANNING,
            URI_FEATURE_EMAIL
        ) {};

	virtual nsc::NSCRESULT GetStatus(   const nsc::IContext* context_in, 
                                    const nsc::IConsoleType*& status_out) 
                                    const throw();
    
    virtual nsc::NSCRESULT GetHealth(   const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw();


    virtual nsc::NSCRESULT GetActions(  const nsc::IContext* context_in, 
                                    const nsc::IActionsCollection*& actions_out)
                                    const throw();

private:
    // hide default methods
    CEmailScanningFeature(const CEmailScanningFeature&);
    CEmailScanningFeature& operator=(const CEmailScanningFeature&) throw();
};

class CSpywareProtectionFeature : public CFeature
{
public:
    CSpywareProtectionFeature () : CFeature (
            IDS_FEATURE_NAME_SPYWARE_PROTECTION, 
            CLSID_NAV_Feature_SpywareProtection,
            IDS_FEATURE_DESC_SPYWARE_PROTECTION,
			IDS_FEATURE_DESC_NONADMIN_SPYWARE_PROTECTION,
            URI_FEATURE_SPYWARE
        ) {};

	virtual nsc::NSCRESULT GetStatus(   const nsc::IContext* context_in, 
                                    const nsc::IConsoleType*& status_out) 
                                    const throw();
    
    virtual nsc::NSCRESULT GetHealth(   const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw();

    virtual nsc::NSCRESULT GetActions(  const nsc::IContext* context_in, 
                                    const nsc::IActionsCollection*& actions_out)
                                    const throw();

private:
    // hide default methods
    CSpywareProtectionFeature(const CSpywareProtectionFeature&);
    CSpywareProtectionFeature& operator=(const CSpywareProtectionFeature&) throw();
};

class CIFPFeature : public CFeature
{
public:
    CIFPFeature () : CFeature (
            IDS_FEATURE_NAME_IFP, 
            CLSID_NAV_Feature_IFP,
            IDS_FEATURE_DESC_IFP,
			IDS_FEATURE_DESC_NONADMIN_IFP,
            URI_FEATURE_INBOUND_FIREWALL
        ) {};

    virtual nsc::NSCRESULT GetStatus(   const nsc::IContext* context_in, 
                                    const nsc::IConsoleType*& status_out) 
                                    const throw();
    
    virtual nsc::NSCRESULT GetHealth(   const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw();

    virtual nsc::NSCRESULT GetActions(  const nsc::IContext* context_in, 
                                    const nsc::IActionsCollection*& actions_out)
                                    const throw();

private:
    // hide default methods
    CIFPFeature(const CIFPFeature&);
    CIFPFeature& operator=(const CIFPFeature&) throw();
};

class CIMScanningFeature : public CFeature
{
public:
    CIMScanningFeature () : CFeature (
            IDS_FEATURE_NAME_IM_SCANNING, 
            CLSID_NAV_Feature_IMScanning,
            IDS_FEATURE_DESC_IM_SCANNING,
			IDS_FEATURE_DESC_NONADMIN_IM_SCANNING,
            URI_FEATURE_IM_SCANNING
        ) {};

    virtual nsc::NSCRESULT GetStatus(   const nsc::IContext* context_in, 
                                    const nsc::IConsoleType*& status_out) 
                                    const throw();
    
    virtual nsc::NSCRESULT GetHealth(   const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw();

    virtual nsc::NSCRESULT GetActions(  const nsc::IContext* context_in, 
                                    const nsc::IActionsCollection*& actions_out)
                                    const throw();

private:
    // hide default methods
    CIMScanningFeature(const CIMScanningFeature&);
    CIMScanningFeature& operator=(const CIMScanningFeature&) throw();
};

class CWormBlockingFeature : public CFeature
{
public:
    CWormBlockingFeature () : CFeature (
            IDS_FEATURE_NAME_WORM_BLOCKING, 
            CLSID_NAV_Feature_WormBlocking,
            IDS_FEATURE_DESC_WORM_BLOCKING,
			IDS_FEATURE_DESC_NONADMIN_WORM_BLOCKING,
            URI_FEATURE_WORM_BLOCKING
        ) {};

    virtual nsc::NSCRESULT GetStatus(   const nsc::IContext* context_in, 
                                    const nsc::IConsoleType*& status_out) 
                                    const throw();
    
    virtual nsc::NSCRESULT GetHealth(   const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw();

    virtual nsc::NSCRESULT GetActions(  const nsc::IContext* context_in, 
                                    const nsc::IActionsCollection*& actions_out)
                                    const throw();

private:
    // hide default methods
    CWormBlockingFeature(const CWormBlockingFeature&);
    CWormBlockingFeature& operator=(const CWormBlockingFeature&) throw();
};

class CFSSFeature : public CFeature
{
public:
    CFSSFeature () : CFeature (
            IDS_FEATURE_NAME_FSS, 
            CLSID_NAV_Feature_FSS,
            IDS_FEATURE_DESC_FSS,
			IDS_FEATURE_DESC_NONADMIN_FSS,
            URI_FEATURE_FULL_SYSTEM_SCAN
        ) {};

    virtual nsc::NSCRESULT GetStatus(   const nsc::IContext* context_in, 
                                    const nsc::IConsoleType*& status_out) 
                                    const throw();
    
    virtual nsc::NSCRESULT GetHealth(   const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw();

    virtual nsc::NSCRESULT GetActions(  const nsc::IContext* context_in, 
                                    const nsc::IActionsCollection*& actions_out)
                                    const throw();

private:
    // hide default methods
    CFSSFeature(const CFSSFeature&);
    CFSSFeature& operator=(const CFSSFeature&) throw();
};


class CVirusDefsFeature : public CFeature
{
public:
    CVirusDefsFeature () : CFeature (
            IDS_FEATURE_NAME_VIRUSDEFS, 
            CLSID_NAV_Feature_VirusDefs,
            IDS_FEATURE_DESC_VIRUSDEFS,
			IDS_FEATURE_DESC_NONADMIN_VIRUSDEFS,
            URI_FEATURE_VIRUS_DEFS
        ) {};

    virtual nsc::NSCRESULT GetStatus(   const nsc::IContext* context_in, 
                                    const nsc::IConsoleType*& status_out) 
                                    const throw();
    
    virtual nsc::NSCRESULT GetHealth(   const nsc::IContext* context_in, 
                                    const nsc::IHealth*& health_out) 
                                    const throw();

    virtual nsc::NSCRESULT GetActions(  const nsc::IContext* context_in, 
                                    const nsc::IActionsCollection*& actions_out)
                                    const throw();

private:
    // hide default methods
    CVirusDefsFeature(const CVirusDefsFeature&);
    CVirusDefsFeature& operator=(const CVirusDefsFeature&) throw();
};


class CScanNowFeature : public CFeature
{
public:
    CScanNowFeature () : CFeature (
            IDS_FEATURE_NAME_SCAN_NOW, 
            CLSID_NAV_Feature_Scan_Now,
            IDS_FEATURE_DESC_SCAN_NOW,
			IDS_FEATURE_DESC_NONADMIN_SCAN_NOW,
            URI_FEATURE_SCAN_NOW
        ) {};

    virtual nsc::NSCRESULT GetActions(  const nsc::IContext* context_in, 
                                    const nsc::IActionsCollection*& actions_out)
                                    const throw();

private:
    // hide default methods
    CScanNowFeature(const CScanNowFeature&);
    CScanNowFeature& operator=(const CScanNowFeature&) throw();
};

class CQuarantineFeature : public CFeature
{
public:
    CQuarantineFeature () : CFeature (
            IDS_FEATURE_NAME_QUARANTINE, 
            CLSID_NAV_Feature_Quarantine,
            IDS_FEATURE_DESC_QUARANTINE,
			IDS_FEATURE_DESC_NONADMIN_QUARANTINE,
            URI_FEATURE_QUARANTINE
        ) {};

    virtual nsc::NSCRESULT GetActions(  const nsc::IContext* context_in, 
                                    const nsc::IActionsCollection*& actions_out)
                                    const throw();

private:
    // hide default methods
    CQuarantineFeature(const CQuarantineFeature&);
    CQuarantineFeature& operator=(const CQuarantineFeature&) throw();
};
