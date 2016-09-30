// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/heurds.h_v   1.0   14 May 1997 13:27:30   CNACHEN  $
//
// Description:
//      Heuristic compiler data structures
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/heurds.h_v  $
// 
//    Rev 1.0   14 May 1997 13:27:30   CNACHEN
// Initial revision.
// 
//************************************************************************


#ifndef _HEURDS_H
#define _HEURDS_H

#define CETYPE_VARIABLE                 1
#define CETYPE_DWORD_CONSTANT           2
#define CETYPE_LABEL                    3       // wID is index into symbol table (unresolved offset)
#define CETYPE_OFFSET                   4       // wID is offset into code stream
#define CETYPE_OPERATOR                 5
#define CETYPE_LITERAL                  6

typedef struct tag_CODE_ELEMENT
{
	BYTE                        byType;
	DWORD                       dwID;
	struct tag_CODE_ELEMENT *   lpstNext;
} CODE_ELEMENT_T, *LPCODE_ELEMENT;

#define STTYPE_VARIABLE                 1       // wValue is index into variable array
#define STTYPE_LABEL                    2       // wValue is starting offset of label
#define STTYPE_LABEL_REFERENCE          3       // wValue is invalid

typedef struct
{
    BYTE            byType;
    char            szName[32];
    WORD            wValue;
} SYMBOL_TABLE_ENTRY_T, *LPSYMBOL_TABLE_ENTRY;

#define MAX_SYMBOL_TABLE_ENTRIES        512

typedef struct
{
    SYMBOL_TABLE_ENTRY_T        astSymbols[MAX_SYMBOL_TABLE_ENTRIES];
    int                         nEntries;
    WORD                        wNextVariable;
    int                         nUnresolvedLabels;
} SYMBOL_TABLE_T, *LPSYMBOL_TABLE;

typedef struct
{
    LPCODE_ELEMENT      lpstRootElement;
    LPCODE_ELEMENT *    lplpstLastNextPtr;
    LPCODE_ELEMENT      lpstUpdateMe;
    WORD                wOffsetOfNextInstr;
} CODE_STREAM_T, *LPCODE_STREAM;

typedef struct
{
    SYMBOL_TABLE_T      stST;
    CODE_STREAM_T       stCS;
    FILE *              lpstCompiledStream;
    FILE *              lpstInputStream;
    DWORD               dwLineNum;
} COMPILER_INFO_T, *LPCOMPILER_INFO;

#endif
