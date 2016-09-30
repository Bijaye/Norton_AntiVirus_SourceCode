// Copyright 1999 Symantec, Peter Norton Group
//***************************************************************************
//
// Description:
//   This provides the structure definition for the Definition Attributes
//   stored in VirScan1.dat.
//
//***************************************************************************

#ifndef DEFATTRS_H_INCLUDED
#define DEFATTRS_H_INCLUDED

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined (SYM_VXD)
#include "pshpack1.h"
#else
#pragma pack(1)
#endif

// Provide the Virscan section number.
#define DEF_ATTR_VIRSCAN_SECTION    14

// Definition Attributes Structure. Only add fields to the end of the
// structure.
typedef struct
{
    DWORD       dwSequenceNumber;           /* Def Sequence #  */
} DEF_ATTRIBUTES_T, FAR *LPDEF_ATTRIBUTES;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma pack()
#endif

#endif // DEFATTRS_H_INCLUDED
