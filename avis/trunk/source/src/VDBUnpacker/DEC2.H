// Dec2.h : Decomposer 2 Interface
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2_H)
#define DEC2_H

/////////////////////////////////////////////////////////////////////////////
// Forward Declarations

class IDecomposer;
class IDecEngine;
class IDecObject;
class IDecContainerObject;
class IDecEventSink;


/////////////////////////////////////////////////////////////////////////////
// DEC_TYPE_xxx Codes

enum DecTypes
{
    DEC_TYPE_UNKNOWN,
    DEC_TYPE_AMG,
    DEC_TYPE_ARC,
    DEC_TYPE_ARJ,
    DEC_TYPE_CAB,
    DEC_TYPE_GHO,
    DEC_TYPE_GZIP,
    DEC_TYPE_HQX,
    DEC_TYPE_HTTP,
    DEC_TYPE_JAR,
    DEC_TYPE_LHA,
    DEC_TYPE_LN,
    DEC_TYPE_LZ,
    DEC_TYPE_MIME,
    DEC_TYPE_OLESS,
    DEC_TYPE_RAR,
    DEC_TYPE_RTF,
    DEC_TYPE_TAR,
    DEC_TYPE_UUE,
    DEC_TYPE_XBO,
    DEC_TYPE_ZIP,
    DEC_TYPE_ZOO,
    DEC_TYPE_EXE
};

/////////////////////////////////////////////////////////////////////////////
// DECRESULT Codes

enum DecResult
{
    DECRESULT_UNCHANGED = 0,
    DECRESULT_CHANGED = 1,
    DECRESULT_TO_BE_REPLACED = 2,
    DECRESULT_TO_BE_DELETED = 3
};


/////////////////////////////////////////////////////////////////////////////
// HRESULT Codes
//
// All Decomposer HRESULT error values use the facility FACILITY_ITF
// and error codes in the range 0x0200 - 0xFFFF.  This is in accordance
// with Microsoft's recommendations in the Platform SDK.
//
// Once created, an error code's value must not change.
// Engine authors should resist adding new error codes whenever possible.
// Applications should properly handle unlisted error codes.

#if defined(UNIX)
#define MAKE_HRESULT(sev,fac,code) ((HRESULT)(((unsigned long)(sev)<<31)|((unsigned long)(fac)<<16)|((unsigned long)(code))))
#define FACILITY_ITF 4 
#endif

#define DECERR(code)  MAKE_HRESULT(1, FACILITY_ITF, 0x0200 + code)

enum DecErr
{
    DEC_OK                          = 0,

    DECERR_ENGINE_NOT_SUPPORTED     = DECERR(10),
    DECERR_ENGINE_FILE_MISSING      = DECERR(11),

    DECERR_OBJECT_FILE_INVALID      = DECERR(20),

    DECERR_CONTAINER_OPEN           = DECERR(30),
    DECERR_CONTAINER_ACCESS         = DECERR(31),
    DECERR_CONTAINER_UPDATE         = DECERR(32),
    DECERR_CONTAINER_CLOSE          = DECERR(33),

    DECERR_CHILD_EXTRACT            = DECERR(40),
    DECERR_CHILD_ACCESS             = DECERR(41),
    DECERR_CHILD_DECRYPT            = DECERR(42),
    DECERR_CHILD_SPLIT              = DECERR(43),
    DECERR_CHILD_SPLIT_PART         = DECERR(44),
    DECERR_CHILD_SIZE_UNKNOWN       = DECERR(45),

    DECERR_TEMPDIR_INVALID          = DECERR(50),

    DECERR_TEMPFILE_CREATE          = DECERR(61),
    DECERR_TEMPFILE_OPEN            = DECERR(62),
    DECERR_TEMPFILE_READ            = DECERR(63),
    DECERR_TEMPFILE_WRITE           = DECERR(64),
    DECERR_TEMPFILE_CLOSE           = DECERR(65),
    DECERR_TEMPFILE_DELETE          = DECERR(66),

