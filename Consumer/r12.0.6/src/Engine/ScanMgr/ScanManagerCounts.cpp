#include "stdafx.h"
#include "scanmanagercounts.h"
#include "resource.h"
#include "AVRESBranding.h"

CScanManagerCounts::CScanManagerCounts(void) : m_bUpdateCurrentFolder(true),
                                               m_szCurrentFolder(NULL),
                                               m_bDisplayInternalText(false),
                                               m_lNumDotsToUse(0),
                                               m_bUpdateFileCounts(true),
                                               m_dwScannedFilesCount(0),
                                               m_dwViralCount(0),
                                               m_dwRepairCount(0),
                                               m_dwQuarantineCount(0),
                                               m_dwViralDeleteCount(0),
                                               m_dwNonViralCount(0),
                                               m_dwNonViralDeleteCount(0),
                                               m_dwNonViralExcludeCount(0),
                                               m_dwNonViralExcludeOnceCount(0),
                                               m_dwMBRsRepaired(0),
                                               m_dwMBRsTotalInfected(0),
                                               m_dwMBRsTotalScanned(0),
                                               m_dwBootRecsRepaired(0),
                                               m_dwBootRecsTotalInfected(0),
                                               m_dwBootRecsTotalScanned(0),
                                               m_dw9xMemInfections(0),
                                               m_bBatchFileCountAdded(false)
{
	CBrandingRes BrandRes;
	m_strProductName = BrandRes.ProductName();
}

CScanManagerCounts::~CScanManagerCounts(void)
{
    // Free the folder string
    if( m_szCurrentFolder != NULL )
    {
        delete [] m_szCurrentFolder;
        m_szCurrentFolder = NULL;
    }
}

void CScanManagerCounts::DoNotUpdateFileScannedCount()
{
    m_bUpdateFileCounts = false;
}

void CScanManagerCounts::UpdateFileScannedCount()
{
    m_bUpdateFileCounts = true;
}

DWORD CScanManagerCounts::GetItemCount(eSMCountType count)
{
    switch(count)
    {
    case FilesScanned:
        return m_dwScannedFilesCount;
    case Viral:
        return m_dwViralCount;
    case NonViral:
        return m_dwNonViralCount;
    case Repaired:
        return m_dwRepairCount;
    case Quarantined:
        return m_dwQuarantineCount;
    case ViralDeleted:
        return m_dwViralDeleteCount;
    case NonViralDeleted:
        return m_dwNonViralDeleteCount;
    case NonViralExcluded:
        return m_dwNonViralExcludeCount;
    case NonViralExcludedOnce:
        return m_dwNonViralExcludeOnceCount;
    case MBRScanned:
        return m_dwMBRsTotalScanned;
    case MBRInfected:
        return m_dwMBRsTotalInfected;
    case MBRRepaired:
        return m_dwMBRsRepaired;
    case BRScanned:
        return m_dwBootRecsTotalScanned;
    case BRInfected:
        return m_dwBootRecsTotalInfected;
    case BRRepaired:
        return m_dwBootRecsRepaired;
    case Memory9x:
        return m_dw9xMemInfections;
    default:
        CCTRACEE(_T("CScanManagerCounts::GetItemCount() - Invalid count type %d"), count);
        return 0;
    }
}

void CScanManagerCounts::IncrementItemCount(eSMCountType count)
{
    switch(count)
    {
    case FilesScanned:
        if( m_bUpdateFileCounts )
            InterlockedIncrement((PLONG)&m_dwScannedFilesCount);
        break;
    case Viral:
        InterlockedIncrement((PLONG)&m_dwViralCount);
        break;
    case NonViral:
        InterlockedIncrement((PLONG)&m_dwNonViralCount);
        break;
    case Repaired:
        InterlockedIncrement((PLONG)&m_dwRepairCount);
        break;
    case Quarantined:
        InterlockedIncrement((PLONG)&m_dwQuarantineCount);
        break;
    case ViralDeleted:
        InterlockedIncrement((PLONG)&m_dwViralDeleteCount);
        break;
    case NonViralDeleted:
        InterlockedIncrement((PLONG)&m_dwNonViralDeleteCount);
        break;
    case NonViralExcluded:
        InterlockedIncrement((PLONG)&m_dwNonViralExcludeCount);
        break;
    case NonViralExcludedOnce:
        InterlockedIncrement((PLONG)&m_dwNonViralExcludeOnceCount);
        break;
    case MBRScanned:
        InterlockedIncrement((PLONG)&m_dwMBRsTotalScanned);
        break;
    case MBRInfected:
        InterlockedIncrement((PLONG)&m_dwMBRsTotalInfected);
        break;
    case MBRRepaired:
        InterlockedIncrement((PLONG)&m_dwMBRsRepaired);
        break;
    case BRScanned:
        InterlockedIncrement((PLONG)&m_dwBootRecsTotalScanned);
        break;
    case BRInfected:
        InterlockedIncrement((PLONG)&m_dwBootRecsTotalInfected);
        break;
    case BRRepaired:
        InterlockedIncrement((PLONG)&m_dwBootRecsRepaired);
        break;
    case Memory9x:
        InterlockedIncrement((PLONG)&m_dw9xMemInfections);
        break;
    default:
        CCTRACEE(_T("CScanManagerCounts::IncrementItemCount() - Invalid count type %d"), count);
        break;
    }
}

