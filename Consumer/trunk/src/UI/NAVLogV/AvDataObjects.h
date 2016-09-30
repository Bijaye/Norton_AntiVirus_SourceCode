////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SymInterface.h>
#include <AvProdWidgets.h>
#include <uiData.h>
#include <uiNumberData.h>
#include <uiDateData.h>
#include <uiStringData.h>

#include <AVDefines.h>
#include <AVInterfaces.h>

#include <NAVEventCommon.h>

class CManagedEventData :
    public CEventData,
    public StahlSoft::CRefCount
{
public:
    CManagedEventData(void);
    virtual ~CManagedEventData(void);
    CManagedEventData(CEventData* pEventData);
};

typedef StahlSoft::CSmartRefCountPtr<CManagedEventData> CManagedEventDataPtr;


class CAvItemBagDataBase : 
    public ui::IData,
    public ui::CData
{
public:
    CAvItemBagDataBase(void);
    virtual ~CAvItemBagDataBase(void);

public: // IData interface
    IMPLEMENT_DATA(ui::CData);

public:
    void SetDataSource(const SYMGUID& guidDataId, CManagedEventData* pItemBag, LONG lIndex);

protected:
    CManagedEventDataPtr m_spItemBag;
    LONG m_lItemBagIndex;
};

class CAvItemBagStringData :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public AvProdWidgets::CSymObjectCreator<&ui::IID_Data, ui::IData, CAvItemBagStringData>,
    public CAvItemBagDataBase,
    public ui::IStringData
{
public:
    CAvItemBagStringData(void);
    virtual ~CAvItemBagStringData(void);
    
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(ui::IID_Data, ui::IData)
        SYM_INTERFACE_ENTRY(ui::IID_StringData, ui::IStringData)
    SYM_INTERFACE_MAP_END()    

public: // IStringData
    virtual HRESULT GetString(cc::IString*& pString) const throw();
    virtual HRESULT SetString(LPCWSTR wszString) throw();

};

class CAvItemBagNumberData :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public AvProdWidgets::CSymObjectCreator<&ui::IID_Data, ui::IData, CAvItemBagNumberData>,
    public CAvItemBagDataBase,
    public ui::INumberData
{
public:
    CAvItemBagNumberData(void);
    virtual ~CAvItemBagNumberData(void);

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(ui::IID_Data, ui::IData)
        SYM_INTERFACE_ENTRY(ui::IID_NumberData, ui::INumberData)
    SYM_INTERFACE_MAP_END()    

public: // INumberData
    virtual HRESULT GetNumber(LONGLONG& qdwNumber) const throw();
    virtual HRESULT SetNumber(LONGLONG lNumber) throw();

};

class CAvItemBagDateData :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public AvProdWidgets::CSymObjectCreator<&ui::IID_Data, ui::IData, CAvItemBagDateData>,
    public CAvItemBagDataBase,
    public ui::IDateData
{
public:
    CAvItemBagDateData(void);
    virtual ~CAvItemBagDateData(void);

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(ui::IID_Data, ui::IData)
        SYM_INTERFACE_ENTRY(ui::IID_DateData, ui::IDateData)
        SYM_INTERFACE_MAP_END()    

public: // IDateData
    virtual HRESULT GetDate(SYSTEMTIME& stDate) const throw();
    virtual HRESULT SetDate(const SYSTEMTIME& stDate) throw();

};

class CAVDwordMapDataBase : 
    public ui::IData,
    public ui::CData
{
public:
    CAVDwordMapDataBase(void);
    virtual ~CAVDwordMapDataBase(void);

public: // IData interface
    IMPLEMENT_DATA(ui::CData);

public:
    void SetDataSource(const SYMGUID& guidDataId, AVModule::IAVMapDwordData* pAVDwordMap, DWORD dwIndex);

protected:
    AVModule::IAVMapDwordDataPtr m_spAVDwordMap;
    DWORD m_dwAVDwordMapIndex;
};

class CAVDwordMapStringData :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public AvProdWidgets::CSymObjectCreator<&ui::IID_Data, ui::IData, CAVDwordMapStringData>,
    public CAVDwordMapDataBase,
    public ui::IStringData
{
public:
    CAVDwordMapStringData(void);
    virtual ~CAVDwordMapStringData(void);

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(ui::IID_Data, ui::IData)
        SYM_INTERFACE_ENTRY(ui::IID_StringData, ui::IStringData)
    SYM_INTERFACE_MAP_END()    

public: // IStringData
    virtual HRESULT GetString(cc::IString*& pString) const throw();
    virtual HRESULT SetString(LPCWSTR wszString) throw();

};

