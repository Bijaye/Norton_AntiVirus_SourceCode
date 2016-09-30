/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#ifndef __QSCONSOLE_H_
#define __QSCONSOLE_H_
#include "resource.h"
#include <atlsnap.h>
#include "afxcoll.h"
#include "itemdata.h"
#include "macros.h"
#include <ecomtype.h>
#include <eicore.h>
#include <eilegtyp.h>
#include <eitclist.h>
#include <eilegacy.h>
#include <ecomlodr.h>
#include <thretcat.h>
#include "avscanner.h"
#include "extend.h"
#include "qsconsoledata.h"
#include "qsconsolecomponent.h"
#include "dataobj.h"

///////////////////////////////////////////////////////////////////////////////////////




class CQSConsole : public CComObjectRootEx<CComSingleThreadModel>,
public CSnapInObjectRoot<1, CQSConsole>,
	public IComponentDataImpl<CQSConsole, CQSConsoleComponent>,
	public IExtendPropertySheetImpl<CQSConsole>,
	public IExtendContextMenuImpl<CQSConsole>,
	public IPersistStream,
    public ISnapinHelp,
	public CComCoClass<CQSConsole, &CLSID_QSConsole>
{
public:
	CQSConsole()
	{
		m_pNode = new CQSConsoleData;
        ((CQSConsoleData*)m_pNode)->m_pComponentData = this;
		_ASSERTE(m_pNode != NULL);
		m_pComponentData = this;
        m_bExtension = FALSE;
        m_bFirstExpand = TRUE;
        m_bDirty = FALSE;
        m_bExplicitAttachAsExtension = FALSE;       

        // 
        // Need to initialize our snapin nodes.
        // 
        SET_EXTENSION_NODE( CExtendComputerManagement, m_pNode );
        SET_EXTENSION_NODE( CExtendNSCStatic, m_pNode );

	}

	~CQSConsole()
	{
		delete m_pNode;
		m_pNode = NULL;

        POSITION pos = m_aNodes.GetHeadPosition();
        while( pos != NULL )
            {
            delete m_aNodes.GetNext( pos );
            }

	}

// 
// Need one of these entries for each type of node that we extend
// Also, we need an entry in the EXTENSION_SNAPIN_NODEINFO_MAP section
// for each node that we can extend.  
// 
EXTENSION_SNAPIN_DATACLASS(CExtendComputerManagement)
EXTENSION_SNAPIN_DATACLASS(CExtendNSCStatic)


BEGIN_EXTENSION_SNAPIN_NODEINFO_MAP(CQSConsole)
    EXTENSION_SNAPIN_NODEINFO_ENTRY(CExtendComputerManagement)
    EXTENSION_SNAPIN_NODEINFO_ENTRY(CExtendNSCStatic)
END_EXTENSION_SNAPIN_NODEINFO_MAP()


BEGIN_COM_MAP(CQSConsole)
	  COM_INTERFACE_ENTRY(IComponentData)
    COM_INTERFACE_ENTRY(IExtendPropertySheet)
    COM_INTERFACE_ENTRY(IExtendContextMenu)
	COM_INTERFACE_ENTRY(IPersistStream)
    COM_INTERFACE_ENTRY(ISnapinHelp)

END_COM_MAP()

DECLARE_REGISTRY_RESOURCEID(IDR_QSCONSOLE)

DECLARE_NOT_AGGREGATABLE(CQSConsole)


	STDMETHOD(GetClassID)(CLSID *pClassID)
	{
		ATLTRACENOTIMPL(_T("CQSConsoleComponent::GetClassID"));
	}	

	STDMETHOD(IsDirty)()
	{
        return m_bDirty ? S_OK : S_FALSE;
	}

	STDMETHOD(Load)(IStream *pStm)
	{
	    HRESULT hr = S_OK;

        // 
        // Don't load server information if we are an extension, since
        // the node we are extending will have this infomration for us.
        // 
        // Note: only for when name derived from its Primary
        // - attachment hasn't been done yet, and the extension
        // state hasn't been tested yet, so the following will
        // always succeed.
        // PAGBABI
        
		
//		if( m_bExtension && !IsAttachExplicit() )
//            return S_OK;

        try
            {
            // 
            // Load state information from stream.
            //
            CQSConsoleData* p = (CQSConsoleData*) m_pNode;
            COleStreamFile file( pStm );
            CArchive ar( &file, CArchive::load );
            CString s;
            ar >> m_bExplicitAttachAsExtension;
            
            // don't get names if we didn't explicitly attach before
            if( m_bExtension && !IsAttachExplicit() )
                return S_OK;

            ar >> s;
            p->SetServerName( s );
            ar >> s;
            p->SetDomainName( s );
            ar >> s;
            p->SetUserName( s );

            m_bDirty = FALSE;
            }
        catch(CArchiveException* e)
            {
			if (e)
				hr = S_OK;
            }
        catch(CFileException* e)
            {
			if (e)
	            hr = S_OK;
            }
        catch(CException* e)
            {
		if (e)
	        hr = E_UNEXPECTED;
            }


        return hr;//ATLTRACENOTIMPL(_T("CQSConsoleComponent::Load"));
	}

	STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty)
	{
	    HRESULT hr = S_OK;

        // 
        // Don't save server information if we are an extension, since
        // the node we are extending will have this infomration for us.
        // 
        if( m_bExtension && !IsAttachExplicit() )
            return S_OK;

        try
            {
            // 
            // Save state information to stream.
            // 
            COleStreamFile file( pStm );
            CArchive ar( &file, CArchive::store );

            ar << m_bExplicitAttachAsExtension;

            if (!m_bExtension)
                {
                CQSConsoleData* p = (CQSConsoleData*) m_pNode;
                ar << p->GetServerName();
                ar << p->GetDomainName();
                ar << p->GetUserName();
                }
            else
                {
                //
                // This portion will not actually work properly
                // for a list since on Load, the nodes haven't been
                // created yet.  To Load correctly, one must
                // deserialized into a separate list, then load up
                // the node objects one by one when they are created.
                //
                // Also, there isn't any code to serialize the 
                // nodes themselves, so at the moment, a list of
                // one is all that will work.
                // PAGBABI
                //
                POSITION pos = m_aNodes.GetHeadPosition();
                while( pos != NULL )
                    {
                    CQSConsoleData* p = (CQSConsoleData*) m_aNodes.GetNext( pos );
                    ar << p->GetServerName();
                    ar << p->GetDomainName();
	                ar << p->GetUserName();
                    }
                }

            if( fClearDirty )
                m_bDirty = FALSE;
            }
        catch(CFileException *e)
            {
			if (e)
            hr = S_OK;
            }
        catch(...)
            {
            hr = STG_E_CANTSAVE;
            }
 
        return hr;//ATLTRACENOTIMPL(_T("CQSConsoleComponent::Load"));
	}

    void SetDirty() { m_bDirty = TRUE; }

	STDMETHOD(GetSizeMax)(ULARGE_INTEGER *pcbSize)
	{
        pcbSize->QuadPart = 0;
    
        return S_OK;//ATLTRACENOTIMPL(_T("CQSConsoleComponent::GetSizeMax"));
	}

    STDMETHOD(QueryDataObject)(long cookie,
        DATA_OBJECT_TYPES type,
        LPDATAOBJECT  *ppDataObject);


    STDMETHOD(Notify)( 
        LPDATAOBJECT lpDataObject,
        MMC_NOTIFY_TYPE event,
        long arg,
        long param);

	STDMETHOD(Initialize)(LPUNKNOWN pUnknown);

    STDMETHOD(GetHelpTopic)(LPOLESTR* lpCHM );

	static void WINAPI ObjectMain(bool bStarting)
	{
		if (bStarting)
            {
			CSnapInItem::Init();
            CQSDataObject::Init();
            }
	}
    static HRESULT ExtractComputerName( LPDATAOBJECT lpDataObject, CString& sSvr );    
  
    CAVScanner* GetNAVAPI() { return &m_AVScanner; }
	BOOL IsExtension() { return m_bExtension; }
    IConsoleNameSpace* GetNameSpace() { return m_pNameSpace; } 

