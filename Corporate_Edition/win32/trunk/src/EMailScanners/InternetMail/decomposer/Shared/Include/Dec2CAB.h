// Dec2CAB.h : Decomposer Engine for CAB files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2CAB_H)
#define DEC2CAB_H

#if defined(_WINDOWS)
#pragma pack(push, 1)
#elif defined(IBMCXX)
#pragma option align=packed
#endif

#define SFX_CAB_TYPE_NONE			0
#define SFX_CAB_TYPE_KM				1
#define SFX_CAB_TYPE_CAB_MANAGER	2

typedef struct tagKMSFXCABHEADER
{
	BYTE	bSigByte1;		// Should be 'K'
	BYTE	bSigByte2;		// Should be 'M'
	DWORD	dwDataOffset;
	DWORD	dwUnknown1;
	DWORD	dwDataSize;
	DWORD	dwUnknown2;
	BYTE	bUnknown3;
#if defined(__GNUC__)
} __attribute__ ((packed)) KMSFXCABHEADER, *PKMSFXCABHEADER;
#else
} KMSFXCABHEADER, *PKMSFXCABHEADER;
#endif


typedef struct tagMSCFHEADER
{
	BYTE	bSigByte1;		// Should be 'M'
	BYTE	bSigByte2;		// Should be 'S'
	BYTE	bSigByte3;		// Should be 'C'
	BYTE	bSigByte4;		// Should be 'F'
	DWORD	dwReserved1;
	DWORD	dwCABSize;
	DWORD	dwReserved2;
	DWORD	dwFileOffset;
	DWORD	dwReserved3;
	BYTE	bMinorVersion;
	BYTE	bMajorVersion;
	WORD	wFolders;
	WORD	wFiles;
	WORD	wFlags;
	WORD	wSetID;
	WORD	wCABNumber;
	DWORD	dwPerCABReservedBytes;		 // Note sure if DWORD size is right...
	DWORD	dwPerFolderReservedBytes;	 // Is this right?
	DWORD	dwPerDatablockReservedBytes;  // Is this right?
#if defined(__GNUC__)
} __attribute__ ((packed)) MSCFHEADER, *PMSCFHEADER;
#else
} MSCFHEADER, *PMSCFHEADER;
#endif

#if defined(_WINDOWS)
#pragma pack(pop)
#elif defined(IBMCXX)
#pragma option align=reset
#endif


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2CAB_CPP
	#define DECCABLINKAGE __declspec(dllexport) 
  #else
	#define DECCABLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECCABLINKAGE EXTERN_C
#endif

DECCABLINKAGE DECRESULT DecNewCabEngine(IDecEngine **ppEngine);
DECCABLINKAGE DECRESULT DecDeleteCabEngine(IDecEngine *pEngine);

#endif	// DEC2CAB_H
