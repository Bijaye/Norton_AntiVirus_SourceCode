/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// 
// QSConsoleComponent.H
// 
#ifndef _QSConsoleComponent_H_
#define _QSConsoleComponent_H_

#include <atlsnap.h>
#include "columninfo.h"
#include "macros.h"





// 
// Forward declaration
// 
class CQSConsole;

class CQSConsoleComponent : public CComObjectRootEx<CComSingleThreadModel>,
	public CSnapInObjectRoot<2, CQSConsole >,
	public IExtendPropertySheetImpl<CQSConsoleComponent>,
	public IExtendContextMenuImpl<CQSConsoleComponent>,
	public IExtendControlbarImpl<CQSConsoleComponent>,
	public IPersistStream,
	public IComponentImpl<CQSConsoleComponent>,
    public IResultDataCompare
{
public:
BEGIN_COM_MAP(CQSConsoleComponent)
	COM_INTERFACE_ENTRY(IComponent)
    COM_INTERFACE_ENTRY(IExtendPropertySheet)
    COM_INTERFACE_ENTRY(IExtendContextMenu)
    COM_INTERFACE_ENTRY(IExtendControlbar)
	COM_INTERFACE_ENTRY(IPersistStream)
    COM_INTERFACE_ENTRY(IResultDataCompare)
END_COM_MAP()

public:
	CQSConsoleComponent()
	{
        m_bDirty = FALSE;
	}

	STDMETHOD(Notify)(LPDATAOBJECT lpDataObject, MMC_NOTIFY_TYPE event, long arg, long param);

	STDMETHOD(GetClassID)(CLSID *pClassID)
	{
		ATLTRACENOTIMPL(_T("CQSConsoleComponent::GetClassID"));
	}	

	STDMETHOD(IsDirty)()
	{
        CComPtr< IConsole > pc = m_spConsole;
        CComQIPtr<IHeaderCtrl, &IID_IHeaderCtrl> ph( pc );
        SaveColumnWidths( ph );
//		if (m_bDirty || 
        return m_bDirty ? S_OK : S_FALSE;
	}

	STDMETHOD(Load)(IStream *pStm)
	{
	    HRESULT hr = S_OK;
        try
            {
            // 
            // Load state information from stream.
            // 
            COleStreamFile file( pStm );
            CArchive ar( &file, CArchive::load );
            m_colInfo.Serialize( ar );
            }
        catch(...)
            {
            hr = E_UNEXPECTED;
            }
    
        return hr;//ATLTRACENOTIMPL(_T("CQSConsoleComponent::Load"));
	}

	STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty)
	{
	    HRESULT hr = S_OK;
        try
            {
            // 
            // Save state information to stream.
            // 
            COleStreamFile file( pStm );
            CArchive ar( &file, CArchive::store );
            m_colInfo.Serialize( ar );
            if( fClearDirty )
                m_bDirty = FALSE;
            }
        catch(...)
            {
            hr = STG_E_CANTSAVE ;
            }
    
        return hr;//ATLTRACENOTIMPL(_T("CQSConsoleComponent::Load"));
	}

	STDMETHOD(GetSizeMax)(ULARGE_INTEGER *pcbSize)
	{
        pcbSize->QuadPart = 0;
    
        return S_OK;//ATLTRACENOTIMPL(_T("CQSConsoleComponent::GetSizeMax"));
	}

    // 
    // Overrides for multiselection
    // 
    STDMETHOD(QueryDataObject)(long cookie,
                                DATA_OBJECT_TYPES type,
                                LPDATAOBJECT  *ppDataObject);
 
    STDMETHOD(CompareObjects)( LPDATAOBJECT lpDataObjectA,
        LPDATAOBJECT lpDataObjectB);

    STDMETHOD(AddMenuItems)(LPDATAOBJECT pDataObject,
        LPCONTEXTMENUCALLBACK piCallback,
        long *pInsertionAllowed);
    
    STDMETHOD(ControlbarNotify)(MMC_NOTIFY_TYPE event,
        long arg,
        long param);

    STDMETHOD(Command)(long lCommandID,
        LPDATAOBJECT pDataObject);

    void SetDirty() { m_bDirty = TRUE; }
    void SaveColumnWidths( IHeaderCtrl* pHeader )
        {
        // 
        // Save column widths
        // 
        for( int i = 0; i < m_colInfo.GetNoColumns(); i++ )
            {
            int iWidth = 0;
            if( SUCCEEDED( pHeader->GetColumnWidth( i, &iWidth ) ) )
                {
                // 
                // Check for change in column width
                // 
                if( (DWORD) iWidth != m_colInfo.m_aColumnWidths[i] )
                    {
                    SetDirty();
                    }

                m_colInfo.m_aColumnWidths[i] = iWidth;
                }
            }
        }

    // 
    // IResultDataCompare
    // 
    STDMETHOD(Compare)( long lUserParam,
                        long cookieA,
                        long cookieB,
                        int * pnResult );

    STDMETHOD(Initialize)(LPCONSOLE lpConsole);
    STDMETHOD(Destroy)(long cookie);

    HRESULT DoHelp( LPDATAOBJECT lpDataObject );

public:
    CMapPtrToPtr            m_dataMap;

    // 
    // Column with info.
    // 
    CColumnInfo     m_colInfo;

private:    
    // 
    // Dirty bit
    // 
    BOOL            m_bDirty;

	static DWORD	m_dwNoOpenWindows;

};


#endif