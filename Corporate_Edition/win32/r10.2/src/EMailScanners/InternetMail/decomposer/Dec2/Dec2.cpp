// Dec2.cpp : Decomposer 2
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2000, 2005 by Symantec Corporation.  All rights reserved.

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

// C Library headers
#include "dec_assert.h"
#include <time.h>

#if defined(_WINDOWS)
	#include <io.h>
#else
	#include <unistd.h>
#endif

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

// Project headers
#define DEC2_CPP
#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2Trak.h"
#include "Dec2LIB.h"
#include "Decstdio.h"
#include "DecIO.h"
#include "Dec2Ver.h"
#include "dectmpf.h"
#include "asc_char.h"

const char gszDecVersion[] = VERSION_STRING;

// Define the special option ID used internally to represent an
// empty slot in the m_Options array.
#define DEC_OPTION_UNUSED	0


typedef struct tagDecOption
{
	DWORD	 dwID;
	DWORD	 dwValue;
} DECOPTION, *PDECOPTION;


/////////////////////////////////////////////////////////////////////////////
// Class CDecomposer
//
// The main Decomposer class.
//
// Applications use DecNewDecomposer() and DecDeleteDecomposer() to create
// and release objects of this class.

class CDecomposer :
	public IDecomposerEx
{
public:
	CDecomposer();
	virtual ~CDecomposer();

	// IDecomposer methods:
	virtual DECRESULT AddEngine(IDecEngine *pEngine);
	virtual DECRESULT RemoveEngines(void);
	virtual DECRESULT SetTempDir(const char *szDir);
	virtual DECRESULT SetMaxExtractSize(DWORD dwMaxSize);
	virtual DECRESULT AbortProcess(bool bAbort);
	virtual DECRESULT SetOption(DWORD dwOptionID, DWORD dwValue);
	virtual DECRESULT GetOption(DWORD dwOptionID, DWORD *pdwValue);
	virtual DECRESULT DecProcess(IDecObject *pObject,
								IDecEventSink *pSink,
								IDecIOCB *pIOCallback,
								WORD *pwResult,
								char *pszNewDataFile);
	virtual DECRESULT Process(IDecObject *pObject,
							 IDecEventSink *pSink,
							 IDecIOCB *pIOCallback,
							 WORD *pwResult,
							 char *pszNewDataFile);

	// IDecomposerEx methods:
	virtual DECRESULT DecProcess(IDecContainerObjectEx *pObject, IDecEventSink *pSink, WORD *pwResult, char *pszNewDataFile);

	//
	// Used by any engine that is not thread-safe.  For instance,
	// this is only used by the AMG engine at the moment.  In the 
	// static lib implementation for Netware, there is no way to
	// initialize a lock object in the engine itself.  We need to
	// ask the Decomposer itself for a lock object that is already
	// created and ready to go.  This frees the engine from worrying
	// about creating/destroying this object.  All platforms and
	// implementations (static lib, dll, shared obj, nlm) will now
	// use this method in order to be consistent.
	//
	virtual CCriticalSection & GetCriticalSection() { return m_CriticalSection; }
	virtual DWORD *GetUniquePtr() { return &m_dwUnique; }
	virtual DWORD GetUniqueValue();

private:
	// This value is used to generate temporary filenames.
	DWORD					m_dwUnique;
	CCriticalSection		m_UniqueCriticalSection;
	CCriticalSection		m_CriticalSection;
	IDecEngineEx			**m_Engines;
	int						m_EnginesCount;
	int						m_EnginesCapacity;
	DWORD					m_dwEventSinkInterfaceVersion;
	DECOPTION				m_Options[DEC_OPTION_COUNT];
};


/////////////////////////////////////////////////////////////////////////////
// Internal Event Sink Trace Class.  This class wraps the callers EventSink
// and logs output.
/////////////////////////////////////////////////////////////////////////////

class CEventSinkTracer: public IDecEventSink
{
public:

	CEventSinkTracer()
	:	m_pSink(NULL),
		m_pDecomposer(NULL),
		m_dwTraceOption(0),
		m_cInOutSeparator('\t')
	{
	}

	virtual ~CEventSinkTracer() {}

	void SetTraceOption(DWORD TraceOption)
	{
		m_dwTraceOption = TraceOption; 

		if (m_dwTraceOption & TRACE_OPTION_LINE_SEPARATOR_CRLF)
		{
			m_cInOutSeparator = '\n';
		}
	}

	void SetEventSink(IDecEventSink* pSink, CDecomposer* pDecomposer)
	{
		m_pSink = pSink;
		m_pDecomposer = pDecomposer;
	}

	const char*	GetOptionName(int option) const
	{
		switch (option)
		{
			case DEC_OPTION_TRUNCATE_TEMP_FILES:			return "dec_option_truncate_temp_files";
			case DEC_OPTION_ENFORCE_CRC:					return "dec_option_enforce_crc";
			case DEC_OPTION_EXTRACT_RTFHTML:				return "dec_option_extract_rtfhtml";
			case DEC_OPTION_EXTRACT_OLE10NATIVE_ONLY:		return "dec_option_extract_ole10native_only";
			case DEC_OPTION_NONMIMETHRESHOLD:				return "dec_option_nonmimethreshold";
			case DEC_OPTION_ENABLE_MIME_ENGINE:				return "dec_option_enable_mime_engine";
			case DEC_OPTION_ENABLE_UUE_ENGINE:				return "dec_option_enable_uue_engine";
			case DEC_OPTION_ENABLE_BINHEX_ENGINE:			return "dec_option_enable_binhex_engine";
			case DEC_OPTION_LOG_WARNINGS:					return "dec_option_log_warnings";
			case DEC_OPTION_MAX_TEXT_SCAN_BYTES:			return "dec_option_max_text_scan_bytes";
			case DEC_OPTION_EVENTSINK_VERSION:				return "dec_option_eventsink_version";
			case DEC_OPTION_MIME_FUZZY_MAIN_HEADER:			return "dec_option_mime_fuzzy_main_header";
			case DEC_OPTION_ENHANCED_TEXT_ID:				return "dec_option_enhanced_text_id";
			case DEC_OPTION_TRANSLATE_NULL:					return "dec_option_translate_null";
			case DEC_OPTION_NONHQXTHRESHOLD:				return "dec_option_nonhqxthreshold";
			case DEC_OPTION_NONUUETHRESHOLD_BYTES:			return "dec_option_nonuuethreshold_bytes";
			case DEC_OPTION_MIN_MIME_SCAN_BYTES:			return "dec_option_min_mime_scan_bytes";
			case DEC_OPTION_ENABLE_MBOX_ENGINE:				return "dec_option_enable_mbox_engine";
			case DEC_OPTION_MIME_IDENTIFICATION_STRENGTH:	return "dec_option_mime_identification_strength";
			case DEC_OPTION_USE_OLE10NATIVE_CHILD_NAME:		return "dec_option_use_ole10native_child_name";
			case DEC_OPTION_EVENTSINK_TRACE:				return "dec_option_eventsink_trace";

			default: 
				return "PLEASE UPDATE GetOptionName in class CEventSinkTracer in file Dec2\\Dec2.cpp";
		}

	}
	void DumpOptions() const
	{
		if (!(m_dwTraceOption & TRACE_OPTION_DISABLE_DUMP_OPTIONS))
		{
			dec_printf("Options {\n");

			for ( int index = DEC_OPTION_FIRST; index <= DEC_OPTION_LAST; index++ )
			{
				DECRESULT	result;
				DWORD		dwValue;

				if ( (result = m_pDecomposer->GetOption(index,&dwValue)) == DEC_OK )
				{
					dec_printf("  DEFINED:   Value[%8d]\t%s\n",dwValue,GetOptionName(index));
				}
				else if ( result == DECERR_GENERIC )
				{
					dec_printf("  UNDEFINED: \t\t\t%s.\n",GetOptionName(index));
				}
			}

			dec_printf("}\n\n");
		}
	}

	const char* ConvertResult(DECRESULT result)
	{
		switch (result)
		{
			case DECRESULT_UNCHANGED:		return "decresult_unchanged";
			case DECRESULT_CHANGED:			return "decresult_changed";
			case DECRESULT_TO_BE_REPLACED:	return "decresult_to_be_replaced";
			case DECRESULT_TO_BE_DELETED:	return "decresult_to_be_deleted";

			default:						return "Unknown Result Value";
		}
	}

	const char* ConvertBoolean(bool bContinue)
	{
		return (bContinue ? "true" : "false");
	}

	const char* ConvertString(const char* pString)
	{
		return (pString == NULL ? "Null" : pString);
	}

	const char* ConvertDecResult(DECRESULT nDecResult)
	{
		switch(nDecResult)
		{
			case 0:				return ("DEC_OK");
			case DECERR(10):	return ("DECERR_ENGINE_NOT_SUPPORTED");
			case DECERR(11):	return ("DECERR_ENGINE_FILE_MISSING");
			case DECERR(20):	return ("DECERR_OBJECT_FILE_INVALID");
			case DECERR(30):	return ("DECERR_CONTAINER_OPEN");
			case DECERR(31):	return ("DECERR_CONTAINER_ACCESS");
			case DECERR(32):	return ("DECERR_CONTAINER_UPDATE");
			case DECERR(33):	return ("DECERR_CONTAINER_CLOSE");
			case DECERR(40):	return ("DECERR_CHILD_EXTRACT");
			case DECERR(41):	return ("DECERR_CHILD_ACCESS");
			case DECERR(42):	return ("DECERR_CHILD_DECRYPT");
			case DECERR(43):	return ("DECERR_CHILD_SPLIT");
			case DECERR(44):	return ("DECERR_CHILD_SPLIT_PART");
			case DECERR(45):	return ("DECERR_CHILD_SIZE_UNKNOWN");
			case DECERR(46):	return ("DECERR_CHILD_MAX_SIZE");
			case DECERR(47):	return ("DECERR_CHILD_MIN_SIZE");
			case DECERR(50):	return ("DECERR_TEMPDIR_INVALID");
			case DECERR(61):	return ("DECERR_TEMPFILE_CREATE");
			case DECERR(62):	return ("DECERR_TEMPFILE_OPEN");
			case DECERR(63):	return ("DECERR_TEMPFILE_READ");
			case DECERR(64):	return ("DECERR_TEMPFILE_WRITE");
			case DECERR(65):	return ("DECERR_TEMPFILE_CLOSE");
			case DECERR(66):	return ("DECERR_TEMPFILE_DELETE");
			case DECERR(80):	return ("DECERR_OUT_OF_MEMORY");
			case DECERR(90):	return ("DECERR_INVALID_PARAMS");
			case DECERR(91):	return ("DECERR_INVALID_CHANGE");
			case DECERR(92):	return ("DECERR_INVALID_REPLACE");
			case DECERR(93):	return ("DECERR_INVALID_DELETE");
			case DECERR(100):	return ("DECERR_USER_CANCEL");
			case DECERR(150):	return ("DECERR_ENGINE_EXCEPTION");
			case DECERR(200):	return ("DECERR_GENERIC");

			default:			return ("UNKNOWN ERROR");
		}
	}


	const char* ConvertType(DWORD type)
	{
		switch ( type )
		{
			case DEC_ITEMTYPE_NORMAL:			return "dec_itemtype_normal";
			case DEC_ITEMTYPE_MIME_BODY:		return "dec_itemtype_mime_body";
			case DEC_ITEMTYPE_MIME_DATE:		return "dec_itemtype_mime_date";
			case DEC_ITEMTYPE_MIME_HEADERS:		return "dec_itemtype_mime_headers";

			default:							return "Unknown Type";
		}
	}

