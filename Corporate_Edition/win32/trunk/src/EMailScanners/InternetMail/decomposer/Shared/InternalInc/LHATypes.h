//#############################################################################
//#
//#     LHATypes.h -- Defines standard data types and constants to be used 
//#		in the SymLHA Library.
//#
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
//#     Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//#
//#     Revisions
//#     ---------
//#     2/3/2004 -- Thomas Hurtt -- Initial Revision
//#
//#############################################################################
#if !defined(_LHA_TYPES_H_)
#define _LHA_TYPES_H_

#if defined(OS400)
	#define NAMESPACE_SYMLHA_BEGIN
	#define NAMESPACE_SYMLHA_END
	#define USE_SYMLHA_NAMESPACE
	#define BAD_ALLOCATION ...
#else
	#define NAMESPACE_SYMLHA_BEGIN namespace SymLHA {
	#define NAMESPACE_SYMLHA_END }
	#define USE_SYMLHA_NAMESPACE using namespace SymLHA;
	#define BAD_ALLOCATION std::bad_alloc
#endif

NAMESPACE_SYMLHA_BEGIN

// *** Standard constants used in LHA ***
#if !defined(UCHAR_MAX)
	#define UCHAR_MAX			((1<<(sizeof(unsigned char)*8))-1)
#endif

#if !defined(IBMCXX)
	#define CHAR_BIT			8
#endif

#define CRCPOLY					0xA001

#define LZHEADER_STRAGE			4096
#define FILENAME_LENGTH			1024
#define METHOD_TYPE_STRAGE		5

#define I_HEADER_CHECKSUM		1
#define I_HEADER_LEVEL			20
#define I_METHOD				2
#define I_PACKED_SIZE			7
#define I_GENERIC_HEADER_BOTTOM	I_EXTEND_TYPE

// + name_length
#define I_EXTEND_TYPE			24	

#define I_HEADER_SIZE			0

// + name_length
#define I_UNIX_EXTEND_BOTTOM	36
#define I_NAME					22

#define UNIX_FILE_REGULAR		0100000
#define UNIX_RW_RW_RW			0000666

#define EXTEND_GENERIC			0x00
#define EXTEND_UNIX				0x55
#define EXTEND_MSDOS			0x4D
#define EXTEND_MACOS			0x6D
#define EXTEND_HUMAN			0x48

#define WRITE_BINARY			"wb"
#define READ_BINARY				"rb"

// -lh0-
const char LZHUFF0_METHOD[]	=	{0x2D, 0x6C, 0x68, 0x30, 0x2D, 0x00};
// -lh1-
const char LZHUFF1_METHOD[]	=	{0x2D, 0x6C, 0x68, 0x31, 0x2D, 0x00};
// -lh2-
const char LZHUFF2_METHOD[]	=	{0x2D, 0x6C, 0x68, 0x32, 0x2D, 0x00};
// -lh3-
const char LZHUFF3_METHOD[]	=	{0x2D, 0x6C, 0x68, 0x33, 0x2D, 0x00};
// -lh4-
const char LZHUFF4_METHOD[]	=	{0x2D, 0x6C, 0x68, 0x34, 0x2D, 0x00};
// -lh5-
const char LZHUFF5_METHOD[]	=	{0x2D, 0x6C, 0x68, 0x35, 0x2D, 0x00};
// -lh6-
const char LZHUFF6_METHOD[]	=	{0x2D, 0x6C, 0x68, 0x36, 0x2D, 0x00};
// -lh7-
const char LZHUFF7_METHOD[]	=	{0x2D, 0x6C, 0x68, 0x37, 0x2D, 0x00};
// -lzs-
const char LARC_METHOD[] =		{0x2D, 0x6C, 0x7A, 0x73, 0x2D, 0x00};
// -lz5-
const char LARC5_METHOD[] =		{0x2D, 0x6C, 0x7A, 0x35, 0x2D, 0x00};
// -lz4-
const char LARC4_METHOD[] =		{0x2D, 0x6C, 0x7A, 0x34, 0x2D, 0x00};
// -lhd-
const char LZHDIRS_METHOD[]	=	{0x2D, 0x6C, 0x68, 0x64, 0x2D, 0x00};

#define LZHUFF0_METHOD_NUM		0
#define LZHUFF1_METHOD_NUM		1
#define LZHUFF2_METHOD_NUM		2
#define LZHUFF3_METHOD_NUM		3
#define LZHUFF4_METHOD_NUM		4
#define LZHUFF5_METHOD_NUM		5
#define LZHUFF6_METHOD_NUM		6
#define LZHUFF7_METHOD_NUM		7
#define LARC_METHOD_NUM			8
#define LARC5_METHOD_NUM		9
#define LARC4_METHOD_NUM		10
#define LZHDIRS_METHOD_NUM		11
#define LZMETHOD_NOT_SUPPORTED  255

