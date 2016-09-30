////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// DIB Image Class
// Written by Abu Wawda
//
// This is a lightweight DIB class which does not use or require MFC.
//
////////////////////////////////////////////////////////////////////////////////

class CDibImage
{
public:
	// constructor
	CDibImage();

	// destructor
	~CDibImage();

	// loads the bitmap from the specified resource id
	BOOL Load(UINT nResourceID, HMODULE hModule = NULL);

	// loads the bitmap from the specified resource string
	BOOL Load(LPCTSTR szResourceName, HMODULE hModule = NULL);

	// draw bitmap onto a device context
	BOOL Draw(HDC hDC, int X, int Y);

	// retrieves bitmap's width
	int GetWidth();

	// retrieves bitmap's height
	int GetHeight();

	// retrieves the number of colors the bitmap supports
	int GetColorCount();

	// retrieves the bitmap's palette
	HPALETTE GetPalette();

private:
	// pointer to the bitmap resource
	BYTE* m_pBitmap;

	// number of colors in this bitmap
	int m_nColors;

	// bitmap palette
	HPALETTE m_hPalette;

	// internal DDB
	HBITMAP m_hBitmap;
};

