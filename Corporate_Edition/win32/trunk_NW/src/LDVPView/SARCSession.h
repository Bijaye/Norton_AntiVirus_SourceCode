// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SarcSession.h: interface for the CSarcSession class.
//
//    Rev 1.0   12 Jan 2005 KTALINKI	
//	Created as part of Enhancement for the support of Anomaly / Extended Threats
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SARCSESSION_H__B0B2D703_E141_11D2_A455_00A02438707D__INCLUDED_)
#define AFX_SARCSESSION_H__B0B2D703_E141_11D2_A455_00A02438707D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vpcommon.h"
#include "iquaran.h"
#include "SarcItem.h"	
#include <afxtempl.h>

class CSarcSession : public IQuarantineSession
{
public:

    // Construction
	CSarcSession();
	~CSarcSession();

    // IUnknown
    STDMETHOD( QueryInterface(REFIID, void**) );
    STDMETHOD_( ULONG, AddRef() );
    STDMETHOD_( ULONG, Release() );

   // IQuarantineSession

	//Get First Quarantine Item
    STDMETHOD( GetFirstItem( IQuarantineItem2**  lplpIQItem ) );

	//Method to next Quarantine Item
	STDMETHOD( GetNextItem( IQuarantineItem2**  lplpIQItem ) );

	//Method to get item at a specified location
	STDMETHOD( GetItem( int iItemIndex, IQuarantineItem2**  lplpIQItem ) );    

	//Method to get number of Quarantine Items
	STDMETHOD( GetItemCount( DWORD*  lpdwItemCount ) ) ;

	//Method to get Session's VBININFO
	STDMETHOD(GetSessionInfo(PVBININFO pvbiSessionInfo)) ;

	//Method to unpackage the session and in turn it's Quarantine Items
	STDMETHOD( Unpackage( LPCSTR lpszDestinationFolder, BOOL bOverwrite ) );
	
	//Initialization Method.
    STDMETHOD( Initialize( LPCSTR lpszSessionID, LPCSTR lpszRecordID=NULL ) );

	//Method to get the unpackaged file name of Quarantine Item at a specified index
	STDMETHOD( GetUnpackedFileNameForItem(DWORD dwItemIndex, LPSTR  lpszUnpackedFileName, DWORD  bufSize  ) );

	//Save session
    STDMETHOD( SaveSession() );

	//Method to delete the session
    STDMETHOD( DeleteSession() ) ;

	//Method to repair the session
    STDMETHOD( RepairSession(IUnknown * pUnk, char*  lpszDestBuf, DWORD  bufSize) );

	//Method to scan the session
    STDMETHOD( ScanSession(IUnknown * pUnk) );

	//Method to forward the session to a specified server
    STDMETHOD( ForwardToServer(LPSTR  lpszServerName, FPFORWARDPROGRESS  lpfnForwardProgress
                                        , DWORD*  dwPtrBytesSent, DWORD*  dwPtrTotalBytes) );

	//Method to restore the session
    STDMETHOD( RestoreSession( LPSTR lpszDestPath, LPSTR lpszDestFilename, BOOL  bOverwrite ) );

	//Method to get the Unique ID of the sessiion.
    STDMETHOD( GetUniqueID( UUID&  uuid ) );

	//Method to set the Unique id
    STDMETHOD( SetUniqueID( UUID  newID ) );
    
	//Method to verify whether the session is submittable to sarc
	//Compressed files are not submittable to sarc
    STDMETHOD( IsSubmittableToSARC( BOOL *lpbSubmittable ) ) ;

	//Method to get the date of quarantine
    STDMETHOD( GetDateQuarantined( SYSTEMTIME* pstdateQuarantined ) ) ;

	//Method to get the date of quarantine
    STDMETHOD( SetDateQuarantined( SYSTEMTIME* pstnewQuarantineDate ) );

	//Method to get the date of last scan
    STDMETHOD( GetDateOfLastScan( SYSTEMTIME* pstdateOfLastScanned ) );

	//Method to set the date of quarantine
    STDMETHOD( SetDateOfLastScan( SYSTEMTIME* pstnewDateLastScan ) );

	//Method to get the date of last scan definitions
    STDMETHOD( GetDateOfLastScanDefs( SYSTEMTIME* pstdateOfLastScannedDefs ) );

	//Method to get the date of quarantine
    STDMETHOD( SetDateOfLastScanDefs( SYSTEMTIME* pstnewDateLastScanDefs ) );

	//Method to get the date of submission to sarc
    STDMETHOD( GetDateSubmittedToSARC( SYSTEMTIME* pstdateSubmittedToSARC ) );

	//Method to get the date of submission to sarc
    STDMETHOD( SetDateSubmittedToSARC( SYSTEMTIME* pstdateSubmittedToSARC ) );

	//Method to get the original owner name
    STDMETHOD( GetOriginalOwnerName( LPSTR  szDestBuf, DWORD  bufSize ) );

	//Method to set the original owner name
    STDMETHOD( SetOriginalOwnerName( LPCSTR  szNewOwnerName ) );

