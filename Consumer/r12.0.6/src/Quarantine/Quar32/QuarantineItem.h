//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// QuarantineItem.h: interface for the CQuarantineItem class.
//

#if !defined(AFX_QUARANTINEITEM_H__6F9F4C24_AD73_11D1_9105_00C04FAC114A__INCLUDED_)
#define AFX_QUARANTINEITEM_H__6F9F4C24_AD73_11D1_9105_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IQuaran.h"

#include "qspak.h"
#include "QScanner.h"
#include "SimpleScanner.h"

#include "ccCriticalSection.h"	// ccLib::CCriticalSection

const TCHAR REMEDIATION_ACTION_FILE_NAME_FORMAT[] = _T("%08X.RMA");
const TCHAR REMEDIATION_UNDO_FILE_NAME_FORMAT[] = _T("%08X.URM");
const TCHAR SYSTEM_SNAPSHOT_FILE_NAME_FORMAT[] = _T("SysSnap.SSS");


class CQuarantineItem : public IQuarantineItem,
                        public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(IID_QuarantineItem, IQuarantineItem)
	SYM_INTERFACE_MAP_END()

    // IQuarantineItem
    STDMETHOD(Initialize(LPSTR szQuarantineItemPath));
	
	STDMETHOD(SaveItem());
    STDMETHOD(DeleteItem());
    
	STDMETHOD(ScanMainFile());
    STDMETHOD(RepairAndRestoreMainFileToTemp(char* szDestPath, DWORD dwPathSize));
    
	//STDMETHOD(RestoreItem(LPSTR lpszDestPath, LPSTR lpszDestFilename, BOOL bOverwrite));

	STDMETHOD(UnpackageMainFile(LPSTR lpszDestinationFolder       // [in]
						  , LPSTR lpszDestinationFileName = NULL  // [in]
                          , LPSTR lpszUnpackagedFileName = NULL   // [out]
						  , DWORD dwBufSize = 0                   // [in]
                          , BOOL bOverwrite = FALSE               // [in]
                          , BOOL bMakeTemporaryFile = FALSE));	  // [in]

    STDMETHOD(OriginalFileExists(BOOL*  bResult));
	
	STDMETHOD(RestoreCCGserSideEffects(char* szRestoredFileName));

    // Construction
	CQuarantineItem();
	~CQuarantineItem();


	STDMETHOD(GetString(char* szProperty, char* szValue, DWORD* pulBuffSize));
	STDMETHOD(SetString(char* szProperty, char* szValue, DWORD dwBuffSize));
	STDMETHOD(GetDWORD(char* szProperty, DWORD* dwValue));
	STDMETHOD(SetDWORD(char* szProperty, DWORD dwValue));
    STDMETHOD(GetBinary(char* szProperty, BYTE* pBuff, DWORD* pulBuffSize));
	STDMETHOD(SetBinary(char* szProperty, BYTE* pBuff, DWORD dwBuffSize));
    STDMETHOD(GetDate(char* szProperty, SYSTEMTIME* stTime));
	STDMETHOD(SetDate(char* szProperty, SYSTEMTIME* stTime));
	
	//////////////////////////////////////
	// Per QuarantineItem access functions

    STDMETHOD(GetUniqueID(UUID& uuid));
    STDMETHOD(SetUniqueID(UUID newID));

    STDMETHOD(GetFileStatus(DWORD* fileStatus));
    STDMETHOD(SetFileStatus(DWORD newFileStatus));

    STDMETHOD(GetFileType(DWORD* fileType));
    STDMETHOD(SetFileType(DWORD newFileType));

    STDMETHOD(GetDateQuarantined(SYSTEMTIME* dateQuarantined));
    STDMETHOD(SetDateQuarantined(SYSTEMTIME* newQuarantineDate));

	STDMETHOD(GetOriginalFileDates(SYSTEMTIME* pstOriginalFileDateCreated,
                                     SYSTEMTIME* pstOriginalFileDateAccessed,
                                     SYSTEMTIME* pstOriginalFileDateWritten));
    STDMETHOD(SetOriginalFileDates(SYSTEMTIME* pstOriginalFileDateCreated,
                                     SYSTEMTIME* pstOriginalFileDateAccessed,
                                     SYSTEMTIME* pstOriginalFileDateWritten));

    STDMETHOD(GetDateOfLastScan(SYSTEMTIME* dateLastScanned));
    STDMETHOD(SetDateOfLastScan(SYSTEMTIME* newDateLastScan));
    STDMETHOD(GetDateOfLastScanDefs(SYSTEMTIME* dateOfLastScannedDefs));
    STDMETHOD(SetDateOfLastScanDefs(SYSTEMTIME* newDateLastScanDefs));

	STDMETHOD(GetOriginalScanDefsDate(SYSTEMTIME* dateOfOriginalScannedDefs));

    STDMETHOD(GetDateSubmittedToSARC(SYSTEMTIME* dateSubmittedToSARC));
    STDMETHOD(SetDateSubmittedToSARC(SYSTEMTIME* newDateSubmittedToSARC));

    STDMETHOD(GetOriginalFilesize(DWORD*  originalFileSize));
    STDMETHOD(SetOriginalFilesize(DWORD  newOriginalFileSize));
	
    STDMETHOD(GetOriginalAnsiFilename(char*  szDestBuf, DWORD* bufSize));
    STDMETHOD(SetOriginalAnsiFilename(char*  szNewOriginalAnsiFilename));

    STDMETHOD(GetOriginalOwnerName(char*  szDestBuf, DWORD* bufSize));
    STDMETHOD(SetOriginalOwnerName(char*  szNewOwnerName));

    STDMETHOD(GetOriginalMachineName(char*  szDestBuf, DWORD* bufSize));
    STDMETHOD(SetOriginalMachineName(char*  szNewMachineName));
    STDMETHOD(GetOriginalMachineDomain(char*  szDestBuf, DWORD* bufSize));
    STDMETHOD(SetOriginalMachineDomain(char*  szNewMachineName));

	STDMETHOD(GetVirusName(char* szVirusName, DWORD* dwBufferSize));
	STDMETHOD(SetVirusName(const char* szVirusName));
	STDMETHOD(GetVirusID(DWORD *dwVirusID));
	STDMETHOD(SetVirusID(DWORD dwVirusID));
	STDMETHOD(ClearVirusName());
	STDMETHOD(ClearVirusID());

	STDMETHOD(GetItemSize(__int64* nItemSize));

	STDMETHOD(GetAnomalyID(char* szValue, DWORD* dwBufferSize));
	STDMETHOD(SetAnomalyID(const char* szValue));
	STDMETHOD(GetAnomalyName(char* szValue, DWORD* dwBufferSize));
	STDMETHOD(SetAnomalyName(const char* szValue));
	STDMETHOD(GetAnomalyCategories(char* szValue, DWORD* dwBufferSize));
	STDMETHOD(SetAnomalyCategories(cc::IIndexValueCollection* pCategories));

	STDMETHOD(GetAnomalyDamageFlag(DWORD dwFlag, DWORD* dwValue));
	STDMETHOD(SetAnomalyDamageFlag(DWORD dwFlag, DWORD dwValue));

	STDMETHOD(GetAnomalyDependencyFlag(DWORD* dwHasDependencies));
	STDMETHOD(SetAnomalyDependencyFlag(DWORD dwHasDependencies));

	
	////////////////////////////////////
	// Per Remediation access functions
	STDMETHOD(GetRemediationDescription(DWORD dwRemediationIndex, char* szValue, DWORD* dwBufferSize));
	STDMETHOD(GetRemediationOriginalFilename(DWORD dwRemediationIndex, char* szValue, DWORD* dwBufferSize));
	STDMETHOD(GetRemediationActionSucceeded(DWORD dwRemediationIndex, bool* bSucceeded));
	STDMETHOD(GetRemediationUndoAvailable(DWORD dwRemediationIndex, bool* bUndoAvailable));
	STDMETHOD(GetRemediationActionType(DWORD dwRemediationIndex, DWORD* dwValue));
	STDMETHOD(GetRemediationOperationType(DWORD dwRemediationIndex, DWORD* dwValue));


	STDMETHOD(AddRemediationData(ccEraser::IRemediationAction* pRemediation, cc::IStream* pStream));
	
	STDMETHOD(RestoreAllRemediations());
	STDMETHOD(RestoreRemediation(DWORD dwRemediationIndex, ccEraser::eResult* pUndoResult = NULL));
	STDMETHOD(RestoreGenericRemediations());
	STDMETHOD(IsGenericAnomaly(bool* bGeneric));

	STDMETHOD(RestoreFileRemediationToTemp(DWORD dwRemediationIndex, 
								/*[out]*/ LPSTR szDestinationFolder,
							 /*[in,out]*/ DWORD* dwDestFolderSize,
								/*[out]*/ LPSTR szDestinationFilename,
							 /*[in,out]*/ DWORD* dwDestFilenameSize,
							    /*[out]*/ ccEraser::eResult* pUndoResult = NULL));

	STDMETHOD(RestoreFileRemediationToLocation(DWORD dwRemediationIndex, 
										/*[in]*/LPTSTR szAlternateDirectory, 
										/*[in]*/LPTSTR szAlternateFilename,
										/*[out]*/ccEraser::eResult* pUndoResult = NULL));

	STDMETHOD(ScanFileRemediation(DWORD dwRemediationIndex));
	STDMETHOD(RepairAndRestoreFileRemediationToTemp(DWORD dwRemediationIndex,
									   /*[out]*/ LPSTR szDestinationFolder,
									 /*[in,out]*/ DWORD* dwDestFolderSize,
									   /*[out]*/ LPSTR szDestinationFilename,
									 /*[in,out]*/ DWORD* dwDestFilenameSize));

	STDMETHOD(GetRemediationCount(DWORD* dwCount));
	STDMETHOD(GetRemediationFailureCount(DWORD* dwCount));

	STDMETHOD(GetRemediationData(LPSTR szDataDestinationFolder, 
								 DWORD dwRemediationIndex,
					   /*[out]*/ LPSTR szRemediationActionName, // Get file name for SND manifest
					   /*[in, out]*/ DWORD* dwBufferSizeAction,	  // Name buffer size
					   /*[out]*/ LPSTR szRemediationUndoName,	  // Get file name for SND manifest
					   /*[in, out]*/ DWORD* dwBufferSizeUndo));	  // Name buffer size

    STDMETHOD(CreateDataSubfolderIfNotExist(/*[out]*/ LPTSTR szDataSubfolderName, /*[out]*/ DWORD* dwBuffSize));
	STDMETHOD(TakeSystemSnapshot());
	STDMETHOD(CopySnapshotToFolder(LPSTR szDestinationFolder, 
							 /*[out]*/ LPSTR szSnapShotName,  // Get snapshot file name for SND manifest
						 /*[in, out]*/ DWORD* dwBufferSize)); // Snapshot name buffer size

	STDMETHOD(InitialDummyFile(bool* bDummy));

    BOOL    IsInitialized();
    BOOL    IsQuarantineFile();
	
