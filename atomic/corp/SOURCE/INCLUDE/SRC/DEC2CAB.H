// Dec2Cab.h : Decomposer 2 Engine for Cab files
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2CAB_H)
#define DEC2CAB_H

#if defined(_WINDOWS)
#pragma pack(push, 1)
#endif

#define SFX_CAB_TYPE_NONE           0
#define SFX_CAB_TYPE_KM             1
#define SFX_CAB_TYPE_CAB_MANAGER    2

typedef struct tagKMSFXCABHEADER
{
    BYTE    bSigByte1;      // Should be 'K'
    BYTE    bSigByte2;      // Should be 'M'
    DWORD   dwDataOffset;
    DWORD   dwUnknown1;
    DWORD   dwDataSize;
    DWORD   dwUnknown2;
    BYTE    bUnknown3;
#if defined(UNIX)
} __attribute__ ((packed)) KMSFXCABHEADER, *PKMSFXCABHEADER;
#else
} KMSFXCABHEADER, *PKMSFXCABHEADER;
#endif


typedef struct tagMSCFHEADER
{
    BYTE    bSigByte1;      // Should be 'M'
    BYTE    bSigByte2;      // Should be 'S'
    BYTE    bSigByte3;      // Should be 'C'
    BYTE    bSigByte4;      // Should be 'F'
    DWORD   dwReserved1;
    DWORD   dwCABSize;
    DWORD   dwReserved2;
    DWORD   dwFileOffset;
    DWORD   dwReserved3;
    BYTE    bMinorVersion;
    BYTE    bMajorVersion;
    WORD    wFolders;
    WORD    wFiles;
    WORD    wFlags;
    WORD    wSetID;
    WORD    wCABNumber;
    DWORD   dwPerCABReservedBytes;        // Note sure if DWORD size is right...
    DWORD   dwPerFolderReservedBytes;     // Is this right?
    DWORD   dwPerDatablockReservedBytes;  // Is this right?
#if defined(UNIX)
} __attribute__ ((packed)) MSCFHEADER, *PMSCFHEADER;
#else
} MSCFHEADER, *PMSCFHEADER;
#endif

#if defined(_WINDOWS)
#pragma pack(pop)
#endif


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
#ifdef DEC2CAB_CPP
#define DEC2CAB_IE __declspec(dllexport) 
#else
#define DEC2CAB_IE __declspec(dllimport) 
#endif

DEC2CAB_IE HRESULT DecNewCabEngine(IDecEngine** ppEngine);
DEC2CAB_IE HRESULT DecDeleteCabEngine(IDecEngine* pEngine);
#endif // _WINDOWS

#if defined(UNIX)
EXTERN_C HRESULT DecNewCabEngine(IDecEngine** ppEngine);
EXTERN_C HRESULT DecDeleteCabEngine(IDecEngine* pEngine);
#endif

#endif  // DEC2CAB_H
