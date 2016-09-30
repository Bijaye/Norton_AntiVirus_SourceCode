////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//
// IQuaran.h

#pragma once

#include "syminterface.h"
#include "ccScanwInterface.h"
#include "ccEraserInterface.h"

#define QUARANTINEW_APP_ID L"NAV_90_QUAR"

// Forward declaration
class IEnumQuarantineItemsW;
class IQuarantineItemW;

///////////////////////////////////////////////////////////////////////////////////////////
//  WIDE VERSION
///////////////////////////////////////////////////////////////////////////////////////////
class IQuarantineDLLW : public ISymBase
{
public:
    STDMETHOD_( UINT, GetVersion() )
        PURE;

	STDMETHOD( Initialize( ) )
        PURE;

    STDMETHOD(GetQuarantineFolderW(LPWSTR lpszFolder, UINT uBufferSize))
        PURE;

    STDMETHOD(CreateNewQuarantineItemFromFileW(LPWSTR lpszSourceFileName,
											   DWORD dwFileType,
											   DWORD dwItemStatus,
											   IQuarantineItemW **pNewItem))
        PURE;

	STDMETHOD(LoadQuarantineItemFromFilenameVIDW(LPCWSTR lpszSourceFileName, 
												 DWORD dwVirusID, 
												 IQuarantineItemW **pMatchingQuarItem))
		PURE;

    STDMETHOD(LoadQuarantineItemFromUUID(UUID uniqueID,
                                         IQuarantineItemW **pMatchingQuarItem))
        PURE;

	STDMETHOD(CreateNewQuarantineItemWithoutFile(DWORD dwItemStatus, IQuarantineItemW **pNewItem))
		PURE;

    STDMETHOD( ProcessIncomingFileW( LPWSTR lpszSourceFileName ) )
        PURE;

    STDMETHOD( ProcessAllIncomingFiles() )
        PURE;

    STDMETHOD( Enum( IEnumQuarantineItemsW** pEnumObj ) )
        PURE;

    STDMETHOD( GetVirusInformation( unsigned long ulVirusID, ccScanw::IScanwVirusInfo** ppInfo ) )
        PURE;

	STDMETHOD(TakeSystemSnapshot())
        PURE;
	STDMETHOD(AddSnapshotToQuarantineItem(IQuarantineItemW *pQuarItem))
        PURE;

    STDMETHOD(SetScanner(ccScanw::IScannerw* pScanner))
        PURE;

};
// {ECFADB04-6639-4234-BB4F-4277F7656C96}
SYM_DEFINE_INTERFACE_ID(IID_QuarantineDLLW,
    0xecfadb04, 0x6639, 0x4234, 0xbb, 0x4f, 0x42, 0x77, 0xf7, 0x65, 0x6c, 0x96);
typedef CSymPtr<IQuarantineDLLW> IQuarantineDLLWPtr;

////////////////////////////////////////////////////////////////////////////
// IQuarantineItemW

class IQuarantineItemW : public ISymBase
{
public:
	STDMETHOD( Initialize( wchar_t* lpszFileName ) )
        PURE;

    STDMETHOD( SaveItem() )
        PURE;

    STDMETHOD( DeleteItem() )
        PURE;

	STDMETHOD(ScanMainFile())
        PURE;

    STDMETHOD(RepairAndRestoreMainFileToTemp(wchar_t* szDestPath, DWORD dwPathSize))
        PURE;

    STDMETHOD(UnpackageMainFile(wchar_t* lpszDestinationFolder, 
								wchar_t* lpszDestinationFileName, 
								wchar_t* lpszUnpackagedFileName, 
								DWORD dwBufSize, 
								BOOL bOverwrite, 
								BOOL bUseTemporaryFile))
        PURE;

    STDMETHOD( OriginalFileExists( BOOL*  bResult ) )
        PURE;

	STDMETHOD( RestoreCCGserSideEffects(wchar_t** szRestoredFileName) )
		PURE;


	STDMETHOD( GetString(wchar_t** szProperty, wchar_t** szValue, DWORD* pulBuffSize) )
		PURE;
	STDMETHOD( SetString(wchar_t** szProperty, wchar_t** szValue, DWORD dwBuffSize) )
		PURE;

	STDMETHOD( GetDWORD(wchar_t** szProperty, DWORD* dwValue) )
		PURE;
	STDMETHOD( SetDWORD(wchar_t** szProperty, DWORD dwValue) )
		PURE;

	STDMETHOD( GetBinary(wchar_t** szProperty, BYTE* pBuff, DWORD* pulBuffSize) )
        PURE;
	STDMETHOD( SetBinary(wchar_t** szProperty, BYTE* pBuff, DWORD dwBuffSize) )
        PURE;

	STDMETHOD( GetDate(wchar_t** szProperty, SYSTEMTIME* stTime) )
		PURE;
	STDMETHOD( SetDate(wchar_t** szProperty, SYSTEMTIME* stTime) )
		PURE;

