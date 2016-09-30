/////////////////////////////////////////////////////////////////////////////
// IVCMSG.H
//
// Include file for interface declaration for the VCOBJ module

#pragma once

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#include <windows.h>

#define VCM_PRIORITY_LOW				0
#define VCM_PRIORITY_NORMAL				1
#define VCM_PRIORITY_HIGH				2

#define FACILITY_VENICE					25
#define FACILITY_LIVEREG				26
#define FACILITY_ADVISOR				27

#define VENICE_E_NORESOURCEDLL			MAKE_HRESULT( SEVERITY_ERROR, FACILITY_VENICE, 1 )
#define VCMSG_E_BAD_QUEUE_PATH			MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ADVISOR, 1 )

#define VCMF_WILLEXPIRESSOON			0x00000002
#define VCMF_HASEXPIRED					0x00000004

/////////////////////////////////////////////////////////////////////////////
// Class ID's

// {AA874A20-EB80-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( CLSID_VcAllMessagesFilter, 
0xaa874a20, 0xeb80, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8 );

// {AA874A21-EB80-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( CLSID_VcUnreadMessagesFilter, 
0xaa874a21, 0xeb80, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8 );

// {AA874A22-EB80-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( CLSID_VcTipsAndTricksFilter, 
0xaa874a22, 0xeb80, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8 );

// {AA874A23-EB80-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( CLSID_VcServiceAndSupportFilter, 
0xaa874a23, 0xeb80, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8 );

// {AA874A24-EB80-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( CLSID_VcProductUpgradeFilter, 
0xaa874a24, 0xeb80, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8 );

// {AA874A25-EB80-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( CLSID_VcProductUpdateFilter, 
0xaa874a25, 0xeb80, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8 );

// {AA874A26-EB80-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( CLSID_VcMessage, 
0xaa874a26, 0xeb80, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8 );
/*
// {AA874A27-EB80-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( CLSID_C, 
0xaa874a27, 0xeb80, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8 );

// {AA874A28-EB80-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( CLSID_C, 
0xaa874a28, 0xeb80, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8 );

// {AA874A29-EB80-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( CLSID_C, 
0xaa874a29, 0xeb80, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8 );
*/

/////////////////////////////////////////////////////////////////////////////
// Interface Declarations

// {9B850400-E2A6-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( IID_IVcMsgFilter, 
0x9b850400, 0xe2a6, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8);

// {9B850401-E2A6-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( IID_IVcMsgHandler, 
0x9b850401, 0xe2a6, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8);

// {9B850402-E2A6-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( IID_IVcMsg, 
0x9b850402, 0xe2a6, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8);
/*
// {9B850403-E2A6-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( IID_IVc, 
0x9b850403, 0xe2a6, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8);

// {9B850404-E2A6-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( IID_IVc, 
0x9b850404, 0xe2a6, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8);

// {9B850405-E2A6-11d2-9E74-0010AD28CFA8}
DEFINE_GUID( IID_IVc, 
0x9b850405, 0xe2a6, 0x11d2, 0x9e, 0x74, 0x0, 0x10, 0xad, 0x28, 0xcf, 0xa8);
*/
///////////////////////////////////////
// IVcMsgFilter

DECLARE_INTERFACE_( IVcMsgFilter, IUnknown )
{
	// IUnknown -------------------------------------------------------------
	STDMETHODIMP QueryInterface( REFIID riid, LPVOID FAR* ppvObj ) PURE;
	STDMETHODIMP_(ULONG) AddRef() PURE;
	STDMETHODIMP_(ULONG) Release() PURE;

	// IVcMsgFilter ---------------------------------------------------------
	STDMETHOD( SetMessageQueuePath )( LPCTSTR pszFolder ) PURE;
	STDMETHOD( GetName )( LPTSTR pszName, UINT cchBuf, HICON* lphIcon ) PURE;
	STDMETHOD( GetDescription )( LPTSTR pszDescription, UINT cchBuf ) PURE;
	STDMETHOD( GetServiceName )( LPTSTR pszName, UINT cchBuf ) PURE;
	STDMETHOD( SetupListCtrl )( HWND hListCtrl ) PURE;
	STDMETHOD( PopulateListCtrl )( HWND hListCtrl ) PURE;
	STDMETHOD( OnNewMessage )( HWND hListCtrl, LPCTSTR pszFileName ) PURE;
	STDMETHOD( OnRefresh )( HWND hListCtrl ) PURE;
	STDMETHOD( GetMessagePath )( int nItem ) PURE;
};

