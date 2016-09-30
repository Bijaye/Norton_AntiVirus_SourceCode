// MBOXFile.h : Decomposer 2 Archive class for MBOX files.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2002, 2005 Symantec, Peter Norton Product Group. All rights reserved.
//************************************************************************

#if !defined(MBOXFILE_H)
#define MBOXFILE_H

#define MBOX_OUTPUT_BUFFER_SIZE	32 * 1024
#define MBOX_ERROR_EOF			EOF
#define MBOX_SUCCESS			0
#define MBOX_ERROR_GENERIC		1 
#define MBOX_ERROR_MAX_EXTRACT	2
#define MBOX_ERROR_ABORT		3

#define MAX_FROM_LEN			256


// This structure describes the data stored for each mbox entry found.
typedef struct tagMBOXENTRY
{
	char			szFrom[MAX_FROM_LEN + 1];		// From address
	size_t			nStartOffset;
	size_t			nEndOffset;
	tagMBOXENTRY	*pNext;
} MBOXENTRY, *PMBOXENTRY;


// Decomposer-specific control functions.
void MBOXSetMaxExtractSize(DWORD dwMaxSize);
void MBOXAbortProcess(bool bAbort);


/////////////////////////////////////////////////////////////////////////
// CMBOXFile

class CMBOXFile
{
public:
	CMBOXFile(bool bEnforceCRC);
	~CMBOXFile();

	DECRESULT Open(FILE *fpin);
	DECRESULT Close(char *pszOutputFile);
	DECRESULT AddFoundEntry(size_t nStartOffset, size_t nEndOffset);
	DECRESULT GetEntryName(size_t nEntry, char *pszName, WORD wNameSize);
	DECRESULT SetEntryName(size_t nEntry, char *pszName);
	DECRESULT GetEntryOffsets(size_t nEntry, size_t *pnStartOffset, size_t *pnEndOffset);
	DECRESULT GetEntryUncompressedSize(size_t nEntry, DWORDLONG *dwlSize);
	DECRESULT ExtractEntry(size_t nEntry, const char *pszFile);
	DECRESULT DeleteEntry(size_t nEntry);
	DECRESULT ReplaceEntry(size_t nEntry, const char *pszNewDataFile, const char *pszNewEntryName);

private:
	int ExtractData(size_t nEntry, const char *pszFile); // Extract an entry to the given file
	size_t PrefixCRtoLF( unsigned char* szInput, size_t nBytesRead, bool& rfPrefixed );
	size_t AddCarriageReturns( unsigned char* szInput, const size_t nBytesRead, unsigned char* szOutput, bool& rfPrefixed );    

	FILE	*m_pInputFile;					// Input file
	MBOXENTRY *m_pMBOXHead;					// Linked-list of mbox entries
	size_t	m_nCurrentEntry;
};

#endif	// MBOXFILE_H