    DECERR_DISK_FULL                = DECERR(70),
    DECERR_DISK_NOT_READY           = DECERR(71),

    DECERR_OUT_OF_MEMORY            = DECERR(80),

    DECERR_INVALID_PARAMS           = DECERR(90),
    DECERR_INVALID_CHANGE           = DECERR(91),
    DECERR_INVALID_REPLACE          = DECERR(92),
    DECERR_INVALID_DELETE           = DECERR(93),

    DECERR_USER_CANCEL              = DECERR(100),

    DECERR_ENGINE_EXCEPTION         = DECERR(150),

    DECERR_GENERIC                  = DECERR(200)
};


/////////////////////////////////////////////////////////////////////////////
// DEC_CHARSET_xxx codes

enum DecCharset
{
    DEC_CHARSET_US_ASCII,
    DEC_CHARSET_ASCII,
    DEC_CHARSET_ISO646_US,
    DEC_CHARSET_ISO_646_IRV_1991,
    DEC_CHARSET_ISO_IR_6,
    DEC_CHARSET_ANSI_X3_4_1968,
    DEC_CHARSET_CP367,
    DEC_CHARSET_IBM367,
    DEC_CHARSET_US,
    DEC_CHARSET_csASCII,
    DEC_CHARSET_UTF_8,
    DEC_CHARSET_UCS_2,
    DEC_CHARSET_ISO_10646_UCS_2,
    DEC_CHARSET_csUnicode,
    DEC_CHARSET_UCS_4,
    DEC_CHARSET_ISO_10646_UCS_4,
    DEC_CHARSET_csUCS4,
    DEC_CHARSET_UTF_16,
    DEC_CHARSET_UTF_7,
    DEC_CHARSET_UNICODE_1_1_UTF_7,
    DEC_CHARSET_csUnicode11UTF7,
    DEC_CHARSET_UCS_2_INTERNAL,
    DEC_CHARSET_UCS_2_SWAPPED,
    DEC_CHARSET_UCS_2_BE,
    DEC_CHARSET_UNICODEBIG,
    DEC_CHARSET_UNICODE_1_1,
    DEC_CHARSET_csUnicode11,
    DEC_CHARSET_UCS_2_LE,
    DEC_CHARSET_UNICODELITTLE,
    DEC_CHARSET_UCS_4_INTERNAL,
    DEC_CHARSET_UCS_4_SWAPPED,
    DEC_CHARSET_UCS_4_BE,
    DEC_CHARSET_UCS_4_LE,
    DEC_CHARSET_JAVA,
    DEC_CHARSET_ISO_8859_1,
    DEC_CHARSET_ISO_8859_1_ALIAS,
    DEC_CHARSET_ISO_8859_1_1987,
    DEC_CHARSET_ISO_IR_100,
    DEC_CHARSET_CP819,
    DEC_CHARSET_IBM819,
    DEC_CHARSET_LATIN1,
    DEC_CHARSET_L1,
    DEC_CHARSET_csISOLatin1,
    DEC_CHARSET_ISO_8859_2,
    DEC_CHARSET_ISO_8859_2_ALIAS,
    DEC_CHARSET_ISO_8859_2_1987,
    DEC_CHARSET_ISO_IR_101,
    DEC_CHARSET_LATIN2,
    DEC_CHARSET_L2,
    DEC_CHARSET_csISOLatin2,
    DEC_CHARSET_ISO_8859_3,
    DEC_CHARSET_ISO_8859_3_ALIAS,
    DEC_CHARSET_ISO_8859_3_1988,
    DEC_CHARSET_ISO_IR_109,
    DEC_CHARSET_LATIN3,
    DEC_CHARSET_L3,
    DEC_CHARSET_csISOLatin3,
    DEC_CHARSET_ISO_8859_4,
    DEC_CHARSET_ISO_8859_4_ALIAS,
    DEC_CHARSET_ISO_8859_4_1988,
    DEC_CHARSET_ISO_IR_110,
    DEC_CHARSET_LATIN4,
    DEC_CHARSET_L4,
    DEC_CHARSET_csISOLatin4,
    DEC_CHARSET_ISO_8859_5,
    DEC_CHARSET_ISO_8859_5_ALIAS,
    DEC_CHARSET_ISO_8859_5_1988,
    DEC_CHARSET_ISO_IR_144,
    DEC_CHARSET_CYRILLIC,
    DEC_CHARSET_csISOLatinCyrillic,
    DEC_CHARSET_ISO_8859_6,
    DEC_CHARSET_ISO_8859_6_ALIAS,
    DEC_CHARSET_ISO_8859_6_1987,
    DEC_CHARSET_ISO_IR_127,
    DEC_CHARSET_ECMA_114,
    DEC_CHARSET_ASMO_708,
    DEC_CHARSET_ARABIC,
    DEC_CHARSET_csISOLatinArabic,
    DEC_CHARSET_ISO_8859_7,
    DEC_CHARSET_ISO_8859_7_ALIAS,
    DEC_CHARSET_ISO_8859_7_1987,
    DEC_CHARSET_ISO_IR_126,
    DEC_CHARSET_ECMA_118,
    DEC_CHARSET_ELOT_928,
    DEC_CHARSET_GREEK8,
    DEC_CHARSET_GREEK,
    DEC_CHARSET_csISOLatinGreek,
    DEC_CHARSET_ISO_8859_8,
    DEC_CHARSET_ISO_8859_8_ALIAS,
    DEC_CHARSET_ISO_8859_8_1988,
    DEC_CHARSET_ISO_IR_138,
    DEC_CHARSET_HEBREW,
    DEC_CHARSET_csISOLatinHebrew,
    DEC_CHARSET_ISO_8859_9,
    DEC_CHARSET_ISO_8859_9_ALIAS,
    DEC_CHARSET_ISO_8859_9_1989,
    DEC_CHARSET_ISO_IR_148,
    DEC_CHARSET_LATIN5,
    DEC_CHARSET_L5,
    DEC_CHARSET_csISOLatin5,
    DEC_CHARSET_ISO_8859_10,
    DEC_CHARSET_ISO_8859_10_ALIAS,
    DEC_CHARSET_ISO_8859_10_1992,
    DEC_CHARSET_ISO_IR_157,
    DEC_CHARSET_LATIN6,
    DEC_CHARSET_L6,
    DEC_CHARSET_csISOLatin6,
    DEC_CHARSET_ISO_8859_13,
    DEC_CHARSET_ISO_8859_13_ALIAS,
    DEC_CHARSET_ISO_IR_179,
    DEC_CHARSET_LATIN7,
    DEC_CHARSET_L7,
    DEC_CHARSET_ISO_8859_14,
    DEC_CHARSET_ISO_8859_14_ALIAS,
    DEC_CHARSET_ISO_8859_14_1998,
    DEC_CHARSET_LATIN8,
    DEC_CHARSET_L8,
    DEC_CHARSET_ISO_8859_15,
    DEC_CHARSET_ISO_8859_15_ALIAS,
    DEC_CHARSET_ISO_8859_15_1998,
    DEC_CHARSET_KOI8_R,
    DEC_CHARSET_csKOI8R,
    DEC_CHARSET_KOI8_U,
    DEC_CHARSET_KOI8_RU,
    DEC_CHARSET_CP1250,
    DEC_CHARSET_WINDOWS_1250,
    DEC_CHARSET_MS_EE,
    DEC_CHARSET_CP1251,
    DEC_CHARSET_WINDOWS_1251,
    DEC_CHARSET_MS_CYRL,
    DEC_CHARSET_CP1252,            // Windows_1252 WinLatin1 (ANSI)
    DEC_CHARSET_WINDOWS_1252,
    DEC_CHARSET_MS_ANSI,
    DEC_CHARSET_CP1253,
    DEC_CHARSET_WINDOWS_1253,
    DEC_CHARSET_MS_GREEK,
    DEC_CHARSET_CP1254,
    DEC_CHARSET_WINDOWS_1254,
    DEC_CHARSET_MS_TURK,
    DEC_CHARSET_CP1255,
    DEC_CHARSET_WINDOWS_1255,
    DEC_CHARSET_MS_HEBR,
    DEC_CHARSET_CP1256,
    DEC_CHARSET_WINDOWS_1256,
    DEC_CHARSET_MS_ARAB,
    DEC_CHARSET_CP1257,
    DEC_CHARSET_WINDOWS_1257,
    DEC_CHARSET_WINBALTRIM,
    DEC_CHARSET_CP1258,
    DEC_CHARSET_WINDOWS_1258,
    DEC_CHARSET_CP850,              // MS_DOS Latin1 (OEM)
    DEC_CHARSET_IBM850,
    DEC_CHARSET_850,
    DEC_CHARSET_csPC850Multilingual,
    DEC_CHARSET_CP866,
    DEC_CHARSET_IBM866,
    DEC_CHARSET_866,
    DEC_CHARSET_csIBM866,
    DEC_CHARSET_MacRoman,
    DEC_CHARSET_MACINTOSH,
    DEC_CHARSET_MAC,
    DEC_CHARSET_csMacintosh,
    DEC_CHARSET_MacCentralEurope,
    DEC_CHARSET_MacIceland,
    DEC_CHARSET_MacCroatian,
    DEC_CHARSET_MacRomania,
    DEC_CHARSET_MacCyrillic,
    DEC_CHARSET_MacUkraine,
    DEC_CHARSET_MacGreek,
    DEC_CHARSET_MacTurkish,
    DEC_CHARSET_MacHebrew,
    DEC_CHARSET_MacArabic,
    DEC_CHARSET_MacThai,
    DEC_CHARSET_HP_ROMAN8,
    DEC_CHARSET_ROMAN8,
    DEC_CHARSET_R8,
    DEC_CHARSET_csHPRoman8,
    DEC_CHARSET_NEXTSTEP,
    DEC_CHARSET_ARMSCII_8,
    DEC_CHARSET_GEORGIAN_ACADEMY,
    DEC_CHARSET_GEORGIAN_PS,
    DEC_CHARSET_MULELAO_1,
    DEC_CHARSET_CP1133,
    DEC_CHARSET_IBM_CP1133,
    DEC_CHARSET_TIS_620,
    DEC_CHARSET_TIS620,
    DEC_CHARSET_TIS620_0,
    DEC_CHARSET_TIS620_2529_1,
    DEC_CHARSET_TIS620_2533_0,
    DEC_CHARSET_TIS620_2533_1,
    DEC_CHARSET_ISO_IR_166,
    DEC_CHARSET_CP874,
    DEC_CHARSET_WINDOWS_874,
    DEC_CHARSET_VISCII,
    DEC_CHARSET_VISCII1_1_1,
    DEC_CHARSET_csVISCII,
    DEC_CHARSET_TCVN,
    DEC_CHARSET_TCVN_5712,
    DEC_CHARSET_TCVN5712_1,
    DEC_CHARSET_TCVN5712_1_1993,
    DEC_CHARSET_JIS_X0201,
    DEC_CHARSET_JISX0201_1976,
    DEC_CHARSET_JISX0201_1976_0,
    DEC_CHARSET_X0201,
    DEC_CHARSET_csHalfWidthKatakana,
    DEC_CHARSET_JIS_X0208,
    DEC_CHARSET_JIS_X0208_1983_0,
    DEC_CHARSET_JIS_X0208_1983_1,
    DEC_CHARSET_JIS_X0208_1983,
    DEC_CHARSET_JIS_X0208_1990,
    DEC_CHARSET_JIS0208,
    DEC_CHARSET_X0208,
    DEC_CHARSET_ISO_IR_87,
    DEC_CHARSET_csISO87JISX0208,
    DEC_CHARSET_JIS_X0212,
    DEC_CHARSET_JIS_X0212_1990_0,
    DEC_CHARSET_JIS_X0212_1990,
    DEC_CHARSET_X0212,
    DEC_CHARSET_ISO_IR_159,
    DEC_CHARSET_csISO159JISX02121990,
    DEC_CHARSET_GB_2312_80,
    DEC_CHARSET_ISO_IR_58,
    DEC_CHARSET_csISO58GB231280,
    DEC_CHARSET_CHINESE,
    DEC_CHARSET_KSC_5601,
    DEC_CHARSET_KSC5601_1987_0,
    DEC_CHARSET_KS_C_5601_1987,
    DEC_CHARSET_KS_C_5601_1989,
    DEC_CHARSET_ISO_IR_149,
    DEC_CHARSET_csKSC56011987,
    DEC_CHARSET_KOREAN,
    DEC_CHARSET_CP949,
    DEC_CHARSET_EUC_JP,
    DEC_CHARSET_EUCJP,
    DEC_CHARSET_Extended_UNIX_Code_Packed_Format_for_Japanese,
    DEC_CHARSET_csEUCPkdFmtJapanese,
    DEC_CHARSET_SJIS,
    DEC_CHARSET_SHIFT_JIS,
    DEC_CHARSET_SHIFT_JIS_ALIAS,
    DEC_CHARSET_MS_KANJI,
    DEC_CHARSET_csShiftJIS,
    DEC_CHARSET_CP932,
    DEC_CHARSET_ISO_2022_JP,
    DEC_CHARSET_csISO2022JP,
    DEC_CHARSET_ISO_2022_JP_1,
    DEC_CHARSET_ISO_2022_JP_2,
    DEC_CHARSET_EUC_CN,
    DEC_CHARSET_EUCCN,
    DEC_CHARSET_GB2312,
    DEC_CHARSET_csGB2312,
    DEC_CHARSET_GBK,
    DEC_CHARSET_CP936,
    DEC_CHARSET_ISO_2022_CN,
    DEC_CHARSET_csISO2022CN,
    DEC_CHARSET_ISO_2022_CN_EXT,
    DEC_CHARSET_HZ,
    DEC_CHARSET_HZ_GB_2312,
    DEC_CHARSET_EUC_TW,
    DEC_CHARSET_EUCTW,
    DEC_CHARSET_csEUCTW,
    DEC_CHARSET_BIG5,
    DEC_CHARSET_BIG_5,
    DEC_CHARSET_BIG_FIVE,
    DEC_CHARSET_BIGFIVE,
    DEC_CHARSET_csBig5,
    DEC_CHARSET_CP950,
    DEC_CHARSET_EUC_KR,
    DEC_CHARSET_EUCKR,
    DEC_CHARSET_csEUCKR,
    DEC_CHARSET_JOHAB,
    DEC_CHARSET_CP1361,
    DEC_CHARSET_ISO_2022_KR,
    DEC_CHARSET_csISO2022KR
};

