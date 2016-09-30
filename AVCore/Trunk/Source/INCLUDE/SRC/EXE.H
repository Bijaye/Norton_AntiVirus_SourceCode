// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:\include\vcs\exe.h_v   1.0   02 Sep 1994 17:24:34   CAREY  $
//
// Description: Extended EXE header definitions...
//
// Contains: NE, PE, LX, LE data structures
//
// See Also:
//************************************************************************
// $Log:   S:\include\vcs\exe.h_v  $
// 
//    Rev 1.0   02 Sep 1994 17:24:34   CAREY
// Initial revision.
//************************************************************************

///////////////////////////////////////////////////////////////////////////////
// all structures have their first 2 bytes commented out since this is read
// separately to determine the file type...
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// NE structure...
///////////////////////////////////////////////////////////////////////////////

typedef struct tagWININFO {
//    WORD   signature;
    BYTE   linkerVersion;
    BYTE   linkerRevision;
    WORD   entryTabOffset;
    WORD   entryTabLen;
    DWORD  reserved1;
    WORD   exeFlags;
    WORD   dataSegNum;
    WORD   localHeapSize;
    WORD   stackSize;
    DWORD  cs_ip;
    DWORD  ss_sp;
    WORD   segTabEntries;
    WORD   modTabEntries;
    WORD   nonResTabSize;
    WORD   segTabOffset;
    WORD   resTabOffset;
    WORD   resNameTabOffset;
    WORD   modTabOffset;
    WORD   impTabOffset;
    WORD   nonResTabOffset;
    WORD   reserved2;
    WORD   numEntryPoints;
    WORD   shiftCount;
    WORD   numResourceSegs;
    BYTE   targetOS;
    BYTE   miscFlags;
    WORD   fastLoadOffset;
    WORD   fastLoadSize;
    WORD   reserved3;
    BYTE   winRevision;
    BYTE   winVersion;
} WININFO;

typedef struct tagTBSEGMENT {
    WORD  segDataOffset;
    WORD  segLen;
    WORD  segFlags;
    WORD  segMinSize;
} TBSEGMENT;

#define NE_SEG_FLAG_DATA_SEG           0x0001

///////////////////////////////////////////////////////////////////////////////
// PE structure
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
//        WORD        signature;
        WORD    zeroFilled;
        WORD    Machine;
        WORD    NumberOfSections;
        DWORD   TimeDateStamp;
        DWORD   PointerToSymbolTable;
        DWORD   NumberOfSymbols;
        WORD    SizeOfOptionalHeader;
        WORD    Characteristics;
} PEMainHeader;


typedef struct {
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
} PEOptionalHeader;

#define IMAGE_SIZEOF_SHORT_NAME              8

typedef struct {
    BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
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
} PE_IMAGE_SECTION_HEADER;

#define PE_SEG_FLAG_CODE_SEG        (0x00000020L | 0x20000000L)


///////////////////////////////////////////////////////////////////////////////
// LX structures
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
//    WORD        signature;        // 0
    BYTE        ByteOrder;          // 2
    BYTE        WordOrder;          // 3
    DWORD       FormatLevel;        // 4
    WORD        CpuType;            // 8
    WORD        OSType;             // 10
    DWORD       ModVersion;         // 12
    DWORD       ModFlags;           // 16
    DWORD       ModNumPgs;          // 20
    DWORD       EIPObjNum;          // 24
    DWORD       EIP;                // 28
    DWORD       ESPObjNum;          // 32
    DWORD       Esp;                // 36
    DWORD       PgSize;             // 40
    DWORD       PgOfsShift;         // 44
    DWORD       FixupSectionSize;   // 48
    DWORD       FixupCksum;         // 52
    DWORD       LdrSecSize;         // 56
    DWORD       LdrSecCksum;        // 60
    DWORD       ObjTblOfs;          // 64
    DWORD       NumObjects;         // 68
    DWORD       ObjPgTblOfs;        // 72
    DWORD       ObjIterPgsOfs;      // 76
    DWORD       RcsTblOfs;          // 80
    DWORD       NumRcsTblEnt;       // 84
    DWORD       ResNameTblOfs;      // 88
    DWORD       EntryTblOfs;        // 92
    DWORD       ModDirOfs;          // 96
    DWORD       NumModDirs;         // 100
    DWORD       FixupPgTblOfs;      //
    DWORD       FixupRecTblOfs;
    DWORD       ImpModTblOfs;
    DWORD       NumImpModEnt;
    DWORD       ImpProcTblOfs;
    DWORD       PerPgCksumOfs;
    DWORD       DataPgOfs;
    DWORD       NumPreloadPg;
    DWORD       NResNameTblOfs;
    DWORD       NResNameTblLen;
    DWORD       NResNameTblCksum;
    DWORD       AutoDSObj;
    DWORD       DebugInfoOfs;
    DWORD       DebugInfoLen;
    DWORD       NumInstPreload;
    DWORD       NumInstDemand;
    DWORD       HeapSize;
    DWORD       StackSize;
} LX_EXE;

