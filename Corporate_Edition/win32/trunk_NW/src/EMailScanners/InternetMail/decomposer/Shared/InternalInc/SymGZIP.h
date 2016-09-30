// SymGZIP.h : Functions exported from SymGZIP library.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(SYMGZIP_H)
#define SYMGZIP_H

/////////////////////////////////////////////////////////////////////////////
// GZIP Error and return codes - yes OK and ERROR are the opposite
// of boolean TRUE and FALSE, but that's the way it was done...
/////////////////////////////////////////////////////////////////////////////
#define GZIP_OK				0
#define GZIP_ERROR			1
#define GZIP_MAX_EXTRACT	0x10
#define GZIP_ABORT			0x11

/////////////////////////////////////////////////////////////////////////////
// Global Functions
/////////////////////////////////////////////////////////////////////////////
int GZipDecompress (char *iname, char *outputname);
int GZipCompress (char *iname, char *pszOrigName, char *outputname);
int GZipGetOriginalName (char *ifname, char *pszOrigName, int iSize);
void GZIP_SetMaxExtractSize(DWORD dwMaxSize);
void GZIP_AbortProcess(bool bAbort);


#endif	// SYMGZIP_H
