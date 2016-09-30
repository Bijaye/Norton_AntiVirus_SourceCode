#pragma once

#include <strsafe.h>

class CScanManagerCounts
{
public:
    enum eSMCountType
    {
        FilesScanned = 0,
        Viral,
        NonViral,
        Repaired,
        Quarantined,
        ViralDeleted,
        NonViralDeleted,
        NonViralExcluded,
        NonViralExcludedOnce,
        MBRScanned,
        MBRInfected,
        MBRRepaired,
        BRScanned,
        BRInfected,
        BRRepaired,
        Memory9x
    };

    CScanManagerCounts(void);
    ~CScanManagerCounts(void);

    void Reset();

    // Support to turn off updates for the scanned files count, useful when doing eraser
    // scanning and we do not want the OnNewFile() callback to actually bump up the scanned
    // files count
    void DoNotUpdateFileScannedCount();
    void UpdateFileScannedCount();

    // Accessor functions for the internal count values
    DWORD GetItemCount(eSMCountType);
    void IncrementItemCount(eSMCountType);

    // Shortcuts for detected counts
    DWORD GetTotalItemsDetected();
    DWORD GetViralItemsDetected();
    
    // Shortcuts for resolved counts
    DWORD GetTotalItemsResolved();
    DWORD GetViralItemsResolved();
    DWORD GetNonViralItemsResolved();
    DWORD GetTotalItemsDeleted();

    // Need to be able to set the BR\MBR scanned counts directly from
    // the ccScan results
    void SetMbrBrScannedCounts(DWORD dwMBRs, DWORD dwBRs);

    //
    // These are the display text methods to manage what is shown to the user
    // in the Common UI
    //
    void UseInternalText(UINT nResIDTextToUse, long lNumDotsToUse);
    void StopUsingInternalText();
    void GetCurrentItemForDisplay(LPSTR szFolder, long nLength);
    void UpdateCurrentFolder(LPCSTR cszCurrentItem, bool bItemIsOem)
    {
        //
        // The following is "fired" when OnCUIGetCurrentFolder is called.
        // This is highly optimized to prevent the need for synchronization like critical sections
        // as well as frequent heap allocations (where the heap allocation code usually also has 
        // synchronization within it in addition to concerns over heap fragmentation).
        // The implementation is "lazy" in that the UI doesn't have to have the exact current status.
        // For example, when OnCUIGetCurrentFolder is called, it gets the results from the last time
        // this method was called with m_bUpdateCurrentFolder. This prevents us having to new/delete
        // our string buffer that's swapped into an InterlockedExchangeXX() managed pointer.
        //

        if( m_spBatchPaths != NULL )
        {
            // We are performing batch file updating, update the folder name
            // if this is matching something in our batch
            m_bUpdateCurrentFolder = false;
            size_t nCount = m_spBatchPaths->GetCount();
            for( size_t i=0; i<nCount; i++ )
            {
                cc::IStringPtr spPath;
                if( m_spBatchPaths->GetValue(i, reinterpret_cast<ISymBase*&>(spPath)) &&
                    _tcsicmp(spPath->GetStringA(), cszCurrentItem) == 0 )
                {
                    m_bUpdateCurrentFolder = true;
                    m_dwScannedFilesCount++;
                    m_bBatchFileCountAdded = true;
                    break;
                }
            }
        }

        if (m_bUpdateCurrentFolder )
        {
            // "Dequeue" the event.
            m_bUpdateCurrentFolder = false;

            // Grab current value from "managed" pointer.
            LPSTR szCurrent = (LPSTR)InterlockedExchangePointer((PVOID*)&m_szCurrentFolder, NULL);
            // Allocate a string if "managed" pointer is empty.
            if (NULL == szCurrent)
            {
                CCTRACEI(_T("CScanManagerCounts::UpdateCurrentFolder() - Allocating memory for folder name"));
                szCurrent = new char[MAX_PATH];
            }
            // Copy value into string.
            StringCbCopyA(szCurrent, MAX_PATH, cszCurrentItem);
            // Perform OEM conversion, if needed.
            if ( bItemIsOem )
                OemToCharBuff(szCurrent, szCurrent, MAX_PATH);
            // Move string back into "managed" pointer.
            szCurrent = (LPSTR)InterlockedExchangePointer((PVOID*)&m_szCurrentFolder, szCurrent);
            if (NULL != szCurrent)
                delete [] szCurrent;
        }
    }

    void UseBatchFolderUpdating(cc::IIndexValueCollection* pPaths);
    void EndBatchUpdating();

protected:
    // Counts to keep track of processing
    DWORD m_dwScannedFilesCount;
    DWORD m_dwViralCount, m_dwRepairCount, m_dwQuarantineCount, m_dwViralDeleteCount;
    DWORD m_dwNonViralCount, m_dwNonViralDeleteCount, m_dwNonViralExcludeCount, m_dwNonViralExcludeOnceCount;

    // BR\MBR\9x memory counts
    DWORD m_dwMBRsRepaired, m_dwMBRsTotalInfected, m_dwMBRsTotalScanned, // MBR results
        m_dwBootRecsRepaired, m_dwBootRecsTotalInfected, m_dwBootRecsTotalScanned, // BR results
        m_dw9xMemInfections;

    // Should the file count updating be disabled temporarily?
    bool m_bUpdateFileCounts;

    //
    // Current Folder status string handling.
    // This is designed to optimize multi-threaded performance by avoiding critical sections
    // and also reducing heap fragmentation. Refer to usage for more details.
    //
    bool m_bUpdateCurrentFolder;
    LPSTR m_szCurrentFolder;

    // Special text to display during eraser scanning
    bool m_bDisplayInternalText;
    ATL::CAtlString m_strInternalText;
    long m_lNumDotsToUse;
    long m_lDotsCurrentlyUsed;

    cc::IIndexValueCollectionPtr m_spBatchPaths;

    bool m_bBatchFileCountAdded;

	CString m_strProductName;
};
