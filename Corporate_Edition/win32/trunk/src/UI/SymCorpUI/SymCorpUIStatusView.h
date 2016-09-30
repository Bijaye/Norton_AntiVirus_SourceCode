// CSymCorpUIStatusView class

#pragma once
#include "stdafx.h"
#include "ViewEx.h"
#include "StaticEx.h"
#include "StatusPTBlockDlg.h"
#include "ActionBox.h"


class CSymCorpUIStatusView : public CViewEx
{
// Message map functions
protected:
    afx_msg void OnActionFixButton();
    afx_msg void OnActionUpcomingButton();

// MFC
public:
    enum { IDD = IDD_VIEW_STATUS };
	virtual void OnDraw( CDC* pDC );
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual void OnInitialUpdate();
    virtual void OnUpdate( CView* sender, LPARAM hintParam, CObject* hintObject );
    DECLARE_MESSAGE_MAP()

// Constructor-destructor
public:
    CSymCorpUIStatusView();
    virtual ~CSymCorpUIStatusView();
    DECLARE_DYNCREATE(CSymCorpUIStatusView)

private:
    // ** FUNCTION MEMBERS **
    // Creates the provider block listings on the status dialog
    HRESULT CreateProviderBlocks( void );
    // Releases the provider block dialogs
    HRESULT ReleaseProviderBlocks( bool updateDisplay = true );

    // ** DATA MEMBRS **
    // Data modelish
    CStatusPTBlockDlgPtrList    providerBlocks;

    // Controls
    CStaticEx                   staticProtectionTechHeader;
    CStaticEx                   staticDescription;
    ActionBox                   actionBox;
};