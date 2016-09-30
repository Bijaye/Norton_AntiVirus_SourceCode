/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// ItemData.h: interface for the CItemData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMDATA_H__80EADD4D_B591_11D2_8F45_3078302C2030__INCLUDED_)
#define AFX_ITEMDATA_H__80EADD4D_B591_11D2_8F45_3078302C2030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
enum ItemState		{ ITEM_DATA_STATE_UNKNOWN = 0, ITEM_DATA_STATE_UNCHANGED = 1, ITEM_DATA_STATE_NEEDS_DELETE, ITEM_DATA_STATE_NEEDS_UPDATE, 
					ITEM_DATA_STATE_NEEDS_ADDED, ITEM_DATA_STATE_REFRESHED};
#define ITEM_AGE_UPDATE_INTERVAL 15
class CItemData  
{
public:
	CItemData();
	virtual ~CItemData();

    HRESULT Initialize( SAFEARRAY* pArray );
    HRESULT Initialize( CItemData* pItemData);
	static void ConvertTimeFromUTC2LT(LPSYSTEMTIME lptime);  // added by terrym 10-11-00 will convert utc time to local time based on the time zone information.

public:    
    static SAFEARRAY* GetDataMemberArray();
    CString GetDateString( SYSTEMTIME * pst );
    void    SetStatusText( );
    void    SetSubmittedText( );
	DWORD GetItemDataState(void) {return m_dwfStateFlag;};
	void SetItemDataState(ItemState state) {m_dwfStateFlag = state;};
	BOOL Compare(const CItemData& itemData);
	BOOL operator==(const CItemData& ciData);

    void ComputeImageIndex();
	BOOL IsSampleErrorCondition(void);
	void SetError(BOOL bError=TRUE) {m_bError = bError;
									ComputeImageIndex();}
	BOOL GetError(void){return m_bError;}
private:
	static 	TIME_ZONE_INFORMATION	m_tzi;  // added by terrym 10-11-00 keep track of time zone information
	static 	DWORD					m_tzRc; // added by terrym 10-11-00 keep track of time zone return code
// 
// Data memebers
// 
public:
    DWORD       m_dwSubmittedBy;
    DWORD       m_dwItemID;
    DWORD       m_dwStatus;
    DWORD       m_dwVirusID;
    SYSTEMTIME  m_stRecieved;
    SYSTEMTIME  m_stSubmitted;
    CString     m_sFileName;
    CString     m_sUserName;
    CString     m_sDomainName;
    CString     m_sMachineName;
    CString     m_sRecievedDateStr;
    CString     m_sSubmittedDateStr;
    CString     m_sFilePath;
    CString     m_sVirusName;
    CString     m_sStatusText;
    CString     m_sSubmittedBy;
    CString     m_sPlatform;
    CString     m_sAddress;
    CString     m_sProduct;
	CString		m_sStatus;

    UINT        m_uImage;
    ULONG       m_ulFileSize;
    GUID        m_ItemGUID;

	// Added 12-2-99 Terrym for addational Bell attributes.
    SYSTEMTIME  m_stCompleted;			// date sample completed
    CString     m_sCompletedDateStr;	// string of date completed
	CString		m_sComplete;			// Boolen yes or no if the sample has completed analysis.
	ULONG		m_ulAgeMinute;			// elapsed time since the sample was quarantined.
	CString		m_sAge;					// text of age converted to resonable units
	CTimeSpan	m_cstAge;				// how old is this sample
	ULONG		m_ulDefNeeded;			// Indicates the minimum definition sequence number needed to disinfect the client
	CString		m_sDefSeqNeeded;		// Text of min virus def sequ number needed 
	CString		m_sError;				// Indicates that an error has occurred while processing the sample
	CString		m_sState;				// Current state of the sample within the Immune System.
	CString		m_sSampleResult;		// result of sample submission
	CString		m_sScanResult;			// result of scan
	CString		m_sAlertStatus;			// result of scan
	DWORD		m_dwAlertStatus;			// result of scan
private:
	// added for sync of lists for results 
	DWORD m_dwfStateFlag;
	BOOL m_bError;					// used to indicate there is an error in the sample
};

// 
// Safe array elements
// 
#define ITEM_DATA_NUM_ELEMENTS      24

#define ITEM_DATA_FILE_NAME         0
#define ITEM_DATA_USER_NAME         1
#define ITEM_DATA_DOMAIN_NAME       2
#define ITEM_DATA_MACHINE_NAME      3
#define ITEM_DATA_RECIEVED_TIME     4
#define ITEM_DATA_SUBMITTED_TIME    5
#define ITEM_DATA_STATUS            6
#define ITEM_DATA_SUBMITTED_BY      7
#define ITEM_DATA_FILE_ID           8
#define ITEM_DATA_FILE_SIZE         9
#define ITEM_DATA_VIRUS_NAME        10
#define ITEM_DATA_VIRUS_ID          11
#define ITEM_DATA_GUID              12
#define ITEM_DATA_OS                13
#define ITEM_DATA_PRODUCT           14
#define ITEM_DATA_ADDRESS           15

// Added 12-2-99 Terrym for addational Bell attributes.
#define ITEM_DATA_COMPLETED_TIME    16
#define ITEM_DATA_STATE				17
#define ITEM_DATA_DEFSEQNEEDED      18
#define ITEM_DATA_ERROR				19
#define ITEM_DATA_SAMPLE_RESULT		20
#define ITEM_DATA_SCAN_RESULT		21
#define ITEM_DATA_ALERT_STATUS		22
#define ITEM_DATA_ANALYZED_TIME     23


// 
// Image list indexes.
// 
#define RESULT_IMAGE_EXE            1
#define RESULT_IMAGE_UNKNOWN        2
#define RESULT_IMAGE_DOCUMENT       3


#endif // !defined(AFX_ITEMDATA_H__80EADD4D_B591_11D2_8F45_3078302C2030__INCLUDED_)
