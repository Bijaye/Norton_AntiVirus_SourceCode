// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998, 2005 Symantec Corporation.. All rights reserved.
//*************************************************************************
// qstructs.h created 11/6/98 1:57:15 PM
//
// $Header$
//
// Description:
//
// Contains:
//
//*************************************************************************
// $Log$
//*************************************************************************

#ifndef _QSTRUCTS_H_
#define _QSTRUCTS_H_

// 
// For STL map and string templates.
// 
#pragma warning(disable:4786)       
#include <map>
#include <string>
using namespace std;

#if defined(__GNUC__)
#define PACKED  __attribute__ ((packed))
#else
#define PACKED 
#endif


//*************************************************************************
// 
// These are the definitions of the on disk structures for Quarantine
// Server items.
// 
//*************************************************************************

#if !defined(__GNUC__)
#pragma pack( push, 1 )
#endif



/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//
// Here is the layout of a quarantine server data file:
//
//                            -------------------------
//                            |      File header      |
//                            -------------------------
//                            |                       |
//                            |  Scrambled orignal    |
//                            |       File data       |
//                            |                       |
//                            -------------------------
//                            |    First data item    |
//                            -------------------------
//                                        .
//                                        .
//                                        .
//                            -------------------------
//                            |    Last  data item    |
//                            -------------------------
//
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
//
// On disk representation of data object.
// 
typedef struct tag_FILEDATADESCRIPTOR
    {
    char                szFieldName[ MAX_FIELD_NAME_LENGTH + 1] PACKED;
    ULONG               ulDataLength PACKED;
    ULONG               ulType PACKED;
    }
FILEDATADESCRIPTOR, FAR* LPFILEDATADESCRIPTOR;

/////////////////////////////////////////////////////////////////////////////////////////
// 
// File header.
// 
typedef struct tag_QSERVERFILEHEADER
    {
    ULONG               ulSignature PACKED;
    ULONG               ulVersion PACKED;
    ULONG               ulOriginalFileSize PACKED;
    ULONG               ulNumDataItems PACKED;
    }
QSERVERFILEHEADER, FAR* LPQSERVERFILEHEADER;

/////////////////////////////////////////////////////////////////////////////////////////
// 
// Date structure
// 

typedef struct tag_QSDATE
    {
    BYTE                byMonth PACKED;
    BYTE                byDay PACKED;
    WORD                wYear PACKED;
    BYTE                byHour PACKED;
    BYTE                byMinute PACKED;
    BYTE                bySecond PACKED;
    }
QSDATE, FAR* LPQSDATE;

/////////////////////////////////////////////////////////////////////////////////////////

#if !defined(__GNUC__)
#pragma pack( pop )
#endif


/////////////////////////////////////////////////////////////////////////////////////////
// 
// In memory representation of data object.
// 
typedef struct tag_DATAFIELD
    {
    char                szFieldName[ MAX_FIELD_NAME_LENGTH + 1];
    LPBYTE              pbyData;
    ULONG               ulDataLength;
    ULONG               ulType;
    }
DATAFIELD, FAR* LPDATAFIELD;

/////////////////////////////////////////////////////////////////////////////////////////
// 
// Map object for keeping list of data fields.
// 
typedef map< string, LPDATAFIELD, less< string > > MAP;

/////////////////////////////////////////////////////////////////////////////////////////
// 
// This is the opaque data item passed to users.
// 

typedef struct tag_QSERVERITEM {
    LPQSERVERFILEHEADER     pHeader;
    MAP                     dataMap;
    char                    szFileName[ MAX_PATH ];
    } 
QSERVERITEM, *LPQSERVERITEM;

//*************************************************************************
// 
// Macros
// 
//*************************************************************************

// 
// Header signature ( QSFI )
// 
#define QSERVER_FILE_SIGNATURE      (DWORD) 0x51534649

// 
// Header Version.
// 
#define QSERVER_FILE_VERSION        1

//*************************************************************************

#endif // #ifndef _QSTRUCTS_H_
