#pragma once
#include "stdafx.h"

// An LDVP UI Snap-In
// Note that this control requires WS_EX_CONTROLPARENT and doesn't seem to inherit the
// setting from the LDVP dialog backing it, so it must be set at runtime after OnInitDialog
// has created the control.  Also, this style must be set on all windows from here to the
// top-level window or this dialog will freeze in cases (e.g., end of tab order).
class CLdvpSnapinCtrl : public CWnd
{
protected:
    DECLARE_DYNCREATE(CLdvpSnapinCtrl)
    
public:
    typedef enum {SavSnapin_Filesystem, SavSnapin_Exchange, SavSnapin_Notes, SavSnapin_InternetEmail} SavSnapin;
    
    CLdvpSnapinCtrl();
    virtual ~CLdvpSnapinCtrl();
    
    void SetSavSnapinID( SavSnapin snapinID );
    SavSnapin GetSnapinID( void );

	CLSID const& GetClsid();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL);

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL);
    virtual void PreSubclassWindow( );

	signed char Load();
	signed char Store();
	long GetDlgHeight();
	void AboutBox();

    // Properties
    long GetMode();
    void SetMode( long newMode );
    long GetType();
    void SetType( long newType );
    long GetConfiguration();
    void SetConfiguration( long newConfig );
    LPUNKNOWN GetConfigInterface();
    void SetConfigInterface( LPUNKNOWN newConfigObject );

    DECLARE_MESSAGE_MAP()
private:
    // ** CONSTANTS **
    static const CLSID FilesystemApSnapinCLSID;
    static const CLSID ExchangeApSnapinCLSID;
    static const CLSID NotesApSnapinCLSID;
    static const CLSID InternetMailApSnapinCLSID;
    
    // ** FUNCTION MEMBERS **
    HRESULT CreationSetup( void );

    // ** DATA MEMBERS **
    SavSnapin           snapinID;
    IGenericConfig*     configObject;
    LPCTSTR             configPath;
};
