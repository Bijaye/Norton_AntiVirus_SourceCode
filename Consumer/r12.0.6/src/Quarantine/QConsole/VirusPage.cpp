//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// VirusPage.cpp : implementation file
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

    m_ulVID = 0;
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
// Description	    : Handle WM_INITDIALOG message.
// 2/19/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
BOOL CVirusPage::OnInitDialog() 
{
	CItemPage::OnInitDialog();
	
    // Create our image list.
	m_ImageList.Create(IDB_VIRUS_CHARS, 8, 0, RGB(0, 255, 0));

    // Set up buttons
    m_Trigger.SetImageList(&m_ImageList);    
    m_SizeStealth.SetImageList(&m_ImageList);
    m_Poly.SetImageList(&m_ImageList);       
    m_Memory.SetImageList(&m_ImageList);
    m_FullStealth.SetImageList(&m_ImageList);
    m_Encrypting.SetImageList(&m_ImageList);

    // Set up virus info
    if(FAILED(m_pDoc->m_pQuarantine->GetVirusInformation(m_ulVID, &m_pScanVirusInfo)))
    {
        CCTRACEE(_T("%s - Failed to get the virus information for vid %d"), __FUNCTION__, m_ulVID);
        return TRUE;
    }

    //
    // Set up text controls.

    char buffer[MAX_PATH];
    DWORD dwSize = MAX_PATH;

    // Virus name
    m_pScanVirusInfo->GetName(buffer, &dwSize);
    m_sVirusName = buffer;

    // Alias
    dwSize = MAX_PATH;
    m_pScanVirusInfo->GetAlias(buffer, &dwSize);
    m_sAliases = buffer;

    // Extended virus information
    dwSize = MAX_PATH;
    m_pScanVirusInfo->GetInfo(buffer, &dwSize);
    m_sComments = buffer;

    // Virus size
    int nInfectionSize = m_pScanVirusInfo->GetInfecctionSize();
    m_sLength.Format(IDS_BYTES_STRING_FORMAT, nInfectionSize);

	// Get liklyhood string
    m_sLikelihood.LoadString(m_pScanVirusInfo->InWild() ? 
                              IDS_COMMON : IDS_RARE);

    // Build infects string
    BuildInfectsString();

    // Set correct images for infection flags
    m_Trigger.SetImageIndex(m_pScanVirusInfo->IsTrigger());
	m_SizeStealth.SetImageIndex(m_pScanVirusInfo->IsSizeStealth());
	m_Poly.SetImageIndex(m_pScanVirusInfo->IsPolymorphic());
	m_Memory.SetImageIndex(m_pScanVirusInfo->IsMemoryResident());
	m_FullStealth.SetImageIndex(m_pScanVirusInfo->IsFullStealth());
	m_Encrypting.SetImageIndex(m_pScanVirusInfo->IsEncrypting());

    // Blast data to controls
    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


////////////////////////////////////////////////////////////////////////////
// Description	    : Build a string like (".COM files, .EXE files, etc")
//                    Stolen from the VirusList project in AvCore
// 3/2/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CVirusPage::BuildInfectsString()
{
    static UINT uInfectsFilesString[] =
                                       {
                                       IDS_NA_BYTES,
                                       IDS_1FILE,
                                       IDS_2FILE,
                                       IDS_3FILE
                                       };

    static UINT uBootRecordString[] =
                                       {
                                       IDS_NA_BYTES,
                                       IDS_1BOOT,
                                       IDS_2BOOT,
                                       IDS_3BOOT
                                       };

    UINT uStringID;
    CString aStrings[3];
	UINT uConditions = 0;

    BOOL bBootType = !m_pScanVirusInfo->InfectsFiles();
	if(bBootType == TRUE)
	{
		// Boot Record viruses
		if(m_pScanVirusInfo->InfectsFloppyBootSectors())
			aStrings[uConditions++].LoadString(IDS_FLOPPY);

		if(m_pScanVirusInfo->InfectsHDBootSectors())
			aStrings[uConditions++].LoadString(IDS_HARDDISK);

		if(m_pScanVirusInfo->InfectsMBRs())
			aStrings[uConditions++].LoadString(IDS_MASTER);

		uStringID = uBootRecordString[uConditions];
	}
	else
	{
		// Program File viruses
		if(m_pScanVirusInfo->InfectsCOMFiless())
			aStrings[uConditions++].LoadString(IDS_COM);

		if(m_pScanVirusInfo->InfectsEXEFiles())
			aStrings[uConditions++].LoadString(IDS_EXE);

		if(m_pScanVirusInfo->InfectsSYSFiles())
			aStrings[uConditions++].LoadString(IDS_SYS);

		uStringID = uInfectsFilesString[uConditions];
	}

    if(uStringID)
        m_sInfects.Format(uStringID, aStrings[0], aStrings[1], aStrings[2]);
    else
        m_sInfects.Empty();
}

