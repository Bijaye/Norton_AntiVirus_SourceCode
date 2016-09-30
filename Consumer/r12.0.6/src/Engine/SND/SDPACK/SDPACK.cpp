// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SDPACK/VCS/sdpack.cpv   1.13   06 Aug 1998 14:36:04   DCHI  $
//
// Description:
//  Source file for Scan and Deliver packaging DLL.
//
//************************************************************************
// $Log:   S:/SDPACK/VCS/sdpack.cpv  $
// 
//    Rev 1.13   06 Aug 1998 14:36:04   DCHI
// - Within SDPackGetProductList(), changed to not return error if
//   LoadString() fails.
// - Fixed ReleaseSubmissionData() to free memory.
// 
//    Rev 1.12   17 Jul 1998 16:21:22   DCHI
// Corrected size computation for presubmission header size...was incorrectly
// computed based on total # files as opposed to actual maximum
// of MAX_SUBMISSION_PACKETS.
// 
//    Rev 1.11   01 Jul 1998 18:11:46   DCHI
// Fixed non-storage of GUID bug in SubmitSample().
// 
//    Rev 1.10   18 May 1998 13:44:50   DCHI
// Removed call to rename SARC.TXT.
// 
//    Rev 1.9   07 May 1998 13:52:36   DCHI
// Corrected problem with calculation of submission header size.
// 
//    Rev 1.8   27 Apr 1998 12:26:46   DCHI
// Fixed iteration in GetTotalFileSizeCount().
// 
//    Rev 1.7   27 Apr 1998 12:23:16   DCHI
// Corrected problem with checking return value on CloseHandle()
// in GetTotalFileSizeCount().
// 
//    Rev 1.6   20 Apr 1998 11:47:16   DCHI
// Changed from AddWildCardFiles() to AddFile().
// 
//    Rev 1.5   14 Apr 1998 15:16:58   DCHI
// Added support for product strings.
// 
//    Rev 1.4   13 Apr 1998 17:38:14   DCHI
// Modified to use hashpack.h and modified submission list to be FIFO.
// 
//    Rev 1.3   08 Apr 1998 17:07:04   DCHI
// Consolidated Platinum and Gold to pin.  Changed CaseNum to pin.
// Modified product and language strings.
// 
//    Rev 1.2   06 Apr 1998 19:20:12   DCHI
// Synced up field values with header file and modified file list output.
// 
//    Rev 1.1   06 Apr 1998 17:51:42   DCHI
// Added SetInfoFieldDateTime(), added machine name and original file name
// to SubmitSample(), and modified field enumerations.
// 
//    Rev 1.0   13 Mar 1998 12:51:38   DCHI
// Initial revision.
// 
//************************************************************************

#define _SDPACK_CPP_

#define SDPACK_VERSION          0x00000001
#define SDPACK_ZIP_ENCRYPT_BYTE 0xCD

#include <windows.h>
#include <tchar.h>
#include "sdpack.h"
#include "arclib.h"
#include "pkarc.h"
#include "alsimple.h"
#include "crc32.h"
#include "psm.h"

HINSTANCE ghInst;

BOOL WINAPI DllMain
(
    HINSTANCE   hInstDLL,
    DWORD       dwReason,
    LPVOID      lpvReserved
)
{
    (void)lpvReserved;

    if (dwReason == DLL_PROCESS_ATTACH)
        ghInst = hInstDLL;

    return(TRUE);
}

typedef struct
{
    CPackageSamples::PRODUCT    eProduct;
    int                         nVersion;
    CPackageSamples::OS         eOS;
    CPackageSamples::PLATFORM   ePlatform;
} PRODUCTS_T;

PRODUCTS_T gastProductList[IDS_SDPACK_COUNT] =
{
    { CPackageSamples::OtherProduct, 300,
      CPackageSamples::OtherOS, CPackageSamples::OtherPlatform},
    { CPackageSamples::NAV, 300,
      CPackageSamples::WIN3X, CPackageSamples::INTEL },
    { CPackageSamples::NAV, 400,
      CPackageSamples::WIN3X, CPackageSamples::INTEL },
    { CPackageSamples::NAV, 200,
      CPackageSamples::WIN95, CPackageSamples::INTEL },
    { CPackageSamples::NAV, 400,
      CPackageSamples::WIN95, CPackageSamples::INTEL },
    { CPackageSamples::NAV, 500,
      CPackageSamples::WIN95, CPackageSamples::INTEL },
    { CPackageSamples::NAV, 400,
      CPackageSamples::WINNT, CPackageSamples::ALPHA },
    { CPackageSamples::NAV, 500,
      CPackageSamples::WINNT, CPackageSamples::ALPHA },
    { CPackageSamples::NAV, 200,
      CPackageSamples::WINNT, CPackageSamples::INTEL },
    { CPackageSamples::NAV, 400,
      CPackageSamples::WINNT, CPackageSamples::INTEL },
    { CPackageSamples::NAV, 500,
      CPackageSamples::WINNT, CPackageSamples::INTEL },
    { CPackageSamples::NAV, 500,
      CPackageSamples::MACOS, CPackageSamples::MAC },
    { CPackageSamples::LOTUSNOTES, 100,
      CPackageSamples::WINNT, CPackageSamples::INTEL },
    { CPackageSamples::MSEXCHANGE, 100,
      CPackageSamples::WINNT, CPackageSamples::INTEL },
    { CPackageSamples::NAV, 100,
      CPackageSamples::NLM, CPackageSamples::INTEL },
    { CPackageSamples::IEG, 100,
      CPackageSamples::WINNT, CPackageSamples::INTEL },
    { CPackageSamples::FW, 100,
      CPackageSamples::WINNT, CPackageSamples::INTEL },
    { CPackageSamples::SAM, 400,
      CPackageSamples::MACOS, CPackageSamples::MAC },
};

typedef struct tagSDPACK_STR *PSDPACK_STR;

typedef struct tagSDPACK_STR
{
    PSDPACK_STR     pstNext;
    LPTSTR          lpsz;
} SDPACK_STR_T, *PSDPACK_STR;


typedef struct tagSDPACK_GUID *PSDPACK_GUID;

typedef struct tagSDPACK_GUID
{
    PSDPACK_GUID    pstNext;
    GUID            stGUID;
} SDPACK_GUID_T, *PSDPACK_GUID;

typedef struct tagSDPACK_INFO
{
    LPTSTR          lpszPackageName;

    PSDPACK_STR     alpstFields[CPackageSamples::FieldReserved];
    PSDPACK_STR     alpstFieldTails[CPackageSamples::FieldReserved];

    DWORD           dwTotalFileSize;
    DWORD           dwTotalFileCount;
    PSDPACK_STR     lpstFileNames;
    PSDPACK_GUID    lpstGUIDs;
    PSDPACK_STR     lpstMachineNames;
    PSDPACK_STR     lpstOriginalNames;
    PSDPACK_STR     lpstFileNamesTail;
    PSDPACK_GUID    lpstGUIDsTail;
    PSDPACK_STR     lpstMachineNamesTail;
    PSDPACK_STR     lpstOriginalNamesTail;
    PSDPACK_STR     lpstCategories;
    PSDPACK_STR     lpstCategoriesTail;
    PSDPACK_STR     lpstMd5;
    PSDPACK_STR     lpstMd5Tail;
    PSDPACK_STR     lpstVID;
    PSDPACK_STR     lpstVIDTail;
    PSDPACK_STR     lpstVirName;
    PSDPACK_STR     lpstVirNameTail;

    TCHAR           szSARCTXTFileName[MAX_PATH + 16];

    LPINT           lpnProductIndex;
    LPTSTR *        lplpszProductList;
} SDPACK_INFO_T, *PSDPACK_INFO;

class SDPackALMonitor : public ALMonitor
{
    public:
        SDPackALMonitor
        (
            ALArchive *             pArchive,
            PPackageProgressCB      lpfnProgressCB,
            LPVOID                  lpvProgressCookie
        );

        void Progress
        (
            long            object_tell,
            ALStorage       &object
        );

    private:
        ALArchive *         m_pArchive;
        PPackageProgressCB  m_lpfnProgressCB;
        LPVOID              m_lpvProgressCookie;
};

