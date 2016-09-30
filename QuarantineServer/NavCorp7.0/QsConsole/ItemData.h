// ItemData.h: interface for the CItemData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMDATA_H__80EADD4D_B591_11D2_8F45_3078302C2030__INCLUDED_)
#define AFX_ITEMDATA_H__80EADD4D_B591_11D2_8F45_3078302C2030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CItemData  
{
public:
	CItemData();
	virtual ~CItemData();

    HRESULT Initialize( SAFEARRAY* pArray );

public:    
    static SAFEARRAY* GetDataMemberArray();
    CString GetDateString( SYSTEMTIME * pst );
    void    SetStatusText( );
    void    SetSubmittedText( );

private:
    void ComputeImageIndex();

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

    UINT        m_uImage;
    ULONG       m_ulFileSize;
    GUID        m_ItemGUID;

};

// 
// Safe array elements
// 
#define ITEM_DATA_NUM_ELEMENTS      16

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



// 
// Image list indexes.
// 
#define RESULT_IMAGE_EXE            1
#define RESULT_IMAGE_UNKNOWN        2
#define RESULT_IMAGE_DOCUMENT       3


#endif // !defined(AFX_ITEMDATA_H__80EADD4D_B591_11D2_8F45_3078302C2030__INCLUDED_)