	const char* ConvertCharset(DWORD charset)
	{
		switch (charset)
		{
			case	DEC_CHARSET_US_ASCII:			return	"dec_charset_us_ascii";
			case	DEC_CHARSET_ASCII:				return	"dec_charset_ascii";
			case	DEC_CHARSET_ISO646_US:			return	"dec_charset_iso646_us";
			case	DEC_CHARSET_ISO_646_IRV_1991:	return	"dec_charset_iso_646_irv_1991";
			case	DEC_CHARSET_ISO_IR_6:			return	"dec_charset_iso_ir_6";
			case	DEC_CHARSET_ANSI_X3_4_1968:		return	"dec_charset_ansi_x3_4_1968";
			case	DEC_CHARSET_CP367:				return	"dec_charset_cp367";
			case	DEC_CHARSET_IBM367:				return	"dec_charset_ibm367";
			case	DEC_CHARSET_US:					return	"dec_charset_us";
			case	DEC_CHARSET_csASCII:			return	"dec_charset_csascii";
			case	DEC_CHARSET_UTF_8:				return	"dec_charset_utf_8";
			case	DEC_CHARSET_UCS_2:				return	"dec_charset_ucs_2";
			case	DEC_CHARSET_ISO_10646_UCS_2:	return	"dec_charset_iso_10646_ucs_2";
			case	DEC_CHARSET_csUnicode:			return	"dec_charset_csunicode";
			case	DEC_CHARSET_UCS_4:				return	"dec_charset_ucs_4";
			case	DEC_CHARSET_ISO_10646_UCS_4:	return	"dec_charset_iso_10646_ucs_4";
			case	DEC_CHARSET_csUCS4:				return	"dec_charset_csucs4";
			case	DEC_CHARSET_UTF_16:				return	"dec_charset_utf_16";
			case	DEC_CHARSET_UTF_7:				return	"dec_charset_utf_7";
			case	DEC_CHARSET_UNICODE_1_1_UTF_7:	return	"dec_charset_unicode_1_1_utf_7";
			case	DEC_CHARSET_csUnicode11UTF7:	return	"dec_charset_csunicode11utf7";
			case	DEC_CHARSET_UCS_2_INTERNAL:		return	"dec_charset_ucs_2_internal";
			case	DEC_CHARSET_UCS_2_SWAPPED:		return	"dec_charset_ucs_2_swapped";
			case	DEC_CHARSET_UCS_2_BE:			return	"dec_charset_ucs_2_be";
			case	DEC_CHARSET_UNICODEBIG:			return	"dec_charset_unicodebig";
			case	DEC_CHARSET_UNICODE_1_1:		return	"dec_charset_unicode_1_1";
			case	DEC_CHARSET_csUnicode11:		return	"dec_charset_csunicode11";
			case	DEC_CHARSET_UCS_2_LE:			return	"dec_charset_ucs_2_le";
			case	DEC_CHARSET_UNICODELITTLE:		return	"dec_charset_unicodelittle";
			case	DEC_CHARSET_UCS_4_INTERNAL:		return	"dec_charset_ucs_4_internal";
			case	DEC_CHARSET_UCS_4_SWAPPED:		return	"dec_charset_ucs_4_swapped";
			case	DEC_CHARSET_UCS_4_BE:			return	"dec_charset_ucs_4_be";
			case	DEC_CHARSET_UCS_4_LE:			return	"dec_charset_ucs_4_le";
			case	DEC_CHARSET_JAVA:				return	"dec_charset_java";
			case	DEC_CHARSET_ISO_8859_1:			return	"dec_charset_iso_8859_1";
			case	DEC_CHARSET_ISO_8859_1_ALIAS:	return	"dec_charset_iso_8859_1_alias";
			case	DEC_CHARSET_ISO_8859_1_1987:	return	"dec_charset_iso_8859_1_1987";
			case	DEC_CHARSET_ISO_IR_100:			return	"dec_charset_iso_ir_100";
			case	DEC_CHARSET_CP819:				return	"dec_charset_cp819";
			case	DEC_CHARSET_IBM819:				return	"dec_charset_ibm819";
			case	DEC_CHARSET_LATIN1:				return	"dec_charset_latin1";
			case	DEC_CHARSET_L1:					return	"dec_charset_l1";
			case	DEC_CHARSET_csISOLatin1:		return	"dec_charset_csisolatin1";
			case	DEC_CHARSET_ISO_8859_2:			return	"dec_charset_iso_8859_2";
			case	DEC_CHARSET_ISO_8859_2_ALIAS:	return	"dec_charset_iso_8859_2_alias";
			case	DEC_CHARSET_ISO_8859_2_1987:	return	"dec_charset_iso_8859_2_1987";
			case	DEC_CHARSET_ISO_IR_101:			return	"dec_charset_iso_ir_101";
			case	DEC_CHARSET_LATIN2:				return	"dec_charset_latin2";
			case	DEC_CHARSET_L2:					return	"dec_charset_l2";
			case	DEC_CHARSET_csISOLatin2:		return	"dec_charset_csisolatin2";
			case	DEC_CHARSET_ISO_8859_3:			return	"dec_charset_iso_8859_3";
			case	DEC_CHARSET_ISO_8859_3_ALIAS:	return	"dec_charset_iso_8859_3_alias";
			case	DEC_CHARSET_ISO_8859_3_1988:	return	"dec_charset_iso_8859_3_1988";
			case	DEC_CHARSET_ISO_IR_109:			return	"dec_charset_iso_ir_109";
			case	DEC_CHARSET_LATIN3:				return	"dec_charset_latin3";
			case	DEC_CHARSET_L3:					return	"dec_charset_l3";
			case	DEC_CHARSET_csISOLatin3:		return	"dec_charset_csisolatin3";
			case	DEC_CHARSET_ISO_8859_4:			return	"dec_charset_iso_8859_4";
			case	DEC_CHARSET_ISO_8859_4_ALIAS:	return	"dec_charset_iso_8859_4_alias";
			case	DEC_CHARSET_ISO_8859_4_1988:	return	"dec_charset_iso_8859_4_1988";
			case	DEC_CHARSET_ISO_IR_110:			return	"dec_charset_iso_ir_110";
			case	DEC_CHARSET_LATIN4:				return	"dec_charset_latin4";
			case	DEC_CHARSET_L4:					return	"dec_charset_l4";
			case	DEC_CHARSET_csISOLatin4:		return	"dec_charset_csisolatin4";
			case	DEC_CHARSET_ISO_8859_5:			return	"dec_charset_iso_8859_5";
			case	DEC_CHARSET_ISO_8859_5_ALIAS:	return	"dec_charset_iso_8859_5_alias";
			case	DEC_CHARSET_ISO_8859_5_1988:	return	"dec_charset_iso_8859_5_1988";
			case	DEC_CHARSET_ISO_IR_144:			return	"dec_charset_iso_ir_144";
			case	DEC_CHARSET_CYRILLIC:			return	"dec_charset_cyrillic";
			case	DEC_CHARSET_csISOLatinCyrillic:	return	"dec_charset_csisolatincyrillic";
			case	DEC_CHARSET_ISO_8859_6:			return	"dec_charset_iso_8859_6";
			case	DEC_CHARSET_ISO_8859_6_ALIAS:	return	"dec_charset_iso_8859_6_alias";
			case	DEC_CHARSET_ISO_8859_6_1987:	return	"dec_charset_iso_8859_6_1987";
			case	DEC_CHARSET_ISO_IR_127:			return	"dec_charset_iso_ir_127";
			case	DEC_CHARSET_ECMA_114:			return	"dec_charset_ecma_114";
			case	DEC_CHARSET_ASMO_708:			return	"dec_charset_asmo_708";
			case	DEC_CHARSET_ARABIC:				return	"dec_charset_arabic";
			case	DEC_CHARSET_csISOLatinArabic:	return	"dec_charset_csisolatinarabic";
			case	DEC_CHARSET_ISO_8859_7:			return	"dec_charset_iso_8859_7";
			case	DEC_CHARSET_ISO_8859_7_ALIAS:	return	"dec_charset_iso_8859_7_alias";
			case	DEC_CHARSET_ISO_8859_7_1987:	return	"dec_charset_iso_8859_7_1987";
			case	DEC_CHARSET_ISO_IR_126:			return	"dec_charset_iso_ir_126";
			case	DEC_CHARSET_ECMA_118:			return	"dec_charset_ecma_118";
			case	DEC_CHARSET_ELOT_928:			return	"dec_charset_elot_928";
			case	DEC_CHARSET_GREEK8:				return	"dec_charset_greek8";
			case	DEC_CHARSET_GREEK:				return	"dec_charset_greek";
			case	DEC_CHARSET_csISOLatinGreek:	return	"dec_charset_csisolatingreek";
			case	DEC_CHARSET_ISO_8859_8:			return	"dec_charset_iso_8859_8";
			case	DEC_CHARSET_ISO_8859_8_ALIAS:	return	"dec_charset_iso_8859_8_alias";
			case	DEC_CHARSET_ISO_8859_8_1988:	return	"dec_charset_iso_8859_8_1988";
			case	DEC_CHARSET_ISO_IR_138:			return	"dec_charset_iso_ir_138";
			case	DEC_CHARSET_HEBREW:				return	"dec_charset_hebrew";
			case	DEC_CHARSET_csISOLatinHebrew:	return	"dec_charset_csisolatinhebrew";
			case	DEC_CHARSET_ISO_8859_9:			return	"dec_charset_iso_8859_9";
			case	DEC_CHARSET_ISO_8859_9_ALIAS:	return	"dec_charset_iso_8859_9_alias";
			case	DEC_CHARSET_ISO_8859_9_1989:	return	"dec_charset_iso_8859_9_1989";
			case	DEC_CHARSET_ISO_IR_148:			return	"dec_charset_iso_ir_148";
			case	DEC_CHARSET_LATIN5:				return	"dec_charset_latin5";
			case	DEC_CHARSET_L5:					return	"dec_charset_l5";
			case	DEC_CHARSET_csISOLatin5:		return	"dec_charset_csisolatin5";
			case	DEC_CHARSET_ISO_8859_10:		return	"dec_charset_iso_8859_10";
			case	DEC_CHARSET_ISO_8859_10_ALIAS:	return	"dec_charset_iso_8859_10_alias";
			case	DEC_CHARSET_ISO_8859_10_1992:	return	"dec_charset_iso_8859_10_1992";
			case	DEC_CHARSET_ISO_IR_157:			return	"dec_charset_iso_ir_157";
			case	DEC_CHARSET_LATIN6:				return	"dec_charset_latin6";
			case	DEC_CHARSET_L6:					return	"dec_charset_l6";
			case	DEC_CHARSET_csISOLatin6:		return	"dec_charset_csisolatin6";
			case	DEC_CHARSET_ISO_8859_13:		return	"dec_charset_iso_8859_13";
			case	DEC_CHARSET_ISO_8859_13_ALIAS:	return	"dec_charset_iso_8859_13_alias";
			case	DEC_CHARSET_ISO_IR_179:			return	"dec_charset_iso_ir_179";
			case	DEC_CHARSET_LATIN7:				return	"dec_charset_latin7";
			case	DEC_CHARSET_L7:					return	"dec_charset_l7";
			case	DEC_CHARSET_ISO_8859_14:		return	"dec_charset_iso_8859_14";
			case	DEC_CHARSET_ISO_8859_14_ALIAS:	return	"dec_charset_iso_8859_14_alias";
			case	DEC_CHARSET_ISO_8859_14_1998:	return	"dec_charset_iso_8859_14_1998";
			case	DEC_CHARSET_LATIN8:				return	"dec_charset_latin8";
			case	DEC_CHARSET_L8:					return	"dec_charset_l8";
			case	DEC_CHARSET_ISO_8859_15:		return	"dec_charset_iso_8859_15";
			case	DEC_CHARSET_ISO_8859_15_ALIAS:	return	"dec_charset_iso_8859_15_alias";
			case	DEC_CHARSET_ISO_8859_15_1998:	return	"dec_charset_iso_8859_15_1998";
			case	DEC_CHARSET_KOI8_R:				return	"dec_charset_koi8_r";
			case	DEC_CHARSET_csKOI8R:			return	"dec_charset_cskoi8r";
			case	DEC_CHARSET_KOI8_U:				return	"dec_charset_koi8_u";
			case	DEC_CHARSET_KOI8_RU:			return	"dec_charset_koi8_ru";
			case	DEC_CHARSET_CP1250:				return	"dec_charset_cp1250";
			case	DEC_CHARSET_WINDOWS_1250:		return	"dec_charset_windows_1250";
			case	DEC_CHARSET_MS_EE:				return	"dec_charset_ms_ee";
			case	DEC_CHARSET_CP1251:				return	"dec_charset_cp1251";
			case	DEC_CHARSET_WINDOWS_1251:		return	"dec_charset_windows_1251";
			case	DEC_CHARSET_MS_CYRL:			return	"dec_charset_ms_cyrl";
			case	DEC_CHARSET_CP1252:				return	"dec_charset_cp1252";
			case	DEC_CHARSET_WINDOWS_1252:		return	"dec_charset_windows_1252";
			case	DEC_CHARSET_MS_ANSI:			return	"dec_charset_ms_ansi";
			case	DEC_CHARSET_CP1253:				return	"dec_charset_cp1253";
			case	DEC_CHARSET_WINDOWS_1253:		return	"dec_charset_windows_1253";
			case	DEC_CHARSET_MS_GREEK:			return	"dec_charset_ms_greek";
			case	DEC_CHARSET_CP1254:				return	"dec_charset_cp1254";
			case	DEC_CHARSET_WINDOWS_1254:		return	"dec_charset_windows_1254";
			case	DEC_CHARSET_MS_TURK:			return	"dec_charset_ms_turk";
			case	DEC_CHARSET_CP1255:				return	"dec_charset_cp1255";
			case	DEC_CHARSET_WINDOWS_1255:		return	"dec_charset_windows_1255";
			case	DEC_CHARSET_MS_HEBR:			return	"dec_charset_ms_hebr";
			case	DEC_CHARSET_CP1256:				return	"dec_charset_cp1256";
			case	DEC_CHARSET_WINDOWS_1256:		return	"dec_charset_windows_1256";
			case	DEC_CHARSET_MS_ARAB:			return	"dec_charset_ms_arab";
			case	DEC_CHARSET_CP1257:				return	"dec_charset_cp1257";
			case	DEC_CHARSET_WINDOWS_1257:		return	"dec_charset_windows_1257";
			case	DEC_CHARSET_WINBALTRIM:			return	"dec_charset_winbaltrim";
			case	DEC_CHARSET_CP1258:				return	"dec_charset_cp1258";
			case	DEC_CHARSET_WINDOWS_1258:		return	"dec_charset_windows_1258";
			case	DEC_CHARSET_CP850:				return	"dec_charset_cp850";
			case	DEC_CHARSET_IBM850:				return	"dec_charset_ibm850";
			case	DEC_CHARSET_850:				return	"dec_charset_850";
			case	DEC_CHARSET_csPC850Multilingual:return	"dec_charset_cspc850multilingual";
			case	DEC_CHARSET_CP866:				return	"dec_charset_cp866";
			case	DEC_CHARSET_IBM866:				return	"dec_charset_ibm866";
			case	DEC_CHARSET_866:				return	"dec_charset_866";
			case	DEC_CHARSET_csIBM866:			return	"dec_charset_csibm866";
			case	DEC_CHARSET_MacRoman:			return	"dec_charset_macroman";
			case	DEC_CHARSET_MACINTOSH:			return	"dec_charset_macintosh";
			case	DEC_CHARSET_MAC:				return	"dec_charset_mac";
			case	DEC_CHARSET_csMacintosh:		return	"dec_charset_csmacintosh";
			case	DEC_CHARSET_MacCentralEurope:	return	"dec_charset_maccentraleurope";
			case	DEC_CHARSET_MacIceland:			return	"dec_charset_maciceland";
			case	DEC_CHARSET_MacCroatian:		return	"dec_charset_maccroatian";
			case	DEC_CHARSET_MacRomania:			return	"dec_charset_macromania";
			case	DEC_CHARSET_MacCyrillic:		return	"dec_charset_maccyrillic";
			case	DEC_CHARSET_MacUkraine:			return	"dec_charset_macukraine";
			case	DEC_CHARSET_MacGreek:			return	"dec_charset_macgreek";
			case	DEC_CHARSET_MacTurkish:			return	"dec_charset_macturkish";
			case	DEC_CHARSET_MacHebrew:			return	"dec_charset_machebrew";
			case	DEC_CHARSET_MacArabic:			return	"dec_charset_macarabic";
			case	DEC_CHARSET_MacThai:			return	"dec_charset_macthai";
			case	DEC_CHARSET_HP_ROMAN8:			return	"dec_charset_hp_roman8";
			case	DEC_CHARSET_ROMAN8:				return	"dec_charset_roman8";
			case	DEC_CHARSET_R8:					return	"dec_charset_r8";
			case	DEC_CHARSET_csHPRoman8:			return	"dec_charset_cshproman8";
			case	DEC_CHARSET_NEXTSTEP:			return	"dec_charset_nextstep";
			case	DEC_CHARSET_ARMSCII_8:			return	"dec_charset_armscii_8";
			case	DEC_CHARSET_GEORGIAN_ACADEMY:	return	"dec_charset_georgian_academy";
			case	DEC_CHARSET_GEORGIAN_PS:		return	"dec_charset_georgian_ps";
			case	DEC_CHARSET_MULELAO_1:			return	"dec_charset_mulelao_1";
			case	DEC_CHARSET_CP1133:				return	"dec_charset_cp1133";
			case	DEC_CHARSET_IBM_CP1133:			return	"dec_charset_ibm_cp1133";
			case	DEC_CHARSET_TIS_620:			return	"dec_charset_tis_620";
			case	DEC_CHARSET_TIS620:				return	"dec_charset_tis620";
			case	DEC_CHARSET_TIS620_0:			return	"dec_charset_tis620_0";
			case	DEC_CHARSET_TIS620_2529_1:		return	"dec_charset_tis620_2529_1";
			case	DEC_CHARSET_TIS620_2533_0:		return	"dec_charset_tis620_2533_0";
			case	DEC_CHARSET_TIS620_2533_1:		return	"dec_charset_tis620_2533_1";
			case	DEC_CHARSET_ISO_IR_166:			return	"dec_charset_iso_ir_166";
			case	DEC_CHARSET_CP874:				return	"dec_charset_cp874";
			case	DEC_CHARSET_WINDOWS_874:		return	"dec_charset_windows_874";
			case	DEC_CHARSET_VISCII:				return	"dec_charset_viscii";
			case	DEC_CHARSET_VISCII1_1_1:		return	"dec_charset_viscii1_1_1";
			case	DEC_CHARSET_csVISCII:			return	"dec_charset_csviscii";
			case	DEC_CHARSET_TCVN:				return	"dec_charset_tcvn";
			case	DEC_CHARSET_TCVN_5712:			return	"dec_charset_tcvn_5712";
			case	DEC_CHARSET_TCVN5712_1:			return	"dec_charset_tcvn5712_1";
			case	DEC_CHARSET_TCVN5712_1_1993:	return	"dec_charset_tcvn5712_1_1993";
			case	DEC_CHARSET_JIS_X0201:			return	"dec_charset_jis_x0201";
			case	DEC_CHARSET_JISX0201_1976:		return	"dec_charset_jisx0201_1976";
			case	DEC_CHARSET_JISX0201_1976_0:	return	"dec_charset_jisx0201_1976_0";
			case	DEC_CHARSET_X0201:				return	"dec_charset_x0201";
			case	DEC_CHARSET_csHalfWidthKatakana:return	"dec_charset_cshalfwidthkatakana";
			case	DEC_CHARSET_JIS_X0208:			return	"dec_charset_jis_x0208";
			case	DEC_CHARSET_JIS_X0208_1983_0:	return	"dec_charset_jis_x0208_1983_0";
			case	DEC_CHARSET_JIS_X0208_1983_1:	return	"dec_charset_jis_x0208_1983_1";
			case	DEC_CHARSET_JIS_X0208_1983:		return	"dec_charset_jis_x0208_1983";
			case	DEC_CHARSET_JIS_X0208_1990:		return	"dec_charset_jis_x0208_1990";
			case	DEC_CHARSET_JIS0208:			return	"dec_charset_jis0208";
			case	DEC_CHARSET_X0208:				return	"dec_charset_x0208";
			case	DEC_CHARSET_ISO_IR_87:			return	"dec_charset_iso_ir_87";
			case	DEC_CHARSET_csISO87JISX0208:	return	"dec_charset_csiso87jisx0208";
			case	DEC_CHARSET_JIS_X0212:			return	"dec_charset_jis_x0212";
			case	DEC_CHARSET_JIS_X0212_1990_0:	return	"dec_charset_jis_x0212_1990_0";
			case	DEC_CHARSET_JIS_X0212_1990:		return	"dec_charset_jis_x0212_1990";
			case	DEC_CHARSET_X0212:				return	"dec_charset_x0212";
			case	DEC_CHARSET_ISO_IR_159:			return	"dec_charset_iso_ir_159";
			case	DEC_CHARSET_csISO159JISX02121990:return	"dec_charset_csiso159jisx02121990";
			case	DEC_CHARSET_GB_2312_80:			return	"dec_charset_gb_2312_80";
			case	DEC_CHARSET_ISO_IR_58:			return	"dec_charset_iso_ir_58";
			case	DEC_CHARSET_csISO58GB231280:	return	"dec_charset_csiso58gb231280";
			case	DEC_CHARSET_CHINESE:			return	"dec_charset_chinese";
			case	DEC_CHARSET_KSC_5601:			return	"dec_charset_ksc_5601";
			case	DEC_CHARSET_KS_C_5601_1987:		return	"dec_charset_ks_c_5601_1987";
			case	DEC_CHARSET_KSC5601_1987_0:		return	"dec_charset_ksc5601_1987_0";
			case	DEC_CHARSET_KS_C_5601_1989:		return	"dec_charset_ks_c_5601_1989";
			case	DEC_CHARSET_ISO_IR_149:			return	"dec_charset_iso_ir_149";
			case	DEC_CHARSET_csKSC56011987:		return	"dec_charset_csksc56011987";
			case	DEC_CHARSET_KOREAN:				return	"dec_charset_korean";
			case	DEC_CHARSET_CP949:				return	"dec_charset_cp949";
			case	DEC_CHARSET_EUC_JP:				return	"dec_charset_euc_jp";
			case	DEC_CHARSET_EUCJP:				return	"dec_charset_eucjp";
			case	DEC_CHARSET_Extended_UNIX_Code_Packed_Format_for_Japanese:	return	"dec_charset_extended_unix_code_packed_format_for_japanese";
			case	DEC_CHARSET_csEUCPkdFmtJapanese:return	"dec_charset_cseucpkdfmtjapanese";
			case	DEC_CHARSET_SJIS:				return	"dec_charset_sjis";
			case	DEC_CHARSET_SHIFT_JIS:			return	"dec_charset_shift_jis";
			case	DEC_CHARSET_SHIFT_JIS_ALIAS:	return	"dec_charset_shift_jis_alias";
			case	DEC_CHARSET_MS_KANJI:			return	"dec_charset_ms_kanji";
			case	DEC_CHARSET_csShiftJIS:			return	"dec_charset_csshiftjis";
			case	DEC_CHARSET_CP932:				return	"dec_charset_cp932";
			case	DEC_CHARSET_ISO_2022_JP:		return	"dec_charset_iso_2022_jp";
			case	DEC_CHARSET_csISO2022JP:		return	"dec_charset_csiso2022jp";
			case	DEC_CHARSET_ISO_2022_JP_1:		return	"dec_charset_iso_2022_jp_1";
			case	DEC_CHARSET_ISO_2022_JP_2:		return	"dec_charset_iso_2022_jp_2";
			case	DEC_CHARSET_EUC_CN:				return	"dec_charset_euc_cn";
			case	DEC_CHARSET_EUCCN:				return	"dec_charset_euccn";
			case	DEC_CHARSET_GB2312:				return	"dec_charset_gb2312";
			case	DEC_CHARSET_csGB2312:			return	"dec_charset_csgb2312";
			case	DEC_CHARSET_GBK:				return	"dec_charset_gbk";
			case	DEC_CHARSET_CP936:				return	"dec_charset_cp936";
			case	DEC_CHARSET_ISO_2022_CN:		return	"dec_charset_iso_2022_cn";
			case	DEC_CHARSET_csISO2022CN:		return	"dec_charset_csiso2022cn";
			case	DEC_CHARSET_ISO_2022_CN_EXT:	return	"dec_charset_iso_2022_cn_ext";
			case	DEC_CHARSET_HZ:					return	"dec_charset_hz";
			case	DEC_CHARSET_HZ_GB_2312:			return	"dec_charset_hz_gb_2312";
			case	DEC_CHARSET_EUC_TW:				return	"dec_charset_euc_tw";
			case	DEC_CHARSET_EUCTW:				return	"dec_charset_euctw";
			case	DEC_CHARSET_csEUCTW:			return	"dec_charset_cseuctw";
			case	DEC_CHARSET_BIG5:				return	"dec_charset_big5";
			case	DEC_CHARSET_BIG_5:				return	"dec_charset_big_5";
			case	DEC_CHARSET_BIG_FIVE:			return	"dec_charset_big_five";
			case	DEC_CHARSET_BIGFIVE:			return	"dec_charset_bigfive";
			case	DEC_CHARSET_csBig5:				return	"dec_charset_csbig5";
			case	DEC_CHARSET_CP950:				return	"dec_charset_cp950";
			case	DEC_CHARSET_EUC_KR:				return	"dec_charset_euc_kr";
			case	DEC_CHARSET_EUCKR:				return	"dec_charset_euckr";
			case	DEC_CHARSET_csEUCKR:			return	"dec_charset_cseuckr";
			case	DEC_CHARSET_JOHAB:				return	"dec_charset_johab";
			case	DEC_CHARSET_CP1361:				return	"dec_charset_cp1361";
			case	DEC_CHARSET_ISO_2022_KR:		return	"dec_charset_iso_2022_kr";
			case	DEC_CHARSET_csISO2022KR:		return	"dec_charset_csiso2022kr";

			default:								return "Unknown Charset";
		}
	}