SDPackALMonitor::SDPackALMonitor
(
    ALArchive *             pArchive,
    PPackageProgressCB      lpfnProgressCB,
    LPVOID                  lpvProgressCookie
)
: ALMonitor(AL_MONITOR_JOB)
{
    m_pArchive = pArchive;
    m_lpfnProgressCB = lpfnProgressCB;
    m_lpvProgressCookie = lpvProgressCookie;
}

void SDPackALMonitor::Progress
(
    long                    object_tell,
    ALStorage               &object
)
{
    ALMonitor::Progress(object_tell,object);

    if (m_lpfnProgressCB != NULL)
    {
        if (m_lpfnProgressCB(m_lpvProgressCookie,miRatio) == FALSE)
        {
            m_pArchive->mStatus.
                SetError(AL_USER_ABORT,"User abort");
        }
    }
}

//////////////////////////////////////////////////////////////////////
// Strings used for creation of SARC.TXT
//////////////////////////////////////////////////////////////////////

static LPTSTR galpszField[] =
{
    "OtherField",
    "contact_first",
    "contact_last",
    "company_name",
    "pin",
    "atlas_id",
    "address1",
    "address2",
    "city",
    "state",
    "zipcode",
    "country",
    "phone",
    "fax",
    "email",
    "product",
    "def_date",
    "source_country",
    "Symptoms",
    "language",
    "version",
    "OSBits",
    "FieldReserved"
};

static LPTSTR galpszProduct[] =
{
    "OtherProduct",
    "NAV",
    "SAM",
    "IEG",
    "FW",
    "LOTUSNOTES",
    "MSEXCHANGE",
    "ProductReserved",
};

static LPTSTR galpszOS[] =
{
    "OtherOS",
    "WIN3X",
    "WIN95",
    "WIN98",
    "WINNT",
    "WINCE",
    "NLM",
    "MAC",
    "SOLARIS",
    "LINUX",
    "PALMOS",
    "OSReserved",
};

static LPTSTR galpszPlatform[] =
{
    "OtherPlatform",
    "INTEL",
    "ALPHA",
    "MAC",
    "PlatformReserved",
};

#define SDPACK_MAX_PRODUCT_STR_SIZE     256

BOOL SDPackGetProductList
(
    PSDPACK_INFO    pstInfo
)
{
    int             i;
    int             j;
    LPINT           lpnID;

    pstInfo->lpnProductIndex = new int[IDS_SDPACK_COUNT];
    if (pstInfo->lpnProductIndex == NULL)
        return(FALSE);
    pstInfo->lpnProductIndex[0] = 0;

    pstInfo->lplpszProductList = new LPTSTR[IDS_SDPACK_COUNT];
    if (pstInfo->lplpszProductList == NULL)
    {
        delete pstInfo->lpnProductIndex;
        return(FALSE);
    }

    pstInfo->lplpszProductList[0] =
        new TCHAR[SDPACK_MAX_PRODUCT_STR_SIZE * IDS_SDPACK_COUNT];
    if (pstInfo->lplpszProductList[0] == NULL)
    {
        delete pstInfo->lplpszProductList;
        delete pstInfo->lpnProductIndex;
        return(FALSE);
    }
    pstInfo->lplpszProductList[0][0] = 0;

    lpnID = new int[IDS_SDPACK_COUNT];
    if (lpnID == NULL)
    {
        delete pstInfo->lplpszProductList[0];
        delete pstInfo->lplpszProductList;
        delete pstInfo->lpnProductIndex;
        return(FALSE);
    }
    lpnID[0] = 0;

    for (i=1;i<IDS_SDPACK_COUNT;i++)
    {
        lpnID[i] = i;

        pstInfo->lplpszProductList[i] =
            pstInfo->lplpszProductList[i - 1] +
            SDPACK_MAX_PRODUCT_STR_SIZE;

        if (LoadString(ghInst,
                       i,
                       pstInfo->lplpszProductList[i],
                       SDPACK_MAX_PRODUCT_STR_SIZE) == 0)
        {
            // Error

            pstInfo->lplpszProductList[i][0] = 0;
        }
    }

    // Sort

    for (i=1;i<(IDS_SDPACK_COUNT-1);i++)
    {
        for (j=i+1;j<IDS_SDPACK_COUNT;j++)
        {
            if (_tcscmp(pstInfo->lplpszProductList[i],
                        pstInfo->lplpszProductList[j]) > 0)
            {
                LPTSTR  lpszTemp;
                int     nTemp;

                nTemp = lpnID[i];
                lpnID[i] = lpnID[j];
                lpnID[j] = nTemp;

                lpszTemp = pstInfo->lplpszProductList[i];
                pstInfo->lplpszProductList[i] =
                    pstInfo->lplpszProductList[j];
                pstInfo->lplpszProductList[j] = lpszTemp;
            }
        }
    }

    // Set the index

    for (i=1;i<IDS_SDPACK_COUNT;i++)
        pstInfo->lpnProductIndex[lpnID[i]] = i;

    return(TRUE);
}

//
// WJORDAN 3-15-03: Creating helper functions to facilitate adding
// new fields
//
BOOL AllocateAndCopySDPackString
(
    PSDPACK_STR* pstToAllocate,
    LPCTSTR pszData,
    LPCTSTR pszDefaultText
);

void AddToEndOfList
(
    PSDPACK_STR* pstInfoItem,
    PSDPACK_STR* pstInfoTailItem,
    PSDPACK_STR pstrItem
);

void AddToEndOfList
(
    PSDPACK_GUID* pstInfoItem,
    PSDPACK_GUID* pstInfoTailItem,
    PSDPACK_GUID pguidItem
);

BOOL WriteString
(
    HANDLE hFile,
    PSDPACK_STR pstrToWrite
);
//*************************************************************************
//
// Function:
//  CPackageSamples::CPackageSamples()
//
// Parameters:
//  None
//
// Description:
//  Allocates SDPACK_INFO_T structure, sets m_lpvData to the memory
//  allocated, and then initializes the structure.
//
// Returns:
//  Nothing
//
//*************************************************************************

CPackageSamples::CPackageSamples()
{
    PSDPACK_INFO    pstInfo;

    m_lpvData = NULL;

    pstInfo = new SDPACK_INFO_T;
    m_lpvData = pstInfo;

    if (m_lpvData == NULL)
    {
        // Failed to allocate info structure

        return;
    }

    // Get the product list

    if (SDPackGetProductList(pstInfo) == FALSE)
    {
        delete pstInfo;
        m_lpvData = NULL;
        return;
    }

    InitData();
}


//*************************************************************************
//
// Function:
//  CPackageSamples::~CPackageSamples()
//
// Parameters:
//  None
//
// Description:
//  Frees all allocated memory.
//
// Returns:
//  Nothing
//
//*************************************************************************

CPackageSamples::~CPackageSamples()
{
    PSDPACK_INFO    pstInfo;

    if (m_lpvData == NULL)
        return;

    pstInfo = (PSDPACK_INFO)m_lpvData;

    // Free the product list

    delete pstInfo->lplpszProductList[0];
    delete pstInfo->lplpszProductList;
    delete pstInfo->lpnProductIndex;

    FreeData();

    // Free the structure

    delete m_lpvData;
}


//*************************************************************************
//
// Function:
//  BOOL CPackageSamples::GetProductList
//
// Parameters:
//  lpnProductCount     Ptr to int for product count
//  lplpnListIndex      Ptr to index to list
//  lplpszProductList   Ptr to array of product name strings
//
// Description:
//  Returns the product name information in the given pointers.
//
// Returns:
//  TRUE                On success
//  FALSE               On memory allocation error
//
//*************************************************************************

