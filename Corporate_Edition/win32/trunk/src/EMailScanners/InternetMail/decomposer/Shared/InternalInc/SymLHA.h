//#############################################################################
//#
//#     SymLHA.h -- Main Header for SymLHA Library
//#
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
//#     Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//#
//#     Revisions
//#     ---------
//#     6/23/2003 -- Thomas Hurtt -- Initial Revision
//#
//#############################################################################

#ifndef _SYMLHA_H_
#define _SYMLHA_H_

#include <stdlib.h>
#include <time.h>
#if defined(SYM_NLM)
	#include <malloc.h>
#else
	#include <memory.h>
#endif
#include <string.h>
#include <ctype.h>
#include <limits.h>
#if defined(OS400)
	#include <new.h>
#else	
	#include <new>
#endif
// Standard Decomposer headers

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "LHATypes.h"
#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Decstdio.h"

// globals
extern volatile DWORD g_dwLHAMaxExtractSize;
extern volatile bool g_bLHAAbort;

USE_SYMLHA_NAMESPACE

class CLHAError
{
public:
	CLHAError();
	~CLHAError();
	void SetLastLHAError(LHA_ERROR lhaerr);
	LHA_ERROR GetLastLHAError();

private:
	LHA_ERROR m_lhaerr;
};

// Encode / Decode forward declarations
class CEncode;
class CDecode;

// Huffman class forward declarations
class CDynamicHuffman;
class CStaticHuffman;

class CSymLHA : public CLHAError
{
public:
	CSymLHA();
	~CSymLHA();

	// Open / Close archives
	LHA_ERROR CloseArchive(bool IgnoreErrors = false);
	LHA_ERROR OpenArchive(char * szArchive, unsigned long ExeSize);

	// Extract / CRD
	LHA_ERROR ChangeEntry(LzHeader * pOldFileHdr, const char * szNewArchive, const char * szNewFile, LHA_OPERATION Operation);
	LHA_ERROR CompressFile(const char * szNewFile, FILE * hArchive, LzHeader * OldHdr);
	LHA_ERROR ExtractEntry(LzHeader * pHdr, char * dstFile);

	// Entry Navigation
	LHA_ERROR GetEntry(LzHeader * pHdr);
	LHA_ERROR Reset();
	LHA_ERROR SkipEntry(LzHeader * pHdr);
	
	// Utility Functions
	LHA_ERROR StripPath(char * szSrc, char * szDst = NULL);
	LHA_ERROR ConvertToLocalPath(char * szPath);
	tm * ConvertFromDosTime(unsigned long dostime);
	unsigned long ConvertToDosTime(unsigned long unixtime);

	 
private:

	// Checksum / CRC
	long calc_sum(char * p, long len);
	void make_crctable();

	// Compress / Extract
	void add_one(FILE * fp, FILE * nafp, LzHeader * hdr);
	void extract_one(LzHeader * hdr, char * szOutFile);

	// Read / Write header
	bool get_header(LzHeader * hdr);
	void write_header(FILE * nafp, LzHeader * hdr);

	// Get / Put data
	unsigned short get_word();
	long get_longword();
	void put_longword(long v);
	void put_word(unsigned short v);

	// Utility functions
	short convert_method(const char * szMethod);
	const char * convert_method(const short Method);

	// CRD
	bool copy_remaining_entries();


	// variables
	FILE			*	mp_archive;
	FILE			*	mh_NewArchive;
	long				m_LastChangePos;
	unsigned short	*	m_crctable;
	char			*	m_filename;
	unsigned long		m_header_level;
	bool				m_generic_format;
	char			*	m_get_ptr;
	char			*	m_get_ptr_base;
	char			*	m_put_ptr;
	char			*	m_put_ptr_base;
	char			*	m_reading_filename;
	char			*	m_temporary_name;
	char			*	m_writing_filename;
	unsigned long		m_exeoffset;
	bool				m_MaxSizeReached;
	bool				m_Aborted;

#ifdef _WINDOWS
	struct _stat m_archive_stat;
#else
	struct stat m_archive_stat;
#endif

	// encode and decode objects
	CEncode *	mp_Encode;
	CDecode *	mp_Decode;

	// inlines
	inline void * bzero(void * d, size_t n)
	{
		return memset(d, 0, n);
	}

	inline void * bcopy(void * s, void * d, size_t n)
	{
		return memmove(d, s, n);
	}
	
	inline void set_get_base(void * ptr)
	{
		m_get_ptr_base = (char *)ptr;
	}
	inline void setup_get(void * ptr)
	{
		m_get_ptr = (char *)ptr;
	}
	
	inline void set_put_base(char * ptr)
	{
		m_put_ptr_base = ptr;
	}
	inline void setup_put(char * ptr)
	{
		m_put_ptr = ptr;
	}
	inline unsigned char get_byte()
	{
		if(m_get_ptr < (m_get_ptr_base + LZHEADER_STRAGE))
		{
			return (*m_get_ptr++ & 0xff);
		}
		return 0;
	}

	inline long bcmp(void * a, void * b, size_t n)		
	{
		return(memcmp(a, b, n));
	}
	inline void put_byte(char c)
	{
		if(m_put_ptr < (m_put_ptr_base + LZHEADER_STRAGE))
		{
			*m_put_ptr++ = c;
		}
	}
};


#endif // _SYMLHA_H_