private:
    HRESULT ScanRepairMainFile(char* szDestPath, DWORD dwPathSize, BOOL bRepair);

	// Helper Functions to translate Date formats
	QSPAKDATE SysTimeToQSDate(SYSTEMTIME stSysTime);
	void QSDateToSysTime(QSPAKDATE stQSDate, SYSTEMTIME* stSysTime);

    // Handle to the QSPak file
	HQSERVERITEM m_hQuarItem;

	char m_szQuarantineItemPath[MAX_PATH];

	// Pointer the QScanner class to handle file scans
	CQScanner* m_pQScanner;

    // Synchronization object to make calls to this object thread safe
    ccLib::CCriticalSection m_critSec;

	// SymInterface loader for ccEraser and IEraser object
    cc::CSymInterfaceTrustedLoader m_ccEraserLoader;
	ccEraser::IEraserPtr m_pEraser;
	CSimpleScanner m_SimpleScan;
	IScannerPtr m_pScanner;
	IScanSinkPtr m_pScanSink;
	IScanPropertiesPtr m_pScanProps;

protected:
	HRESULT GetItemPath(char* szDestBuf, DWORD* bufSize);
	HRESULT SetItemPath(char* szNewPath);
	HRESULT GetItemFilename(char* szDestBuf, DWORD* bufSize);
	HRESULT SetItemFilename(char* szNewCurrentFilename);

	bool DeleteDataFolder();
	bool CreateFolderIfNotExist(LPTSTR szFolderName);
	HRESULT SaveRemediationToFile(ccEraser::IRemediationAction* pRemediation, LPTSTR szFileName);
	HRESULT SaveUndoToFile(cc::IStream* pStream, LPTSTR szFileName);
	
	HRESULT LoadRemediationFromFile(LPTSTR szRemdiationFilename, ccEraser::IRemediationAction*& pRemediation);
	HRESULT LoadUndoFromFile(LPTSTR szUndoFilename, cc::IStream*& pStream);
	
	HRESULT UndoRemediation(LPTSTR szRemdiationFilename, LPTSTR szUndoFilename, ccEraser::eResult* pUndoResult = NULL);

	HRESULT RestoreFileRemediationToLocationFromDataFiles(LPTSTR szRemdiationFilename, LPTSTR szUndoFilename, LPTSTR szAlternateDirectory, LPTSTR szAlternateFilename, ccEraser::eResult* pUndoResult = NULL);

	HRESULT RestoreFileRemediationToTempAndScanRepair(DWORD dwRemediationIndex, 
										/*[in]*/ bool bKeepCleanFile,
									   /*[out]*/ LPSTR szDestinationFolder,
									/*[in,out]*/ DWORD* dwDestFolderSize,
									   /*[out]*/ LPSTR szDestinationFilename,
									/*[in,out]*/ DWORD* dwDestFilenameSize);

	bool IsSuccessfulRemediation(ccEraser::IRemediationAction* pRemediation);
	ccEraser::eResult GetRemediationActionType(ccEraser::IRemediationAction* pRemediation, ccEraser::eObjectType& objectType);
	DWORD GetRemediationOperationType(ccEraser::IRemediationAction* pRemediation);
	HRESULT SaveRemediationDescription(DWORD dwRemediationIndex, ccEraser::IRemediationAction* pRemediation, ccEraser::eObjectType eType);
	HRESULT SaveRemediationOriginalFilename(DWORD dwRemediationIndex, ccEraser::IRemediationAction* pRemediation, ccEraser::eObjectType eType);

	bool GenerateRemeditionDataName(LPTSTR szFilename, bool bRemediationAction, DWORD dwCount);
	HRESULT SetRemediationCount(DWORD dwCount);
	HRESULT SetRemediationFailureCount(DWORD dwCount);
	HRESULT IncrementRemediationCount(DWORD* dwCount = NULL);
	HRESULT IncrementRemediationFailureCount(DWORD* dwCount = NULL);

	HRESULT InitializeEraser(ccEraser::eResult* pEraserResult = NULL);
};

#endif // !defined(AFX_QUARANTINEITEM_H__6F9F4C24_AD73_11D1_9105_00C04FAC114A__INCLUDED_)
