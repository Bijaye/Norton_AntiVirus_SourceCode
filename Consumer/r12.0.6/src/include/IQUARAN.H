//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// IQuaran.h

#pragma once

#include "syminterface.h"
#include "ccScanInterface.h"
#include "ccEraserInterface.h"

#define QUARANTINE_APP_ID "NAV_90_QUAR"


////////////////////////////////////////////////////////////////////////////
// IQuaranineDLL

// Forward declaration
class IEnumQuarantineItems;
class IQuarantineItem;

class IQuarantineDLL : public ISymBase
{
public:
    STDMETHOD_( UINT, GetVersion() )
        PURE;

	STDMETHOD( Initialize( ) )
        PURE;

    STDMETHOD(GetQuarantineFolder(LPSTR lpszFolder, UINT uBufferSize))
        PURE;

    STDMETHOD(CreateNewQuarantineItemFromFile(LPSTR lpszSourceFileName,
											  DWORD dwFileType,
											  DWORD dwItemStatus,
											  IQuarantineItem **pNewItem))
        PURE;

	STDMETHOD(LoadQuarantineItemFromFilenameVID(LPCSTR lpszSourceFileName, 
												DWORD dwVirusID, 
												IQuarantineItem **pMatchingQuarItem))
		PURE;

    STDMETHOD(LoadQuarantineItemFromUUID(UUID uniqueID,
                                         IQuarantineItem **pMatchingQuarItem))
        PURE;

	STDMETHOD(CreateNewQuarantineItemWithoutFile(DWORD dwItemStatus, IQuarantineItem **pNewItem))
		PURE;

    STDMETHOD( ProcessIncomingFile( LPSTR lpszSourceFileName ) )
        PURE;

    STDMETHOD( ProcessAllIncomingFiles() )
        PURE;

    STDMETHOD( Enum( IEnumQuarantineItems** pEnumObj ) )
        PURE;

    STDMETHOD( GetVirusInformation( unsigned long ulVirusID, IScanVirusInfo** ppInfo ) )
        PURE;

	STDMETHOD(TakeSystemSnapshot())
        PURE;
	STDMETHOD(AddSnapshotToQuarantineItem(IQuarantineItem *pQuarItem))
        PURE;
};

// {A7B79461-ACB0-11d1-9104-00C04FAC114A}
SYM_DEFINE_INTERFACE_ID(IID_QuarantineDLL,
    0xa7b79461, 0xacb0, 0x11d1, 0x91, 0x4, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a);
typedef CSymPtr<IQuarantineDLL> IQuarantineDLLPtr;

////////////////////////////////////////////////////////////////////////////
// IQuarantineItem2

class IQuarantineDLL2 : public IQuarantineDLL
{
public:
    STDMETHOD(SetScanner(IScanner* pScanner))
        PURE;
};

// {EE98E979-92EC-4ace-BE5E-F1102EC1D497}
SYM_DEFINE_INTERFACE_ID( IID_QuarantineDLL2, 0xee98e979, 0x92ec, 0x4ace, 0xbe, 0x5e, 0xf1, 0x10, 0x2e, 0xc1, 0xd4, 0x97);
typedef CSymPtr<IQuarantineDLL2> IQuarantineDLL2Ptr;
typedef CSymQIPtr<IQuarantineDLL2, &IID_QuarantineDLL2> IQuarantineDLL2QIPtr;

////////////////////////////////////////////////////////////////////////////
// IQuarantineItem

class IQuarantineItem : public ISymBase
{
public:
	STDMETHOD( Initialize( LPSTR lpszFileName ) )
        PURE;

    STDMETHOD( SaveItem() )
        PURE;

    STDMETHOD( DeleteItem() )
        PURE;

	STDMETHOD(ScanMainFile())
        PURE;

    STDMETHOD(RepairAndRestoreMainFileToTemp(char* szDestPath, DWORD dwPathSize))
        PURE;

    /*STDMETHOD( RestoreItem( LPSTR lpszDestPath, LPSTR lpszDestFilename, BOOL  bOverwrite ) )
        PURE;*/

    STDMETHOD(UnpackageMainFile(LPSTR lpszDestinationFolder, 
								LPSTR lpszDestinationFileName, 
								LPSTR lpszUnpackagedFileName, 
								DWORD dwBufSize, 
								BOOL bOverwrite, 
								BOOL bUseTemporaryFile))
        PURE;

    STDMETHOD( OriginalFileExists( BOOL*  bResult ) )
        PURE;

	STDMETHOD( RestoreCCGserSideEffects(char* szRestoredFileName) )
		PURE;


	STDMETHOD( GetString(char* szProperty, char* szValue, DWORD* pulBuffSize) )
		PURE;
	STDMETHOD( SetString(char* szProperty, char* szValue, DWORD dwBuffSize) )
		PURE;

	STDMETHOD( GetDWORD(char* szProperty, DWORD* dwValue) )
		PURE;
	STDMETHOD( SetDWORD(char* szProperty, DWORD dwValue) )
		PURE;

	STDMETHOD( GetBinary(char* szProperty, BYTE* pBuff, DWORD* pulBuffSize) )
        PURE;
	STDMETHOD( SetBinary(char* szProperty, BYTE* pBuff, DWORD dwBuffSize) )
        PURE;

	STDMETHOD( GetDate(char* szProperty, SYSTEMTIME* stTime) )
		PURE;
	STDMETHOD( SetDate(char* szProperty, SYSTEMTIME* stTime) )
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

    STDMETHOD( GetOriginalAnsiFilename( char*  szDestBuf, DWORD* bufSize ) )
        PURE;
    STDMETHOD( SetOriginalAnsiFilename( char*  szNewOriginalAnsiFilename ) )
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