	const char* ConvertDecNameType(DecNameType type)
	{
		switch (type)
		{
			case DEC_NAMETYPE_UNKNOWN:	return "dec_nametype_unknown";
			case DEC_NAMETYPE_INTERNAL:	return "dec_nametype_internal";
			case DEC_NAMETYPE_EXTERNAL:	return "dec_nametype_external";

			default:					return "Unknown DecNameType";
		}
	}

	//
	// Methods supported when DEC_OPTION_EVENTSINK_VERSION == EVENT_SINK_VERSION_1
	//
	virtual DECRESULT OnObjectBegin(
		IDecObject *pObject,
		WORD *pwResult,
		char *pszNewDataFile,
		bool *pbContinue)
	{

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONOBJECTBEGIN) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{
			dec_printf(	"OnObjectBegin---------[In] (0x%X) IDecObject:\t%s\tpwResult:\t%s\tpszNewDataFile:\t%s\tpbContinue:\t%s%c",
					 this,
					 pObject->ToString(3),
					 ConvertResult(*pwResult),
					 ConvertString(pszNewDataFile),
					 ConvertBoolean(*pbContinue),
					 m_cInOutSeparator
				  );
		}
		
		DECRESULT result = m_pSink->OnObjectBegin(pObject,pwResult,pszNewDataFile,pbContinue);

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONOBJECTBEGIN) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_LEAVING_CALLBACK) )
		{
			dec_printf(	"OnObjectBegin--------[Out] (0x%X) IDecObject:\t%s\tpwResult:\t%s\tpszNewDataFile:\t%s\tpbContinue:\t%s\tReturnValue:\t%s\n",
					 this,
					 pObject->ToString(3),
					 ConvertResult(*pwResult),
					 ConvertString(pszNewDataFile),
					 ConvertBoolean(*pbContinue),
					 ConvertDecResult(result)
				  );
		}
		return result;
	};
	

	virtual DECRESULT OnObjectEnd(
		IDecContainerObject *pObject,
		DECRESULT hr)
	{
		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONOBJECTEND) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{
			dec_printf(	"OnObjectEnd-----------[In] (0x%X) IDecObject:\t%s\thr:\t%s%c",
					 this,
					 pObject->ToString(3),
					 ConvertDecResult(hr),
					 m_cInOutSeparator
				  );
		}

		DECRESULT result = m_pSink->OnObjectEnd(pObject,hr);

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONOBJECTEND) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_LEAVING_CALLBACK) )
		{
			dec_printf(	"OnObjectEnd----------[Out] (0x%X) IDecObject:\t%s\thr:\t%s\tReturnValue:\t%s\n",
					 this,
					 pObject->ToString(3),
					 ConvertDecResult(hr),
					 ConvertDecResult(result)
				  );
		}

		return result;
	};

	virtual DECRESULT OnDiskFileNeeded(
		IDecContainerObject *pObject,
		char *pszdiskfile,
		int iBufferSize)
	{
		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONDISKFILENEEDED) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{
			dec_printf(	"OnDiskFileNeeded------[In] (0x%X) IDecObject:\t%s\tpszdiskfile:\t%s\tiBufferSize:\t%d%c",
					 this,
					 pObject->ToString(3),
					 ConvertString(pszdiskfile),
					 iBufferSize,
					 m_cInOutSeparator
				  );
		}

		DECRESULT result = m_pSink->OnDiskFileNeeded(pObject,pszdiskfile,iBufferSize);

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONDISKFILENEEDED) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_LEAVING_CALLBACK) )
		{
			dec_printf(	"OnDiskFileNeeded-----[Out] (0x%X) IDecObject:\t%s\tpszdiskfile:\t%s\tiBufferSize:\t%d\tReturnValue:\t%s\n",
					 this,
					 pObject->ToString(3),
					 ConvertString(pszdiskfile),
					 iBufferSize,
					 ConvertDecResult(result)
				  );
		}

		return result;
	};

	virtual DECRESULT OnObjectNotIdentified(
		IDecObject *pObject,
		WORD *pwResult,
		char *pszNewDataFile)
	{
		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONOBJECTNOTIDENTIFIED) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{
			dec_printf(	"OnObjectNotIdentified-[In] (0x%X) IDecObject:\t%s\tpwResult:\t%s\tpszNewDataFile:\t%s%c",
					 this,
					 pObject->ToString(3),
					 ConvertResult(*pwResult),
					 ConvertString(pszNewDataFile),
					 m_cInOutSeparator
				  );
		}

		DECRESULT result = m_pSink->OnObjectNotIdentified(pObject,pwResult,pszNewDataFile);

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONOBJECTNOTIDENTIFIED) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_LEAVING_CALLBACK) )
		{
			dec_printf(	"OnObjectNotIdentified[Out] (0x%X) IDecObject:\t%s\tpwResult:\t%s\tpszNewDataFile:\t%s\tReturnValue:\t%s\n",
					 this,
					 pObject->ToString(3),
					 ConvertResult(*pwResult),
					 ConvertString(pszNewDataFile),
					 ConvertDecResult(result)
				  );
		}

		return result;
	};

	virtual DECRESULT OnContainerIdentified(
		IDecContainerObject *pObject,
		bool *pbContinue)
	{
		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONCONTAINERIDENTIFIED) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{
			dec_printf(	"OnContainerIdentified-[In] (0x%X) IDecObject:\t%s\tpbContinue:\t%s%c",
					 this,
					 pObject->ToString(3),
					 ConvertBoolean(*pbContinue),
					 m_cInOutSeparator
				  );
		}

		DECRESULT result = m_pSink->OnContainerIdentified(pObject,pbContinue);

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONCONTAINERIDENTIFIED) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_LEAVING_CALLBACK) )
		{
			dec_printf(	"OnContainerIdentified[Out] (0x%X) IDecObject:\t%s\tpbContinue:\t%s\tReturnValue:\t%s\n",
					 this,
					 pObject->ToString(3),
					 ConvertBoolean(*pbContinue),
					 ConvertDecResult(result)
				  );
		}

		return result;
	};

	virtual DECRESULT OnContainerEnd(
		IDecContainerObject *pObject,
		void **ppInsertObjects)
	{
		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONCONTAINEREND) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{
			dec_printf(	"OnContainerEnd--------[In] (0x%X) IDecObject:\t%s\tppInsertObjects:\t0x%x%c",
					 this,
					 pObject->ToString(3),
					 ppInsertObjects,
					 m_cInOutSeparator
				  );
		}

		DECRESULT result = m_pSink->OnContainerEnd(pObject,ppInsertObjects);

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONCONTAINEREND) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_LEAVING_CALLBACK) )
		{
			dec_printf(	"OnContainerEnd-------[Out] (0x%X) IDecObject:\t%s\tppInsertObjects:\t0x%x\tReturnValue:\t%s\n",
					 this,
					 pObject->ToString(3),
					 ppInsertObjects,
					 ConvertDecResult(result)
				  );
		}
		return result;
	};

	virtual DECRESULT OnChildFound(
		IDecContainerObject *pObject,
		DWORD dwType,
		const char *pszChildName,
		DWORD dwCharset,
		bool *pbContinue)
	{
		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONCHILDFOUND) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{
			dec_printf(	"OnChildFound----------[In] (0x%X) IDecObject:\t%s\tdwType:\t%s\tpszChildName:\t%s\tdwCharset:\t%s\tpbContinue:\t%s%c",
					 this,
					 pObject->ToString(3),
					 ConvertType(dwType),
					 ConvertString(pszChildName),
					 ConvertCharset(dwCharset),
					 ConvertBoolean(*pbContinue),
					 m_cInOutSeparator
				  );
		}

		DECRESULT result = m_pSink->OnChildFound(pObject,dwType,pszChildName,dwCharset,pbContinue);

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONCHILDFOUND) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_LEAVING_CALLBACK) )
		{
			dec_printf(	"OnChildFound---------[Out] (0x%X) IDecObject:\t%s\tdwType:\t%s\tpszChildName:\t%s\tdwCharset:\t%s\tpbContinue:\t%s\tReturnValue:\t%s\n",
					 this,
					 pObject->ToString(3),
					 ConvertType(dwType),
					 ConvertString(pszChildName),
					 ConvertCharset(dwCharset),
					 ConvertBoolean(*pbContinue),
					 ConvertDecResult(result)
				  );
		}

		return result;
	};

	virtual DECRESULT OnChildBad(
		IDecContainerObject *pObject,
		DECRESULT hr,
		const char *pszChildName,
		bool *pbContinue)
	{

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONCHILDBAD) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{

			dec_printf(	"OnChildBad------------[In] (0x%X) IDecObject:\t%s\thr:\t%s\tpszChildName:\t%s\tpbContinue:\t%s%c",
					 this,
					 pObject->ToString(3),
					 ConvertResult(hr),
					 ConvertString(pszChildName),
					 ConvertBoolean(*pbContinue),
					 m_cInOutSeparator
				  );
		}

		DECRESULT result = m_pSink->OnChildBad(pObject,hr,pszChildName,pbContinue);

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONCHILDBAD) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_LEAVING_CALLBACK) )
		{
			dec_printf(	"OnChildBad-----------[Out] (0x%X) IDecObject:\t%s\thr:\t%s\tpszChildName:\t%s\tpbContinue:\t%s\tReturnValue:\t%s\n",
				 this,
				 pObject->ToString(3),
				 ConvertResult(hr),
				 ConvertString(pszChildName),
				 ConvertBoolean(*pbContinue),
				 ConvertDecResult(result)
			  );
		}

		return result;
	};

	virtual DECRESULT OnBusy()
	{
		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONBUSY) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{

			dec_printf(	"OnBusy----------------[In] (0x%X) %c",
					 this,
					 m_cInOutSeparator
				  );
		}

		DECRESULT result = m_pSink->OnBusy();

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONBUSY) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_LEAVING_CALLBACK) )
		{
			dec_printf(	"OnBusy---------------[Out] (0x%X)\tReturnValue:\t%s\n",
					 this,
					 ConvertDecResult(result)
				  );
		}

		return result;
	};

	virtual DECRESULT OnEngineError(
		IDecContainerObject *pObject,
		DECRESULT hr,
		WORD *pwResult,
		char *pszNewDataFile)
	{
		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONENGINEERROR) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{
			dec_printf(	"OnEngineError---------[In] (0x%X) IDecObject:\t%s\thr:\t%s\tpwResult:\t%s\tpszNewDataFile:\t%s%c",
					 this,
					 pObject->ToString(3),
					 ConvertResult(hr),
					 ConvertResult(*pwResult),
					 ConvertString(pszNewDataFile),
					 m_cInOutSeparator
				  );
		}

		DECRESULT result = m_pSink->OnEngineError(pObject,hr,pwResult,pszNewDataFile);

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONENGINEERROR) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_LEAVING_CALLBACK) )
		{
			dec_printf(	"OnEngineError--------[Out] (0x%X) IDecObject:\t%s\thr:\t%s\tpwResult:\t%s\tpszNewDataFile:\t%s\tReturnValue:\t%s\n",
					 this,
					 pObject->ToString(3),
					 ConvertResult(hr),
					 ConvertResult(*pwResult),
					 ConvertString(pszNewDataFile),
					 ConvertDecResult(result)
				  );
		}
		
		return result;
	};

	virtual DECRESULT OnKeyNeeded(
		IDecContainerObject *pObject,
		unsigned char *pbyKey,
		int nKeyBufferLen,
		int *piKeyLen,
		bool *pbContinue,
		DWORD *lpdwParam)
	{
		
		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONKEYNEEDED) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{
			dec_printf(	"OnKeyNeeded-----------[In] (0x%X) %c",
					 this,
					 m_cInOutSeparator
				  );
		}

		DECRESULT result = m_pSink->OnKeyNeeded(pObject,pbyKey,nKeyBufferLen,piKeyLen,pbContinue,lpdwParam);

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONKEYNEEDED) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{
			dec_printf(	"OnKeyNeeded----------[Out] (0x%X)\tReturnValue:\t%s\n",
					 this,
					 ConvertDecResult(result)
				  );
		}
		
		return result;
	};
	
	//
	// Methods supported when DEC_OPTION_EVENTSINK_VERSION == EVENT_SINK_VERSION_2
	//
	virtual DECRESULT OnChildFound2(
		IDecContainerObject *pObject,
		DWORD dwType,
		const char *pszChildName,
		DecNameType ChildNameType,
		DWORD dwCharset,
		bool *pbContinue)
	{
		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONCHILDFOUND) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{
			dec_printf(	"OnChildFound2---------[In] (0x%X) IDecObject:\t%s\tdwType:\t%s\tpszChildName:\t%s\tChildNameType:\t%s\tdwCharset:\t%s\tpbContinue:\t%s%c",
					 this,
					 pObject->ToString(3),
					 ConvertType(dwType),
					 ConvertString(pszChildName),
					 ConvertDecNameType(ChildNameType),
					 ConvertCharset(dwCharset),
					 ConvertBoolean(*pbContinue),
					 m_cInOutSeparator
				  );
		}

		DECRESULT result = m_pSink->OnChildFound2(pObject,dwType,pszChildName,ChildNameType,dwCharset,pbContinue);

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONCHILDFOUND) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_LEAVING_CALLBACK) )
		{
			dec_printf(	"OnChildFound2--------[Out] (0x%X) IDecObject:\t%s\tdwType:\t%s\tpszChildName:\t%s\tChildNameType:\t%s\tdwCharset:\t%s\tpbContinue:\t%s\tReturnValue:\t%s\n",
					 this,
					 pObject->ToString(3),
					 ConvertType(dwType),
					 ConvertString(pszChildName),
					 ConvertDecNameType(ChildNameType),
					 ConvertCharset(dwCharset),
					 ConvertBoolean(*pbContinue),
					 ConvertDecResult(result)
				  );
		}

		return result;
	};

	virtual DECRESULT OnChildBad2(
		IDecContainerObject *pObject,
		DECRESULT hr,
		const char *pszChildName,
		DecNameType ChildNameType,
		bool *pbContinue)
	{
		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONCHILDBAD) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_ENTERING_CALLBACK) )
		{
			dec_printf(	"OnChildBad2-----------[In] (0x%X) IDecObject:\t%s\thr:\t%s\tpszChildName:\t%s\tChildNameType:\t%s\tpbContinue:\t%s%c",
					 this,
					 pObject->ToString(3),
					 ConvertResult(hr),
					 ConvertString(pszChildName),
					 ConvertDecNameType(ChildNameType),
					 ConvertBoolean(*pbContinue),
					 m_cInOutSeparator
				  );
		}


		DECRESULT result = m_pSink->OnChildBad2(pObject,hr,pszChildName,ChildNameType,pbContinue);

		if ( !(m_dwTraceOption & TRACE_OPTION_DISABLE_ONCHILDBAD) && !(m_dwTraceOption & TRACE_OPTION_DISABLE_LEAVING_CALLBACK) )
		{
			dec_printf(	"OnChildBad2----------[Out] (0x%X) IDecObject:\t%s\thr:\t%s\tpszChildName:\t%s\tChildNameType:\t%s\tpbContinue:\t%s\tReturnValue:\t%s\n",
					 this,
					 pObject->ToString(3),
					 ConvertResult(hr),
					 ConvertString(pszChildName),
					 ConvertDecNameType(ChildNameType),
					 ConvertBoolean(*pbContinue),
					 ConvertDecResult(result)
				  );
		}

		return result;
	};

