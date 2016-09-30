// UUEParser.cpp : Parser for the UUE engine
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"
#include "UUE.h"
#include "dec_assert.h"

#include "asc_char.h"
#include "asc_ctype.h"
#include "bstricmp.h"

extern unsigned char g_uulookup[256];

#ifdef _DEBUG
char *g_pszUUETokenID[UUE_TOKEN_LAST + 1] =
{
	"LWSP",
	"NEWLINE",
	"BEGIN",
	"END",
	"TABLE",
	"ATOM",
	"EOF",
	"END_MARKER",
	"LAST",
};

#define DUMP_EMIT(et) \
	printf("  iTokenID:     %d  %s\n", et.iTokenID, g_pszUUETokenID[et.iTokenID]); \
	printf("  nTokenOffset: %d\n", et.nTokenOffset); \
	printf("  dwFlags:      0x%08x\n", et.dwFlags)
#endif


CUUEParser::CUUEParser()
{
	m_iTokenID = 0;
	m_pString = NULL;
	m_nStringLength = 0;
	m_nStartOffset = 0;

// Variables used for decoding
	m_bNewLine = false;
	m_nEncodedLineLength = 0;
	m_byQuad[0] = 0;
	m_byTri[0] = 0;
	m_uulookup[0] = 0;
	m_nQuadIndex = 0;
	m_dwMode = 0;

	m_pDecomposer = NULL;
	m_pEngine = NULL;
	m_pArchive = NULL;
    m_ParseState = UUE_PARSER_STATE_HEADERLINE2;
	m_bIdentified = false;
	m_nTableIndex = 0;
	m_nLineNumber = 0;
	m_nSectionStart = 0;

// From UUEFile.cpp:
	m_pFile = NULL;
	m_bChanged = false;
	m_nOffset = 0;
	m_nHeaderOffset = 0;
	m_nEndOffset = 0;
	m_nTopUnchangedOffset = 0;
	m_nBottomUnchangedOffset = 0;
	m_nTerminator = TERMINATOR_NONE;
	m_szOutFile[0] = 0;
	m_szFilename[0] = 0;
	m_fValidateUUEBody = true;
	m_fNumValidUUELines = 0;
	m_fStrongHeader = false;

	//
	// Option defaults
	//
	m_dwTextNonUUEThreshold = 4096;

}


CUUEParser::~CUUEParser()
{
// From UUEFile.cpp:
	if (m_pFile)
		Close();
}


bool CUUEParser::Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer)
{
	m_bIdentified = false;
	m_nTableIndex = 0;
	m_pArchive = pArchive;
	m_pEngine = pEngine;
	m_pDecomposer = pDecomposer;
	m_nLineNumber = 0;
	m_nSectionStart = 0;
	m_nTerminator = TERMINATOR_NONE;
	m_fValidateUUEBody = true;
	m_fNumValidUUELines = 0;
	m_fStrongHeader = false;

	DWORD dwValue = 0;

	//
	// Set any applicable UUE options here
	//
	if (SUCCEEDED(m_pDecomposer->GetOption(DEC_OPTION_NONUUETHRESHOLD_BYTES, &dwValue)))
		m_dwTextNonUUEThreshold = dwValue;

	// Copy the default decoding table.
	for (int i = 0; i < sizeof(g_uulookup); i++)
		m_uulookup[i] = g_uulookup[i];

	return true;
}


int CUUEParser::ConsumeToken(
	int iTokenID,
	char *pString,
	size_t nStringLength,
	size_t nOffset)
{
	int		rc = PARSER_OK;	// Assume parsing needs to continue

	// Copy the input parameters into the parser object.  These parameters
	// now become the things that drive the state machine.
	m_iTokenID = iTokenID;
	m_pString = pString;
	m_nStringLength = nStringLength;
	m_nOffset = nOffset;

	if (iTokenID == UUE_TOKEN_NEWLINE)
	{
		// Increment the line number.
		m_nLineNumber++;
	}

	// Where we go next depends on what state the parser is in right now.
	// The parsing always begins in state UUE_PARSER_STATE_HEADERFIELD1.
	// Call the function associated with the current parse state.
	switch (m_ParseState)
	{
		case UUE_PARSER_STATE_HEADERLINE1:
            // We are looking newlines only
            rc = State_HEADERLINE1();
            break;

        case UUE_PARSER_STATE_HEADERLINE2:
            // We are looking for the beginning of a UUE header line.
            rc = State_HEADERLINE2();
			break;

		case UUE_PARSER_STATE_BEGIN1:
			rc = State_BEGIN1();
			break;

		case UUE_PARSER_STATE_BEGIN2:
			rc = State_BEGIN2();
			break;

		case UUE_PARSER_STATE_BEGIN3:
			rc = State_BEGIN3();
			break;
		
		case UUE_PARSER_STATE_BEGIN4:
			rc = State_BEGIN4();
			break;

		case UUE_PARSER_STATE_BODY1:
			rc = State_BODY1();
			break;

		case UUE_PARSER_STATE_END1:
			rc = State_END1();
			break;

		case UUE_PARSER_STATE_END2:
			rc = State_END2();
			break;

		case UUE_PARSER_STATE_TABLE1:
			rc = State_TABLE1();
			break;

		case UUE_PARSER_STATE_TABLE2:
			rc = State_TABLE2();
			break;

		case UUE_PARSER_STATE_TABLE3:
			rc = State_TABLE3();
			break;

		case UUE_PARSER_STATE_SECTION1:
			rc = State_SECTION1();
			break;

		case UUE_PARSER_STATE_SECTION2:
			rc = State_SECTION2();
			break;

		case UUE_PARSER_STATE_SUM1:
			rc = State_SUM1();
			break;

		case UUE_PARSER_STATE_SINGLESPACE1:
			rc = State_SINGLESPACE1();
			break;

		case UUE_PARSER_STATE_DOUBLESPACE:
			rc = State_DOUBLESPACE();
			break;

		case UUE_PARSER_STATE_OCTALNUMBER:
			rc = State_OCTALNUMBER();
			break;

		case UUE_PARSER_STATE_SINGLESPACE2:
			rc = State_SINGLESPACE2();
			break;

		case UUE_PARSER_STATE_FILENAME:
			rc = State_FILENAME();
			break;

		case UUE_PARSER_STATE_FORMATERROR:
			rc = State_FORMATERROR();
			break;

		default:
			dec_assert(0);	// We should never fall outside the state machine!
			break;
	}

	return (rc);
}


#include "HeaderLine.cpp"
#include "Begin.cpp"
#include "UUEBody.cpp"
#include "End.cpp"
#include "Section.cpp"
#include "Sum.cpp"
#include "Table.cpp"
