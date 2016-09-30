// Dec2PDF.h : Decomposer 2 Engine for PDF files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2PDF_H)
#define DEC2PDF_H

/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2PDF_CPP
	#define DECPDFLINKAGE  __declspec(dllexport) 
  #else
	#define DECPDFLINKAGE  __declspec(dllimport) 
  #endif
#else
  #define DECPDFLINKAGE EXTERN_C
#endif

DECPDFLINKAGE DECRESULT DecNewPDFEngine(IDecEngine** ppEngine);
DECPDFLINKAGE DECRESULT DecDeletePDFEngine(IDecEngine* pEngine);

#endif	// DEC2PDF_H
