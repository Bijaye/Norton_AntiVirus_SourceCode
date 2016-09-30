// RTFLexer.cpp : Lexer for the RTF engine
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"
#include "RTF.h"

CRTFLexer::CRTFLexer()
{
	// We need to construct and attach the CRTFParser object here...
	m_bWantBytes = true;
}


CRTFLexer::~CRTFLexer()
{
	// Destroy the CRTFParser object here...
}


bool CRTFLexer::Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer, IDecContainerObjectEx *pObject)
{
	m_bWantBytes = true;
	return true;
}


int CRTFLexer::GetInterface(void **ppInterface)
{
	if (ppInterface)
	{
		*ppInterface = NULL;
		return (LEXER_OK);
	}

	return (LEXER_ERROR);
}


int CRTFLexer::ConsumeByte(unsigned char uchByte)
{
	return (LEXER_DONE);
}


int CRTFLexer::ConsumeBuffer(unsigned char *pBuffer, size_t *pnBufferSize, int *piBufferIndex, size_t *pnOffset)
{
	return (LEXER_DONE);
}


int CRTFLexer::ConsumeEOF(size_t nOffset)
{
	// Emit an EOF token.
	return (LEXER_DONE);
}


int CRTFLexer::LexComplete(void)
{
	return (LEXER_OK);
}


int CRTFLexer::ProcessComplete(void)
{
	return (LEXER_OK);
}


int CRTFLexer::FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData)
{
	return (LEXER_OK);
//    return (m_pParser->FindFirstEntry(fpin, pData));
}


int CRTFLexer::FindNextEntry(TEXT_FIND_DATA *pData)
{
	return (LEXER_OK);
//    return (m_pParser->FindNextEntry(pData));
}


int CRTFLexer::SkipExtractEntry(TEXT_FIND_DATA *pData)
{
	// Not used by this engine.
	return LEXER_OK;
}


int CRTFLexer::ExtractEntry(char *pszFilename, FILE *fpin)
{
	return (LEXER_OK);
//    return (m_pParser->ExtractEntry(pszFilename), fpin);
}


int CRTFLexer::ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile)
{
	return (LEXER_OK);
//    return (m_pParser->ReplaceEntry(pData));
}


int CRTFLexer::AddEntry(TEXT_CHANGE_DATA *pData)
{
	return (LEXER_OK);
//    return (m_pParser->AddEntry(pData));
}


int CRTFLexer::DeleteEntry(TEXT_CHANGE_DATA *pData)
{
	return (LEXER_OK);
//    return (m_pParser->DeleteEntry(pData));
}