    // Note: Added accessors for all members of QFILE_INFO_STRUCT
    STDMETHOD( GetUniqueID( UUID&  uuid ) )
        PURE;
    STDMETHOD( SetUniqueID( UUID  newID ) )
        PURE;

    STDMETHOD( GetFileStatus( DWORD*  fileStatus ) )
        PURE;
    STDMETHOD( SetFileStatus( DWORD  newFileStatus ) )
        PURE;

    STDMETHOD( GetFileType( DWORD*  fileType ) )
        PURE;
    STDMETHOD( SetFileType( DWORD  newFileType ) )
        PURE;

    STDMETHOD( GetDateQuarantined( SYSTEMTIME* pstdateQuarantined ) )
        PURE;
    STDMETHOD( SetDateQuarantined( SYSTEMTIME* pstnewQuarantineDate ) )
        PURE;

    STDMETHOD( GetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                     SYSTEMTIME* pstOriginalFileDateAccessed,
                                     SYSTEMTIME* pstOriginalFileDateWritten ) )
        PURE;
    STDMETHOD( SetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                     SYSTEMTIME* pstOriginalFileDateAccessed,
                                     SYSTEMTIME* pstOriginalFileDateWritten ) )
        PURE;

    STDMETHOD( GetDateOfLastScan( SYSTEMTIME* pstdateOfLastScanned ) )
        PURE;
    STDMETHOD( SetDateOfLastScan( SYSTEMTIME* pstnewDateLastScan ) )
        PURE;

    STDMETHOD( GetDateOfLastScanDefs( SYSTEMTIME* pstdateOfLastScannedDefs ) )
        PURE;
    STDMETHOD( SetDateOfLastScanDefs( SYSTEMTIME* pstnewDateLastScanDefs ) )
        PURE;

	STDMETHOD(GetOriginalScanDefsDate(SYSTEMTIME* dateOfOriginalScannedDefs))
		PURE;

    STDMETHOD( GetDateSubmittedToSARC( SYSTEMTIME* pstdateSubmittedToSARC ) )
        PURE;
    STDMETHOD( SetDateSubmittedToSARC( SYSTEMTIME* pstdateSubmittedToSARC ) )
        PURE;

    STDMETHOD( GetOriginalFilesize( DWORD*  originalFileSize ) )
        PURE;
    STDMETHOD( SetOriginalFilesize( DWORD  newOriginalFileSize ) )
        PURE;

    STDMETHOD( GetOriginalOwnerName( char*  szDestBuf, DWORD* bufSize ) )
        PURE;
    STDMETHOD( SetOriginalOwnerName( char*  szNewOwnerName ) )
        PURE;

    STDMETHOD( GetOriginalMachineName( char*  szDestBuf, DWORD* bufSize ) )
        PURE;
    STDMETHOD( SetOriginalMachineName( char*  szNewMachineName ) )
        PURE;

    STDMETHOD( GetOriginalMachineDomain( char*  szDestBuf, DWORD* bufSize ) )
        PURE;
    STDMETHOD( SetOriginalMachineDomain( char*  szNewMachineName ) )
        PURE;
            
	STDMETHOD(GetVirusName(char* szVirusName, DWORD* dwBufferSize))
        PURE;
	STDMETHOD(SetVirusName(const char* szVirusName))
        PURE;

	STDMETHOD(GetVirusID(DWORD *dwVirusID))
        PURE;
	STDMETHOD(SetVirusID(DWORD dwVirusID))
        PURE;

	STDMETHOD(ClearVirusName())
        PURE;

	STDMETHOD(ClearVirusID())
        PURE;

	STDMETHOD(GetItemSize(__int64* nItemSize))
		PURE;

	STDMETHOD(GetAnomalyID(wchar_t* szValue, DWORD* dwBufferSize))
		PURE;
	STDMETHOD(SetAnomalyID(const wchar_t* szValue))
		PURE;
	STDMETHOD(GetAnomalyName(wchar_t* szValue, DWORD* dwBufferSize))
		PURE;
	STDMETHOD(SetAnomalyName(const wchar_t* szValue))
		PURE;
	STDMETHOD(GetAnomalyCategories(wchar_t* szValue, DWORD* dwBufferSize))
		PURE;
	STDMETHOD(SetAnomalyCategories(cc::IIndexValueCollection* pCategories))
		PURE;

	STDMETHOD(GetAnomalyDamageFlag(DWORD dwFlag, DWORD* dwValue))
		PURE;
	STDMETHOD(SetAnomalyDamageFlag(DWORD dwFlag, DWORD dwValue))
		PURE;

	STDMETHOD(GetAnomalyDependencyFlag(DWORD* dwHasDependencies))
		PURE;
	STDMETHOD(SetAnomalyDependencyFlag(DWORD dwHasDependencies))
		PURE;

	STDMETHOD(GetRemediationDescription(DWORD dwRemediationIndex, wchar_t* szValue, DWORD* dwBufferSize))
		PURE;
	STDMETHOD(GetRemediationOriginalFilename(DWORD dwRemediationIndex, wchar_t* szValue, DWORD* dwBufferSize))
		PURE;
	STDMETHOD(GetRemediationActionSucceeded(DWORD dwRemediationIndex, bool* bSucceeded))
		PURE;
	STDMETHOD(GetRemediationUndoAvailable(DWORD dwRemediationIndex, bool* bUndoAvailable))
		PURE;
	STDMETHOD(GetRemediationActionType(DWORD dwRemediationIndex, DWORD* dwValue))
		PURE;
	STDMETHOD(GetRemediationOperationType(DWORD dwRemediationIndex, DWORD* dwValue))
		PURE;


	STDMETHOD(AddRemediationData(ccEraser::IRemediationAction* pRemediation, cc::IStream* pStream))
		PURE;

	STDMETHOD(RestoreAllRemediations())
        PURE;

	STDMETHOD(RestoreRemediation(DWORD dwRemediationIndex, ccEraser::eResult* pUndoResult = NULL))
		PURE;

	STDMETHOD(RestoreGenericRemediations())
        PURE;

	STDMETHOD(IsGenericAnomaly(bool* bGeneric))
		PURE;

	STDMETHOD(RestoreFileRemediationToTemp(DWORD dwRemediationIndex, 
								/*[out]*/ wchar_t* szDestinationFolder,
							 /*[in,out]*/ DWORD* dwDestFolderSize,
								/*[out]*/ wchar_t* szDestinationFilename,
							 /*[in,out]*/ DWORD* dwDestFilenameSize,
							    /*[out]*/ ccEraser::eResult* pUndoResult = NULL))
		PURE;

	STDMETHOD(RestoreFileRemediationToLocation(DWORD dwRemediationIndex, 
										/*[in]*/wchar_t* szAlternateDirectory, 
										/*[in]*/wchar_t* szAlternateFilename,
										/*[out]*/ccEraser::eResult* pUndoResult = NULL))
		PURE;

	STDMETHOD(ScanFileRemediation(DWORD dwRemediationIndex))
		PURE;

	STDMETHOD(RepairAndRestoreFileRemediationToTemp(DWORD dwRemediationIndex,
									   /*[out]*/ wchar_t* szDestinationFolder,
									 /*[in,out]*/ DWORD* dwDestFolderSize,
									   /*[out]*/ wchar_t* szDestinationFilename,
									 /*[in,out]*/ DWORD* dwDestFilenameSize))
		PURE;

	STDMETHOD(GetRemediationCount(DWORD* dwCount))
		PURE;

	STDMETHOD(GetRemediationFailureCount(DWORD* dwCount))
		PURE;

	STDMETHOD(GetRemediationData(wchar_t* szDataDestinationFolder, 
								 DWORD dwRemediationIndex,
					   /*[out]*/ wchar_t* szRemediationActionName, // Get file name for SND manifest
				   /*[in, out]*/ DWORD* dwBufferSizeAction,		// Name buffer size
					   /*[out]*/ wchar_t* szRemediationUndoName,	// Get file name for SND manifest
				   /*[in, out]*/ DWORD* dwBufferSizeUndo))		// Name buffer size
		PURE;

	STDMETHOD(CreateDataSubfolderIfNotExist(/*[out]*/ wchar_t* szDataSubfolderName, /*[out]*/ DWORD* dwBuffSize))
		PURE;
	STDMETHOD(TakeSystemSnapshot())
        PURE;

	STDMETHOD(CopySnapshotToFolder(wchar_t* szDestinationFolder, 
							 /*[out]*/ wchar_t* szSnapShotName, // Get snapshot file name for SND manifest
						 /*[in, out]*/ DWORD* dwBufferSize)) // Snapshot name buffer size
		 PURE;

	STDMETHOD(InitialDummyFile(bool* bDummy))
        PURE;
};

// {16D22904-5DBC-493d-BB19-785B49A6BE66}
SYM_DEFINE_INTERFACE_ID(IID_QuarantineItemW,
    0x16d22904, 0x5dbc, 0x493d,  0xbb, 0x19, 0x78, 0x5b, 0x49, 0xa6, 0xbe, 0x66);

////////////////////////////////////////////////////////////////////////////
// IEnumQuarantineItemsW

class IEnumQuarantineItemsW : public ISymBase
{
public:
    STDMETHOD( Next( ULONG celt,
        IQuarantineItemW ** pItems,
        ULONG * pceltFetched ) )
        PURE;

    STDMETHOD( Skip( ULONG celt ) )
        PURE;

    STDMETHOD( Reset( void ) )
        PURE;

    STDMETHOD( Clone( IEnumQuarantineItemsW ** ppvOut ) )
        PURE;

};

// {C381491F-C1AF-40da-B840-0F1EAF591B4B}
SYM_DEFINE_INTERFACE_ID(IID_EnumQuarantineItemsW,
    0xc381491f, 0xc1af, 0x40da, 0xb8, 0x40, 0xf, 0x1e, 0xaf, 0x59, 0x1b, 0x4b
);
