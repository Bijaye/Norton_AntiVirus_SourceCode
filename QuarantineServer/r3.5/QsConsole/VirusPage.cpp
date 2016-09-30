/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// VirusPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "VirusPage.h"
#include "NumberString.h"
#include "macros.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVirusPage property page

IMPLEMENT_DYNCREATE(CVirusPage, CQPropPage)

CVirusPage::CVirusPage() : CQPropPage(CVirusPage::IDD)
{
    //{{AFX_DATA_INIT(CVirusPage)
    m_sAliases = _T("");
    m_sComments = _T("");
    m_sInfects = _T("");
    m_sLength = _T("");
    m_sLikelihood = _T("");
    m_sVirusName = _T("");
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_dwRef = 1;
}

CVirusPage::~CVirusPage()
{
}

void CVirusPage::DoDataExchange(CDataExchange* pDX)
{
    CQPropPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CVirusPage)
    DDX_Control(pDX, IDC_TRIGGER_PICTURE, m_Trigger);
    DDX_Control(pDX, IDC_SIZE_STEALTH_PICTURE, m_SizeStealth);
    DDX_Control(pDX, IDC_POLY_PICTURE, m_Poly);
    DDX_Control(pDX, IDC_MEMORY_PICTURE, m_Memory);
    DDX_Control(pDX, IDC_FULL_STEALTH_PICTURE, m_FullStealth);
    DDX_Control(pDX, IDC_ENCRYPTING_PICTURE, m_Encrypting);
    DDX_Text(pDX, IDC_ALIASES_STATIC, m_sAliases);
    DDX_Text(pDX, IDC_COMMENTS_STATIC, m_sComments);
    DDX_Text(pDX, IDC_INFECTS_STATIC, m_sInfects);
    DDX_Text(pDX, IDC_LENGTH_STATIC, m_sLength);
    DDX_Text(pDX, IDC_LIKELIHOOD_STATIC, m_sLikelihood);
    DDX_Text(pDX, IDC_VIRUS_NAME_STATIC, m_sVirusName);

        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVirusPage, CQPropPage)
    //{{AFX_MSG_MAP(CVirusPage)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVirusPage message handlers


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CVirusPage::OnInitDialog
//
// Description   : 
//
// Return type   : BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/19/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CVirusPage::OnInitDialog() 
{
    CQPropPage::OnInitDialog();
    
    // Create our image list.
    m_ImageList.Create( IDB_VIRUS_BMPS, 8, 0, RGB( 0, 255, 0) );

    // Set up buttons
    m_Trigger.SetImageList( &m_ImageList );    
    m_SizeStealth.SetImageList( &m_ImageList );
    m_Poly.SetImageList( &m_ImageList );       
    m_Memory.SetImageList( &m_ImageList );
    m_FullStealth.SetImageList( &m_ImageList );
    m_Encrypting.SetImageList( &m_ImageList ); 
    // TODO: Add extra initialization here
    
    m_dwRef ++;

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CVirusPage::PostNcDestroy
//
// Description   : 
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/19/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CVirusPage::PostNcDestroy() 
{
    CQPropPage::PostNcDestroy();

    Release();
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CVirusPage::Initialize
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     : DWORD dwVirusID
// Argument     : CAVScanner *pScan
//
///////////////////////////////////////////////////////////////////////////////
// 3/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CVirusPage::Initialize(DWORD dwVirusID, CAVScanner *pScan)
{
USES_CONVERSION;

    // 
    // Get virus def info.
    // 
    LPVOID pVirusDef;
    if( FAILED( pScan->LoadVirusDef( dwVirusID, &pVirusDef ) ) )
        return E_FAIL;


    // Set correct images for infection flags
    DWORD dwBufferSize = MAX_PATH;
    char buffer[ MAX_PATH ];

    m_Trigger.SetImageIndex( pScan->GetVirusDefInfo( pVirusDef, LEG_VI_BOOL_TRIGGER, buffer, &dwBufferSize) );
    m_SizeStealth.SetImageIndex( pScan->GetVirusDefInfo( pVirusDef, LEG_VI_BOOL_SIZE_STEALTH, buffer, &dwBufferSize) );
    m_Poly.SetImageIndex( pScan->GetVirusDefInfo( pVirusDef, LEG_VI_BOOL_POLYMORPHIC, buffer, &dwBufferSize) );
    m_Memory.SetImageIndex( pScan->GetVirusDefInfo( pVirusDef, LEG_VI_BOOL_MEMORY_RES, buffer, &dwBufferSize) );
    m_FullStealth.SetImageIndex( pScan->GetVirusDefInfo( pVirusDef, LEG_VI_BOOL_FULL_STEALTH, buffer, &dwBufferSize) );
    m_Encrypting.SetImageIndex( pScan->GetVirusDefInfo( pVirusDef, LEG_VI_BOOL_ENCRYPTING, buffer, &dwBufferSize) );
    
    // Set up text controls.
    dwBufferSize = MAX_PATH;
    pScan->GetVirusDefInfo( pVirusDef, LEG_VI_VIRUS_NAME, buffer, &dwBufferSize);
    m_sVirusName = A2T( buffer );

    dwBufferSize = MAX_PATH;
    pScan->GetVirusDefInfo( pVirusDef, LEG_VI_VIRUS_ALIAS, buffer, &dwBufferSize);
    m_sAliases = A2T( buffer );
	if( m_sAliases.IsEmpty() )
		m_sAliases.LoadString( IDS_NONE );

    dwBufferSize = MAX_PATH;
    pScan->GetVirusDefInfo( pVirusDef, LEG_VI_VIRUS_INFO, buffer, &dwBufferSize);
    m_sComments = A2T( buffer );

    // Get liklyhood string
    dwBufferSize = MAX_PATH;
    m_sLikelihood.LoadString( pScan->GetVirusDefInfo( pVirusDef, LEG_VI_BOOL_INFECT_WILD, buffer, &dwBufferSize) ? 
                              IDS_COMMON : IDS_RARE );

    // Length is only valid if we are a not a MAC virus
    dwBufferSize = MAX_PATH;
    if ( pScan->GetVirusDefInfo( pVirusDef, LEG_VI_BOOL_MAC_VIRUS, buffer, &dwBufferSize) == TRUE )
        m_sLength.LoadString( IDS_NA );
    else
        {
        dwBufferSize = MAX_PATH;
        pScan->GetVirusDefInfo( pVirusDef, LEG_VI_VIRUS_SIZE, buffer, &dwBufferSize);
        CNumberString numStr( (DWORD) atol( buffer ) );

        m_sLength.Format( IDS_BYTES_STR_FORMAT, (LPCTSTR) numStr );
        }
    
    // 
    // Build infects string here.
    // 
    BuildInfectsString( pVirusDef, pScan, m_sInfects );

    // 
    // Cleanup
    // 
    pScan->FreeVirusDef( pVirusDef );
    
    return S_OK;   
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CVirusPage::BuildInfectsString
//
// Description      : Build a string like (".COM files, .EXE files, etc" )
//                    Stolen from the VirusList project in AvCore
//
// Return type      : void BOOL
//
// Argument         : VLVIRUSENTRY hEntry - entry we are concerned with
// Argument         : CString&     sBuffer - output buffer
//
////////////////////////////////////////////////////////////////////////////
// 3/2/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CVirusPage::BuildInfectsString(
    LPVOID  pVirusDef,
    CAVScanner *pScan,
    CString&      sBuffer )
{
    static      UINT            uInfectsFilesString[] =
                                       {
                                       IDS_NA,
                                       IDS_1FILE,
                                       IDS_2FILE,
                                       IDS_3FILE
                                       };

    static      UINT            uBootRecordString[] =
                                       {
                                       IDS_NA,
                                       IDS_1BOOT,
                                       IDS_2BOOT,
                                       IDS_3BOOT
                                       };

    auto        UINT            uConditions;
    auto        UINT            uStringID;
    auto        char           szWork  [ MAX_PATH ];
    auto        BOOL            bBootType;
    auto        DWORD           dwBufferSize = MAX_PATH;

    CString aStrings[3];

    uConditions = 0;

    bBootType = !pScan->GetVirusDefInfo ( pVirusDef, LEG_VI_BOOL_INFECT_FILES, szWork, &dwBufferSize );

    if ( bBootType == TRUE )
        {
                                        // Boot Record viruses
        dwBufferSize = MAX_PATH;
        if ( pScan->GetVirusDefInfo ( pVirusDef, LEG_VI_BOOL_INFECT_FLOPPY_BOOT, szWork, &dwBufferSize ) )
            aStrings[uConditions++].LoadString( IDS_FLOPPY );

        if ( pScan->GetVirusDefInfo ( pVirusDef, LEG_VI_BOOL_INFECT_HD_BOOT, szWork, &dwBufferSize ) )
            aStrings[uConditions++].LoadString( IDS_HARDDISK );

        if ( pScan->GetVirusDefInfo ( pVirusDef, LEG_VI_BOOL_INFECT_MBR, szWork, &dwBufferSize ) )
            aStrings[uConditions++].LoadString( IDS_MASTER );

        uStringID = uBootRecordString[uConditions];
        }
    else
        {
                                        // Program File viruses
        if ( pScan->GetVirusDefInfo ( pVirusDef, LEG_VI_BOOL_INFECT_COM, szWork, &dwBufferSize ) )
            aStrings[uConditions++].LoadString( IDS_COM );

        if ( pScan->GetVirusDefInfo ( pVirusDef, LEG_VI_BOOL_INFECT_EXE, szWork, &dwBufferSize ) )
            aStrings[uConditions++].LoadString( IDS_EXE );

        if ( pScan->GetVirusDefInfo ( pVirusDef, LEG_VI_BOOL_INFECT_SYS, szWork, &dwBufferSize ) )
            aStrings[uConditions++].LoadString( IDS_SYS );

        uStringID = uInfectsFilesString[uConditions];
        }

    if ( uStringID )
        {
        sBuffer.Format( uStringID,
                        aStrings[0],
                        aStrings[1],
                        aStrings[2]);
        }
    else
        sBuffer.Empty();
}

BOOL CVirusPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    char szPath[512];
    CString sTopic;
    // INITIALIZE
    HWND hHelpWin = NULL;
    sTopic = _T(" ");

	AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    NMHDR* pNMHDR = (NMHDR*)lParam;

	if( pNMHDR->code != PSN_HELP)   //********* in "Prsht.h"
    return(S_FALSE);

     CWnd *pwnd = GetActiveWindow();
    sTopic.Format( _T("%s::/%s"), m_sHelpFile, QCONSOLE_HELPTOPIC_SAMP_VIRUS_PROP);
#ifdef _UNICODE
     int iRet = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)sTopic.GetBuffer(0),-1,
                                   (LPSTR)szPath,sizeof(szPath),NULL,NULL);
     sTopic.ReleaseBuffer(-1);
     if( iRet == FALSE )
        return(FALSE);
#else
    vpstrncpy( szPath, sTopic.GetBuffer(0), sizeof (szPath) );
    sTopic.ReleaseBuffer(-1);
#endif

    hHelpWin =HtmlHelpA(
                    pwnd->m_hWnd,
                    szPath,                // (LPCTSTR)sTopic,    // "c:\\Help.chm::/Intro.htm"
                    HH_DISPLAY_TOPIC,         // 
                    NULL) ;
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}