public:

    //
    // Did we explicit attach to server in extension mode?
    //
    BOOL            IsAttachExplicit() { return m_bExplicitAttachAsExtension; }
    void            SetAttachExplicit( BOOL bState ) 
                    { m_bExplicitAttachAsExtension = bState; }

private:
    // 
    // Extension specific stuff
    // 
    HRESULT CheckForExtensions( LPDATAOBJECT lpDataObject, long parentID );
    HRESULT OnExpand( LPDATAOBJECT lpDataObject,
                      long arg,
                      long param );


private:

    // 
    // Handle to NAVAPI
    // 
    CAVScanner      m_AVScanner;

    // 
    // Q: Are we an extension?
    // 
    BOOL            m_bExtension;
    BOOL            m_bFirstExpand;
    BOOL            m_bDirty;

    //
    // Did we explicit attach to server in extension mode?
    //
    BOOL            m_bExplicitAttachAsExtension;

    // 
    // Pointer to namespace
    // 
    CComPtr< IConsoleNameSpace2 > m_pNameSpace;

    // 
    // List of nodes we create.
    // 
    CTypedPtrList< CPtrList, CQSConsoleData* > m_aNodes;

};

class ATL_NO_VTABLE CQSConsoleAbout : public ISnapinAbout,
	public CComObjectRoot,
	public CComCoClass< CQSConsoleAbout, &CLSID_QSConsoleAbout>
{
public:
//	DECLARE_REGISTRY(CQSConsoleAbout, _T("QSConsoleAbout.1"), _T("QSConsoleAbout.1"), IDS_QSCONSOLE_DESC, THREADFLAGS_BOTH);
	DECLARE_REGISTRY_RESOURCEID(IDR_QSCONSOLEABOUT)

	BEGIN_COM_MAP(CQSConsoleAbout)
		COM_INTERFACE_ENTRY(ISnapinAbout)
	END_COM_MAP()

	STDMETHOD(GetSnapinDescription)(LPOLESTR *lpDescription)
	{
		USES_CONVERSION;
		TCHAR szBuf[256];
		if (::LoadString(_Module.GetResourceInstance(), IDS_QSCONSOLE_DESC, szBuf, 256) == 0)
			return E_FAIL;

		*lpDescription = (LPOLESTR)CoTaskMemAlloc((lstrlen(szBuf) + 1) * sizeof(OLECHAR));
		if (*lpDescription == NULL)
			return E_OUTOFMEMORY;

		ocscpy(*lpDescription, T2OLE(szBuf));

		return S_OK;
	}

	STDMETHOD(GetProvider)(LPOLESTR *lpName)
	{
		USES_CONVERSION;
		TCHAR szBuf[256];
		if (::LoadString(_Module.GetResourceInstance(), IDS_QSCONSOLE_PROVIDER, szBuf, 256) == 0)
			return E_FAIL;

		*lpName = (LPOLESTR)CoTaskMemAlloc((lstrlen(szBuf) + 1) * sizeof(OLECHAR));
		if (*lpName == NULL)
			return E_OUTOFMEMORY;

		ocscpy(*lpName, T2OLE(szBuf));

		return S_OK;
	}

	STDMETHOD(GetSnapinVersion)(LPOLESTR *lpVersion)
	{
		USES_CONVERSION;
		TCHAR szBuf[256];
		if (::LoadString(_Module.GetResourceInstance(), IDS_QSCONSOLE_VERSION, szBuf, 256) == 0)
			return E_FAIL;

		*lpVersion = (LPOLESTR)CoTaskMemAlloc((lstrlen(szBuf) + 1) * sizeof(OLECHAR));
		if (*lpVersion == NULL)
			return E_OUTOFMEMORY;

		ocscpy(*lpVersion, T2OLE(szBuf));

		return S_OK;
	}

	STDMETHOD(GetSnapinImage)(HICON *hAppIcon)
	{
		*hAppIcon = LoadIcon( _Module.GetResourceInstance(), MAKEINTRESOURCE( IDI_MAIN_ICON ) );
		return S_OK;
	}

	STDMETHOD(GetStaticFolderImage)(HBITMAP *hSmallImage,
		HBITMAP *hSmallImageOpen,
		HBITMAP *hLargeImage,
		COLORREF *cMask)
	{
		// 
        // Set up proper bitmaps
        // 
        *hSmallImageOpen = LoadBitmap( _Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_QSCONSOLE_16) );
        *hSmallImage = LoadBitmap( _Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_QSCONSOLE_16) );
        *hLargeImage = LoadBitmap( _Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_QSCONSOLE_32) );
		*cMask = RGB( 255, 0, 255 );
        return S_OK;
	}
};

#endif
