/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/viruspage.cpv   1.4   20 May 1998 17:15:16   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// VirusPage.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/viruspage.cpv  $
// 
//    Rev 1.4   20 May 1998 17:15:16   DBuches
// 1st pass at help
// 
//    Rev 1.3   07 May 1998 15:07:46   DBuches
// Added help button.
// 
//    Rev 1.2   06 Mar 1998 11:20:26   DBuches
// Updated to use IQuarantineItem pointer to update data.
// 
//    Rev 1.1   02 Mar 1998 15:25:30   DBuches
// Checked in work in progress.
// 
//    Rev 1.0   27 Feb 1998 15:09:00   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "VirusPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVirusPage property page

IMPLEMENT_DYNCREATE(CVirusPage, CItemPage)

CVirusPage::CVirusPage() : CItemPage(CVirusPage::IDD)
{
	//{{AFX_DATA_INIT(CVirusPage)
	m_sAliases = _T("");
	m_sComments = _T("");
	m_sInfects = _T("");
	m_sLength = _T("");
	m_sLikelihood = _T("");
	m_sVirusName = _T("");
	//}}AFX_DATA_INIT

    m_VirusIndex = 0;

}

CVirusPage::~CVirusPage()
{
}

void CVirusPage::DoDataExchange(CDataExchange* pDX)
{
	CItemPage::DoDataExchange(pDX);
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
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVirusPage, CItemPage)
	//{{AFX_MSG_MAP(CVirusPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVirusPage message handlers



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CVirusPage::OnInitDialog
// Description	    : Handle WM_INITDIALOG message.
// Return type		: BOOL 
//
////////////////////////////////////////////////////////////////////////////////
// 2/19/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
BOOL CVirusPage::OnInitDialog() 
{
	CItemPage::OnInitDialog();
	
    // Create our image list.
	m_ImageList.Create( IDB_VIRUS_CHARS, 8, 0, RGB( 0, 255, 0) );


    // Set up virus info 
    m_hv = VLGetVirusEntry( m_pDoc->GetAVAPIContext(), m_VirusIndex );

    // Set up buttons
    m_Trigger.SetImageList( &m_ImageList );    
    m_SizeStealth.SetImageList( &m_ImageList );
    m_Poly.SetImageList( &m_ImageList );       
    m_Memory.SetImageList( &m_ImageList );
    m_FullStealth.SetImageList( &m_ImageList );
    m_Encrypting.SetImageList( &m_ImageList ); 

    // Set up text controls.
    char buffer[MAX_PATH];
    VLDefInfo( m_hv, VI_VIRUS_NAME ,buffer );
    m_sVirusName = buffer;
    VLDefInfo( m_hv, VI_VIRUS_ALIAS ,buffer );
    m_sAliases = buffer;
    VLDefInfo( m_hv, VI_VIRUS_INFO ,buffer );	                                                
    m_sComments = buffer;
    VLDefInfo( m_hv, VI_VIRUS_SIZE ,buffer );	                                                
    m_sLength.Format( IDS_BYTES_STRING_FORMAT, buffer );
    
    // Length is only valid if we are a not a MAC virus
    if ( VLDefInfo ( m_hv, VI_BOOL_MAC_VIRUS, buffer ) == TRUE )
        m_sLength.LoadString( IDS_NA );

	// Get liklyhood string
    m_sLikelihood.LoadString( VLDefInfo( m_hv, VI_BOOL_INFECT_WILD, buffer ) ? 
                              IDS_COMMON : IDS_RARE );
    
    // Build infects string
    BuildInfectsString( m_hv, m_sInfects );

    // Set correct images for infection flags
    m_Trigger.SetImageIndex( VLDefInfo( m_hv, VI_BOOL_TRIGGER, buffer) );
	m_SizeStealth.SetImageIndex( VLDefInfo( m_hv, VI_BOOL_SIZE_STEALTH, buffer) );
	m_Poly.SetImageIndex( VLDefInfo( m_hv, VI_BOOL_POLYMORPHIC, buffer) );
	m_Memory.SetImageIndex( VLDefInfo( m_hv, VI_BOOL_MEMORY_RES, buffer) );
	m_FullStealth.SetImageIndex( VLDefInfo( m_hv, VI_BOOL_FULL_STEALTH, buffer) );
	m_Encrypting.SetImageIndex( VLDefInfo( m_hv, VI_BOOL_ENCRYPTING, buffer) );

    // Blast data to controls
    UpdateData( FALSE );    

    // Release our virus entry    
    VLReleaseEntry( m_hv );

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



////////////////////////////////////////////////////////////////////////////
// Function name	: CVirusPage::BuildInfectsString
//
// Description	    : Build a string like (".COM files, .EXE files, etc" )
//                    Stolen from the VirusList project in AvCore
//
// Return type		: void BOOL 
//
// Argument         : VLVIRUSENTRY hEntry - entry we are concerned with
// Argument         : CString&     sBuffer - output buffer
//
////////////////////////////////////////////////////////////////////////////
// 3/2/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CVirusPage::BuildInfectsString(   
    HVLVIRUSENTRY hEntry,                 
    CString&      sBuffer )
{
    static      UINT            uInfectsFilesString[] =
                                       {
                                       IDS_NA_BYTES,
                                       IDS_1FILE,
                                       IDS_2FILE,
                                       IDS_3FILE
                                       };

    static      UINT            uBootRecordString[] =
                                       {
                                       IDS_NA_BYTES,
                                       IDS_1BOOT,
                                       IDS_2BOOT,
                                       IDS_3BOOT
                                       };

    auto        UINT            uConditions;
    auto        UINT            uStringID;
    auto        TCHAR           szWork  [ MAX_PATH ];
    auto        BOOL            bBootType;

    CString aStrings[3];
    
    uConditions = 0;

    bBootType = !VLDefInfo ( hEntry, VI_BOOL_INFECT_FILES, szWork );

    if ( bBootType == TRUE )
        {
                                        // Boot Record viruses
        if ( VLDefInfo ( hEntry, VI_BOOL_INFECT_FLOPPY_BOOT, szWork ) )
            aStrings[uConditions++].LoadString( IDS_FLOPPY );

        if ( VLDefInfo ( hEntry, VI_BOOL_INFECT_HD_BOOT, szWork ) )
            aStrings[uConditions++].LoadString( IDS_HARDDISK );

        if ( VLDefInfo ( hEntry, VI_BOOL_INFECT_MBR, szWork ) )
            aStrings[uConditions++].LoadString( IDS_MASTER );

        uStringID = uBootRecordString[uConditions];
        }
    else
        {
                                        // Program File viruses
        if ( VLDefInfo ( hEntry, VI_BOOL_INFECT_COM, szWork ) )
            aStrings[uConditions++].LoadString( IDS_COM );

        if ( VLDefInfo ( hEntry, VI_BOOL_INFECT_EXE, szWork ) )
            aStrings[uConditions++].LoadString( IDS_EXE );

        if ( VLDefInfo ( hEntry, VI_BOOL_INFECT_SYS, szWork ) )
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

