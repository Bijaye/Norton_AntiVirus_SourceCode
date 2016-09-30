// RTF.h : Header for primary RTF engine APIs
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

//
// Note: The AS400 build system performs its builds one directory
// level higher than the other platforms.  It is easier to change
// just the specific AS400 cases rather than change every workspace's
// include search path.
//
#if defined(OS400)
	#include "TextFile.h"
#else
	#include "../TextFile.h"
#endif


class CRTFLexer : public ILexer
{
public:
	CRTFLexer();
	~CRTFLexer();

	// ILexer methods:
	bool	Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer, IDecContainerObjectEx *pObject);
	int		GetInterface(void **ppInterface);
	int		ConsumeByte(unsigned char uchByte);
	int     ConsumeBuffer(unsigned char *pBuffer, size_t *pnBufferSize, int *piBufferIndex, size_t *pnOffset);
	int		ConsumeEOF(size_t nOffset);
	int		LexComplete(void);
	int		ProcessComplete(void);
	int		FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData);
	int		FindNextEntry(TEXT_FIND_DATA *pData);
	int		SkipExtractEntry(TEXT_FIND_DATA *pData);
	int		ExtractEntry(char *pszFilename, FILE *fpin);
	int		ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile);
	int		AddEntry(TEXT_CHANGE_DATA *pData);
	int		DeleteEntry(TEXT_CHANGE_DATA *pData);
};