private:
	IDecEventSink*	m_pSink;
	CDecomposer*	m_pDecomposer;
	char			m_cInOutSeparator;
	DWORD			m_dwTraceOption;
};

CDecomposer::CDecomposer()
{
	m_Engines = NULL;
	m_EnginesCount = 0;
	m_EnginesCapacity = 0;


	// Initialize the m_Options array.
	for (int i = 0; i < DEC_OPTION_COUNT; i++)
	{
		m_Options[i].dwID = DEC_OPTION_UNUSED;
		m_Options[i].dwValue = 0;
	}

	// Create base CDecomposer class object with no options set.
	SetOption(DEC_OPTION_TRUNCATE_TEMP_FILES, 0);
	SetOption(DEC_OPTION_ENFORCE_CRC, 0);
	SetOption(DEC_OPTION_EXTRACT_RTFHTML, 0);
	SetOption(DEC_OPTION_EXTRACT_OLE10NATIVE_ONLY, 0);
	SetOption(DEC_OPTION_NONMIMETHRESHOLD, 200 * 1024);  // Set default 200K limit
	SetOption(DEC_OPTION_NONUUETHRESHOLD_BYTES, 16 * 1024);   // Set default to 16K bytes
	SetOption(DEC_OPTION_ENABLE_MIME_ENGINE, 0);
	SetOption(DEC_OPTION_ENABLE_UUE_ENGINE, 0);
	SetOption(DEC_OPTION_ENABLE_BINHEX_ENGINE, 0);
	SetOption(DEC_OPTION_LOG_WARNINGS, 0);
	SetOption(DEC_OPTION_MAX_TEXT_SCAN_BYTES, 0);
	SetOption(DEC_OPTION_EVENTSINK_VERSION, EVENT_SINK_VERSION_1);	// Only support version 1 unless set by a client
	SetOption(DEC_OPTION_MIME_FUZZY_MAIN_HEADER, 1);
	SetOption(DEC_OPTION_ENHANCED_TEXT_ID, 0);
	SetOption(DEC_OPTION_TRANSLATE_NULL, ASC_CHR_PERIOD);
	SetOption(DEC_OPTION_NONHQXTHRESHOLD, 4 * 1024);     // Set default to 4K bytes
	SetOption(DEC_OPTION_MIN_MIME_SCAN_BYTES, 0);		// Set default to 0 bytes
	SetOption(DEC_OPTION_USE_OLE10NATIVE_CHILD_NAME, 0);
//	SetOption(DEC_OPTION_TYPE_IDENTIFICATION, 1);		// Enable type-identification

	// Set the first unique value.  This value is used to generate
	// temporary filenames.
	m_dwUnique = 0x10000000;
}


