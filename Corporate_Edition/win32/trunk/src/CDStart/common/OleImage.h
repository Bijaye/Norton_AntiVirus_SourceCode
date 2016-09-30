// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------
//
// OLE Image Class
// Written by John Lenz, Oct. 27, 2005
//
// This is a lightweight OLE Image class which does not use or require MFC.
// Designed to be a drop in replacement for CDibImage. With the following 
// exception:  It requires images to be stored as "BINARY" resource type.
//
//  It addition to Bitmap files, it supports JPEG and GIF.
//
// ----------------------------------------------------------------------------

// #include <olectl.h>
// #include <comdef.h>

class COleImage
{
public:
	// constructor
	COleImage();

	// destructor
	~COleImage();

	// loads the bitmap from the specified resource id
	BOOL Load( UINT nResourceID );

	// loads the bitmap from the specified resource string
	BOOL Load( LPCTSTR szResourceName );

	// draw bitmap onto a device context
	BOOL Draw( HDC hDC, int X, int Y );

	// retrieves bitmap's width
	int GetWidth();

	// retrieves bitmap's height
	int GetHeight();

	// retrieves the number of colors the bitmap supports
	// int GetColorCount();

	// retrieves the bitmap's palette
	HPALETTE GetPalette();

private:
    IPicture *LoadPicture(HRSRC res);

    HANDLE m_hRes;
    IPicture * m_pPicture; // pointer to the picture
    long m_lWidth; // picture witdh (in pixels)
    long m_lHeight; // picture height (in pixels)
    OLE_XSIZE_HIMETRIC m_cx; // Amount which will be used for copy horizontally in source picture
    OLE_YSIZE_HIMETRIC m_cy; // Amount which will be used for copy vertically in source picture
};