DWORD CScanManagerCounts::GetTotalItemsDetected()
{
    return m_dwViralCount + m_dwNonViralCount + 
        m_dwBootRecsTotalInfected + m_dwMBRsTotalInfected +
        m_dw9xMemInfections;
}

DWORD CScanManagerCounts::GetTotalItemsResolved()
{
    return m_dwRepairCount + m_dwQuarantineCount + m_dwViralDeleteCount + 
        m_dwNonViralDeleteCount + m_dwNonViralExcludeCount + m_dwNonViralExcludeOnceCount +
        m_dwBootRecsRepaired + m_dwMBRsRepaired;
}

DWORD CScanManagerCounts::GetTotalItemsDeleted()
{
    return m_dwViralDeleteCount + m_dwNonViralDeleteCount;
}

DWORD CScanManagerCounts::GetViralItemsDetected()
{
    return  m_dwViralCount + m_dwBootRecsTotalInfected + m_dwMBRsTotalInfected +  m_dw9xMemInfections;
}

DWORD CScanManagerCounts::GetViralItemsResolved()
{
    return  m_dwRepairCount + m_dwQuarantineCount + m_dwViralDeleteCount +
            m_dwBootRecsRepaired + m_dwMBRsRepaired;
}

DWORD CScanManagerCounts::GetNonViralItemsResolved()
{
    return m_dwNonViralDeleteCount + m_dwNonViralExcludeCount + m_dwNonViralExcludeOnceCount;
}

void CScanManagerCounts::SetMbrBrScannedCounts(DWORD dwMBRs, DWORD dwBRs)
{
    m_dwMBRsTotalScanned = dwMBRs;
    m_dwBootRecsTotalScanned = dwBRs;
}

void CScanManagerCounts::UseBatchFolderUpdating(cc::IIndexValueCollection* pPaths)
{
    if( pPaths == NULL )
    {
        CCTRACEE(_T("CScanManagerCounts::UseBatchFolderUpdating() - Invalid collection of paths"));
        return;
    }

    // Make sure we have paths
    if( pPaths->GetCount() == 0 )
    {
        CCTRACEE(_T("CScanManagerCounts::UseBatchFolderUpdating() - No paths in collection"));
        return;
    }

    // First put the first path in our list in the UI
    cc::IStringPtr spStrInitialPath;
    if( pPaths->GetValue(0, reinterpret_cast<ISymBase*&>(spStrInitialPath)) )
    {
        m_bUpdateCurrentFolder = true;
        UpdateCurrentFolder(spStrInitialPath->GetStringA(), false);
    }

    // Save off the paths
    m_spBatchPaths = pPaths;

    // Manually control the file count updates when doing batch file updates
    m_bUpdateFileCounts = false;

    // Indicate that a file count for this batch file session has not occurred
    m_bBatchFileCountAdded = false;
}

void CScanManagerCounts::EndBatchUpdating()
{
    if( m_spBatchPaths )
    {
        m_bUpdateFileCounts = true;
        m_bUpdateCurrentFolder = true;
        m_spBatchPaths.Release();

        // If this batch scan had no files actually scanned add a fakey one
        // to make things warm and fuzzy
        if( m_bBatchFileCountAdded == false )
            m_dwScannedFilesCount++;
    }
}

