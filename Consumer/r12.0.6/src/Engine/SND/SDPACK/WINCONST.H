//************************************************************************
//
// $Header:   S:/SDPACK/VCS/WINCONST.H_v   1.0   13 Mar 1998 12:51:28   DCHI  $
//
// Description:
//      Contains constants and structures dealing with Windows
//      executables.
//
//************************************************************************
// $Log:   S:/SDPACK/VCS/WINCONST.H_v  $
// 
//    Rev 1.0   13 Mar 1998 12:51:28   DCHI
// Initial revision.
// 
//    Rev 1.6   04 Nov 1997 20:18:10   JWILBER
// Added far (long) pointer types for a few structures that needed it.
//
//    Rev 1.5   21 Nov 1996 17:12:10   AOONWAL
// No change.
//
//    Rev 1.4   29 Oct 1996 13:00:02   AOONWAL
// No change.
//
//    Rev 1.3   06 Sep 1996 14:53:48   DCHI
// Added SYM_VXD to pack changes.
//
//    Rev 1.2   06 Sep 1996 14:20:42   DCHI
// pack changes
//
//    Rev 1.1   28 Aug 1996 16:43:06   DCHI
// Added pack pragma's and corrected a couple of structure field types.
//
//    Rev 1.0   05 Feb 1996 17:23:22   DCHI
// Initial revision.
//
//************************************************************************

#ifndef _WINCONSTH_

#define _WINCONSTH_

#include "platform.h"

#define WINDOWS_NE_FORMAT		0
#define WINDOWS_PE_FORMAT		1
#define WINDOWS_UNKNOWN_FORMAT	0xFFFF

#define SCAN_BUFFER_SIZE        512
#define SCAN_OVERLAP_SIZE       16
#define WILD_CARD_VALUE         0xFFFF

#define WINDOWS_NE_SIGNATURE	0x454E
#define WINDOWS_PE_SIGNATURE	0x4550


#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct
{
    BYTE exesig[2];                 /* MZ */
	WORD len_mod_512;       /* length of file % 512 */
	WORD page_size;         /* size of file in 512 byte pages */
	WORD reloc_table_items; /* # of relocation table items */
	WORD header_size;       /* size of header in paragraphs */
	WORD min_par_above;     /* min # of pars needed above program */
	WORD max_par_above;     /* max " */
	WORD ss;
	WORD sp;
	WORD checksum;
	WORD ip;
	WORD cs;
	WORD first_rel_offset;  /* offset of first relocation table */
	WORD wOverlay;
	WORD wDontCare[0x10];	/* offset 1c */
	WORD w2ndHdrOffset;		/* Offset 3c */
} EXEHEADER_T, FAR *LPEXEHEADER_T;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

//
// File header format.
//

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct _NAVEX_IMAGE_FILE_HEADER {
    WORD    Machine;
    WORD    NumberOfSections;
    DWORD   TimeDateStamp;
    DWORD   PointerToSymbolTable;
    DWORD   NumberOfSymbols;
    WORD    SizeOfOptionalHeader;
    WORD    Characteristics;
} NAVEX_IMAGE_FILE_HEADER, FAR *LPNAVEX_IMAGE_FILE_HEADER;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

#define NAVEX_IMAGE_SIZEOF_FILE_HEADER             20

#define NAVEX_IMAGE_FILE_RELOCS_STRIPPED           0x0001  // Relocation info stripped from file.
#define NAVEX_IMAGE_FILE_EXECUTABLE_IMAGE          0x0002  // File is executable  (i.e. no unresolved externel references).
#define NAVEX_IMAGE_FILE_LINE_NUMS_STRIPPED        0x0004  // Line nunbers stripped from file.
#define NAVEX_IMAGE_FILE_LOCAL_SYMS_STRIPPED       0x0008  // Local symbols stripped from file.
#define NAVEX_IMAGE_FILE_BYTES_REVERSED_LO         0x0080  // Bytes of machine word are reversed.
#define NAVEX_IMAGE_FILE_32BIT_MACHINE             0x0100  // 32 bit word machine.
#define NAVEX_IMAGE_FILE_DEBUG_STRIPPED            0x0200  // Debugging info stripped from file in .DBG file
#define NAVEX_IMAGE_FILE_SYSTEM                    0x1000  // System File.
#define NAVEX_IMAGE_FILE_DLL                       0x2000  // File is a DLL.
#define NAVEX_IMAGE_FILE_BYTES_REVERSED_HI         0x8000  // Bytes of machine word are reversed.