///////////////////////////////////////
// IVcMsgService

DECLARE_INTERFACE_( IVcMsgService, IVcMsgFilter )
{
	// IVcMsgService --------------------------------------------------------
	STDMETHOD( GetDescription )( LPTSTR pszDescription, UINT cchName ) PURE;
	STDMETHOD( IsSubscribed )( void ) PURE;
	STDMETHOD( OnSubscribe )( BOOL bSubscribe ) PURE;
};

///////////////////////////////////////
// IVcMsgHandler

DECLARE_INTERFACE_( IVcMsgHandler, IUnknown )
{
	// IUnknown -------------------------------------------------------------
	STDMETHODIMP QueryInterface( REFIID riid, LPVOID FAR* ppvObj ) PURE;
	STDMETHODIMP_(ULONG) AddRef() PURE;
	STDMETHODIMP_(ULONG) Release() PURE;

	// IVcMsgHandler --------------------------------------------------------
	STDMETHOD( SetMessagePath )( LPCTSTR pszFileName ) PURE;
	STDMETHOD( OnOpen )( HWND hwndParent ) PURE;
	STDMETHOD( OnPrint )( HWND hwndParent ) PURE;
	STDMETHOD( MarkAsRead )( BOOL bRead ) PURE;
};

///////////////////////////////////////
// IVcMsg

DECLARE_INTERFACE_( IVcMsg, IUnknown )
{
	// IUnknown -------------------------------------------------------------
	STDMETHODIMP QueryInterface( REFIID riid, LPVOID FAR* ppvObj ) PURE;
	STDMETHODIMP_(ULONG) AddRef() PURE;
	STDMETHODIMP_(ULONG) Release() PURE;

	// IVcMsg ---------------------------------------------------------------
	STDMETHOD( Load )( LPCTSTR pszFileName, BOOL bDefer ) PURE;
	STDMETHOD( Save )( void ) PURE;
	STDMETHOD( Print )( PRINTDLG* pPrintDlg ) PURE;
	STDMETHOD( SaveAsText )( LPCTSTR pszFileName ) PURE;

	STDMETHOD( IsRead )( void ) PURE;
	STDMETHOD( MarkRead )( BOOL bRead ) PURE;
	STDMETHOD( IsFlagged )( void ) PURE;
	STDMETHOD( FlagMessage )( BOOL bFlag ) PURE;
	STDMETHOD( GetFlags )( LPDWORD pdwFlags ) PURE;

	STDMETHOD( IsEqual )( IVcMsg* pMsg ) PURE;
	STDMETHOD( Compare )( IVcMsg* pMsg, UINT uCol, LPINT pnResult ) PURE;
	STDMETHOD( GetDisplayInfo )( NMLVDISPINFO* pDispInfo ) PURE;
	STDMETHOD( PopulateLinkList )( HWND hListCtrl ) PURE;
	STDMETHOD( PopupLinkMenu )( HWND hwndParent, LPPOINT pPoint, UINT uFlags ) PURE;
	STDMETHOD( ActivateLink )( HWND hwndParent, LPARAM lParam ) PURE;

	STDMETHOD( GetID )( UINT* puID ) PURE;
	STDMETHOD( GetPriority )( UINT* puPriority ) PURE;
	STDMETHOD( GetSubject )( LPCTSTR* ppszText ) PURE;
	STDMETHOD( GetBodyText )( LPCTSTR* ppszText ) PURE;
	STDMETHOD( GetTimeReceived )( LPSYSTEMTIME pSystemTime ) PURE;
	STDMETHOD( GetFileName )( LPTSTR pszText, UINT cchText ) PURE;
};