	STDMETHOD(GetAnomalyID(char* szValue, DWORD* dwBufferSize))
		PURE;
	STDMETHOD(SetAnomalyID(const char* szValue))
		PURE;
	STDMETHOD(GetAnomalyName(char* szValue, DWORD* dwBufferSize))
		PURE;
	STDMETHOD(SetAnomalyName(const char* szValue))
		PURE;
	STDMETHOD(GetAnomalyCategories(char* szValue, DWORD* dwBufferSize))
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

	STDMETHOD(GetRemediationDescription(DWORD dwRemediationIndex, char* szValue, DWORD* dwBufferSize))
		PURE;
	STDMETHOD(GetRemediationOriginalFilename(DWORD dwRemediationIndex, char* szValue, DWORD* dwBufferSize))
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
								/*[out]*/ LPSTR szDestinationFolder,
							 /*[in,out]*/ DWORD* dwDestFolderSize,
								/*[out]*/ LPSTR szDestinationFilename,
							 /*[in,out]*/ DWORD* dwDestFilenameSize,
							    /*[out]*/ ccEraser::eResult* pUndoResult = NULL))
		PURE;

	STDMETHOD(RestoreFileRemediationToLocation(DWORD dwRemediationIndex, 
										/*[in]*/LPTSTR szAlternateDirectory, 
										/*[in]*/LPTSTR szAlternateFilename,
										/*[out]*/ccEraser::eResult* pUndoResult = NULL))
		PURE;

	STDMETHOD(ScanFileRemediation(DWORD dwRemediationIndex))
		PURE;

	STDMETHOD(RepairAndRestoreFileRemediationToTemp(DWORD dwRemediationIndex,
									   /*[out]*/ LPSTR szDestinationFolder,
									 /*[in,out]*/ DWORD* dwDestFolderSize,
									   /*[out]*/ LPSTR szDestinationFilename,
									 /*[in,out]*/ DWORD* dwDestFilenameSize))
		PURE;

	STDMETHOD(GetRemediationCount(DWORD* dwCount))
		PURE;

	STDMETHOD(GetRemediationFailureCount(DWORD* dwCount))
		PURE;

	STDMETHOD(GetRemediationData(LPSTR szDataDestinationFolder, 
								 DWORD dwRemediationIndex,
					   /*[out]*/ LPSTR szRemediationActionName, // Get file name for SND manifest
				   /*[in, out]*/ DWORD* dwBufferSizeAction,		// Name buffer size
					   /*[out]*/ LPSTR szRemediationUndoName,	// Get file name for SND manifest
				   /*[in, out]*/ DWORD* dwBufferSizeUndo))		// Name buffer size
		PURE;

	STDMETHOD(CreateDataSubfolderIfNotExist(/*[out]*/ LPTSTR szDataSubfolderName, /*[out]*/ DWORD* dwBuffSize))
		PURE;
	STDMETHOD(TakeSystemSnapshot())
        PURE;

	STDMETHOD(CopySnapshotToFolder(LPSTR szDestinationFolder, 
							 /*[out]*/ LPSTR szSnapShotName, // Get snapshot file name for SND manifest
						 /*[in, out]*/ DWORD* dwBufferSize)) // Snapshot name buffer size
		 PURE;

	STDMETHOD(InitialDummyFile(bool* bDummy))
        PURE;
};

// {A7B79463-ACB0-11d1-9104-00C04FAC114A}
SYM_DEFINE_INTERFACE_ID(IID_QuarantineItem,
    0xa7b79463, 0xacb0, 0x11d1, 0x91, 0x4, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a);

////////////////////////////////////////////////////////////////////////////
// IEnumQuarantineItems

class IEnumQuarantineItems : public ISymBase
{
public:
    STDMETHOD( Next( ULONG celt,
        IQuarantineItem ** pItems,
        ULONG * pceltFetched ) )
        PURE;

    STDMETHOD( Skip( ULONG celt ) )
        PURE;

    STDMETHOD( Reset( void ) )
        PURE;

    STDMETHOD( Clone( IEnumQuarantineItems ** ppvOut ) )
        PURE;

};

// {A7B79465-ACB0-11d1-9104-00C04FAC114A}
SYM_DEFINE_INTERFACE_ID(IID_EnumQuarantineItems,
    0xa7b79465, 0xacb0, 0x11d1, 0x91, 0x4, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a);

////////////////////////////////////////////////////////////////////////////
// IQuarantineOpts

class IQuarantineOpts : public ISymBase
{
public:
    STDMETHOD( ReadQuarOptsFile( ) )
        PURE;

    STDMETHOD( WriteQuarOptsFile( ) )
        PURE;

    STDMETHOD( Initialize(LPSTR lpszFileName, BOOL  bCreate = FALSE ) )
        PURE;

    STDMETHOD( GetScanAtStartup( BOOL*  bValue ) )
        PURE;

    STDMETHOD( SetScanAtStartup( BOOL  bResult ) )
        PURE;

    STDMETHOD( GetValidFileOperations(DWORD*  dwFileOps, int  nIndex) )
        PURE;

    STDMETHOD( SetValidFileOperations(DWORD  dwFileOps, int  nIndex) )
        PURE;
};

// {A89310A1-B92F-11d1-A7A0-0000E8D34392}
SYM_DEFINE_INTERFACE_ID(IID_QuarantineOpts,
    0xa89310a1, 0xb92f, 0x11d1, 0xa7, 0xa0, 0x0, 0x0, 0xe8, 0xd3, 0x43, 0x92);