#define NAVEX_IMAGE_FILE_MACHINE_UNKNOWN           0
#define NAVEX_IMAGE_FILE_MACHINE_I386              0x14c   // Intel 386.
#define NAVEX_IMAGE_FILE_MACHINE_R3000             0x162   // MIPS little-endian, 0540 big-endian
#define NAVEX_IMAGE_FILE_MACHINE_R4000             0x166   // MIPS little-endian
#define NAVEX_IMAGE_FILE_MACHINE_ALPHA             0x184   // Alpha_AXP
#define NAVEX_IMAGE_FILE_MACHINE_POWERPC           0x1F0   // IBM PowerPC Little-Endian


///////////////////////////////////////////////////////////////////////////////
// NE structure...
///////////////////////////////////////////////////////////////////////////////

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct tagWININFO {
    WORD   signature;                   // 0
    BYTE   linkerVersion;               // 2
    BYTE   linkerRevision;              // 3
    WORD   entryTabOffset;              // 4
    WORD   entryTabLen;                 // 6
    DWORD  reserved1;                   // 8
    WORD   exeFlags;                    // 12
    WORD   dataSegNum;                  // 14
    WORD   localHeapSize;               // 16
    WORD   stackSize;                   // 18
    WORD   ip;                          // 20
    WORD   cs;                          // 22
    DWORD  ss_sp;                       // 24
    WORD   segTabEntries;               // 28
    WORD   modTabEntries;               // 30
    WORD   nonResTabSize;               // 32
    WORD   segTabOffset;                // 34
    WORD   resTabOffset;                // 36
    WORD   resNameTabOffset;            // 38 RES NAME OFFSET (From top of NE)
    WORD   modTabOffset;                // 40
    WORD   impTabOffset;                // 42
    WORD   nonResTabOffset;             // 44 NONRES NAME OFFSET (From TOF)
    WORD   reserved2;                   // 46
    WORD   numEntryPoints;              // 48
    WORD   shiftCount;                  // 50
    WORD   numResourceSegs;             // 52
    BYTE   targetOS;                    // 54
    BYTE   miscFlags;                   // 55
    WORD   fastLoadOffset;              // 56
    WORD   fastLoadSize;                // 58
    WORD   reserved3;                   // 60
    BYTE   winRevision;                 // 62
    BYTE   winVersion;                  // 63
} WININFO, FAR *LPWININFO;

typedef struct tagTBSEGMENT {
    WORD  segDataOffset;
    WORD  segLen;
    WORD  segFlags;
    WORD  segMinSize;
} TBSEGMENT;

typedef struct tagBUNDLEHEADER {
    BYTE    byBundleCount;
    BYTE    byBundleType;
} BUNDLEHEADER;

typedef struct tagFIXEDBUNDLE {
    BYTE    byFlags;
    WORD    wEntry;
} FIXEDBUNDLE;

typedef struct tagMOVEABLEBUNDLE {
    BYTE    byFlags;
    WORD    wInt3F;
    BYTE    bySegNum;
    WORD    wEntry;
} MOVEABLEBUNDLE;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

#define NE_SEG_FLAG_DATA_SEG            0x0001

#define NE_BUNDLE_NULL					0x00
#define NE_BUNDLE_MOVEABLE             	0xFF
#define NE_BUNDLE_CONSTANT_MODULE      	0xFE


///////////////////////////////////////////////////////////////////////////////
// PE structure...
///////////////////////////////////////////////////////////////////////////////

//
// DLL support.
//

//
// Export Format
//

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct _NAVEX_IMAGE_EXPORT_DIRECTORY {
    DWORD   Characteristics;
    DWORD   TimeDateStamp;
    WORD    MajorVersion;
    WORD    MinorVersion;
    DWORD   Name;
    DWORD   Base;
    DWORD   NumberOfFunctions;
    DWORD   NumberOfNames;
    DWORD   AddressOfFunctions;
    DWORD   AddressOfNames;
    DWORD   AddressOfNameOrdinals;
} NAVEX_IMAGE_EXPORT_DIRECTORY, *PNAVEX_IMAGE_EXPORT_DIRECTORY,
    FAR *LPNAVEX_IMAGE_EXPORT_DIRECTORY;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

