//************************************************************************
//
// $Header:   S:/NAVEX/VCS/javashr.h_v   1.2   21 Nov 1996 17:12:44   AOONWAL  $
//
// Description:
//      Header for Java scanner/repair shared functions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/javashr.h_v  $
// 
//    Rev 1.2   21 Nov 1996 17:12:44   AOONWAL
// No change.
// 
//    Rev 1.1   29 Oct 1996 13:00:06   AOONWAL
// No change.
// 
//    Rev 1.0   04 Apr 1996 14:54:28   DCHI
// Initial revision.
//
//************************************************************************

#ifndef _JAVASHRH_

#define _JAVASHRH_

#include "platform.h"
#include "callback.h"
#include "ctsn.h"
#include "navex.h"
#include "navexshr.h"

// constant types

#define CONSTANT_Class              7
#define CONSTANT_Fieldref           9
#define CONSTANT_Methodref          10
#define CONSTANT_InterfaceMethodref 11
#define CONSTANT_String             8
#define CONSTANT_Integer            3
#define CONSTANT_Float              4
#define CONSTANT_Long               5
#define CONSTANT_Double             6
#define CONSTANT_NameAndType        12
#define CONSTANT_Utf8               1
#define CONSTANT_Unicode            2

typedef struct
{
    WORD wNameIndex;
} CONSTANT_Class_T;

typedef struct
{
    WORD wClassIndex;
    WORD wNameTypeIndex;
} CONSTANT_Fieldref_T, CONSTANT_Methodref_T, CONSTANT_InterfaceMethodref_T;

typedef struct
{
    WORD wStringIndex;
} CONSTANT_String_T;

typedef struct
{
    DWORD dwInteger;
} CONSTANT_Integer_T;

typedef struct
{
    float fFloat;
} CONSTANT_Float_T;

typedef struct
{
    DWORD   dwHiDWORD;
    DWORD   dwLoDWORD;
} CONSTANT_Long_T;

typedef struct
{
    double  dDouble;
} CONSTANT_Double_T;

typedef struct
{
    WORD    wNameIndex;
    WORD    wSignatureIndex;
} CONSTANT_NameAndType_T;

typedef struct
{
    WORD    wLen;
    BYTE *  pBytes;
} CONSTANT_Utf8_T, CONSTANT_Unicode_T;

typedef union
{
    CONSTANT_Class_T                cClass;
    CONSTANT_Fieldref_T             cFieldref;
    CONSTANT_Methodref_T            cMethodref;
    CONSTANT_InterfaceMethodref_T   cInterfaceMethodref;
    CONSTANT_String_T               cString;
    CONSTANT_Integer_T              cInteger;
    CONSTANT_Float_T                cFloat;
    CONSTANT_Long_T                 cLong;
    CONSTANT_Double_T               cDouble;
    CONSTANT_NameAndType_T          cNameAndType;
    CONSTANT_Utf8_T                 cUtf8;
    CONSTANT_Unicode_T              cUnicode;
} CONSTANT_T;

WORD    WordConvert(WORD wIn);
DWORD   DWordConvert(DWORD dwIn);


#endif