class CAVDwordMapNumberData :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public AvProdWidgets::CSymObjectCreator<&ui::IID_Data, ui::IData, CAVDwordMapNumberData>,
    public CAVDwordMapDataBase,
    public ui::INumberData
{
public:
    CAVDwordMapNumberData(void);
    virtual ~CAVDwordMapNumberData(void);

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(ui::IID_Data, ui::IData)
        SYM_INTERFACE_ENTRY(ui::IID_NumberData, ui::INumberData)
    SYM_INTERFACE_MAP_END()    

public: // INumberData
    virtual HRESULT GetNumber(LONGLONG& qdwNumber) const throw();
    virtual HRESULT SetNumber(LONGLONG lNumber) throw();

};

class CAVDwordMapDateData :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public AvProdWidgets::CSymObjectCreator<&ui::IID_Data, ui::IData, CAVDwordMapDateData>,
    public CAVDwordMapDataBase,
    public ui::IDateData
{
public:
    CAVDwordMapDateData(void);
    virtual ~CAVDwordMapDateData(void);

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(ui::IID_Data, ui::IData)
        SYM_INTERFACE_ENTRY(ui::IID_DateData, ui::IDateData)
    SYM_INTERFACE_MAP_END()    

public: // IDateData
    virtual HRESULT GetDate(SYSTEMTIME& stDate) const throw();
    virtual HRESULT SetDate(const SYSTEMTIME& stDate) throw();

};

class CAVStrMapDataBase : 
    public ui::IData,
    public ui::CData
{
public:
    CAVStrMapDataBase(void);
    virtual ~CAVStrMapDataBase(void);

public: // IData interface
    IMPLEMENT_DATA(ui::CData);

public:
    void SetDataSource(const SYMGUID& guidDataId, AVModule::IAVMapStrData* pAVStrMap, LPCWSTR wszIndex);

protected:
    AVModule::IAVMapStrDataPtr m_spAVStrMap;
    CStringW m_cwszAVStrMapIndex;
};

class CAVStrMapStringData :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public AvProdWidgets::CSymObjectCreator<&ui::IID_Data, ui::IData, CAVStrMapStringData>,
    public CAVStrMapDataBase,
    public ui::IStringData
{
public:
    CAVStrMapStringData(void);
    virtual ~CAVStrMapStringData(void);

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(ui::IID_Data, ui::IData)
        SYM_INTERFACE_ENTRY(ui::IID_StringData, ui::IStringData)
    SYM_INTERFACE_MAP_END()    

public: // IStringData
    virtual HRESULT GetString(cc::IString*& pString) const throw();
    virtual HRESULT SetString(LPCWSTR wszString) throw();

};

class CAVStrMapNumberData :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public AvProdWidgets::CSymObjectCreator<&ui::IID_Data, ui::IData, CAVStrMapNumberData>,
    public CAVStrMapDataBase,
    public ui::INumberData
{
public:
    CAVStrMapNumberData(void);
    virtual ~CAVStrMapNumberData(void);

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(ui::IID_Data, ui::IData)
        SYM_INTERFACE_ENTRY(ui::IID_NumberData, ui::INumberData)
        SYM_INTERFACE_MAP_END()    

public: // INumberData
    virtual HRESULT GetNumber(LONGLONG& qdwNumber) const throw();
    virtual HRESULT SetNumber(LONGLONG lNumber) throw();

};

class CAVStrMapDateData :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public AvProdWidgets::CSymObjectCreator<&ui::IID_Data, ui::IData, CAVStrMapDateData>,
    public CAVStrMapDataBase,
    public ui::IDateData
{
public:
    CAVStrMapDateData(void);
    virtual ~CAVStrMapDateData(void);

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(ui::IID_Data, ui::IData)
        SYM_INTERFACE_ENTRY(ui::IID_DateData, ui::IDateData)
        SYM_INTERFACE_MAP_END()    

public: // IDateData
    virtual HRESULT GetDate(SYSTEMTIME& stDate) const throw();
    virtual HRESULT SetDate(const SYSTEMTIME& stDate) throw();

};