#define BUFFERSIZE				2048

#define rindex					strrchr

// (CHAR_BIT * sizeof(ushort))
#define USHRT_BIT				16

// lh7 use 16bits
#define MAX_DICBIT				16

// bit size of length field for tree output
#define LENFIELD				4

#define THRESHOLD				3
#define MAXMATCH				256	
#define NC 						(UCHAR_MAX + MAXMATCH + 2 - THRESHOLD)

// alphabet size
#define N1						286

// # of nodes in Huffman tree
#define N2						(2 * N1 - 1)

#define NP						(MAX_DICBIT + 1)
#define NPT						0x80
#define NT						(USHRT_BIT + 3)

// smallest integer such that (1 << PBIT) > * NP
#define PBIT					5

// smallest integer such that (1 << TBIT) > * NT
#define TBIT 					5

// $\lfloor \log_2 NC \rfloor + 1$
#define CBIT					9

#define MAGIC0					18
#define MAGIC5					19

#define N_CHAR					(256 + 60 - THRESHOLD + 1)
#define TREESIZE_C				(N_CHAR * 2)
#define TREESIZE_P				(128 * 2)
#define TREESIZE				(TREESIZE_C + TREESIZE_P)
#define ROOT_C					0
#define ROOT_P					TREESIZE_C

// >= log2(F-THRESHOLD+258-N1)
#define EXTRABITS				8

// >= log2(MAXBUF) 
#define BUFBITS					16

// bit size of length field for tree output
#define LENFIELD				4

#define DICSIZ					(1L << 16)
#define TXTSIZ					(DICSIZ * 2L + MAXMATCH)
#define HSHSIZ					(((unsigned long)1) <<15)

#define HEADER_LEVEL0			0x00
#define HEADER_LEVEL1			0x01
#define HEADER_LEVEL2			0x02

#define CURRENT_UNIX_MINOR_VERSION		0x00
#define DELIM2					(0xff)

// chain limit 
#define LIMIT					0x100	

// *** Path delimiters ***
const unsigned char INTERNAL_DELIM = 0xFF;
const unsigned char WIN_NOVELL_DELIM = 0x5C;
const unsigned char UNIX_DELIM = 0x2F;

#if defined(WIN32) || defined(SYM_NLM)
	const unsigned char PLATFORM_DELIM = WIN_NOVELL_DELIM;
#else
	const unsigned char PLATFORM_DELIM = UNIX_DELIM;
#endif


// *** SymLHA structs ***
typedef struct _S_LzHeader 
{
	unsigned char			header_size;
	char            method[METHOD_TYPE_STRAGE];
	long           packed_size;
	long           original_size;
	long           last_modified_stamp;
	unsigned short		    attribute;
	unsigned char			header_level;
	char            name[FILENAME_LENGTH];
	unsigned short			crc;
	long					has_crc;
	unsigned char			extend_type;
	unsigned char			minor_version;
	time_t          unix_last_modified_stamp;
	unsigned short			unix_mode;
	unsigned short			unix_uid;
	unsigned short			unix_gid;

	// each header will contain the base offset that the header starts at.
	unsigned long offset;

}  LzHeader;


typedef enum _E_LHA_ERROR
{
	UNKNOWN_COMPRESSION_METHOD,
	UNKNOWN_HEADER_LEVEL,
	UNSUPPORTED_HEADER_LEVEL,
	UNSUPPORTED_COMPRESSION_METHOD,
	UNKOWN_HEADER,
	CHECKSUM_ERROR,
	INVALID_HEADER,
	ARCHIVE_ERROR,
	MEM_ALLOCATE_ERROR,
	FILE_READ_ERROR,
	FILE_WRITE_ERROR,
	FILE_OPEN_ERROR,
	ARCHIVE_OPEN_ERROR,
	ARCHIVE_CLOSE_ERROR,
	ARCHIVE_NOT_OPEN,
	ARCHIVE_EOF,
	NULL_POINTER,
	CRC_ERROR,
	BAD_TABLE,
	MAX_EXTRACT,
	USER_ABORT,
	DIRECTORY_ENTRY,
	FILENAME_TOO_LONG_FOR_PLATFORM,
	LHA_OK

} LHA_ERROR;

typedef enum _E_LHA_OPERATION
{
	CHANGE_ENTRY,
	REPLACE_ENTRY,
	DELETE_ENTRY

} LHA_OPERATION;

NAMESPACE_SYMLHA_END

#endif // _LHA_TYPES_H_