void CScanManagerCounts::GetCurrentItemForDisplay(LPSTR szFolder, long nLength)
{
    if( m_bDisplayInternalText && !m_strInternalText.IsEmpty() )
    {
        // The internal text is not designed to be used in multi-threaded scans
        if( m_lNumDotsToUse > 0 )
        {
            // This will only update the dot animation every other call to this method
            if( m_lDotsCurrentlyUsed % 2 != 0 )
            {
                // Don't add a dot just update the count so it is added
                // or they are removed on the next display call
                m_lDotsCurrentlyUsed++;
            }
            else if( m_lNumDotsToUse*2 == m_lDotsCurrentlyUsed )
            {
                // Once we hit the dot threshold erase them all
                int nFirstDot = m_strInternalText.Find('.');
                m_strInternalText = m_strInternalText.Left(nFirstDot);
                m_lDotsCurrentlyUsed = 0;
            }
            else
            {
                // Append a dot
                m_strInternalText += ". ";
                m_lDotsCurrentlyUsed++;
            }
        }

        // Provide the display string to the caller
        StringCbCopyA(szFolder, nLength, m_strInternalText);
    }
    else
    {
        // Provide the managed string that was provided in the update current folder

        //
        // This is highly optimized to prevent the need for synchronization like critical sections
        // as well as frequent heap allocations (where the heap allocation code usually also has 
        // synchronization within it in addition to concerns over heap fragmentation).
        // The implementation is "lazy" in that the UI doesn't have to have the exact current status.
        // For example, when OnCUIGetCurrentFolder is called, it gets the results from the last time
        // the scan engine called back with a folder update. This prevents us having to new/delete
        // our string buffer that's swapped into an InterlockedExchangeXX() managed pointer.
        //

        // Get "managed" string pointer.
        LPSTR szCurrent = (LPSTR)InterlockedExchangePointer((PVOID*)&m_szCurrentFolder, NULL);
        HRESULT hrResult = S_OK;

        // Copy data out of string.
        if (NULL == szCurrent)
            hrResult = StringCbCopyA(szFolder, nLength, "");
        else
            hrResult = StringCbCopyA(szFolder, nLength, szCurrent);

        // Restore "managed" pointer only if it was non-NULL.
        if (NULL != szCurrent)
            szCurrent = (LPSTR)InterlockedCompareExchangePointer((PVOID*)&m_szCurrentFolder, szCurrent, NULL);
        if (NULL != szCurrent)
            delete [] szCurrent;

        // Queue an update of the current folder string.
        m_bUpdateCurrentFolder = true;
    }
}

void CScanManagerCounts::UseInternalText(UINT nResIDTextToUse, long lNumDotsToUse)
{
    m_bDisplayInternalText = true;
    m_lNumDotsToUse = lNumDotsToUse;
    m_lDotsCurrentlyUsed = 0;

	if(nResIDTextToUse == IDS_GSE_DELAYED_TEXT)
	{
		m_strInternalText.Format(nResIDTextToUse, m_strProductName);
	}
	else
	{
		m_strInternalText.LoadString(nResIDTextToUse);
	}

    // Ignore the folders passed in while we are using internal text
    m_bUpdateCurrentFolder = false;
}

void CScanManagerCounts::StopUsingInternalText()
{
    m_bDisplayInternalText = false;
    m_strInternalText.Empty();
    m_lNumDotsToUse = 0;
    m_lDotsCurrentlyUsed = 0;

    // Make sure the next UpdateCurrentFolder call sets the folder name
    m_bUpdateCurrentFolder = true;
}

void CScanManagerCounts::Reset()
{
    if( m_szCurrentFolder )
    {
        StringCbCopyA(m_szCurrentFolder, 1, "");
    }

    m_bUpdateCurrentFolder = true;
    m_bDisplayInternalText = false;
    m_strInternalText.Empty();
    m_dwScannedFilesCount = 0;
    m_lNumDotsToUse = 0;
    m_lDotsCurrentlyUsed = 0;

    m_bUpdateFileCounts = true;
    m_dwViralCount = 0;
    m_dwRepairCount = 0;
    m_dwQuarantineCount = 0;
    m_dwViralDeleteCount = 0;
    m_dwNonViralCount = 0;
    m_dwNonViralDeleteCount = 0;
    m_dwNonViralExcludeCount = 0;
    m_dwNonViralExcludeOnceCount = 0;
    m_dwMBRsRepaired = 0;
    m_dwMBRsTotalInfected = 0;
    m_dwMBRsTotalScanned = 0;
    m_dwBootRecsRepaired = 0;
    m_dwBootRecsTotalInfected = 0;
    m_dwBootRecsTotalScanned = 0;
    m_dw9xMemInfections = 0;
}