//
// Directory format.
//

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct _NAVEX_IMAGE_DATA_DIRECTORY {
    DWORD   VirtualAddress;
    DWORD   Size;
} NAVEX_IMAGE_DATA_DIRECTORY, FAR *LPNAVEX_IMAGE_DATA_DIRECTORY;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

#define NAVEX_IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

//
// Optional header format.
//

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct _NAVEX_IMAGE_OPTIONAL_HEADER {
    //
    // Standard fields.
    //

    WORD    Magic;
    BYTE    MajorLinkerVersion;
    BYTE    MinorLinkerVersion;
    DWORD   SizeOfCode;
    DWORD   SizeOfInitializedData;
    DWORD   SizeOfUninitializedData;
    DWORD   AddressOfEntryPoint;
    DWORD   BaseOfCode;
    DWORD   BaseOfData;

    //
    // NT additional fields.
    //

    DWORD   ImageBase;
    DWORD   SectionAlignment;
    DWORD   FileAlignment;
    WORD    MajorOperatingSystemVersion;
    WORD    MinorOperatingSystemVersion;
    WORD    MajorImageVersion;
    WORD    MinorImageVersion;
    WORD    MajorSubsystemVersion;
    WORD    MinorSubsystemVersion;
    DWORD   Reserved1;
    DWORD   SizeOfImage;
    DWORD   SizeOfHeaders;
    DWORD   CheckSum;
    WORD    Subsystem;
    WORD    DllCharacteristics;
    DWORD   SizeOfStackReserve;
    DWORD   SizeOfStackCommit;
    DWORD   SizeOfHeapReserve;
    DWORD   SizeOfHeapCommit;
    DWORD   LoaderFlags;
    DWORD   NumberOfRvaAndSizes;
    NAVEX_IMAGE_DATA_DIRECTORY DataDirectory[NAVEX_IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} NAVEX_IMAGE_OPTIONAL_HEADER, FAR *LPNAVEX_IMAGE_OPTIONAL_HEADER;

typedef struct _NAVEX_IMAGE_ROM_OPTIONAL_HEADER {
    WORD   Magic;
    BYTE   MajorLinkerVersion;
    BYTE   MinorLinkerVersion;
    DWORD  SizeOfCode;
    DWORD  SizeOfInitializedData;
    DWORD  SizeOfUninitializedData;
    DWORD  AddressOfEntryPoint;
    DWORD  BaseOfCode;
    DWORD  BaseOfData;
    DWORD  BaseOfBss;
    DWORD  GprMask;
    DWORD  CprMask[4];
    DWORD  GpValue;
} NAVEX_IMAGE_ROM_OPTIONAL_HEADER, FAR *LPNAVEX_IMAGE_ROM_OPTIONAL_HEADER;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

#define NAVEX_IMAGE_SIZEOF_ROM_OPTIONAL_HEADER      56
#define NAVEX_IMAGE_SIZEOF_STD_OPTIONAL_HEADER      28
#define NAVEX_IMAGE_SIZEOF_NT_OPTIONAL_HEADER      224

#define NAVEX_IMAGE_NT_OPTIONAL_HDR_MAGIC        0x10b
#define NAVEX_IMAGE_ROM_OPTIONAL_HDR_MAGIC       0x107

typedef struct _NAVEX_IMAGE_NT_HEADERS {
    DWORD Signature;
    NAVEX_IMAGE_FILE_HEADER FileHeader;
    NAVEX_IMAGE_OPTIONAL_HEADER OptionalHeader;
} NAVEX_IMAGE_NT_HEADERS, FAR *LPNAVEX_IMAGE_NT_HEADERS;

//
// Section header format.
//

#define NAVEX_IMAGE_SIZEOF_SHORT_NAME              8

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct _NAVEX_IMAGE_SECTION_HEADER {
    BYTE    Name[NAVEX_IMAGE_SIZEOF_SHORT_NAME];
    union {
            DWORD   PhysicalAddress;
            DWORD   VirtualSize;
    } Misc;
    DWORD   VirtualAddress;
    DWORD   SizeOfRawData;
    DWORD   PointerToRawData;
    DWORD   PointerToRelocations;
    DWORD   PointerToLinenumbers;
    WORD    NumberOfRelocations;
    WORD    NumberOfLinenumbers;
    DWORD   Characteristics;
} NAVEX_IMAGE_SECTION_HEADER, *PNAVEX_IMAGE_SECTION_HEADER,
    FAR *LPNAVEX_IMAGE_SECTION_HEADER;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

#define NAVEX_IMAGE_SIZEOF_SECTION_HEADER          40

#define NAVEX_IMAGE_SCN_TYPE_NO_PAD                0x00000008  // Reserved.

#define NAVEX_IMAGE_SCN_CNT_CODE                   0x00000020  // Section contains code.
#define NAVEX_IMAGE_SCN_CNT_INITIALIZED_DATA       0x00000040  // Section contains initialized data.
#define NAVEX_IMAGE_SCN_CNT_UNINITIALIZED_DATA     0x00000080  // Section contains uninitialized data.

#define NAVEX_IMAGE_SCN_LNK_OTHER                  0x00000100  // Reserved.
#define NAVEX_IMAGE_SCN_LNK_INFO                   0x00000200  // Section contains comments or some other type of information.
#define NAVEX_IMAGE_SCN_LNK_REMOVE                 0x00000800  // Section contents will not become part of image.
#define NAVEX_IMAGE_SCN_LNK_COMDAT                 0x00001000  // Section contents comdat.

#define NAVEX_IMAGE_SCN_ALIGN_1BYTES               0x00100000  //
#define NAVEX_IMAGE_SCN_ALIGN_2BYTES               0x00200000  //
#define NAVEX_IMAGE_SCN_ALIGN_4BYTES               0x00300000  //
#define NAVEX_IMAGE_SCN_ALIGN_8BYTES               0x00400000  //
#define NAVEX_IMAGE_SCN_ALIGN_16BYTES              0x00500000  // Default alignment if no others are specified.
#define NAVEX_IMAGE_SCN_ALIGN_32BYTES              0x00600000  //
#define NAVEX_IMAGE_SCN_ALIGN_64BYTES              0x00700000  //

#define NAVEX_IMAGE_SCN_MEM_DISCARDABLE            0x02000000  // Section can be discarded.
#define NAVEX_IMAGE_SCN_MEM_NOT_CACHED             0x04000000  // Section is not cachable.
#define NAVEX_IMAGE_SCN_MEM_NOT_PAGED              0x08000000  // Section is not pageable.
#define NAVEX_IMAGE_SCN_MEM_SHARED                 0x10000000  // Section is shareable.
#define NAVEX_IMAGE_SCN_MEM_EXECUTE                0x20000000  // Section is executable.
#define NAVEX_IMAGE_SCN_MEM_READ                   0x40000000  // Section is readable.
#define NAVEX_IMAGE_SCN_MEM_WRITE                  0x80000000  // Section is writeable.

// Directory Entries

#define NAVEX_IMAGE_DIRECTORY_ENTRY_EXPORT         0   // Export Directory
#define NAVEX_IMAGE_DIRECTORY_ENTRY_IMPORT         1   // Import Directory
#define NAVEX_IMAGE_DIRECTORY_ENTRY_RESOURCE       2   // Resource Directory
#define NAVEX_IMAGE_DIRECTORY_ENTRY_EXCEPTION      3   // Exception Directory
#define NAVEX_IMAGE_DIRECTORY_ENTRY_SECURITY       4   // Security Directory
#define NAVEX_IMAGE_DIRECTORY_ENTRY_BASERELOC      5   // Base Relocation Table
#define NAVEX_IMAGE_DIRECTORY_ENTRY_DEBUG          6   // Debug Directory
#define NAVEX_IMAGE_DIRECTORY_ENTRY_COPYRIGHT      7   // Description String
#define NAVEX_IMAGE_DIRECTORY_ENTRY_GLOBALPTR      8   // Machine Value (MIPS GP)
#define NAVEX_IMAGE_DIRECTORY_ENTRY_TLS            9   // TLS Directory
#define NAVEX_IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG   10   // Load Configuration Directory

#endif