BOOL CPackageSamples::GetProductList
(
    LPINT               lpnProductCount,
    LPINT *             lplpnListIndex,
    LPTSTR **           lplplpszProductList
)
{
    PSDPACK_INFO        pstInfo;

    if (m_lpvData == NULL)
        return(FALSE);

    pstInfo = (PSDPACK_INFO)m_lpvData;

    *lpnProductCount = IDS_SDPACK_COUNT - 1;
    *lplpnListIndex = pstInfo->lpnProductIndex;
    *lplplpszProductList = pstInfo->lplpszProductList;

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL CPackageSamples::Reset
//
// Parameters:
//  lpszPackageName     File name of the package to create
//
// Description:
//  Frees all data fields and initializes for a new package with the
//  given package name.  The function also attempts to delete a file
//  by the same name in preparation for creating a new archive.
//
// Returns:
//  TRUE                On success
//  FALSE               On memory allocation error
//
//*************************************************************************

BOOL CPackageSamples::Reset
(
    LPCTSTR             lpszPackageName
)
{
    PSDPACK_INFO        pstInfo;

    if (m_lpvData == NULL)
        return(FALSE);

    FreeData();

    pstInfo = (PSDPACK_INFO)m_lpvData;

    pstInfo->lpszPackageName = new TCHAR[strlen(lpszPackageName) + 1];
    if (pstInfo->lpszPackageName == NULL)
        return(FALSE);

    strcpy(pstInfo->lpszPackageName,lpszPackageName);

    // Delete the file

    DeleteFile(lpszPackageName);

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL CPackageSamples::SubmitSample()
//
// Parameters:
//  lpszSampleName      Ptr to sample file name
//  lpstGUID            Ptr to sample GUID
//  lpszMachineName     Ptr to original machine name of sample
//  lpszOriginalName    Ptr to original name of sample
//  lpszMd5             Ptr to md5 hash for sample file
//  lpszVID             Ptr to VID for sample file
//  lpszCategories      Ptr to categories for sample file
//
// Description:
//  Adds the sample's file name, GUID, machine name, original
//  file name, md5 hash, VID, and categories to the sample list 
//  and increments the sample count.
//
//  lpszSampleName and lpstGUID must not be NULL.
//
// Returns:
//  TRUE                On success
//  FALSE               On memory allocation error
//*************************************************************************

BOOL CPackageSamples::SubmitSample
(
    LPCTSTR             lpszSampleName,
    LPGUID              lpstGUID,
    LPCTSTR             lpszMachineName,
    LPCTSTR             lpszOriginalName,
    LPCTSTR             lpszMd5,
    LPCTSTR             lpszVID,
    LPCTSTR             lpszCategories,
    LPCTSTR             lpszVirName
)
{
    BOOL                bRet                = TRUE;
    PSDPACK_INFO        pstInfo             = NULL;
    PSDPACK_STR         pstSample           = NULL;
    PSDPACK_STR         pstMachineName      = NULL;
    PSDPACK_STR         pstOriginalName     = NULL;
    PSDPACK_STR         pstCategories       = NULL;
    PSDPACK_STR         pstVID              = NULL;
    PSDPACK_STR         pstMd5              = NULL;
    PSDPACK_STR         pstVirName          = NULL;
    PSDPACK_GUID        pstGUID             = NULL;

    if (m_lpvData == NULL)
        return(FALSE);

    // Both parameters must be valid

    if (lpszSampleName == NULL || lpstGUID == NULL)
        return(FALSE);

    pstInfo = (PSDPACK_INFO)m_lpvData;

    ////////////////////////////////////////////////////////
    // Store sample GUID

    pstGUID = new SDPACK_GUID_T;
    if (pstGUID == NULL)
        return(FALSE);

    pstGUID->stGUID = *lpstGUID;

    ////////////////////////////////////////////////////////
    // Store sample name
    if( !AllocateAndCopySDPackString(&pstSample, lpszSampleName, "") )
        bRet = FALSE;

    ////////////////////////////////////////////////////////
    // Store machine name
    if( !AllocateAndCopySDPackString(&pstMachineName, lpszMachineName, "machine name not specified") )
        bRet = FALSE;

    ////////////////////////////////////////////////////////
    // Store original name
    if( !AllocateAndCopySDPackString(&pstOriginalName, lpszOriginalName, "original name not specified") )
        bRet = FALSE;

    ////////////////////////////////////////////////////////
    // Store md5 hash
    if( !AllocateAndCopySDPackString(&pstMd5, lpszMd5, "md5 hash not specified") )
        bRet = FALSE;

    ////////////////////////////////////////////////////////
    // Store vid
    if( !AllocateAndCopySDPackString(&pstVID, lpszVID, "No VID available") )
        bRet = FALSE;

    ////////////////////////////////////////////////////////
    // Store categories
    if( !AllocateAndCopySDPackString(&pstCategories, lpszCategories, "No categories available") )
        bRet = FALSE;

    ////////////////////////////////////////////////////////
    // Store virus name
    if( !AllocateAndCopySDPackString(&pstVirName, lpszVirName, "No Virus Name available") )
        bRet = FALSE;
    
    // On any failure free all of the information that has been allocated
    if( bRet == FALSE )
    {
        if( NULL != pstGUID)
            delete pstGUID;

        if( NULL != pstSample )
        {
            if( NULL != pstSample->lpsz )
                delete pstSample->lpsz;
            delete pstSample;
        }
            
        if( NULL != pstMachineName )
        {
            if( NULL != pstMachineName->lpsz )
                delete pstMachineName->lpsz;
            delete pstMachineName;
        }
            
        if( NULL != pstCategories )
        {
            if( NULL != pstCategories->lpsz )
                delete pstCategories->lpsz;
            delete pstCategories;
        }
            
        if( NULL != pstVID )
        {
            if( NULL != pstVID->lpsz )
                delete pstVID->lpsz;
            delete pstVID;
        }
            
        if( NULL != pstMd5 )
        {
            if( NULL != pstMd5->lpsz )
                delete pstMd5->lpsz;
            delete pstMd5;
        }

        if( NULL != pstVirName )
        {
            if( NULL != pstVirName->lpsz )
                delete pstVirName->lpsz;
            delete pstVirName;
        }

        return FALSE;
    }
    ////////////////////////////////////////////////////////
    // Add to end of sample list

    // Add GUID
    AddToEndOfList(&pstInfo->lpstGUIDs, &pstInfo->lpstGUIDsTail, pstGUID);

    // Add sample name
    AddToEndOfList(&pstInfo->lpstFileNames, &pstInfo->lpstFileNamesTail, pstSample);

    // Add machine name
    AddToEndOfList(&pstInfo->lpstMachineNames, &pstInfo->lpstMachineNamesTail, pstMachineName);

    // Add original name
    AddToEndOfList(&pstInfo->lpstOriginalNames, &pstInfo->lpstOriginalNamesTail, pstOriginalName);

    // Add vid
    AddToEndOfList(&pstInfo->lpstVID, &pstInfo->lpstVIDTail, pstVID);

    // Add categories
    AddToEndOfList(&pstInfo->lpstCategories, &pstInfo->lpstCategoriesTail, pstCategories);

    // Add md5 hash
    AddToEndOfList(&pstInfo->lpstMd5, &pstInfo->lpstMd5Tail, pstMd5);

    // Add virus name
    AddToEndOfList(&pstInfo->lpstVirName, &pstInfo->lpstVirNameTail, pstVirName);

    // Increment count

    pstInfo->dwTotalFileCount++;

    return(TRUE);
}

void AddToEndOfList (PSDPACK_STR* pstInfoItem, PSDPACK_STR* pstInfoTailItem, PSDPACK_STR pstrItem)
{
    pstrItem->pstNext = NULL;
    if (*pstInfoTailItem != NULL)
        (*pstInfoTailItem)->pstNext = pstrItem;

    (*pstInfoTailItem) = pstrItem;

    if ((*pstInfoItem) == NULL)
        (*pstInfoItem) = pstrItem;
}

void AddToEndOfList (PSDPACK_GUID* pstInfoItem, PSDPACK_GUID* pstInfoItemTail, PSDPACK_GUID pguidItem)
{
    pguidItem->pstNext = NULL;
    if (*pstInfoItemTail != NULL)
        (*pstInfoItemTail)->pstNext = pguidItem;

    (*pstInfoItemTail) = pguidItem;

    if (*pstInfoItem == NULL)
        (*pstInfoItem) = pguidItem;
}

// Allocate storage for strings
BOOL AllocateAndCopySDPackString(PSDPACK_STR* pstToAllocate, LPCTSTR pszData, LPCTSTR pszDefaultText)
{
    ////////////////////////////////////////////////////////
    // Store original name

    // Allocate storage

    (*pstToAllocate) = new SDPACK_STR_T;
    if (*pstToAllocate == NULL)
    {
        return(FALSE);
    }

    if (pszData != NULL)
        (*pstToAllocate)->lpsz = new TCHAR[strlen(pszData) + 1];
    else
        (*pstToAllocate)->lpsz = new TCHAR[256];

    if ((*pstToAllocate)->lpsz == NULL)
    {
        delete (*pstToAllocate);
        (*pstToAllocate) = NULL;

        return(FALSE);
    }

    // Copy value

    if (pszData != NULL)
        strcpy((*pstToAllocate)->lpsz,pszData);
    else if(pszDefaultText != NULL)
        strcpy((*pstToAllocate)->lpsz,pszDefaultText);
    else
        strcpy((*pstToAllocate)->lpsz,"original data not specified");

    return TRUE;
}

//*************************************************************************
//
// Function:
//  BOOL CPackageSamples::GetTotalFileSizeCount()
//
// Parameters:
//  lpdwFileSize        Ptr to DWORD for total file size (may be NULL)
//  lpdwFileCount       Ptr to DWORD for total file count (may be NULL)
//
// Description:
//  Stores in *lpdwFileSize the total number of files bytes of the
//  samples and stores in *lpdwFileCount the total number of samples.
//
//  Either parameter may be NULL if the corresponding value  is not
//  desired.
//
//  If the total file size returned is 2^32-1 then the total file
//  size is at least that amount.
//
// Returns:
//  TRUE                On success
//  FALSE               On memory allocation error
//
//*************************************************************************

BOOL CPackageSamples::GetTotalFileSizeCount
(
    LPDWORD             lpdwFileSize,
    LPDWORD             lpdwFileCount
)
{
    PSDPACK_INFO        pstInfo;
    PSDPACK_STR         pstSample;
    PSDPACK_STR         pstNext;
    HANDLE              hFile;

    if (m_lpvData == NULL)
        return(FALSE);

    pstInfo = (PSDPACK_INFO)m_lpvData;

    if (lpdwFileCount != NULL)
        *lpdwFileCount = pstInfo->dwTotalFileCount;

    if (lpdwFileSize != NULL)
    {
        DWORD           dwTotalFileSize;
        DWORD           dwFileSizeLow;
        DWORD           dwFileSizeHigh;

        dwTotalFileSize = 0;
        pstSample = pstInfo->lpstFileNames;
        while (pstSample != NULL)
        {
            pstNext = pstSample->pstNext;

            // Open the file

            hFile = CreateFile(pstSample->lpsz,
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               0,
                               NULL);

            if (hFile == INVALID_HANDLE_VALUE)
                return(FALSE);

            // Get the file size

            dwFileSizeLow = GetFileSize(hFile,
                                        &dwFileSizeHigh);

            // Close the file

            if (CloseHandle(hFile) == FALSE)
                return(FALSE);

            if (dwFileSizeLow == 0xFFFFFFFF &&
                GetLastError() != NO_ERROR)
                return(FALSE);

            // Check for overflow

            if (dwFileSizeHigh != 0)
            {
                // Overflow

                dwTotalFileSize = 0xFFFFFFFF;
                break;
            }

            if (dwTotalFileSize + dwFileSizeLow < dwTotalFileSize)
            {
                // Overflow

                dwTotalFileSize = 0xFFFFFFFF;
                break;
            }

            // No overflow, add normally

            dwTotalFileSize += dwFileSizeLow;

            pstSample = pstNext;
        }

        *lpdwFileSize = dwTotalFileSize;
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  PACKAGE_STATUS CPackageSamples::Finish
//
// Parameters:
//  lpfnProgressCB              Ptr to progress callback
//  lpvProgressCookie           Progress callback cookie
//  dwFlags                     Currently unused
//  lplpbyPreSubmissionData     Ptr to ptr for pre-submission data
//  lpnInvalidField             Ptr to FIELD for invalid field
//
// Description:
//  The function performs the following steps:
//      1. Validates the information fields.  If any required fields
//         are invalidate, the function returns PACKAGE_FIELD_INVALID
//         and sets *lpnInvalidField to the invalid field.
//      2. Creates a temporary file for SARC.TXT containing the
//         submission information.
//      3. Creates the package.
//      4. Encrypts the package
//      5. Creates the presubmission data.
//
// Returns:
//  PACKAGE_NO_RESET            If reset was not performed
//  PACKAGE_FIELD_INVALID       If a required info field is invalid
//  PACKAGE_GENERAL_ERROR       On error creating SARC.TXT
//
//*************************************************************************

CPackageSamples::PACKAGE_STATUS CPackageSamples::Finish
(
    PPackageProgressCB  lpfnProgressCB,
    LPVOID              lpvProgressCookie,
    DWORD               dwFlags,
    LPBYTE *            lplpbyPreSubmissionData,
    FIELD *             lpnInvalidField
)
{
    PSDPACK_INFO        pstInfo;
    PACKAGE_STATUS      status;

    if (m_lpvData == NULL)
        return(PACKAGE_GENERAL_ERROR);

    pstInfo = (PSDPACK_INFO)m_lpvData;

    // Make sure we have a package name

    if (pstInfo->lpszPackageName == NULL)
    {
        return(PACKAGE_NO_RESET);
    }

    // Validate fields

    *lpnInvalidField = ValidateInfoFields();
    if (*lpnInvalidField != NoField)
        return(PACKAGE_FIELD_INVALID);

    // Create SARC.TXT

    status = CreateSARCTXT();
    if (status != PACKAGE_OK)
        return(status);

    // Package samples

    status = CreatePackage(lpfnProgressCB,lpvProgressCookie);
    if (status != PACKAGE_OK)
        return(status);

//Greenleaf libraries create inconsistent directory when this is done!!!
//    status = RenameSARCTXT();
//    if (status != PACKAGE_OK)
//        return(status);

    status = EncryptPackage();
    if (status != PACKAGE_OK)
        return(status);

    status = CreatePreSubmissionData(lplpbyPreSubmissionData);
    if (status != PACKAGE_OK)
        return(status);

    return(PACKAGE_OK);
}


//*************************************************************************
//
// Function:
//  void CPackageSamples::ReleaseSubmissionData
//
// Parameters:
//
// Description:
//
// Returns:
//
//*************************************************************************

void CPackageSamples::ReleaseSubmissionData
(
    LPBYTE              lpbyPreSubmissionData
)
{
    if (lpbyPreSubmissionData == NULL)
        return;

    delete lpbyPreSubmissionData;
}


//*************************************************************************
//
// Function:
//  void CPackageSamples::Release
//
// Parameters:
//
// Description:
//
// Returns:
//
//*************************************************************************

void CPackageSamples::Release
(
    void
)
{
    FreeData();
}


//*************************************************************************
//
// Function:
//  BOOL CPackageSamples::SetInfoField
//
// Parameters:
//  eField              Field to set
//  lpszField           Field value
//
// Description:
//  The string given in lpszField is added to the end
//  of the string list for the given field.  This allows
//  field expansion in future versions.
//
// Returns:
//  TRUE                On success
//  FALSE               On memory allocation error or invalid parameters
//
//*************************************************************************

BOOL CPackageSamples::SetInfoField
(
    FIELD               eField,
    LPCTSTR             lpszField
)
{
    PSDPACK_INFO        pstInfo;
    PSDPACK_STR         pstStr;

    if (m_lpvData == NULL)
    {
        // Failed to allocate memory before

        return(FALSE);
    }

    if (eField < OtherField || eField >= FieldReserved)
    {
        // Unrecognized field

        return(FALSE);
    }

    pstInfo = (PSDPACK_INFO)m_lpvData;

    // Validate parameter

    if (lpszField == NULL)
        return(FALSE);

    // Allocate storage

    pstStr = new SDPACK_STR_T;
    if (pstStr == NULL)
        return(FALSE);

    pstStr->lpsz = new TCHAR[strlen(lpszField) + 1];
    if (pstStr->lpsz == NULL)
    {
        delete pstStr;
        return(FALSE);
    }

    // Copy value

    strcpy(pstStr->lpsz,lpszField);

    // Add to end of list

    pstStr->pstNext = NULL;
    if (pstInfo->alpstFieldTails[eField] != NULL)
        pstInfo->alpstFieldTails[eField]->pstNext = pstStr;

    pstInfo->alpstFieldTails[eField] = pstStr;

    if (pstInfo->alpstFields[eField] == NULL)
        pstInfo->alpstFields[eField] = pstStr;

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL CPackageSamples::SetInfoFieldDateTime
//
// Parameters:
//  eField              Field to set
//  nYear               Year
//  nMonth              Month  (1-12)
//  nDay                Day    (1-31)
//  nHour               Hour   (0-23)
//  nMinute             Minute (1-60)
//  nSecond             Second (1-60)
//
// Description:
//  The field is set to the given date and time in the format:
//      Year/Month/Day Hour/Minute/Second
//  ie  1998/4/6 16:53:00
//
// Returns:
//  TRUE                On success
//  FALSE               On memory allocation error or invalid parameters
//
//*************************************************************************

BOOL CPackageSamples::SetInfoFieldDateTime
(
    FIELD               eField,
    int                 nYear,
    int                 nMonth,
    int                 nDay,
    int                 nHour,
    int                 nMinute,
    int                 nSecond
)
{
    TCHAR               szBuf[256];

    if (m_lpvData == NULL)
    {
        // Failed to allocate memory before

        return(FALSE);
    }

    if (eField < OtherField || eField >= FieldReserved)
    {
        // Unrecognized field

        return(FALSE);
    }

    wsprintf(szBuf,"%d/%d/%d %d:%d:%d",
        nYear,nMonth,nDay,nHour,nMinute,nSecond);

    return SetInfoField(eField,szBuf);
}


//*************************************************************************
//
// Function:
//  BOOL CPackageSamples::SetProductOSField
//
// Parameters:
//  nProductID          Product ID
//
// Description:
//  Forms a string out of the parameters and adds it to the list
//  of products.
//
// Returns:
//  TRUE                On success
//  FALSE               On memory allocation error or invalid parameters
//
//*************************************************************************

BOOL CPackageSamples::SetProductOSField
(
    int                 nProductID
)
{
    // Validate parameter

    if (nProductID <= 0 || nProductID >= IDS_SDPACK_COUNT)
        return(FALSE);

    return(TRUE);
    return SetProductOSField(gastProductList[nProductID].eProduct,
                             gastProductList[nProductID].nVersion,
                             gastProductList[nProductID].eOS,
                             gastProductList[nProductID].ePlatform);
}


//*************************************************************************
//
// Function:
//  BOOL CPackageSamples::SetProductOSField
//
// Parameters:
//  eProduct            Product
//  nVersion            Version in form M*NN (i.e., 301 = 3.01)
//  eOS                 Operating system of product
//  ePlatform           Platform of OS
//
// Description:
//  Forms a string out of the parameters and adds it to the list
//  of products.
//
// Returns:
//  TRUE                On success
//  FALSE               On memory allocation error or invalid parameters
//
//*************************************************************************

BOOL CPackageSamples::SetProductOSField
(
    PRODUCT             eProduct,
    int                 nVersion,
    OS                  eOS,
    PLATFORM            ePlatform
)
{
    TCHAR               szBuf[256];

    // Validate product

    if (eProduct < OtherProduct || eProduct >= ProductReserved)
        return(FALSE);

    // Validate OS

    if (eOS < OtherOS || eOS >= OSReserved)
        return(FALSE);

    // Validate Platform

    if (ePlatform < OtherPlatform || ePlatform >= PlatformReserved)
        return(FALSE);

    // Form the string

    wsprintf(szBuf,"%s %d %s %s",
        galpszProduct[eProduct],
        nVersion,
        galpszOS[eOS],
        galpszPlatform[ePlatform]);

    // Add it to the list

    return SetInfoField(ProductOS,
                        szBuf);
}


//*************************************************************************
//
// Function:
//  void CPackageSamples::InitData()
//
// Parameters:
//  None
//
// Description:
//  Initializes all data fields.  The function should not be called
//  if any data fields are pointers to as yet unfreed memory.
//
// Returns:
//  Nothing
//
//*************************************************************************

void CPackageSamples::InitData(void)
{
    PSDPACK_INFO        pstInfo;
    int                 i;

    if (m_lpvData == NULL)
        return;

    pstInfo = (PSDPACK_INFO)m_lpvData;

    // Package name

    pstInfo->lpszPackageName = NULL;

    // Initialize all fields

    for (i=0;i<FieldReserved;i++)
    {
        pstInfo->alpstFields[i] = NULL;
        pstInfo->alpstFieldTails[i] = NULL;
    }

    pstInfo->dwTotalFileSize = 0;
    pstInfo->dwTotalFileCount = 0;
    pstInfo->lpstFileNames = NULL;
    pstInfo->lpstGUIDs = NULL;
    pstInfo->lpstMachineNames = NULL;
    pstInfo->lpstOriginalNames = NULL;
    pstInfo->lpstFileNamesTail = NULL;
    pstInfo->lpstGUIDsTail = NULL;
    pstInfo->lpstMachineNamesTail = NULL;
    pstInfo->lpstOriginalNamesTail = NULL;
    pstInfo->lpstCategories = NULL;
    pstInfo->lpstCategoriesTail = NULL;
    pstInfo->lpstMd5 = NULL;
    pstInfo->lpstMd5Tail = NULL;
    pstInfo->lpstVID = NULL;
    pstInfo->lpstVIDTail = NULL;
    pstInfo->lpstVirName = NULL;
    pstInfo->lpstVirNameTail = NULL;

    pstInfo->szSARCTXTFileName[0] = 0;
}


//*************************************************************************
//
// Function:
//  void CPackageSamples::FreeData()
//
// Parameters:
//  None
//
// Description:
//  Frees and initializes all data fields.
//
// Returns:
//  Nothing
//
//*************************************************************************

void CPackageSamples::FreeData
(
    void
)
{
    PSDPACK_INFO        pstInfo;
    int                 i;
    PSDPACK_STR         pstStr;
    PSDPACK_STR         pstNext;
    PSDPACK_GUID        pstGUID;
    PSDPACK_GUID        pstNextGUID;

    if (m_lpvData == NULL)
        return;

    pstInfo = (PSDPACK_INFO)m_lpvData;

    // Free package name

    if (pstInfo->lpszPackageName != NULL)
        delete pstInfo->lpszPackageName;

    // Free field info

    for (i=OtherField;i<FieldReserved;i++)
    {
        pstStr = pstInfo->alpstFields[i];
        while (pstStr != NULL)
        {
            pstNext = pstStr->pstNext;
            if( pstStr->lpsz)
                delete pstStr->lpsz;
            delete pstStr;
            pstStr = pstNext;
        }
    }

    // Free file name list

    pstStr = pstInfo->lpstFileNames;
    while (pstStr != NULL)
    {
        pstNext = pstStr->pstNext;
        if( pstStr->lpsz )
            delete pstStr->lpsz;
        delete pstStr;
        pstStr = pstNext;
    }

    // Free GUID list

    pstGUID = pstInfo->lpstGUIDs;
    while (pstGUID != NULL)
    {
        pstNextGUID = pstGUID->pstNext;
        delete pstGUID;
        pstGUID = pstNextGUID;
    }

    // Free machine name list

    pstStr = pstInfo->lpstMachineNames;
    while (pstStr != NULL)
    {
        pstNext = pstStr->pstNext;
        if( pstStr->lpsz )
            delete pstStr->lpsz;
        delete pstStr;
        pstStr = pstNext;
    }

    // Free original name list

    pstStr = pstInfo->lpstOriginalNames;
    while (pstStr != NULL)
    {
        pstNext = pstStr->pstNext;
        if( pstStr->lpsz )
            delete pstStr->lpsz;
        delete pstStr;
        pstStr = pstNext;
    }

    // Free vid list

    pstStr = pstInfo->lpstVID;
    while (pstStr != NULL)
    {
        pstNext = pstStr->pstNext;
        if( pstStr->lpsz )
            delete pstStr->lpsz;
        delete pstStr;
        pstStr = pstNext;
    }

    // Free categories list

    pstStr = pstInfo->lpstCategories;
    while (pstStr != NULL)
    {
        pstNext = pstStr->pstNext;
        if( pstStr->lpsz )
            delete pstStr->lpsz;
        delete pstStr;
        pstStr = pstNext;
    }

    // Free Md5 list

    pstStr = pstInfo->lpstMd5;
    while (pstStr != NULL)
    {
        pstNext = pstStr->pstNext;
        if( pstStr->lpsz )
            delete pstStr->lpsz;
        delete pstStr;
        pstStr = pstNext;
    }

    // Free Virus Name list

    pstStr = pstInfo->lpstVirName;
    while (pstStr != NULL)
    {
        pstNext = pstStr->pstNext;
        if( pstStr->lpsz )
            delete pstStr->lpsz;
        delete pstStr;
        pstStr = pstNext;
    }

    // Delete SARCTXT if it exists

    if (pstInfo->szSARCTXTFileName[0] != 0)
        DeleteFile(pstInfo->szSARCTXTFileName);

    // Initialize the structure

    InitData();
}


//*************************************************************************
//
// Function:
//  FIELD CPackageSamples::ValidateInfoFields
//
// Parameters:
//  None
//
// Description:
//  Validates the fields.  Here are the criteria:
//
//      Name                        Required for tracking
//      Company                     Required to track and prioritize
//      Platinum/Gold support #     Required to track and prioritize
//                                  (If the user doesn't have a support
//                                   # they are given standard
//                                   "customer" priority)
//      Case #                      (optional)
//      Address                     (optional)
//      City                        (optional)
//      State/Province              (optional)
//      Zip code                    (optional)
//      Country                     Required for prioritization
//      Phone                       (optional)
//      Fax                         (optional)
//      Email                       Required for definition response
//      Operating system            Required for definition response
//      NAV Versions                [user should be able to ask for
//                                   additional intelligent updaters in
//                                   addition to an atomic package]
//      Virus definition date       [required if we want to do
//                                  Incremental update???]
//      State/Province of infection (optional)
//      Country of infection        Required for prioritization
//      Symptoms                    (optional)
//      Type of infection(s)        Optional
//      Language of product         Required
//
// Returns:
//  OtherField      If all fields are TRUE
//  FIELD           First invalid field encountered
//
//*************************************************************************

CPackageSamples::FIELD CPackageSamples::ValidateInfoFields
(
    void
)
{
    PSDPACK_INFO        pstInfo;

    if (m_lpvData == NULL)
        return(NoField);

    pstInfo = (PSDPACK_INFO)m_lpvData;

    // Require a name

    if (pstInfo->alpstFields[FirstName] == NULL)
        return(FirstName);

    if (pstInfo->alpstFields[LastName] == NULL)
        return(LastName);

    // Require country

    if (pstInfo->alpstFields[Country] == NULL)
        return(Country);

    // Require email

    if (pstInfo->alpstFields[Email] == NULL)
        return(Country);

    // Require product OS

    if (pstInfo->alpstFields[ProductOS] == NULL)
        return(ProductOS);

    // Require infection country

    if (pstInfo->alpstFields[InfectionCountry] == NULL)
        return(InfectionCountry);

    // Require language

    if (pstInfo->alpstFields[Language] == NULL)
        return(Language);

    return(NoField);
}


//*************************************************************************
//
// Function:
//  PACKAGE_STATUS CPackageSamples::CreateSARCTXT()
//
// Parameters:
//  None
//
// Description:
//  Creates a temporary file containing the contents of SARC.TXT.
//
// Returns:
//  PACKAGE_OK              On success
//  PACKAGE_GENERAL_ERROR   On error
//
//*************************************************************************

CPackageSamples::PACKAGE_STATUS CPackageSamples::CreateSARCTXT
(
    void
)
{
    PSDPACK_INFO        pstInfo;
    HANDLE              hFile;
    TCHAR               szBuf[256];
    DWORD               dwBytesToWrite;
    DWORD               dwBytesWritten;
    int                 nField;
    PSDPACK_STR         pstStr;
    PSDPACK_STR         pstMachineNameStr;
    PSDPACK_STR         pstOriginalNameStr;
    PSDPACK_GUID        pstGUID;
    DWORD               dwCRC;
    PSDPACK_STR         pstVID;
    PSDPACK_STR         pstCategories;
    PSDPACK_STR         pstMd5;
    PSDPACK_STR         pstVirName;

    if (m_lpvData == NULL)
        return(PACKAGE_GENERAL_ERROR);

    pstInfo = (PSDPACK_INFO)m_lpvData;

    // Create the temporary file is there isn't already one

    if (pstInfo->szSARCTXTFileName[0] == 0)
    {
        DWORD           dwTemp;
        TCHAR           szTempPath[MAX_PATH];

        dwTemp = GetTempPath(MAX_PATH,szTempPath);

        if (dwTemp == 0 || dwTemp > MAX_PATH)
        {
            // GetTempPath() failed or the buffer is not large enough

            return(PACKAGE_GENERAL_ERROR);
        }

        if (GetTempFileName(szTempPath,
                            "NSD",
                            0,
                            pstInfo->szSARCTXTFileName) == 0)
        {
            // Failed to get temporary name

            return(PACKAGE_GENERAL_ERROR);
        }
    }

    // Open the file for writing

    hFile = CreateFile(pstInfo->szSARCTXTFileName,
                       GENERIC_WRITE,
                       0,
                       NULL,
                       CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return(PACKAGE_GENERAL_ERROR);

    // Initialize CRC

    dwCRC = SDPACK_VERSION;;

    ////////////////////////////////////////////////////////
    // Write header

    wsprintf(szBuf,"!%08lX!\r\n",SDPACK_VERSION);
    dwBytesToWrite = strlen(szBuf);
    dwCRC = CRC32Compute(dwBytesToWrite,(LPBYTE)szBuf,dwCRC);
    if (WriteFile(hFile,
                  szBuf,
                  dwBytesToWrite,
                  &dwBytesWritten,
                  NULL) == 0)
    {
        CloseHandle(hFile);
        return(PACKAGE_GENERAL_ERROR);
    }

    ////////////////////////////////////////////////////////
    // Write all fields

    for (nField=OtherField;nField<FieldReserved;nField++)
    {
        // Write start delimiter

        wsprintf(szBuf,"#%08lX:%s#\r\n",SDPACK_VERSION,galpszField[nField]);
        dwBytesToWrite = strlen(szBuf);
        dwCRC = CRC32Compute(dwBytesToWrite,(LPBYTE)szBuf,dwCRC);
        if (WriteFile(hFile,
                      szBuf,
                      dwBytesToWrite,
                      &dwBytesWritten,
                      NULL) == 0)
        {
            CloseHandle(hFile);
            return(PACKAGE_GENERAL_ERROR);
        }

        // Write all fields of the string

        pstStr = pstInfo->alpstFields[nField];
        while (pstStr != NULL)
        {
            dwBytesToWrite = strlen(pstStr->lpsz);
            dwCRC = CRC32Compute(dwBytesToWrite,(LPBYTE)pstStr->lpsz,dwCRC);
            if (WriteFile(hFile,
                          pstStr->lpsz,
                          dwBytesToWrite,
                          &dwBytesWritten,
                          NULL) == 0)
            {
                CloseHandle(hFile);
                return(PACKAGE_GENERAL_ERROR);
            }

            dwCRC = CRC32Continue(dwCRC,'\r');
            dwCRC = CRC32Continue(dwCRC,'\n');
            if (WriteFile(hFile,
                          "\r\n",
                          2,
                          &dwBytesWritten,
                          NULL) == 0)
            {
                CloseHandle(hFile);
                return(PACKAGE_GENERAL_ERROR);
            }

            pstStr = pstStr->pstNext;
        }

        // Write end delimiter

        wsprintf(szBuf,"#~%08lX:%s#\r\n",SDPACK_VERSION,galpszField[nField]);
        dwBytesToWrite = strlen(szBuf);
        dwCRC = CRC32Compute(dwBytesToWrite,(LPBYTE)szBuf,dwCRC);
        if (WriteFile(hFile,
                      szBuf,
                      dwBytesToWrite,
                      &dwBytesWritten,
                      NULL) == 0)
        {
            CloseHandle(hFile);
            return(PACKAGE_GENERAL_ERROR);
        }
    }

    ////////////////////////////////////////////////////////
    // Write file listing

    // Write file listing start delimiter

    wsprintf(szBuf,"#%08lX:FileList#\r\n",SDPACK_VERSION);
    dwBytesToWrite = strlen(szBuf);
    dwCRC = CRC32Compute(dwBytesToWrite,(LPBYTE)szBuf,dwCRC);
    if (WriteFile(hFile,
                  szBuf,
                  dwBytesToWrite,
                  &dwBytesWritten,
                  NULL) == 0)
    {
        CloseHandle(hFile);
        return(PACKAGE_GENERAL_ERROR);
    }

    // Write file count

    wsprintf(szBuf,"%lu\r\n",pstInfo->dwTotalFileCount);
    dwBytesToWrite = strlen(szBuf);
    dwCRC = CRC32Compute(dwBytesToWrite,(LPBYTE)szBuf,dwCRC);
    if (WriteFile(hFile,
                  szBuf,
                  dwBytesToWrite,
                  &dwBytesWritten,
                  NULL) == 0)
    {
        CloseHandle(hFile);
        return(PACKAGE_GENERAL_ERROR);
    }

    // Write files

    pstGUID = pstInfo->lpstGUIDs;
    pstStr = pstInfo->lpstFileNames;
    pstMachineNameStr = pstInfo->lpstMachineNames;
    pstOriginalNameStr = pstInfo->lpstOriginalNames;
    pstVID = pstInfo->lpstVID;
    pstCategories = pstInfo->lpstCategories;
    pstMd5 = pstInfo->lpstMd5;
    pstVirName = pstInfo->lpstVirName;
    while (pstStr != NULL)
    {
        // GUID

        wsprintf(szBuf,"%08lX-%04X-%04X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X",
            pstGUID->stGUID.Data1,
            pstGUID->stGUID.Data2,
            pstGUID->stGUID.Data3,
            pstGUID->stGUID.Data4[0],
            pstGUID->stGUID.Data4[1],
            pstGUID->stGUID.Data4[2],
            pstGUID->stGUID.Data4[3],
            pstGUID->stGUID.Data4[4],
            pstGUID->stGUID.Data4[5],
            pstGUID->stGUID.Data4[6],
            pstGUID->stGUID.Data4[7]);
        dwBytesToWrite = strlen(szBuf);
        dwCRC = CRC32Compute(dwBytesToWrite,(LPBYTE)szBuf,dwCRC);
        if (WriteFile(hFile,
                      szBuf,
                      dwBytesToWrite,
                      &dwBytesWritten,
                      NULL) == 0)
        {
            CloseHandle(hFile);
            return(PACKAGE_GENERAL_ERROR);
        }

        dwCRC = CRC32Continue(dwCRC,'\r');
        dwCRC = CRC32Continue(dwCRC,'\n');
        if (WriteFile(hFile,
                      "\r\n",
                      2,
                      &dwBytesWritten,
                      NULL) == 0)
        {
            CloseHandle(hFile);
            return(PACKAGE_GENERAL_ERROR);
        }

        // File name
        WriteString(hFile, pstStr);

        // Machine name
        WriteString(hFile, pstMachineNameStr);

        // Original name
        WriteString(hFile, pstOriginalNameStr);

        // VID
        WriteString(hFile, pstVID);

        // Virus Name
        WriteString(hFile, pstVirName);

        // Categories
        WriteString(hFile, pstCategories);

        // Md5 hashes
        WriteString(hFile, pstMd5);

        pstGUID = pstGUID->pstNext;
        pstStr = pstStr->pstNext;
        pstMachineNameStr = pstMachineNameStr->pstNext;
        pstOriginalNameStr = pstOriginalNameStr->pstNext;
        pstVID = pstVID->pstNext;
        pstCategories = pstCategories->pstNext;
        pstMd5 = pstMd5->pstNext;
        pstVirName = pstVirName->pstNext;
    }

    // Write file listing end delimiter

    wsprintf(szBuf,"#~%08lX:FileList#\r\n",SDPACK_VERSION);
    dwBytesToWrite = strlen(szBuf);
    dwCRC = CRC32Compute(dwBytesToWrite,(LPBYTE)szBuf,dwCRC);
    if (WriteFile(hFile,
                  szBuf,
                  dwBytesToWrite,
                  &dwBytesWritten,
                  NULL) == 0)
    {
        CloseHandle(hFile);
        return(PACKAGE_GENERAL_ERROR);
    }

    ////////////////////////////////////////////////////////
    // Write footer

    wsprintf(szBuf,"!~%08lX!\r\n",SDPACK_VERSION);
    dwBytesToWrite = strlen(szBuf);
    dwCRC = CRC32Compute(dwBytesToWrite,(LPBYTE)szBuf,dwCRC);
    if (WriteFile(hFile,
                  szBuf,
                  dwBytesToWrite,
                  &dwBytesWritten,
                  NULL) == 0)
    {
        CloseHandle(hFile);
        return(PACKAGE_GENERAL_ERROR);
    }

    ////////////////////////////////////////////////////////
    // Write CRC

    dwCRC = (dwCRC << (SDPACK_VERSION & 0x1F)) |
        (dwCRC >> (32 - (SDPACK_VERSION & 0x1F)));
    wsprintf(szBuf,"%08lX",dwCRC);
    if (WriteFile(hFile,
                  szBuf,
                  8,
                  &dwBytesWritten,
                  NULL) == 0)
    {
        CloseHandle(hFile);
        return(PACKAGE_GENERAL_ERROR);
    }

    // Close the file

    CloseHandle(hFile);

    return(PACKAGE_OK);
}

BOOL WriteString(HANDLE hFile, PSDPACK_STR pstrToWrite)
{
    DWORD dwBytesToWrite = strlen(pstrToWrite->lpsz);
    DWORD dwCRC = 0;
    dwCRC = CRC32Compute(dwBytesToWrite,
                            (LPBYTE)pstrToWrite->lpsz,dwCRC);
    DWORD dwBytesWritten = 0;

    if (WriteFile(hFile,
                    pstrToWrite->lpsz,
                    dwBytesToWrite,
                    &dwBytesWritten,
                    NULL) == 0)
    {
        CloseHandle(hFile);
        return(FALSE);
    }

    dwCRC = CRC32Continue(dwCRC,'\r');
    dwCRC = CRC32Continue(dwCRC,'\n');
    if (WriteFile(hFile,
                    "\r\n",
                    2,
                    &dwBytesWritten,
                    NULL) == 0)
    {
        CloseHandle(hFile);
        return(FALSE);
    }

    return(TRUE);
}

//*************************************************************************
//
// Function:
//  PACKAGE_STATUS CPackageSamples::CreatePackage
//
// Parameters:
//  lpfnProgressCB              Ptr to progress callback
//  lpvProgressCookie           Progress callback cookie
//
// Description:
//  ZIPs SARC.TXT and the file all into one package.
//
// Returns:
//  PACKAGE_OK              On success
//  PACKAGE_GENERAL_ERROR   On error
//
//*************************************************************************

CPackageSamples::PACKAGE_STATUS CPackageSamples::CreatePackage
(
    PPackageProgressCB  lpfnProgressCB,
    LPVOID              lpvProgressCookie
)
{
    PSDPACK_INFO        pstInfo;
    PSDPACK_STR         pstStr;
    int                 nResult;
    ALPkArchive *       pArchive;
    SDPackALMonitor *   pMonitor;
    ALEntryList *       pEntryList;

    if (m_lpvData == NULL)
        return(PACKAGE_GENERAL_ERROR);

    pstInfo = (PSDPACK_INFO)m_lpvData;

    pArchive = new ALPkArchive(pstInfo->lpszPackageName);
    if (pArchive == NULL)
        return(PACKAGE_GENERAL_ERROR);

    pMonitor = new SDPackALMonitor(pArchive,lpfnProgressCB,lpvProgressCookie);
    if (pMonitor == NULL)
    {
        delete pArchive;
        return(PACKAGE_GENERAL_ERROR);
    }

    pEntryList = new ALEntryList(pMonitor,PkCompressTools());
    if (pEntryList == NULL)
    {
        delete pMonitor;
        delete pArchive;
        return(PACKAGE_GENERAL_ERROR);
    }

    // Add SARCTXT to list

    if (pEntryList->AddFile(pstInfo->szSARCTXTFileName) != 1)
    {
        delete pEntryList;
        delete pMonitor;
        delete pArchive;
        return(PACKAGE_GENERAL_ERROR);
    }

    // Add file names to list

    pstStr = pstInfo->lpstFileNames;
    while (pstStr != NULL)
    {
        if (pEntryList->AddFile(pstStr->lpsz) != 1)
        {
            delete pEntryList;
            delete pMonitor;
            delete pArchive;
            return(PACKAGE_GENERAL_ERROR);
        }

        pstStr = pstStr->pstNext;
    }

    // Mark all entries

    pEntryList->SetMarks();

    // Compress

    pArchive->ClearError();
    nResult = pArchive->Create(*pEntryList);

    delete pEntryList;
    delete pMonitor;
    delete pArchive;

    // Was the operation canceled?

    if (nResult == AL_USER_ABORT)
        return(PACKAGE_CANCELED);

    // Was the operation successful?

    if (nResult != pstInfo->dwTotalFileCount + 1)
        return(PACKAGE_GENERAL_ERROR);

    return(PACKAGE_OK);
}


//*************************************************************************
//
// Function:
//  PACKAGE_STATUS CPackageSamples::RenameSARCTXT
//
// Parameters:
//  None
//
// Description:
//  Renames the first directory entry to sarc.txt.
//
// Returns:
//  PACKAGE_OK              On success
//  PACKAGE_GENERAL_ERROR   On error
//
//*************************************************************************

CPackageSamples::PACKAGE_STATUS CPackageSamples::RenameSARCTXT
(
    void
)
{
    PSDPACK_INFO        pstInfo;
    int                 nResult;
    ALPkArchive *       pArchive;
    ALEntryList *       pEntryList;

    if (m_lpvData == NULL)
        return(PACKAGE_GENERAL_ERROR);

    pstInfo = (PSDPACK_INFO)m_lpvData;

    pArchive = new ALPkArchive(pstInfo->lpszPackageName);
    if (pArchive == NULL)
        return(PACKAGE_GENERAL_ERROR);

    pEntryList = new ALEntryList(NULL,PkCompressTools());
    if (pEntryList == NULL)
    {
        delete pArchive;
        return(PACKAGE_GENERAL_ERROR);
    }

    // Rename the first entry to SARC.TXT

    nResult = pArchive->ReadDirectory(*pEntryList);
    if (nResult != AL_SUCCESS)
    {
        delete pEntryList;
        delete pArchive;
        return(PACKAGE_GENERAL_ERROR);
    }

    // Change the name of the first entry to sarc.txt

    pEntryList->GetFirstEntry()->mpStorageObject->mName = "sarc.txt";

    // Write back the directory

    if (pArchive->WriteDirectory(*pEntryList) != AL_SUCCESS)
    {
        delete pEntryList;
        delete pArchive;
        return(PACKAGE_GENERAL_ERROR);
    }

    delete pEntryList;
    delete pArchive;

    return(PACKAGE_OK);
}


//*************************************************************************
//
// Function:
//  PACKAGE_STATUS CPackageSamples::EncryptPackage
//
// Parameters:
//  None
//
// Description:
//  Encrypts the package by XORing each byte with SDPACK_ENCRYPT_ZIP_BYTE.
//
// Returns:
//  PACKAGE_OK              On success
//  PACKAGE_GENERAL_ERROR   On error
//
//*************************************************************************

CPackageSamples::PACKAGE_STATUS CPackageSamples::EncryptPackage
(
    void
)
{
    PSDPACK_INFO        pstInfo;
    HANDLE              hFile;
    DWORD               dwOffset;
    DWORD               dwSize;
    DWORD               dwBytesToEncrypt;
    DWORD               dwTemp;
    BYTE                abyBuf[4096];

    if (m_lpvData == NULL)
        return(PACKAGE_GENERAL_ERROR);

    pstInfo = (PSDPACK_INFO)m_lpvData;

    // Open the file for reading/writing

    hFile = CreateFile(pstInfo->lpszPackageName,
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return(PACKAGE_GENERAL_ERROR);

    // Get the size

    dwSize = GetFileSize(hFile,NULL);
    if (dwSize == 0xFFFFFFFF)
    {
        CloseHandle(hFile);
        return(PACKAGE_GENERAL_ERROR);
    }

    // Encrypt the file

    dwBytesToEncrypt = sizeof(abyBuf);
    dwOffset = 0;
    while (dwOffset < dwSize)
    {
        if (dwSize - dwOffset < dwBytesToEncrypt)
            dwBytesToEncrypt = dwSize - dwOffset;

        // Read a chunk

        dwTemp = SetFilePointer(hFile,
                                dwOffset,
                                NULL,
                                FILE_BEGIN);

        if (dwTemp != dwOffset)
        {
            CloseHandle(hFile);
            return(PACKAGE_GENERAL_ERROR);
        }

        if (ReadFile(hFile,
                     abyBuf,
                     dwBytesToEncrypt,
                     &dwTemp,
                     NULL) == FALSE ||
            dwTemp != dwBytesToEncrypt)
        {
            CloseHandle(hFile);
            return(PACKAGE_GENERAL_ERROR);
        }

        // Encrypt chunk

        for (dwTemp=0;dwTemp<dwBytesToEncrypt;dwTemp++)
            abyBuf[dwTemp] ^= SDPACK_ZIP_ENCRYPT_BYTE;

        // Write the chunk

        dwTemp = SetFilePointer(hFile,
                                dwOffset,
                                NULL,
                                FILE_BEGIN);

        if (dwTemp != dwOffset)
        {
            CloseHandle(hFile);
            return(PACKAGE_GENERAL_ERROR);
        }

        if (WriteFile(hFile,
                      abyBuf,
                      dwBytesToEncrypt,
                      &dwTemp,
                      NULL) == FALSE ||
            dwTemp != dwBytesToEncrypt)
        {
            CloseHandle(hFile);
            return(PACKAGE_GENERAL_ERROR);
        }

        // Update offset

        dwOffset += dwBytesToEncrypt;
    }

    CloseHandle(hFile);

    return(PACKAGE_OK);
}


//*************************************************************************
//
// Function:
//  PACKAGE_STATUS CPackageSamples::CreatePreSubmissionData
//
// Parameters:
//  lplpbyPreSubmissionData     Ptr to ptr for pre-submission data
//
// Description:
//  Creates the presubmission data.  If lplpbyPreSubmissionData is
//  NULL, the function just returns success, assuming that the
//  presubmission data is not needed.
//
// Returns:
//  PACKAGE_OK              On success
//  PACKAGE_GENERAL_ERROR   On error
//
//*************************************************************************

#include "hashpack.h"

CPackageSamples::PACKAGE_STATUS CPackageSamples::CreatePreSubmissionData
(
    LPBYTE *            lplpbyPreSubmissionData
)
{
    PSDPACK_INFO        pstInfo;
    PSDPACK_STR         pstStr;
    LPBYTE              lpbyData;
    DWORD               dwSize;
    PPRE_SUBMISSION_HDR pstHdr;
    PPRE_PACKET         pstPacket;
    int                 i;
    int                 nHashPackCount;

    if (lplpbyPreSubmissionData == NULL)
        return(PACKAGE_OK);

    *lplpbyPreSubmissionData = NULL;

    if (m_lpvData == NULL)
        return(PACKAGE_GENERAL_ERROR);

    pstInfo = (PSDPACK_INFO)m_lpvData;

    // Get number of hash packets to create

    if (pstInfo->dwTotalFileCount > MAX_SUBMISSION_PACKETS)
        nHashPackCount = MAX_SUBMISSION_PACKETS;
    else
        nHashPackCount = (int)pstInfo->dwTotalFileCount;

    // Allocate memory for submission data

    dwSize = sizeof(PRE_SUBMISSION_HDR_T) +
        nHashPackCount * sizeof(PRE_PACKET_T);

    lpbyData = new BYTE[dwSize];
    if (lpbyData == NULL)
        return(PACKAGE_GENERAL_ERROR);

    // Initialize header

    pstHdr = (PPRE_SUBMISSION_HDR)lpbyData;
    pstHdr->dwType = PRE_SUBMISSION_HEADER;
    pstHdr->dwVersion = PRE_SUBMISSION_VERSION;
    pstHdr->dwSize = dwSize - 3 * sizeof(DWORD);
    pstHdr->dwTotalNumFiles = pstInfo->dwTotalFileCount;
    pstHdr->dwNumPackets = nHashPackCount;

    // Iterate through items

    pstPacket = (PPRE_PACKET)(pstHdr+1);
    pstStr = pstInfo->lpstFileNames;
    for (i=0;i<nHashPackCount;i++)
    {
        pstPacket->dwType = HASH_PACKET_TYPE;
        pstPacket->dwVersion = PRE_SUBMISSION_VERSION;
        pstPacket->dwSize = SUBMIT_BUFFER_SIZE;

        if (PreSubmitDataFill(pstStr->lpsz,
                              pstPacket->byTrojanData,
                              SUBMIT_BUFFER_SIZE) == FALSE)
        {
            delete lpbyData;
            return(PACKAGE_GENERAL_ERROR);
        }

        ++pstPacket;
        pstStr = pstStr->pstNext;
    }

    // Store buffer ptr

    *lplpbyPreSubmissionData = lpbyData;

    return(PACKAGE_OK);
}