typedef struct
{
    DWORD       size;
    DWORD       reloc_base_addr;
    DWORD       obj_flags;
    DWORD       pg_tbl_index;
    DWORD       num_pg_tbl_entries;
    DWORD       reserved;
} LX_OBJ;


typedef struct
{
    DWORD       offset;
    WORD        size;
    WORD        flags;
} LX_PG;

#define LX_SEG_FLAG_CODE_SEG        0x00000004L




///////////////////////////////////////////////////////////////////////////////
// LE structures
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
//    WORD        signature;        // 0
    BYTE        ByteOrder;          // 2
    BYTE        WordOrder;          // 3
    DWORD       FormatLevel;        // 4
    WORD        CpuType;            // 8
    WORD        OSType;             // 10
    DWORD       ModVersion;         // 12
    DWORD       ModFlags;           // 16
    DWORD       ModNumPgs;          // 20
    DWORD       EIPObjNum;          // 24
    DWORD       EIP;                // 28
    DWORD       ESPObjNum;          // 32
    DWORD       Esp;                // 36
    DWORD       PgSize;             // 40
    DWORD       BytesOnLastPage;    // 44
    DWORD       FixupSectionSize;   // 48
    DWORD       FixupCksum;         // 52
    DWORD       LdrSecSize;         // 56
    DWORD       LdrSecCksum;        // 60
    DWORD       ObjTblOfs;          // 64
    DWORD       NumObjects;         // 68
    DWORD       ObjPgTblOfs;        // 72
    DWORD       ObjIterPgsOfs;      // 76
    DWORD       RcsTblOfs;          // 80
    DWORD       NumRcsTblEnt;       // 84
    DWORD       ResNameTblOfs;      // 88
    DWORD       EntryTblOfs;        // 92
    DWORD       ModDirOfs;          // 96
    DWORD       NumModDirs;         // 100
    DWORD       FixupPgTblOfs;      //
    DWORD       FixupRecTblOfs;
    DWORD       ImpModTblOfs;
    DWORD       NumImpModEnt;
    DWORD       ImpProcTblOfs;
    DWORD       PerPgCksumOfs;
    DWORD       DataPgOfs;
    DWORD       NumPreloadPg;
    DWORD       NResNameTblOfs;
    DWORD       NResNameTblLen;
    DWORD       NResNameTblCksum;
    DWORD       AutoDSObj;
    DWORD       DebugInfoOfs;
    DWORD       DebugInfoLen;
    DWORD       NumInstPreload;
    DWORD       NumInstDemand;
    DWORD       HeapSize;
    DWORD       StackSize;
} LE_EXE;

typedef struct
{
    DWORD       size;                   // valid size
    DWORD       reloc_base_addr;
    DWORD       obj_flags;
    DWORD       pg_tbl_index;           // (this - 1) * PgSize + DataPgOfs -> object
    DWORD       num_pg_tbl_entries;
    DWORD       reserved;
} LE_OBJ;

#define LE_SEG_FLAG_CODE_SEG        0x00000004L
