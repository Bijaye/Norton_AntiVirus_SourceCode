// CTypeID.h : Generic data-type identifier class and definitions
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

class CTypeID2
{
public:
	int		DetectDataType(unsigned char *pBuffer, size_t size, DWORD *pdwTypes, size_t nTypes);
};