#define DEC_CHARSET_LAST    DEC_CHARSET_ISO_2022_JP


/////////////////////////////////////////////////////////////////////////////
// Interface IDecomposer

class IDecomposer
{
public:
    // IDecomposer methods:
    virtual HRESULT AddEngine(IDecEngine* pEngine) = 0;
    virtual HRESULT RemoveEngines() = 0;
    virtual HRESULT SetTempDir(LPCTSTR szDir) = 0;
    virtual HRESULT SetTrustFileExtensions(BOOL bEnabled) = 0;
    virtual HRESULT SetMaxExtractSize(DWORD dwMaxSize) = 0;
    virtual HRESULT SetAbortSignal(BOOL bAbort) = 0;
    virtual HRESULT Process(IDecObject* pObject, IDecEventSink* pSink, WORD* pwResult, LPTSTR pszNewDataFile) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecEngine

class IDecEngine
{
public:
    // IDecEngine methods:
    virtual HRESULT SetTempDir(LPCTSTR szDir) = 0;
    virtual HRESULT SetTrustFileExtensions(BOOL bEnabled) = 0;
    virtual HRESULT SetMaxExtractSize(DWORD dwMaxSize) = 0;
    virtual HRESULT SetAbortSignal(BOOL bAbort) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecObject

class IDecObject
{
public:
    // IDecObject methods:
    virtual HRESULT GetName(LPTSTR pszName) = 0;
    virtual HRESULT GetNameCharacterSet(DWORD *pdwCharset) = 0;
    virtual HRESULT GetParent(IDecContainerObject** ppParentObject) = 0;
    virtual HRESULT GetDepth(DWORD* pdwDepth) = 0;
    virtual HRESULT GetDataFile(LPTSTR pszDataFile) = 0;
    virtual HRESULT GetCanDelete(BOOL* pbCanDelete) = 0;
    virtual HRESULT GetCanDeleteAncestor(BOOL* pbCanDelete, DWORD* pdwDepth) = 0;
    virtual HRESULT GetCanModify(BOOL* pbCanModify) = 0;
    virtual HRESULT GetCanReplace(BOOL* pbCanReplace) = 0;
    virtual HRESULT GetProperty(LPCTSTR szName, void** ppValue) = 0;
    virtual HRESULT SetProperty(LPCTSTR szName, void* pValue) = 0;
    virtual HRESULT RemoveProperty(LPCTSTR szName) = 0;
    virtual HRESULT GetNormalizedName(LPTSTR pszInput, LPTSTR pszOutput, DWORD dwOutputSize, BOOL bANSI) = 0;
    virtual HRESULT GetRenameItemName(LPTSTR *ppszName) = 0;
    virtual HRESULT SetRenameItemName(LPCTSTR pszName, bool bSetParent) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecContainerObject

class IDecContainerObject :
    public IDecObject
{
public:
    // IDecObject methods:
    virtual HRESULT GetName(LPTSTR pszName) = 0;
    virtual HRESULT GetNameCharacterSet(DWORD *pdwCharset) = 0;
    virtual HRESULT GetParent(IDecContainerObject** ppParentObject) = 0;
    virtual HRESULT GetDepth(DWORD* pdwDepth) = 0;
    virtual HRESULT GetDataFile(LPTSTR pszDataFile) = 0;
    virtual HRESULT GetCanDelete(BOOL* pbCanDelete) = 0;
    virtual HRESULT GetCanDeleteAncestor(BOOL* pbCanDelete, DWORD* pdwDepth) = 0;
    virtual HRESULT GetCanModify(BOOL* pbCanModify) = 0;
    virtual HRESULT GetCanReplace(BOOL* pbCanReplace) = 0;
    virtual HRESULT GetProperty(LPCTSTR szName, void** ppValue) = 0;
    virtual HRESULT SetProperty(LPCTSTR szName, void* pValue) = 0;
    virtual HRESULT RemoveProperty(LPCTSTR szName) = 0;
    virtual HRESULT GetNormalizedName(LPTSTR pszInput, LPTSTR pszOutput, DWORD dwOutputSize, BOOL bANSI) = 0;
    virtual HRESULT GetRenameItemName(LPTSTR *ppszName) = 0;
    virtual HRESULT SetRenameItemName(LPCTSTR pszName, bool bSetParent) = 0;

    // IDecContainerObject methods:
    virtual HRESULT GetType(DWORD* pdwType) = 0;
    virtual HRESULT GetUncompressedSize(DWORDLONG* pdwlSize) = 0;
    virtual HRESULT GetCompressedSize(DWORDLONG* pdwlSize) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecEventSink

class IDecEventSink
{
public:
    // IDecEventSink methods:

    virtual HRESULT OnNew(
        IDecObject* pObject, 
        WORD* pwResult, 
        LPTSTR pszNewDataFile, 
        BOOL* pbContinue) = 0;

    virtual HRESULT OnIdentified(
        IDecContainerObject* pObject, 
        BOOL* pbContinue) = 0;

    virtual HRESULT OnChildFound(
        IDecContainerObject* pObject, 
        LPCTSTR pszChildName,
        BOOL* pbContinue) = 0;

    virtual HRESULT OnBadChild(
        IDecContainerObject* pObject, 
        HRESULT hr,
        LPCTSTR pszChildName,
        BOOL* pbContinue) = 0;

    virtual HRESULT OnNotIdentified(
        IDecObject* pObject, 
        WORD* pwResult, 
        LPTSTR pszNewDataFile) = 0; 

    virtual HRESULT OnBusy() = 0;

    virtual HRESULT OnEngineError(
        IDecContainerObject* pObject, 
        HRESULT hr,
        WORD* pwResult, 
        LPTSTR pszNewDataFile) = 0; 
};


/////////////////////////////////////////////////////////////////////////////
// Class CDecEventSink
//
// Standard implementation of a Decomposer event sink.

class CDecEventSink : 
    public IDecEventSink
{
public:
    virtual HRESULT OnNew(
        IDecObject* /*pObject*/,
        WORD* pwResult,
        LPTSTR pszNewDataFile,
        BOOL* pbContinue)
    {
        *pwResult = DECRESULT_UNCHANGED;
        *pszNewDataFile = '\0';
        *pbContinue = TRUE;
        return DEC_OK;
    }

    virtual HRESULT OnIdentified(
        IDecContainerObject* /*pObject*/,
        BOOL* pbContinue)
    {
        *pbContinue = TRUE;
        return DEC_OK;
    }

    virtual HRESULT OnChildFound(
        IDecContainerObject* /*pObject*/,
        LPCTSTR /*pszChildName*/,
        BOOL* pbContinue)
    { 
        *pbContinue = TRUE;
        return DEC_OK;
    }

    virtual HRESULT OnBadChild(
        IDecContainerObject* /*pObject*/,
        HRESULT hr,
        LPCTSTR /*pszChildName*/,
        BOOL* pbContinue)
    {
        *pbContinue = FALSE;
        return hr;
    }

    virtual HRESULT OnNotIdentified(
        IDecObject* /*pObject*/,
        WORD* pwResult,
        LPTSTR pszNewDataFile)
    {
        *pwResult = DECRESULT_UNCHANGED;
        *pszNewDataFile = '\0';
        return DEC_OK;
    } 

    virtual HRESULT OnBusy() 
    { 
        return DEC_OK; 
    }

    virtual HRESULT OnEngineError(
        IDecContainerObject* /*pObject*/,
        HRESULT hr,
        WORD* pwResult,
        LPTSTR pszNewDataFile)
    {
        *pwResult = DECRESULT_UNCHANGED;
        *pszNewDataFile = '\0';
        return hr;
    } 
};


/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2_CPP
#define DEC2IE __declspec(dllexport) 
#else
#define DEC2IE __declspec(dllimport) 
#endif
#endif

#if defined(_WINDOWS)
DEC2IE HRESULT DecNewDecomposer(IDecomposer** ppDecomposer);
DEC2IE HRESULT DecDeleteDecomposer(IDecomposer* pDecomposer);
DEC2IE HRESULT DecNewFileObject(LPCTSTR szName, LPCTSTR szDataFile, IDecObject** ppObject);
DEC2IE HRESULT DecDeleteFileObject(IDecObject* pObject);
DEC2IE void    DecGetVersion(const char **ppszVersion);
#endif

#if defined(UNIX)
EXTERN_C HRESULT DecNewDecomposer(IDecomposer** ppDecomposer);
EXTERN_C HRESULT DecDeleteDecomposer(IDecomposer* pDecomposer);
EXTERN_C HRESULT DecNewFileObject(LPCTSTR szName, LPCTSTR szDataFile, IDecObject** ppObject);
EXTERN_C HRESULT DecDeleteFileObject(IDecObject* pObject);
EXTERN_C void    DecGetVersion(const char **ppszVersion);
#endif

#endif  // DEC2_H