	//Method to get the original machine name
    STDMETHOD( GetOriginalMachineName( LPSTR  szDestBuf, DWORD  bufSize ) );

	//Method to get the original machine name
    STDMETHOD( SetOriginalMachineName( LPCSTR  szNewMachineName ) );

	//Method to get the original machine Domain
    STDMETHOD( GetOriginalMachineDomain( LPSTR  szDestBuf, DWORD  bufSize ) );

	//Method to set the original machine Domain
    STDMETHOD( SetOriginalMachineDomain( LPCSTR  szNewMachineName ) );

	//Method to get session status
	STDMETHOD( GetStatus( DWORD*  Status ) );

	//Method to set session status
    STDMETHOD( SetStatus( DWORD  newStatus ) );

	//Method to get the Anomaly ID
	STDMETHOD( GetAnomalyID( UUID&  AnomalyID ) );

	//Method to set the Anomaly ID
    STDMETHOD( SetAnomalyID( UUID  AnomalyID) );

	//Method to get the Anomaly Category
	STDMETHOD( GetAnomalyCat( DWORD  *lpdwAnomalyCat) );

	//Method to set the Anomaly Category
    STDMETHOD( SetAnomalyCat( DWORD  dwAnomalyCat) );
	
	//Method to get the VirusName
	STDMETHOD( GetVirusName( LPSTR lpszVirusName, DWORD  *lpdwBufSize ) );

	//Method to set the VirusName
    STDMETHOD( SetVirusName( LPCSTR lpszVirusName) );

	//Method to get the Virus Threat Cat
	STDMETHOD( GetThreatCat( DWORD *lpdwThreatCat) );

	//Method to set the Virus Threat Cat
    STDMETHOD( SetThreatCat( DWORD dwThreatCat) );

	//Method to get the session unique id in string format, with buffer size sent in as [IN]
	STDMETHOD( GetUniqueID( LPSTR  lpszuuid, DWORD  bufSize  ) );

	//Method to set the session unique id in string format
    STDMETHOD( SetUniqueID( LPCSTR  lpszNewID ) );

	//Method to get the anomaly id in string format, with buffer size sent in as [IN]
	STDMETHOD( GetAnomalyID( LPSTR  lpszAnomalyID, DWORD  bufSize  ) );

	//Method to set the anomaly id in string format
    STDMETHOD( SetAnomalyID(LPCSTR  lpszAnomalyID) );

	//Method to get Virus Sample type
	STDMETHOD( GetSampleType( DWORD *lpdwSampleType) );

	//Method to set Virus Sample type
    STDMETHOD( SetSampleType( DWORD dwSampleType) );

	//Method to get the Virus ID
	STDMETHOD( GetVirusID( DWORD *lpdwVirusID) );

	//Method to set the Virus ID
    STDMETHOD( SetVirusID( DWORD dwVirusID) );

	//Method to get the Virus Definitions Date
	STDMETHOD( GetDefinitionsDate( SYSTEMTIME *lptTime ) ); 

	//Method to get the Description to display the Threats
	STDMETHOD( GetDescription( LPSTR lpszThreatDesc,		//[OUT]
								DWORD  *lpdwBufSize		//[IN,OUT]
								) );
    
	//Method to get the anomaly categories in string format
	STDMETHOD( GetAnomalyCat( LPSTR lpszBuf,		//[OUT]
								DWORD  *lpdwBufSize		//[IN,OUT]
								) );

	//Internal method to verify whether session is intialized or not
	BOOL	IsInitialized();

	//Method to set the log string.
	void    SetLogString(LPCTSTR lpLogString);

private:
    // Reference count
    DWORD   m_dwRef;
	//Boolean value to save the initialization state.
	BOOL	m_bInitialized;

protected:
	CString					m_strAnomalyID;					//Anomaly ID
	//DWORD					m_dwThreatCat;					//Virus Threat Category
	//DWORD					m_dwAnomalyCat;					//Anomaly Category
	DWORD					m_dwSampleType;					//Sample Type

	//CArray<VBININFO,VBININFO>		m_arItemInfo;			//Array of VBINFO structures.
	//CArray<IQuarantineItem2 *, IQuarantineItem2 *>	m_arIQuarItems;		//Array of Quarantine Items
	CArray<CSarcItem *, CSarcItem *>	m_arIQuarItems;		//Array of Quarantine Items
	CStringArray			m_arUnpackedFileNames;			//Array of unpacked file names for the items.

	int						m_iCurIdx;						//navigation index of the current Quarantine Item
	BOOL					CheckStatus(PEVENTBLOCK lpEventBlock);	//
	CString					m_szLogString;					//String for the log string of the event block
	IVBin2					*m_pVBin;						
	IVBinSession			*m_pVBinSes;					
    VBININFO				m_stVBinInfo;				
    PEVENTBLOCK				m_pEventBlk;
    DWORD					m_dwSessionId;
    CString					m_strComputerName;
    HINSTANCE				m_hResourceInstance;	
	CString					m_sTempFolder;	
	
};

#endif // !defined(AFX_SARCSESSION_H__B0B2D703_E141_11D2_A455_00A02438707D__INCLUDED_)
