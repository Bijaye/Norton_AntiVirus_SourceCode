//********************************************************************
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2000, 2005 Symantec Corporation. All rights reserved.
//********************************************************************
//
// File:        visioapi.h
//
// Description: Prototypes Visio API functions.
//
// Notes:       #include "olessapi.h"
//
//********************************************************************

#ifndef _VISIOAPI_H_

#define _VISIOAPI_H_

LPVOID VisioVBADecompressInit
(
	LPSS_STREAM		lpstStream
);

bool VisioVBADecompressDeInit
(
	LPVOID			lpvVBADec
);

bool VisioVBADecompressToFile
(
	LPVOID			lpvVBADec,
	LPVOID			lpvFile
);

bool VisioFindVBAStorageChunk
(
	LPSS_STREAM			lpstStream,
	LPDWORD				lpdwOffset
);

bool VisioExtractVBAtoFile
(
	LPSS_STREAM			lpstStream,
	DWORD				dwOffset,
	LPVOID				lpvFile
);

#endif // #ifndef _VISIOAPI_H_

