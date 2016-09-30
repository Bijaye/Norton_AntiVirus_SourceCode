//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// qspak.h - created 11/6/98 1:57:15 PM
//

#ifndef _QSPAK_H_
#define _QSPAK_H_

//*************************************************************************

#ifdef WIN32

#ifdef BUILDING_QSPAK
#define DECLSPEC_QSPAK __declspec( dllexport )
#else
#define DECLSPEC_QSPAK __declspec( dllimport )
#endif

#else  // NLM or UNIX?

#define DECLSPEC_QSPAK

#endif

#if defined(UNIX)
#define FAR
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
#endif

#if defined(__GNUC__)
#define PACKED  __attribute__ ((packed))
#else
#define PACKED
#endif


//*************************************************************************
//
// Forward declarations
//
typedef struct tag_HQSERVERITEM     FAR * HQSERVERITEM;

//*************************************************************************
//
// Status definition.
//
typedef unsigned long QSPAKSTATUS;

#define QSPAKSTATUS_OK                  0
#define QSPAKSTATUS_ERROR               1
#define QSPAKSTATUS_FILE_NOT_FOUND      2
#define QSPAKSTATUS_MEMORY_ERROR        3
#define QSPAKSTATUS_INVALID_ARG         4
#define QSPAKSTATUS_BUFFER_TOO_SMALL    5
#define QSPAKSTATUS_OUT_OF_DISK_SPACE   6
#define QSPAKSTATUS_ACCESS_DENIED       7
#define QSPAKSTATUS_NO_SUCH_FIELD       8
#define QSPAKSTATUS_BOGUS_FILE          9
#define QSPAKSTATUS_INVALID_TYPE        10

//*************************************************************************
// 
// Value types
// 

typedef enum QSPAKDATATYPE {
	QSPAK_TYPE_BINARY = 0,
	QSPAK_TYPE_STRING,
	QSPAK_TYPE_DWORD,
	QSPAK_TYPE_DATE
}
QSPAKDATATYPE;

//*************************************************************************
// 
// Maximum length in BYTEs of a attribute field name.
// 
#define MAX_FIELD_NAME_LENGTH               64

//*************************************************************************
//
// Structures
//

//
// Date structure used for date attributes.
//
#if !defined(__GNUC__)
#pragma pack(push)
#pragma pack(1)
#endif

typedef struct tag_QSPAKDATE
{
	BYTE byMonth PACKED;
	BYTE byDay PACKED;
	WORD wYear PACKED;
	BYTE byHour PACKED;
	BYTE byMinute PACKED;
	BYTE bySecond PACKED;
}
QSPAKDATE, FAR* LPQSPAKDATE;

#if !defined(__GNUC__)
#pragma pack(pop)
#endif


// 
// Data structure defining linked list of FIELDINFO structures.  Returned by
// QsPakGetItemData().
// 
typedef struct tag_FIELDINFO
{

	char szFieldName[ MAX_FIELD_NAME_LENGTH ]; // Name of this field
	QSPAKDATATYPE eType; // Field type
	unsigned int uDataLength; // Data length
}
FIELDINFO, FAR * LPFIELDINFO;


//*************************************************************************
// 
// Prototypes.
// 

#ifdef __cplusplus
extern "C" {
#endif

QSPAKSTATUS DECLSPEC_QSPAK QsPakOpenItem(
                          const char * pszFile,
                          HQSERVERITEM* phItem
                          );

QSPAKSTATUS DECLSPEC_QSPAK QsPakCreateItemFromFile(
                          HQSERVERITEM* phItem,
                          const char* pszInfectedFileName,
                          const char* pszDestFileName );

QSPAKSTATUS DECLSPEC_QSPAK QsPakCreateItemFromBuffer(
                          HQSERVERITEM* phItem,
                          unsigned char* lpDataBuffer,
                          DWORD dwDataBufferSize,
                          const char* pszDestFileName );

QSPAKSTATUS DECLSPEC_QSPAK QsPakSaveItem( HQSERVERITEM hItem );

QSPAKSTATUS DECLSPEC_QSPAK QsPakReleaseItem( HQSERVERITEM hItem );

QSPAKSTATUS DECLSPEC_QSPAK QsPakUnpackageMainFile(HQSERVERITEM hItem, 
												const char * pszDestFileName);

QSPAKSTATUS DECLSPEC_QSPAK QsPakQueryItemValue( HQSERVERITEM hItem,
                            const char * szValue,
                            QSPAKDATATYPE eType,
                            void * pBuffer,
                            unsigned long * pulBufferLength );

QSPAKSTATUS DECLSPEC_QSPAK QsPakSetItemValue( HQSERVERITEM hItem,
                          const char * szValue,
                          QSPAKDATATYPE eType,
                          void * pData,
                          unsigned long ulDataLength );

QSPAKSTATUS DECLSPEC_QSPAK QsPakIsQserverFile( const char * pszFileName );

QSPAKSTATUS DECLSPEC_QSPAK QsPakIsQserverHeaderBuffer( const unsigned char * pBuffer,
                           unsigned int uBufferSize );

QSPAKSTATUS DECLSPEC_QSPAK QsPakGetRawFileHere( HQSERVERITEM hItem,
                                                unsigned char * pBuffer,
                                                unsigned long * pulBufferLength );

QSPAKSTATUS DECLSPEC_QSPAK QsPakGetItemFieldInfo( HQSERVERITEM hItem,
                                                  unsigned int * puNumFields,
                                                  LPFIELDINFO * pFieldInfo );

QSPAKSTATUS DECLSPEC_QSPAK QsPakFreeItemFieldInfo( LPFIELDINFO pFieldInfo );

QSPAKSTATUS DECLSPEC_QSPAK QsPakDeleteItemField(HQSERVERITEM hItem, const char * szValue);

#ifdef __cplusplus
};
#endif

//*************************************************************************

#endif // #ifndef _QSPAK_H_

