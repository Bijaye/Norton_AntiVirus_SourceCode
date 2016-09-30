// HTTPLexer.cpp : Lexer for the HTTP engine
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"
#include "HTTP.h"

CHTTPLexer::CHTTPLexer()
{
	// We need to construct and attach the CHTTPParser object here...
	m_bWantBytes = true;
}


CHTTPLexer::~CHTTPLexer()
{
	// Destroy the CHTTPParser object here...
}


bool CHTTPLexer::Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer, IDecContainerObjectEx *pObject)
{
	m_bWantBytes = true;
	return true;
}


int CHTTPLexer::GetInterface(void **ppInterface)
{
	if (ppInterface)
	{
		*ppInterface = NULL;
		return (LEXER_OK);
	}

	return (LEXER_ERROR);
}


int CHTTPLexer::ConsumeByte(unsigned char uchByte)
{
	return (LEXER_DONE);
}


int CHTTPLexer::ConsumeBuffer(unsigned char *pBuffer, size_t *pnBufferSize, int *piBufferIndex, size_t *pnOffset)
{
	return (LEXER_DONE);
}


int CHTTPLexer::ConsumeEOF(size_t nOffset)
{
	// Emit an EOF token.
	return (LEXER_DONE);
}


int CHTTPLexer::LexComplete(void)
{
	return (LEXER_OK);
}


int CHTTPLexer::ProcessComplete(void)
{
	return (LEXER_OK);
}


int CHTTPLexer::FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData)
{
	return (LEXER_OK);
//    return (m_pParser->FindFirstEntry(fpin, pData));
}


int CHTTPLexer::FindNextEntry(TEXT_FIND_DATA *pData)
{
	return (LEXER_OK);
//    return (m_pParser->FindNextEntry(pData));
}


int CHTTPLexer::SkipExtractEntry(TEXT_FIND_DATA *pData)
{
	// Not used by this engine.
	return LEXER_OK;
}


int CHTTPLexer::ExtractEntry(char *pszFilename, FILE *fpin)
{
	return (LEXER_OK);
//    return (m_pParser->ExtractEntry(pszFilename, fpin));
}


int CHTTPLexer::ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile)
{
	return (LEXER_OK);
//    return (m_pParser->ReplaceEntry(pData));
}


int CHTTPLexer::AddEntry(TEXT_CHANGE_DATA *pData)
{
	return (LEXER_OK);
//    return (m_pParser->AddEntry(pData));
}


int CHTTPLexer::DeleteEntry(TEXT_CHANGE_DATA *pData)
{
	return (LEXER_OK);
//    return (m_pParser->DeleteEntry(pData));
}