CDecomposer::~CDecomposer()
{
	RemoveEngines();
	free(m_Engines);

#ifdef DEC_REUSE_TEMP_FILES
	/* When we are caching temp files, force a purge of the
	cache on the current thread. This avoids the case of a thread
	starting, doing some decomposer work, ending, and leaving behind
	a bunch of large temp files until the process reclaims them. */
	dec_purge_tfn_cache();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CDecomposer::AddEngine()

DECRESULT CDecomposer::AddEngine(IDecEngine *pEngine)
{
	IDecEngineEx *eng;
	int new_count;
	int new_capacity;
	IDecEngineEx **new_array;

	// Validate parameters.
	if (pEngine == NULL)
	{
		dec_assert(0);
		return DECERR_INVALID_PARAMS;
	}
	
	// Set the MaxExtractSize to 2GB, which is the most Decomposer can handle
	pEngine->SetMaxExtractSize(MAX_FILE_SIZE);

	eng = (IDecEngineEx*)pEngine;

	new_count = m_EnginesCount + 1;
	if (new_count > m_EnginesCapacity)
	{
		if (m_EnginesCapacity == 0)
		{
			new_capacity = 10;
		}
		else
		{
			new_capacity = m_EnginesCapacity * 2;
		}

		new_array = (IDecEngineEx**)realloc(m_Engines, new_capacity * sizeof(IDecEngineEx*));
		if (new_array == NULL)
		{
			return DECERR_OUT_OF_MEMORY;
		}

		m_Engines = new_array;
		m_EnginesCapacity = new_capacity;
	}

	m_Engines[m_EnginesCount] = eng;
	m_EnginesCount = new_count;

	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDecomposer::RemoveEngines()

DECRESULT CDecomposer::RemoveEngines(void)
{
	m_EnginesCount = 0;
	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDecomposer::SetTempDir()

DECRESULT CDecomposer::SetTempDir(const char *szDir)
{
	DECRESULT hrFinal = DEC_OK;
	int i;
	DECRESULT hr;

#ifdef DEC_REUSE_TEMP_FILES
	//
	// The temp file caching system cannot be used with any decomposer
	// namespaces (mem:// or rapi:// for instance).  It is only to be
	// used with on-disk files.  
	//
	if((szDir != NULL) && HAS_SPECIAL_NAMESPACE(szDir))
	{
		dec_assert(0);
		return DECERR_TEMPDIR_INVALID;
	}

	/* This is not strictly necessary, but it is a good idea because of
	the wide variety of decomposer usage scenarios. The temp file cache
	selects the first directory in which a temp file is requested as its
	directory for caching, and from that point on it only caches requests
	in that directory. In a vast majority of cases, this will work fine,
	because as of this writing, all decomposer engines that use temp file
	caching ask for files in the same directory. It is possible, however
	unlikely, that some engine will eventually be set up to ask us for
	a temp file in a different directory before a request is made on the
	"main" directory. This would throw off the temp file cache, eliminating
	most of its performance gains. To hedge our bets, ask for a temp file here.
	This is usually called as the decomposer is set up, before any
	decomposing is actually done, so we can try to guarantee that the first
	request is for a temp file in the correct directory. The down side of
	doing it this way would be if there is a client out there that calls
	this function more than once before doing any decomposing, and it
	passes different directories each time. We'll hope that doesn't
	happen. */

	char name_out[MAX_PATH + 1];
	int res;

	/* It is legal to pass NULL as szDir in order to cause the decomposer
	to reset its temp directory to the default (which is platform specific).
	If they do that, we aren't getting any useful information here, so we just
	skip all of this. We will pick up the actual temp directory the first
	time a lower level function calls the cache to get a temp file. */
	if(szDir != NULL)
	{
		res = dec_get_tempname(szDir, "TMP", name_out);
		dec_assert(res == 0);
		res = dec_rm(name_out);
		dec_assert(res == 0);
	}

#endif

	// For each pointer in the engine pointer array...
	for (i = 0; i < m_EnginesCount; ++i)
	{
		// Set the engine's temporary directory
		hr = m_Engines[i]->SetTempDir(szDir);

		// If there's an error, save it but keep going.
		if (FAILED(hr))
			hrFinal = hr;
	}

	// Return success, or last error if any.
	return hrFinal;
}


/////////////////////////////////////////////////////////////////////////////
// CDecomposer::SetMaxExtractSize()

DECRESULT CDecomposer::SetMaxExtractSize(DWORD dwMaxSize)
{
	DECRESULT hrFinal = DEC_OK;
	DECRESULT hr;
	int i;

	// For each pointer in the engine pointer array...
	for (i = 0; i < m_EnginesCount; ++i)
	{
		// Set the engine's trust state
		hr = m_Engines[i]->SetMaxExtractSize(dwMaxSize);

		// If there's an error, save it but keep going.
		if (FAILED(hr))
			hrFinal = hr;
	}

	// Return success, or last error if any.
	return hrFinal;
}


/////////////////////////////////////////////////////////////////////////////
// CDecomposer::AbortProcess()

DECRESULT CDecomposer::AbortProcess(bool bAbort)
{
	DECRESULT hrFinal = DEC_OK;
	DECRESULT hr;
	int i;

	// For each pointer in the engine pointer array...
	for (i = 0; i < m_EnginesCount; ++i)
	{
		// Set the engine's trust state
		hr = m_Engines[i]->AbortProcess(bAbort);

		// If there's an error, save it but keep going.
		if (FAILED(hr))
			hrFinal = hr;
	}

	// Return success, or last error if any.
	return hrFinal;
}


/////////////////////////////////////////////////////////////////////////////
// CDecomposer::SetOption()

DECRESULT CDecomposer::SetOption(DWORD dwOptionID, DWORD dwValue)
{
	int		 index;
	int		 iSlot = -1;

	for (index = 0; index < DEC_OPTION_COUNT; index++)
	{
		if (m_Options[index].dwID == DEC_OPTION_UNUSED &&
			iSlot == -1)
		{
			// This slot is not in use.  Save its index in case we need it.
			iSlot = index;
		}

		if (m_Options[index].dwID == dwOptionID)
		{
			// Found a matching ID already in the array.
			// Overwrite the information in this slot with the new value.
			iSlot = index;
			break;
		}
	}

	if (iSlot != -1)
	{
		m_Options[iSlot].dwID = dwOptionID;
		m_Options[iSlot].dwValue = dwValue;
		return DEC_OK;
	}

	return DECERR_GENERIC;
}


/////////////////////////////////////////////////////////////////////////////
// CDecomposer::GetOption()

DECRESULT CDecomposer::GetOption(DWORD dwOptionID, DWORD *pdwValue)
{
	int		 index;
	int		 iSlot = -1;

	if (dwOptionID == DEC_OPTION_UNUSED || pdwValue == NULL)
		return DECERR_INVALID_PARAMS;

	// Scan the list for the specified option ID.
	for (index = 0; index < DEC_OPTION_COUNT; index++)
	{
		if (m_Options[index].dwID == dwOptionID)
		{
			// Found a matching ID in the array.
			// Return its value information.
			*pdwValue = m_Options[index].dwValue;
			return DEC_OK;
		}
	}

	return DECERR_GENERIC;
}


/////////////////////////////////////////////////////////////////////////////
// CDecomposer::Process(). This is the version for clients to use. We must
// be able to differentiate between top level calls and recursive calls in
// order to handle temp file caching correctly.
//
DECRESULT CDecomposer::Process(
	IDecObject *pObject,
	IDecEventSink *pSink,
	IDecIOCB *pIOCallback,
	WORD *pwResult,
	char *pszNewDataFile)
{
	DECRESULT	hr;
	int			i;

	// Set the IO callback pointer for this module.
	DecSetIOCallback(pIOCallback);

	// For each pointer in the engine pointer array...
	for (i = 0; i < m_EnginesCount; ++i)
	{
		// Set the engine's I/O callback.
		hr = m_Engines[i]->SetIOCallback(pIOCallback);
		if (FAILED(hr))
			return hr;
	}

	
	// See if the caller wants us to trace the event sink callback.

	CEventSinkTracer	traceSink;
	DWORD				dwOptionValue;

	if ( GetOption(DEC_OPTION_EVENTSINK_TRACE,&dwOptionValue) == DEC_OK )
	{
		traceSink.SetTraceOption(dwOptionValue);
		traceSink.SetEventSink(pSink,this);
		traceSink.DumpOptions();
		pSink = &traceSink;
	}

	hr = DecProcess(pObject, pSink, pIOCallback, pwResult, pszNewDataFile);

#ifdef DEC_REUSE_TEMP_FILES
	// Release any temp file that we are giving back to the user
	// from the cache.
	if ((pszNewDataFile != NULL) && (*pszNewDataFile != 0)) 
	{
		dec_release_name(pszNewDataFile);
	}
#endif

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDecomposer::DecProcess(). This is used internally to decomposer for
// recursive calls only. External clients must call Process().

DECRESULT CDecomposer::DecProcess(
	IDecObject* pObject,
	IDecEventSink* pSink,
	IDecIOCB *pIOCallback,
	WORD *pwResult,
	char *pszNewDataFile)
{
	DECRESULT	hr;
	WORD		wTempResult;
	bool		bIdentified;
	bool		bContinue;
	char		*pszDataName;
	char		*pszTempDataFile;
	CDecResultTracker *ptrack;
	int			i;
   
	// Validate parameters.
	if (pObject == NULL || pSink == NULL || pwResult == NULL || pszNewDataFile == NULL)
		return DECERR_INVALID_PARAMS;

	pszTempDataFile = (char *)malloc(MAX_PATH);
	if (!pszTempDataFile)
		return DECERR_OUT_OF_MEMORY;

	*pszTempDataFile = '\0';

	// Init the processing results from the engines and event sink.
	wTempResult = DECRESULT_UNCHANGED;

	// Downcast to the internal (Extended) object interface
	// using IDecObjectEx's IDecObject (vs IDecContainerObject's).
#if defined(UNIX)
	IDecContainerObjectEx *pObjectEx = 
		(IDecContainerObjectEx *)(
			(IDecObjectEx *)(pObject));
#else
	IDecContainerObjectEx *pObjectEx = 
		static_cast<IDecContainerObjectEx *>(
			static_cast<IDecObjectEx *>(pObject));
#endif

	// Use a tracker object to correctly apply multiple changes to pObjectEx,
	// putting the final results in pwResult and pszNewDataFile.
	ptrack = new CDecResultTracker(pObjectEx, pwResult, pszNewDataFile);
	if (!ptrack)
	{
		free(pszTempDataFile);
		return DECERR_OUT_OF_MEMORY;
	}

	// Reset the rename item name for this new object.
	pObject->SetRenameItemName (NULL, true);

	// Reset the MIME Content-Type for this new object.
	pObject->SetMIMEItemContentType (NULL, true);

	// Allocate a buffer for the item's data file name.
	pszDataName = (char *)malloc(MAX_PATH);
	if (!pszDataName)
	{
		delete ptrack;
		free(pszTempDataFile);
		return DECERR_OUT_OF_MEMORY;
	}

	// Get the item's data file name.
	pObject->GetDataFile(pszDataName, NULL);

	// Make an event sink callback.
	bContinue = true;
	hr = pSink->OnObjectBegin(pObject, &wTempResult, pszTempDataFile, &bContinue);
	if (FAILED(hr))
	{
		delete ptrack;
		free(pszTempDataFile);
		free(pszDataName);
		return hr;
	}

	// Open the data file - we don't need to save the returned file pointer
	// because it's stored in the object. We also don't care about retrieving
	// the data buffer, buffer size, or file size at this point.
	//
	// NOTE: This MUST be done after the call to OnObjectBegin since the client
	// may modify the file.
	if (bContinue)
	{
		if (NULL == pObjectEx->OpenDataFile(pszDataName, "rb", NULL, NULL, NULL))
		{
			delete ptrack;
			free(pszTempDataFile);
			free(pszDataName);
			return DECERR_CONTAINER_OPEN;
		}
	}

	// Clear out the secondary name here.  If we don't, then subsequent calls
	// to GetSecondaryName will pick up the secondary name (if any) set on this
	// object's parent.
	pObjectEx->SetSecondaryName("");

	// Apply any changes requested by event sink.
	hr = ptrack->Apply(wTempResult, pszTempDataFile);
	if (FAILED(hr) || !bContinue || ptrack->Deleted())
		goto process_done;

	// For each pointer in the engine pointer array...
	bIdentified = false;
	for (i = 0; i < m_EnginesCount; ++i)
	{
		// Make an event sink callback.
		hr = pSink->OnBusy();
		if (FAILED(hr))
			goto process_done;

		// Process pObjectEx with the engine.
		hr = m_Engines[i]->Process(this, pObjectEx, pSink, &wTempResult, 
		 pszTempDataFile, &bIdentified, &bContinue);

		// If the engine failed, make an event sink callback.
		if (FAILED(hr))
			hr = pSink->OnEngineError(pObjectEx, hr, &wTempResult, pszTempDataFile);

		// If the callback didn't suppress the error, obey it.
		if (FAILED(hr))
		{
			if (*pszTempDataFile != '\0')
				dec_remove(pszTempDataFile);

			goto process_done;
		}

		// Apply any changes requested by the engine or by OnEngineError().
		hr = ptrack->Apply(wTempResult, pszTempDataFile);
		if (FAILED(hr) || ptrack->Deleted())
			goto process_done;

		// If the engine says "don't use the remaining engines", then don't.
		if (!bContinue)
			break;
	}

	// If no engine was able to identify pObjectEx as a container object...
	if (!bIdentified)
	{

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
		pObjectEx->FinalizeAttributes();
#endif //*** End AS400 Attribute API Removal *** 

		// Make an event sink callback.
		hr = pSink->OnObjectNotIdentified(pObject, &wTempResult, pszTempDataFile);
		if (FAILED(hr))
			goto process_done;

		// Apply any changes requested by the event sink.
		hr = ptrack->Apply(wTempResult, pszTempDataFile);
		if (FAILED(hr) || ptrack->Deleted())
			goto process_done;
	}

process_done:
	// Free the data file name buffer.
	if (pszDataName)
		free(pszDataName);
	if (pszTempDataFile)
		free(pszTempDataFile);
	if (ptrack)
		delete ptrack;

	// Tell the client that we are finished processing this object.
	// Note that we are expecting the client's OnObjectEnd callback to
	// return the same hr as we pass in unless the client wants to return
	// something else.
	hr = pSink->OnObjectEnd(pObjectEx, hr);

	// Close the data file
	if(EOF == pObjectEx->CloseDataFile())
	{
		hr = DECERR_CONTAINER_CLOSE;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDecomposer::DecProcess(). Used internally only.

DECRESULT CDecomposer::DecProcess(
	IDecContainerObjectEx *pObject,
	IDecEventSink *pSink,
	WORD *pwResult,
	char *pszNewDataFile)
{
	// This is the Process methods used by engines.
	// Upcast from the internal (Extended) object interface
	// using IDecObjectEx's IDecObject (vs IDecContainerObject's).
#if defined(UNIX)
	return DecProcess((IDecObjectEx *)(pObject), pSink, NULL, pwResult, pszNewDataFile);
#else
	return DecProcess(static_cast<IDecObjectEx *>(pObject), pSink, NULL, pwResult, pszNewDataFile);
#endif
}


DWORD CDecomposer::GetUniqueValue()
{
	DWORD	 dwUnique;

	m_UniqueCriticalSection.In();
	if (m_dwUnique > 0x1FFFFFFF)
		m_dwUnique = 0x10000000;
	dwUnique = m_dwUnique++;
	m_UniqueCriticalSection.Out();

	return (dwUnique);
}


/////////////////////////////////////////////////////////////////////////////
// DecNewDecomposer()
DECLINKAGE DECRESULT DecNewDecomposer(IDecomposer **ppDecomposer)
{
#ifndef OS400
	try
	{
#endif
		*ppDecomposer = new CDecomposer;
		return (*ppDecomposer != NULL) ? DEC_OK : DECERR_OUT_OF_MEMORY;
#ifndef OS400
	}
	catch (...)
	{
		*ppDecomposer = NULL;
		return DECERR_OUT_OF_MEMORY;
	}
#endif
}


/////////////////////////////////////////////////////////////////////////////
// DecDeleteDecomposer()

DECLINKAGE DECRESULT DecDeleteDecomposer(IDecomposer *pDecomposer)
{
#if defined(UNIX)
	delete (CDecomposer *)(pDecomposer);
#else
	delete static_cast<CDecomposer *>(pDecomposer);
#endif
	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// DecNewFileObject()

DECLINKAGE DECRESULT DecNewFileObject(const char *szName, DWORD dwCharset, const char *szDataFile, IDecObject **ppObject)
{
	// Validate parameters.
	if (szName == NULL || szDataFile == NULL || ppObject == NULL)
		return DECERR_INVALID_PARAMS;

#ifndef OS400
	try
	{
#endif
		// Create a standard Decomposer data object to represent the data file.
		// We can always create a replacement data file, even if the Decomposer
		// user can't do anything with it.
		CDecObj *pObject = new CDecObj();
		if (pObject == NULL)
			return DECERR_OUT_OF_MEMORY;
		pObject->SetName(szName);
		pObject->SetNameCharacterSet(dwCharset);
		pObject->SetDataFile(szDataFile);
		pObject->SetCanDelete(true);
		pObject->SetCanModify(true);
		pObject->SetCanReplace(true);
		pObject->SetCanRename(true);

		// Upcast from the internal (Extended) object interface
		// using IDecObjectEx's IDecObject (vs IDecContainerObject's).
#if defined(UNIX)
		*ppObject = (IDecObjectEx *)(pObject);
#else
		*ppObject = static_cast<IDecObjectEx *>(pObject);
#endif

		return (*ppObject != NULL) ? DEC_OK : DECERR_OUT_OF_MEMORY;
#ifndef OS400
	}
	catch (...)
	{
		*ppObject = NULL;
		return DECERR_OUT_OF_MEMORY;
	}
#endif
}


/////////////////////////////////////////////////////////////////////////////
// DecDeleteFileObject()

DECLINKAGE DECRESULT DecDeleteFileObject(IDecObject *pObject)
{
	// Downcast from the internal (Extended) object interface
	// using IDecObjectEx's IDecObject (vs IDecContainerObject's).
#if defined(UNIX)
	CDecObj *pCDecObj = (CDecObj *)(IDecObjectEx *)(pObject);
#else
	CDecObj *pCDecObj = 
		static_cast<CDecObj *>(
			static_cast<IDecObjectEx *>(pObject));
#endif

	delete pCDecObj;
	return DEC_OK;
}


DECLINKAGE void DecGetVersion(const char **ppszVersion)
{
	*ppszVersion = &gszDecVersion[0];
}


/* Temp file reuse support. On the AS/400 platform, creating and deleting
temporary files is expensive. Instead, we keep a pool of temp files around
that we can reuse. It turns out that this is true on other platforms as well,
so after this mechanism was created, it was turned on for other platforms.
The support for this mechanism follows. */
#ifdef DEC_REUSE_TEMP_FILES

#define MAX_TFC_NAME 32
#define TFC_PURGE_INTERVAL 20
#ifndef DEC_TFC_TEST_PURGE
#define TFC_ABSOLUTE_PURGE_INTERVAL 1200
#else
#define TFC_ABSOLUTE_PURGE_INTERVAL 5
#endif
#define MAX_TFC_STACK 128
#ifndef DEC_TFC_TEST_RENAME
#define MAX_CACHED_FILES 128
#else
#define MAX_CACHED_FILES 4
#endif

typedef enum {
   TFC_FREE,
   TFC_INUSE
} TFC_NODE_STATE;

typedef struct tagTFC_NODE {
	struct tagTFC_NODE *children[16];
	TFC_NODE_STATE state;
	short int cur_child;
	short int num_nybbles;
	unsigned char nybbles[MAX_TFC_NAME * 2];
	char fname[MAX_TFC_NAME + 1];
} TFC_NODE;

/* Stack used for walking the tree in iteration functions. */
class CTFCNodeStack
{
public:
	CTFCNodeStack() { m_ptr = -1; }
	~CTFCNodeStack() {}

	void push(TFC_NODE *node)
	{
		dec_assert(node != NULL);
		if(m_ptr == (MAX_TFC_STACK - 1)) {
			dec_assert(0);
			return;
		}

		++m_ptr;
		m_stack[m_ptr] = node;
	}

	TFC_NODE *pop(void)
	{
		TFC_NODE *rc;

		dec_assert(m_ptr >= -1);

		if(m_ptr == -1) {
			return NULL;
		}

		rc = m_stack[m_ptr];
		--m_ptr;
		return rc;		 
	}

	void clear(void) { m_ptr = -1; }

private:
	TFC_NODE *m_stack[MAX_TFC_STACK];
	long m_ptr;
};

/* An instance of this class is allocated for each thread that calls
into the temp file caching mechanism. Since instances are thread-specific,
they do not have to be threadsafe internally. */
class CDecTempFileCache
{
public:
	CDecTempFileCache();
	~CDecTempFileCache();
	int rm(const char *fname);
	int get_tempname(const char *dir, const char *ext, char *name_out);
	void force_purge(void);
	void release_name(const char *name);

private:
	void DoPurge(void);
	void PurgeNode(TFC_NODE *node);
	TFC_NODE *FindNode(const char *name, TFC_NODE **parent = NULL, 
	 unsigned char *parent_nybble = NULL);
	void GetFirst(TFC_NODE *node);
	TFC_NODE *GetNext(TFC_NODE *node);
	TFC_NODE *AddName(const char *name);
	void RemoveName(const char *name);
	int CreateTempFile(const char *dir, const char *ext, char *name_out, size_t name_bytes);
	void DestroyNode(TFC_NODE *node);

	char m_tmpdir[MAX_PATH + 1];
	TFC_NODE *m_root;
	time_t m_lastpurge;
	time_t m_lastact;
	CTFCNodeStack m_stack;
	long m_count;
	unsigned long m_tmpFileNum;

#ifdef DEC_TFC_COUNTERS
	unsigned long m_nodes;
	unsigned long m_peaknodes;
	unsigned long m_depth;
	unsigned long m_peakdepth;
	unsigned long m_gethits;
	unsigned long m_rmhits;
	unsigned long m_getmisses;
	unsigned long m_rmmisses;
	unsigned long m_searches;
	unsigned long m_compares;
	unsigned long m_renames;
	unsigned long m_purges;
	unsigned long m_released;

	void CalcDepth() 
	{ 
		m_depth = NodeDepth(m_root);
		if(m_depth > m_peakdepth) {
			m_peakdepth = m_depth;
		}
	}

	int NodeDepth(TFC_NODE *node);
#endif
};

CDecTempFileCache::CDecTempFileCache()
{
	m_tmpdir[0] = 0;
	m_root = NULL;
	m_count = 0;
	time(&m_lastpurge);
	m_lastact = m_lastpurge;
	m_tmpFileNum = (unsigned long)m_lastact * 300;

#ifdef DEC_TFC_COUNTERS
	m_nodes = 0;
	m_peaknodes = 0;
	m_depth = 0;
	m_peakdepth = 0;
	m_gethits = 0;
	m_rmhits = 0;
	m_getmisses = 0;
	m_rmmisses = 0;
	m_searches = 0;
	m_compares = 0;
	m_renames = 0;
	m_purges = 0;
	m_released = 0;
#endif
}

#ifdef DEC_TFC_COUNTERS
/* This is used to find the depth of the tree for diagnostic
purposes. */
int CDecTempFileCache::NodeDepth(TFC_NODE *node)
{
	TFC_NODE *child;
	int greatest = 0;
	int depth;
	int i;

	if(node == NULL) {
		return 0;
	}

	for(i = 0; i < 16; ++i) {
		child = node->children[i];
		if(child != NULL) {
			depth = NodeDepth(child);
			if(depth > greatest) {
				greatest = depth;
			}
		}
	}

	return greatest + 1;
}

#endif

/* Recursive function for cleaning up in the destructor */
void CDecTempFileCache::DestroyNode(TFC_NODE *node)
{
	int i;

	dec_assert(node != NULL);

	for(i = 0; i < 16; ++i) {
		if(node->children[i] != NULL) {
			DestroyNode(node->children[i]);
		}
	}

	delete node;
#ifdef DEC_TFC_COUNTERS
	--m_nodes;
#endif
}

/* Called automatically when the process exits. Must delete 
the files in the cache from the file system and clean up the 
tree. */
CDecTempFileCache::~CDecTempFileCache()
{
	TFC_NODE *node;
	char path[MAX_PATH + 1];
	char *p;
	size_t len;

#ifdef DEC_TFC_DUMP
	int header = 0;
	node = m_root;
	GetFirst(node);

	while(node != NULL) {
		if(node->fname[0] != 0) {
			if(!header) {
				printf("Cached temp files at shutdown:\n");
				header = 1;
			}

			printf(" '%s'\n", node->fname);
		}

		node = GetNext(node);
	}

	if(header == 0) {
		printf("Tree is empty at shutdown\n");
	}

	printf("\nCount is %lu\n\n", m_count);
#endif

	if(m_root != NULL) {
		strcpy(path, m_tmpdir);
		len = strlen(path);
		if(len == 0) {
			p = path;
		}
		else {
			p = path + len - 1;
			if(*p != '/') {
				++p;
				*p = '/';
			}

			++p;
		}

		node = m_root;
		GetFirst(node);
		while(node != NULL) {
			if(node->fname[0] != 0) {
				dec_assert(node->state == TFC_FREE);
				strcpy(p, node->fname);
				unlink(path);
			}
			node = GetNext(node);
		}

		/* This recursively deletes the whole tree. We can't use 
		GetFirst/GetNext because the nodes are presented in 
		preorder, and we can't delete a parent until its children 
		have gone away. Preorder, depth-first traversal is used with 
		radix trees to traverse the nodes in key order, but it is
		not what we want when deleting them. */
		DestroyNode(m_root);
	}

#ifdef DEC_TFC_COUNTERS
	m_depth = 0;;

	printf("Temp file cache stats:\n"
		  " Current Nodes:    %lu\n"
		  " Peak Nodes:       %lu\n"
		  " Current Depth:    %lu\n"
		  " Peak Depth:       %lu\n"
		  " Hits on Get:      %lu\n"
		  " Hits on Remove:   %lu\n"
		  " Misses on Get:    %lu\n"
		  " Misses on Remove: %lu\n"
		  " Searches:         %lu\n"
		  " Average Compares: %f\n"
		  " Renames:          %lu\n"
		  " Released names:   %lu\n"
		  " Purges:           %lu\n\n",
		  m_nodes,
		  m_peaknodes,
		  m_depth,
		  m_peakdepth,
		  m_gethits,
		  m_rmhits,
		  m_getmisses,
		  m_rmmisses,
		  m_searches,
		  (float)m_compares / (float)m_searches,
		  m_renames,
		  m_released,
		  m_purges);
#endif
}

/* Used to create the actual files that we cache */
int CDecTempFileCache::CreateTempFile(const char *dir, const char *ext, char *name_out, size_t name_bytes)
{
	int i;
	const char *p;
	const char *between = "";
	int fd;
	char copy[10];

	dec_assert(name_out != NULL);
	dec_assert(dir != NULL);

	if((ext == NULL) || (*ext == 0)) {
		ext = ".TMP";
	}
	else {
		strncpy(copy, ext, 9);
		copy[9] = 0;
		ext = copy;
	}

	p = strrchr(dir, '/');
	if((p == NULL) || (p[1] != 0)) {
		between = "/";
	}

	/* If we get a name collision, retry up to 50 times. */
	for(i = 0; i < 50; ++i) {
		m_tmpFileNum = (m_tmpFileNum + 1 + (i * 200)) & 0x0FFFF;

		sssnprintf(name_out, name_bytes, "%s%sTMA%04X%s", dir, between, m_tmpFileNum, ext);

		fd = open(name_out, O_RDWR | O_CREAT | O_EXCL, 0660);
		if(fd < 0) {
			if(errno != EEXIST) {
				name_out[0] = 0;
				return -1;
			}
		}
		else {
			close(fd);
			return 0;
		}
	}

	/* Here we made it through all 50 retries without finding a
	unique name. */
	return -1;
}

/* Public interface. Erase a file, or just mark it as free
for reuse if it is one of our cached files. Note that we decide
what will be cached and what won't when a temp file is created,
not when it is removed. */
int CDecTempFileCache::rm(const char *fpath)
{
	TFC_NODE *node;
	char *p;
	char *path;
	char copy[MAX_PATH + 1];
	size_t len;

	dec_assert((fpath != NULL) && (*fpath != 0));

#ifdef DEC_TFC_TRACE
	printf("TFC: rm(%s)\n", fpath);
#endif

	/* Periodically purge free files */
	DoPurge();

	/* Ensure that all paths are absolute for comparison
	purposes. The paths we compare also have no trailing
	slashes, which matches what we capture in get_tempname
	below. */
	if(*fpath == '/') {
		strcpy(copy, fpath);
	}
	else {
		if(getcwd(copy, MAX_PATH + 1) == NULL) {
			dec_assert(0);
			strcpy(copy, fpath);
		}
		else {
			len = strlen(copy);
			p = copy + len - 1;
			if(*p != '/') {
				++p;
				*p = '/';
			}
			++p;
			strcpy(p, fpath);
		}
	}

	p = strrchr(copy, '/');
	if(p == NULL) {
		p = copy;
		path = "";
	}
	else {
		*p = 0;
		++p;
		path = copy;
		if(path[0] == 0) {
			path = "/";
		}
	}

	if(strcmp(path, m_tmpdir) != 0) {
		/* Not in our directory, so not one of ours */
#ifdef DEC_TFC_COUNTERS
		++m_rmmisses;
#endif
		return (unlink(fpath) == 0) ? 0 : -1;
	}

	dec_assert(*p != 0);

	if(strlen(p) > MAX_TFC_NAME) {
		/* This file name is too long to be one of ours. We can't 
		continue processing it because our search logic will choke on
		it. */
		return (unlink(fpath) == 0) ? 0 : -1;
	}

	node = FindNode(p);
	if(node == NULL) {
		/* This is not one of ours */
#ifdef DEC_TFC_COUNTERS
		++m_rmmisses;
#endif
		return (unlink(fpath) == 0) ? 0 : -1;
	}

	if(strcmp(p, node->fname) != 0) {
		/* Not one of ours */
#ifdef DEC_TFC_COUNTERS
		++m_rmmisses;
#endif
		return (unlink(fpath) == 0) ? 0 : -1;
	}

	/* One of ours. Mark it as free */
#ifdef DEC_TFC_COUNTERS
		++m_rmhits;
#endif
	node->state = TFC_FREE;

	return 0;
}

/* Public interface. Return the name of a temp file from the
cache if possible, otherwise create a new one and add it to
the cache. We can only do either if they ask for a file in the
directory that we are using for cached temp files. If they want
another directory, we just create a temp file in that directory
and give them the name of it. */
int CDecTempFileCache::get_tempname(const char *dir, const char *ext, char *name_out, size_t name_bytes)
{
	TFC_NODE *node;
	char *p;
	char *between = "";
	int rc;
	char old[MAX_PATH + 1];
	char copy[10];
	char *dir_copy = old;
	size_t len;
	size_t len2;

#ifdef DEC_TFC_TRACE
		printf("TFC: get_tempname(%s, %s, name_out)\n", dir != NULL ? dir : "NULL", 
		 ext != NULL ? ext : "NULL");
#endif

	/* Periodically purge free files */
	DoPurge();

	if((ext == NULL) || (*ext == 0)) {
		ext = ".TMP";
	}
	else {
		strncpy(copy, ext, 9);
		copy[9] = 0;
		ext = copy;
	}

	/* To be more robust, let's treat a directory with a trailing slash as
	equivalent to the same directory without one. Also, convert relative
	paths to absolute paths. This is all done only for comparison
	purposes. The directory as provided by the caller is used for the
	actual files. */
	if((dir != NULL) && (*dir != 0)) {
		/* Make sure all paths are absolute */
		if(*dir == '/') {
			len = strlen(dir);
			memcpy(dir_copy, dir, len + 1);
		}
		else {
			if(getcwd(dir_copy, MAX_PATH + 1) == NULL) {
				dec_assert(0);
				len = strlen(dir);
				memcpy(dir_copy, dir, len + 1);
			}
			else {
				len = strlen(dir_copy);
				if(dir_copy[len - 1] != '/') {
					dir_copy[len++] = '/';
				}
				len2 = strlen(dir);
				memcpy(dir_copy + len, dir, len2 + 1);
				len += len2;
			}
		}

		--len;
		if((len != 0) && (dir_copy[len] == '/')) {
			dir_copy[len] = 0;
		}
	}
	else {
		dir_copy = NULL;
	}
	

	if(m_tmpdir[0] == 0) {
		if(dir_copy == NULL) {
			/* No temp directory provided and we don't yet have one. */
#ifdef DEC_TFC_COUNTERS
			++m_getmisses;
#endif
			return CreateTempFile("/tmp", ext, name_out, name_bytes);
		}

		/* We don't have a temp directory yet. Take this one. */
#ifdef DEC_TFC_TRACE
		printf("TFC: Selecting '%s' as temp directory\n", dir_copy);
#endif
		strcpy(m_tmpdir, dir_copy);
	}
	else if((dir_copy != NULL) && (strcmp(m_tmpdir, dir_copy) != 0)) {
		/* They want a file in some other directory. */
#ifdef DEC_TFC_COUNTERS
		++m_getmisses;
#endif
		return CreateTempFile(dir, ext, name_out, name_bytes);
	}

	/* Look for a matching entry in our cache. A matching entry
	is any file having the requested extension. Because we store the
	files keyed in extension order, this is a fast lookup. */
	dec_assert(*ext != 0);
	node = FindNode(ext);
	if(node != NULL) {
		/* Look for a suitable file in the cache. Here we iterate
		all files that have the requested extension looking for one that
		is not in use. */
		GetFirst(node);
		while(node != NULL) {
			if((node->fname[0] != 0) && (node->state == TFC_FREE)) {
				/* Use this one */
				node->state = TFC_INUSE;
				p = strrchr(dir, '/');
				if((p == NULL) || (p[1] != 0)) {
					between = "/";
				}
				sssnprintf(name_out, name_bytes, "%s%s%s", dir, between, node->fname);
#ifdef DEC_TFC_COUNTERS
				++m_gethits;
#endif

#ifdef DEC_TFC_TRACEHITS
				printf(" %s --> %s\n", ext, name_out);
#endif

				return 0;
			}

			node = GetNext(node);
		}
	}

	/* Unable to find a suitable file in the cache */
	if(m_count < MAX_CACHED_FILES) {
		/* We can handle more. Just create another and add it. */
		rc = CreateTempFile(dir, ext, name_out, name_bytes);
		if(rc == 0) {
			p = strrchr(name_out, '/');
			if(p == NULL) {
				p = name_out;
			}
			else {
				++p;
			}
			node = AddName(p);
			if(node != NULL) {
				++m_count;
				node->state = TFC_INUSE;
#ifdef DEC_TFC_COUNTERS
				++m_gethits;
#endif
			}
		}

		return rc;
	}

	/* We can't add any more cached files. Look for an entry to reuse. */
	node = m_root;
	GetFirst(node);
	while(node != NULL) {
		if((node->state == TFC_FREE) && (node->fname[0] != 0)) {
			/* Reuse this one */
			strcpy(name_out, node->fname);
			
			p = strrchr(dir, '/');
			if((p == NULL) || (p[1] != 0)) {
				between = "/";
			}

			sssnprintf(old, sizeof(old), "%s%s%s", dir, between, node->fname);

			p = strrchr(name_out, '.');
			if(p == NULL) {
				p = name_out + strlen(name_out);
			}

			strcpy(p, ext);

			RemoveName(node->fname);
			node = AddName(name_out);
			if(node == NULL) {
				dec_assert(0);
				return -1;
			}

			node->state = TFC_INUSE;
			sssnprintf(name_out, name_bytes, "%s%s%s", dir, between, node->fname);

			if(rename(old, name_out) != 0) {
				/* This is possible under normal circumstances due to a
				name collision. */
				RemoveName(node->fname);
				--m_count;
#ifdef DEC_TFC_COUNTERS
				++m_getmisses;
#endif
				return CreateTempFile(dir, ext, name_out, name_bytes);
			}

#ifdef DEC_TFC_COUNTERS
			++m_renames;
#endif
			return 0;
		}
		node = GetNext(node);
	}

	/* Unable to find an entry to reuse. They are all in use. */
#ifdef DEC_TFC_COUNTERS
	++m_getmisses;
#endif
	return CreateTempFile(dir, ext, name_out, name_bytes);
}

/* Public interface. Release a temp file name from the cache. This
is done when we want to give the file to somebody outside of the
decomposer. We simply stop tracking it and it becomes their
responsibility to delete it when they are done with it. If the
name is not in the cache then we ignore it. */
void CDecTempFileCache::release_name(const char *name)
{
	char *p;
	char *path;
	char copy[MAX_PATH + 1];
	size_t len;

	dec_assert((name != NULL) && (*name != 0));

#ifdef DEC_TFC_TRACE
	printf("TFC: release_name(%s)\n", name);
#endif

	/* Periodically purge free files */
	DoPurge();

	/* Ensure that all paths are absolute for comparison
	purposes. The paths we compare also have no trailing
	slashes, which matches what we capture in get_tempname. */
	if(*name == '/') {
		strcpy(copy, name);
	}
	else {
		if(getcwd(copy, MAX_PATH + 1) == NULL) {
			dec_assert(0);
			strcpy(copy, name);
		}
		else {
			len = strlen(copy);
			p = copy + len - 1;
			if(*p != '/') {
				++p;
				*p = '/';
			}
			++p;
			strcpy(p, name);
		}
	}

	p = strrchr(copy, '/');
	if(p == NULL) {
		p = copy;
		path = "";
	}
	else {
		*p = 0;
		++p;
		path = copy;
		if(path[0] == 0) {
			path = "/";
		}
	}

	if(strcmp(path, m_tmpdir) != 0) {
		/* Not in our directory, so not one of ours */
		return;
	}

	dec_assert(*p != 0);

	/* If the name is too long, it can't be one of ours. We can't
	continue processing it because the logic below assumes it is
	within the correct boundaries. */
	if(strlen(p) > MAX_TFC_NAME) {
		return;
	}


#ifdef DEC_TFC_COUNTERS
	node = FindNode(p);
	if(node == NULL) {
		/* This is not one of ours */
		return;
	}

	if(strcmp(p, node->fname) != 0) {
		/* Not one of ours */
		return;
	}

	++m_releases;
#endif

	RemoveName(p);
}

/* Called on each public interface call. Occasionally goes through and
purges old temp files that are not in use. Criteria are when we have had
a period of no activity (calls to this function) for a certain number of
seconds, or when we have not purged for a certain number of minutes due
to continuous activity. Only files that are in the cache, but are not
currently in use are purged. They are not erased, but they are truncated
to zero length. */
void CDecTempFileCache::DoPurge(void)
{
	time_t now;
	int do_purge = 0;
	TFC_NODE *node;

	time(&now);

	if(m_lastpurge > now) {
		do_purge = 1;
	}
	else if((now - m_lastact) >= TFC_PURGE_INTERVAL) {
		do_purge = 1;
	}
	else if((now - m_lastpurge) >= TFC_ABSOLUTE_PURGE_INTERVAL) {
		do_purge = 1;
	}

	m_lastact = now;

	if(!do_purge) {
		return;
	}

#ifdef DEC_TFC_TRACE
	printf("TFC: Running a purge\n");
#endif

#ifdef DEC_TFC_COUNTERS
	++m_purges;
#endif

	m_lastpurge = now;

	if(m_root == NULL) {
		return;
	}

	node = m_root;
	GetFirst(node);

	while(node != NULL) {
		PurgeNode(node);
		node = GetNext(node);
	}
}

/* Public interface. Forces a purge of all files in this cache.
Used to force a purge when decomposer object is destroyed. */
void CDecTempFileCache::force_purge(void)
{
	time_t now;
	TFC_NODE *node;

	time(&now);

	m_lastact = now;

#ifdef DEC_TFC_TRACE
	printf("TFC: Running a purge\n");
#endif

#ifdef DEC_TFC_COUNTERS
	++m_purges;
#endif

	m_lastpurge = now;

	if(m_root == NULL) {
		return;
	}

	node = m_root;
	GetFirst(node);

	while(node != NULL) {
		PurgeNode(node);
		node = GetNext(node);
	}
}

/* Purge (truncate) the file at this node if necessary */
void CDecTempFileCache::PurgeNode(TFC_NODE *node)
{
	char path[MAX_PATH + 1];
	char *p;
	size_t len;
	FILE *fp;

	dec_assert(node != NULL);

	if((node->state == TFC_FREE) && (node->fname[0] != 0)) {
		strcpy(path, m_tmpdir);
		len = strlen(path);
		if(len > 0) {
			p = path + len - 1;
			if(*p != '/') {
				++p;
				*p = '/';
			}
			++p;
		}
		else {
			p = path;
		}

		strcpy(p, node->fname);

		fp = fopen(path, "wb");
		dec_assert(fp != NULL);
		if(fp != NULL) {
			fclose(fp);
		}
	}
}

/* The next few functions form our implementation of a hex radix tree.
This is the data structure that we use to store and index information
about temp files in our cache.

This structure is complex, so it bears some explanation.

Each node (TFC_NODE item) can be a leaf, branch, or both. Leaf nodes
contain information about temp files in the cache. Branch items have
child nodes. An item can be both, in that it can contain information
about a temp file while it also has child nodes.

This is a hex radix tree, so we proceed by hex digit, or binary nybble.
Each node matches some sequence of zero or more nybbles. Only the root
node may match zero. All other nodes must match at least one, since a
child node always begins with the nybble of its parent link. The
nybbles in a node indicate that all nodes under this one, inclusive,
have those nybbles in common in a given position in their key strings.

Our implementation uses file names with no paths as the key strings, and
we store them with the most-significant nybble of the LAST byte of
the string first, followed by the least-significant, then proceeding
BACKWARD through the string. This allows us to easily find all items
in the cache that END with a particular string, which is useful since
we need to look up files that have a certain extension.

In cases where a leaf node represents a file whose name is a righmost
substring of other files in the tree, that node is also a branch node,
and its children represent the items with longer names that begin (end) 
with the same sequence of nybbles.

Leaf nodes are identified as such by the fact that they contain the
full key string (file name) in its proper order. Branch-only nodes
have an empty file name.

To locate an item in the tree, we start at the root node. If that is
NULL then the tree is empty. Otherwise, we match nybbles from the end
of our key string backward with those in the node until either a
mismatch, or until we reach the end of either the key string or the
nybbles in the node. If we reach the end of the key string then we 
have located the nearest node that corresponds with that key string.
If this node is a leaf node with no extraneous nybbles, then there is 
a match. If we mismatch a nybble then the given key is not in the tree. 
If we reach the end of the nybbles in the node before a key match, we 
select the next node by using the next nybble in the key string as an 
index into a table of 16 pointers in the node. If the pointer is NULL 
then the given key is not in the tree. Otherwise we start again with the 
next node and repeat the process. The first nybble in the next node 
must be the same as the nybble that we used as an index into the parent's 
table of children. When we reach the end of the input key, we have located 
the nearest matching node. If we hit a mismatch or a NULL link before 
reaching the end of the input key then there are no matching nodes in the 
tree. In the case where we match a node, but it is not a leaf node, we know 
that there are items in the cache that END with the give string, but none 
that IS the given string. We can then traverse the tree below the given node
to locate all items that end with the given string.

To add an item to the tree, we traverse the tree until one of three
things happens: either there is a mismatch in a node, or we follow a
NULL link, or we reach the end of the new item's key. If there is a
mismatch in a node, we split that node by creating a new node to be
its parent. This new node contains all nybbles of the old node up to, 
but not including the mismatched one. It takes the place of the old
node, and the pointer in its table that corresponds to the next
nybble in the original node is pointed to the original node. Then the
original node is set to contain only the nybbles after the mismatching
one, including it. After splitting the node we can create a new node
that is also a child of the new parent, but that starts with the
actual nybble of the key we are inserting and contains all nybbles
from that point on. It is made a leaf node, so we copy the name of
the new item into it, and we link to it from the appropriate pointer
in the new parent. In the case where we follow a NULL link, the
tree already has a logical parent for our new leaf node. We simply
create a new leaf node, change the NULL link to point to it, and set 
its nybbles to be the remaining nybbles of the key. In the case where
we run into the end (beginning) of the key string there are really
three possibilities. Maybe there is already a node that perfectly
matches our name, but that is not yet a leaf node. In that case, we
just make it a leaf node by copying the name into it. The second
case would be there are more nybbles left in the node after the
last one in our key. In that case, we split the node. The new
parent takes the place of the existing node in the tree, and becomes
our leaf node. It is linked to the existing node, which is set to
contain all of the nybbles following those in the key we are
adding. The third case would be the node is already a leaf node
that ends with the last nybble in our name. In that case, the
name in the node had better match ours, and we have detected an
attempt to insert a duplicate into the tree.

To remove an item from the tree, we first locate its node. If it
is not found, or if it is not a leaf node, then the item is not
in the tree. If the given item has no children, then we remove it
from the tree and delete it. Otherwise we wipe its name out so that
it becomes a branch-only node.

This data structure is complex, but it is one of the most efficient
available, especially for searching for a group of items that all
end with the same characters. Some variant of this structure is
commonly used for database indexing. Since we expect to have to
search the items in the cache in various ways, and quite often, it
is better to have an efficient, though complex representation. */

TFC_NODE *CDecTempFileCache::FindNode(const char *name, TFC_NODE **parent, 
  unsigned char *parent_nybble)
{
	const unsigned char *p;
	TFC_NODE *node;
	size_t len;
	int index;
	unsigned char nybble;
	unsigned char mask;
	TFC_NODE *par = NULL;
	unsigned char par_nyb = 16;

	dec_assert(name != NULL);

#ifdef DEC_TFC_COUNTERS
	++m_searches;
#endif

	len = strlen(name);
	dec_assert(len != 0);
	dec_assert(len <= MAX_TFC_NAME);

	/* Names are stored backwards in the tree. To walk the tree,
	start at the end of the name and work back. */
	p = (const unsigned char*)name + len - 1;

	/* Mask off a nybble at a time, high order first. */
	mask = 0xF0;

	node = m_root;

	/* When we hit a null link, including the root, we have failed
	to find a corresponding node. */
	while(node != NULL) {
		index = 0;

		/* Walk the nybbles in this node, matching those in our
		name until we either hit the end of the node's nybbles, or
		the beginning of the name. It is legal for the root node to
		have no nybbles to match. All others must have at least one. */
		while(1) {
			nybble = *p & mask;
			if(mask == 0xF0) {
				nybble = nybble >> 4;
			}

			if((len == 0) || (index == node->num_nybbles)) {
				break;
			}

#ifdef DEC_TFC_COUNTERS
			++m_compares;
#endif

			if(nybble != node->nybbles[index]) {
				/* Mismatch means name not found. */
				return NULL;
			}

			++index;

			/* Reverse the mask bits. */
			mask ^= 0xFF;

			/* Advance to the prior byte every other time through. */
			if(mask == 0xF0) {
				--p;
				--len;
			}
		}

		if(len == 0) {
			/* We have located the nearest matching node. */
			if(parent != NULL) {
				*parent = par;
			}

			if(parent_nybble != NULL) {
				*parent_nybble = par_nyb;
			}

			return node;
		}

		/* We matched all nybbles in the node. Follow the link to the
		next node. */
		par_nyb = nybble;
		par = node;
		node = node->children[nybble];
	}

	/* Hit a NULL link. No match. */
	return NULL;
}

/* Add a name to the tree. Here is where all of the magic (i.e. 
complexity) lies. */
TFC_NODE *CDecTempFileCache::AddName(const char *name)
{
	const unsigned char *p;
	TFC_NODE *node;
	TFC_NODE *parent;
	TFC_NODE *new_parent;
	unsigned char parent_nybble;
	size_t len;
	int index;
	unsigned char nybble;
	unsigned char mask;
	int i;

	dec_assert(name != NULL);

	len = strlen(name);
	dec_assert(len != 0);
	dec_assert(len <= MAX_TFC_NAME);

	/* Start at the end of the name. We are going to walk the
	tree to find the insertion point and method. */
	p = (const unsigned char*)name + len - 1;

	/* Process a nybble at a time, starting with the most
	significant. */
	mask = 0xF0;

	/* If the root node is NULL then the tree is empty. In that
	case this is easy. Just add a single leaf node for the item
	at hand. */
	if(m_root == NULL) {
		m_root = new TFC_NODE;
		if(m_root == NULL) {
			dec_assert(0);
			return NULL;
		}

#ifdef DEC_TFC_COUNTERS
		++m_nodes;
		if(m_nodes > m_peaknodes) {
			m_peaknodes = m_nodes;
		}

		m_depth = 1;
		if(m_depth > m_peakdepth) {
			m_peakdepth = 1;
		}
#endif

		for(index = 0; index < 16; ++index) {
			m_root->children[index] = NULL;
		}

		m_root->state = TFC_INUSE;
		m_root->cur_child = 0;
		m_root->num_nybbles = len * 2;
		strcpy(m_root->fname, name);

		index = 0;
		while(len > 0) {
			m_root->nybbles[index] = *p & mask;
			if(mask == 0xF0) {
				m_root->nybbles[index] = m_root->nybbles[index] >> 4;
			}
			++index;
			mask ^= 0xFF;
			if(mask == 0xF0) {
				--p;
				--len;
			}
		}

		dec_assert(index == m_root->num_nybbles);

		return m_root;
	}

	/* Start at the top */
	node = m_root;

	/* Keep track of the parent of the current node, along with the
	nybble link that lead to the present one. This is used when splitting
	nodes. */
	parent = NULL;
	parent_nybble = 16;

	/* Walk the tree. When we hit a NULL link, we have found a place to
	hang the given item. */
	while(node != NULL) {
		index = 0;

		/* Walk the nybbles of the current node until we reach either
		their end or the end of the new key. */
		while(1) {
			nybble = *p & mask;
			if(mask == 0xF0) {
				nybble = nybble >> 4;
			}

			if((len == 0) || (index == node->num_nybbles)) {
				break;
			}

			if(nybble != node->nybbles[index]) {
				/* Mismatch in the current node. Split the node. The
				index can be zero if this is the root node. */
				new_parent = new TFC_NODE;
				if(new_parent == NULL) {
					dec_assert(0);
					return NULL;
				}

#ifdef DEC_TFC_COUNTERS
				++m_nodes;
				if(m_nodes > m_peaknodes) {
					m_peaknodes = m_nodes;
				}
#endif

				for(i = 0; i < 16; ++i) {
					new_parent->children[i] = NULL;
				}

				new_parent->state = TFC_FREE;
				new_parent->cur_child = 0;
				new_parent->num_nybbles = index;

				for(i = 0; i < index; ++i) {
					new_parent->nybbles[i] = node->nybbles[i];
				}

				/* The new parent links down to the old node. */
				new_parent->children[node->nybbles[index]] = node;

				/* The old node's nybbles are truncated so they start at
				the point of the mismatch (index). */
				if(index != 0) {
					i = 0;
					while(index < node->num_nybbles) {
						node->nybbles[i++] = node->nybbles[index++];
					}
				}
				node->num_nybbles -= new_parent->num_nybbles;

				/* Link the new parent into the tree instead of the old
				node. */
				if(parent == NULL) {
					m_root = new_parent;
				}
				else {
					parent->children[parent_nybble] = new_parent;
				}

				new_parent->fname[0] = 0;

				/* Now add a new node for the item being inserted. */
				node = new TFC_NODE;
				if(node == NULL) {
					dec_assert(0);
					return NULL;
				}

#ifdef DEC_TFC_COUNTERS
				++m_nodes;
				if(m_nodes > m_peaknodes) {
					m_peaknodes = m_nodes;
				}
#endif

				/* Point the parent down to the item for the nybble that did
				not match in the original. */
				new_parent->children[nybble] = node;

				/* Initialize the new node */
				for(index = 0; index < 16; ++index) {
					node->children[index] = NULL;
				}

				node->state = TFC_INUSE;
				node->cur_child = 0;

				/* The new node is a leaf node that takes all remaining
				nybbles in the key string (in reverse). */
				node->num_nybbles = len * 2;
				if(mask == 0x0F) {
					--node->num_nybbles;
				}

				strcpy(node->fname, name);

				index = 0;
				while(len > 0) {
					node->nybbles[index] = *p & mask;
					if(mask == 0xF0) {
						node->nybbles[index] = node->nybbles[index] >> 4;
					}
					++index;
					mask ^= 0xFF;
					if(mask == 0xF0) {
						--p;
						--len;
					}
				}

#ifdef DEC_TFC_COUNTERS
				CalcDepth();
#endif

				return node;
			}

			/* Advance to the next nybble in this node and the prior
			nybble in the new key string. */
			++index;
			mask ^= 0xFF;
			if(mask == 0xF0) {
				--p;
				--len;
			}
		}

		if(len == 0) {
			/* Here we have reached the end of our key. If the node goes on
			further then it needs to be split. Otherwise the current node is
			ours. */
			if(index < node->num_nybbles) {
				/* Split the node. The new parent will become ours. */
				new_parent = new TFC_NODE;
				if(new_parent == NULL) {
					dec_assert(0);
					return NULL;
				}

#ifdef DEC_TFC_COUNTERS
				++m_nodes;
				if(m_nodes > m_peaknodes) {
					m_peaknodes = m_nodes;
				}
#endif

				for(i = 0; i < 16; ++i) {
					new_parent->children[i] = NULL;
				}

				new_parent->state = TFC_INUSE;
				new_parent->cur_child = 0;
				new_parent->num_nybbles = index;

				for(i = 0; i < index; ++i) {
					new_parent->nybbles[i] = node->nybbles[i];
				}

				/* Link the new parent down to the existing node */
				new_parent->children[node->nybbles[index]] = node;

				/* The existing node takes all nybbles after those that
				matched our name. */
				if(index != 0) {
					i = 0;
					while(index < node->num_nybbles) {
						node->nybbles[i++] = node->nybbles[index++];
					}
				}
				node->num_nybbles -= new_parent->num_nybbles;

				/* Link in the new parent in place of the existing node. The new
				parent links down to the existing node. */
				if(parent == NULL) {
					m_root = new_parent;
				}
				else {
					parent->children[parent_nybble] = new_parent;
				}

				/* The new parent becomes a leaf node for our name. */
				strcpy(new_parent->fname, name);

#ifdef DEC_TFC_COUNTERS
				CalcDepth();
#endif
				return new_parent;
			}
			else {
				/* This looks like our node. If it already has a file name
				then we have a duplicate entry. */
				if(node->fname[0] != 0) {
					/* If this fires then there is something wrong with
					the tree. */
					dec_assert(strcmp(node->fname, name) == 0);

					/* We should not be trying to insert duplicates. */
					dec_assert(0);
					return node;
				}

				/* This is our node. Just make it a leaf and initialize it. */
				strcpy(node->fname, name);
				node->state = TFC_INUSE;
				node->cur_child = 0;
				return node;
			}
		}

		/* Done walking this node. Link to the proper child and continue.
		Remember this node as the parent of the next. */
		parent = node;
		parent_nybble = nybble;
		node = node->children[nybble];
	}

	/* Followed a NULL link. This means we add a new node under the
	current one, with no need to split. */
	new_parent = parent;
	node = new TFC_NODE;
	if(node == NULL) {
		dec_assert(0);
		return NULL;
	}

#ifdef DEC_TFC_COUNTERS
	++m_nodes;
	if(m_nodes > m_peaknodes) {
		m_peaknodes = m_nodes;
	}
#endif

	/* Link to the new node */
	new_parent->children[nybble] = node;

	/* Initialize the node */
	for(index = 0; index < 16; ++index) {
		node->children[index] = NULL;
	}

	node->state = TFC_INUSE;
	node->cur_child = 0;
	strcpy(node->fname, name);

	/* The new node takes all nybbles left in our key */
	node->num_nybbles = len * 2;
	if(mask == 0x0F) {
		--node->num_nybbles;
	}

	index = 0;
	while(len > 0) {
		node->nybbles[index] = *p & mask;
		if(mask == 0xF0) {
			node->nybbles[index] = node->nybbles[index] >> 4;
		}
		++index;
		mask ^= 0xFF;
		if(mask == 0xF0) {
			--p;
			--len;
		}
	}

#ifdef DEC_TFC_COUNTERS
	CalcDepth();
#endif

	return node;
}

void CDecTempFileCache::RemoveName(const char *name)
{
	TFC_NODE *node;
	int i;
	int leaf;
	TFC_NODE *parent;
	unsigned char parent_nybble;
#ifndef NDEBUG
	size_t len;
#endif

	dec_assert(name != NULL);

#ifndef NDEBUG
	len = strlen(name);
	dec_assert(len != 0);
	dec_assert(len <= MAX_TFC_NAME);
#endif

	/* Look up the given name */
	node = FindNode(name, &parent, &parent_nybble);

	if(node == NULL) {
		/* Not there */
		return;
	}

	/* Is it our leaf node? */
	if(strcmp(node->fname, name) == 0) {
		/* Does it have any children? */
		leaf = 1;
		for(i = 0; i < 16; ++i) {
			if(node->children[i] != NULL) {
				leaf = 0;
				break;
			}
		}

		if(leaf) {
			/* Found our leaf with no children. Link it out and delete it. */
			parent->children[parent_nybble] = NULL;
			delete node;
#ifdef DEC_TFC_COUNTERS
			--m_nodes;
			CalcDepth();
#endif
		}
		else {
			/* It has children. Convert it to a non-leaf. */
			node->fname[0] = 0;
			node->state = TFC_FREE;
		}
	}
}

/* Tree iteration. Call GetFirst with the node that you want to walk. Then
call GetNext repeatedly passing the current node to get the next, until NULL
is returned. */

void CDecTempFileCache::GetFirst(TFC_NODE *node)
{
	dec_assert(node != NULL);
	m_stack.clear();
}

TFC_NODE *CDecTempFileCache::GetNext(TFC_NODE *node)
{
	dec_assert(node != NULL);

	node->cur_child = 0;

	while(node->children[node->cur_child] == NULL) {
		++(node->cur_child);
		while(node->cur_child == 16) {
			node = m_stack.pop();
			if(node == NULL) {
				return NULL;
			}
			++(node->cur_child);
		}
	}

	m_stack.push(node);
	return node->children[node->cur_child];
}

/* An instance of this class manages temp file caches for each
individual thread. The first time a thread calls this API, its
cache object is created. Subsequent calls are dispatched through
to that object. */

class CDecTempFileCacher
{
public:
	CDecTempFileCacher();
	~CDecTempFileCacher();

	int rm(const char *fname);
	int get_tempname(const char *dir, const char *ext, char *name_out);
	void purge_tfn_cache();
	void release_name(const char *name);

private:
	CDecTempFileCache *GetCache();
	pthread_key_t m_key;
	CDecTempFileCache **m_caches;
	unsigned long m_cachecount;
	unsigned long m_cachecapacity;
};

CDecTempFileCacher::CDecTempFileCacher()
{
	int rc;

	rc = pthread_key_create(&m_key, NULL);
	dec_assert(rc == 0);

	m_caches = NULL;
	m_cachecount = 0;
	m_cachecapacity = 0;
}

CDecTempFileCacher::~CDecTempFileCacher()
{
	int i;

	/* Clean up all caches */
	for(i = 0; i < m_cachecount; ++i) {
#ifdef DEC_TFC_TRACE
		printf("Calling cache destructor\n");
#endif
		dec_assert(m_caches[i] != NULL);
		delete m_caches[i];
	}
}

CDecTempFileCache *CDecTempFileCacher::GetCache()
{
	CDecTempFileCache *rc;
	int result;
	unsigned long new_count;
	unsigned long new_capacity;
	CDecTempFileCache **new_array;

#ifdef NONPORTABLE_TLS
	rc = NULL;
	pthread_getspecific(m_key, (void**)(&rc));
#else
	rc = (CDecTempFileCache*)pthread_getspecific(m_key);
#endif

	if(rc == NULL) {
#ifdef DEC_TFC_TRACE
		printf("TFC: Adding a thread\n");
#endif
		rc = new CDecTempFileCache;
		if(rc == NULL) {
			dec_assert(0);
			return NULL;
		}

		result = pthread_setspecific(m_key, (void*)rc);
		if(result != 0) {
			dec_assert(0);
			delete rc;
			return NULL;
		}

		new_count = m_cachecount + 1;
		if(new_count > m_cachecapacity) {
			if(m_cachecapacity == 0) {
				new_capacity = 5;
			}
			else {
				new_capacity = m_cachecapacity * 2;
			}

			new_array = (CDecTempFileCache**)
			 realloc(m_caches, new_capacity * sizeof(CDecTempFileCache*));
			if(new_array == NULL) {
				dec_assert(0);
				return rc;
			}

			m_caches = new_array;
			m_cachecapacity = new_capacity;
		}

		m_caches[m_cachecount] = rc;
		m_cachecount = new_count;
	}

	return rc;
}

int CDecTempFileCacher::rm(const char *fname)
{
	CDecTempFileCache *c;

	c = GetCache();

	if(c == NULL) {
		dec_assert(0);
		return (unlink(fname) == 0) ? 0 : -1;
	}

	return c->rm(fname);
}

void CDecTempFileCacher::release_name(const char *name)
{
	CDecTempFileCache *c;

	dec_assert((name != NULL) && (*name != 0));

#ifdef NONPORTABLE_TLS
	c = NULL;
	pthread_getspecific(m_key, (void**)&c);
#else
	c = (CDecTempFileCache*)pthread_getspecific(m_key);
#endif

	if(c != NULL) {
		c->release_name(name);
	}
}

int CDecTempFileCacher::get_tempname(const char *dir, const char *ext, char *name_out)
{
	CDecTempFileCache *c;
	char *rc;

	c = GetCache();

	if(c == NULL) {
		dec_assert(0);
		if (name_out == NULL) // tmpnam() isn't thread safe.  In the greater context,
							 // not even tmpnam_r() is thread safe.  They also both
		  return -1;		 // have potential security holes. - DTM
		rc = tmpnam(name_out);
		return (rc == NULL) ? -1 : 0;
	}

	return c->get_tempname(dir, ext, name_out);
}

void CDecTempFileCacher::purge_tfn_cache()
{
	CDecTempFileCache *c;

#ifdef NONPORTABLE_TLS
	c = NULL;
	pthread_getspecific(m_key, (void**)&c);
#else
	c = (CDecTempFileCache*)pthread_getspecific(m_key);
#endif

	if(c != NULL) {
		c->force_purge();
	}
}

CDecTempFileCacher g_tempFileCacher;

/* Here are the actual APIs. These are wrappers that call into
the global instance of CDecTempFileCacher, which manages the actual
caching. */
int dec_get_tempname(const char *dir, const char *ext, char *name_out)
{
	dec_assert(name_out != NULL);
	return g_tempFileCacher.get_tempname(dir, ext, name_out);
}

int dec_rm(const char *fname)
{
	dec_assert(fname != NULL);
	return g_tempFileCacher.rm(fname);
}

void dec_purge_tfn_cache(void)
{
	g_tempFileCacher.purge_tfn_cache();
}

void dec_release_name(const char *name)
{
	g_tempFileCacher.release_name(name);
}

#endif // defined DEC_REUSE_TEMP_FILES
