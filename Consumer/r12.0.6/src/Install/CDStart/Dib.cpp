
////////////////////////////////////////////////////////////////////////////////
//
// DIB Image Class
// Written by Abu Wawda
//
// This is a lightweight DIB class which does not use or require MFC.
//
// Revision 1.1		Speed up the bitmap draw by converting the DIB to a DDB
//					during Load() and then using BitBlt to pop it on the screen.
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Dib.h"

CDibImage::CDibImage()
{
	m_nColors = 0;
	m_hPalette = NULL;
	m_pBitmap = NULL;
	m_hBitmap = NULL;
}

CDibImage::~CDibImage()
{
	if (m_hBitmap != NULL)
		DeleteObject(m_hBitmap);

	if (m_hPalette != NULL)
		DeleteObject(m_hPalette);
}

BOOL CDibImage::Load(UINT nResourceID, HMODULE hModule)
{
	return Load(MAKEINTRESOURCE(nResourceID), hModule);
}

BOOL CDibImage::Load(LPCTSTR szResourceName, HMODULE hModule)
{
	// Find the resource and save a pointer to it

	HANDLE hResource = FindResource(hModule, szResourceName, RT_BITMAP);

	if (hResource == NULL)
		return FALSE;

	m_pBitmap = (BYTE*) LoadResource(hModule, (HRSRC)hResource);

	if (m_pBitmap == NULL)
		return FALSE;

	// Figure out the number of colors in this bitmap

	BITMAPINFO* pBitmapInfo = (BITMAPINFO*) m_pBitmap;

	if (pBitmapInfo->bmiHeader.biClrUsed != 0)
		m_nColors = pBitmapInfo->bmiHeader.biClrUsed;
	else
		m_nColors = 1 << pBitmapInfo->bmiHeader.biBitCount;

	// Now create a logical palette from the bitmap and save it

	if (m_nColors <= 256)
	{
		DWORD dwSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * m_nColors);

		LOGPALETTE* pLogPalette = (LOGPALETTE*) new BYTE[dwSize];

		if (pLogPalette == NULL)
			return FALSE;

		pLogPalette->palVersion = 0x300;
		pLogPalette->palNumEntries = m_nColors;

		for(int nIndex=0; nIndex<m_nColors; nIndex++)
		{
			pLogPalette->palPalEntry[nIndex].peRed =
				pBitmapInfo->bmiColors[nIndex].rgbRed;
			pLogPalette->palPalEntry[nIndex].peGreen =
				pBitmapInfo->bmiColors[nIndex].rgbGreen;
			pLogPalette->palPalEntry[nIndex].peBlue =
				pBitmapInfo->bmiColors[nIndex].rgbBlue;
			pLogPalette->palPalEntry[nIndex].peFlags = 0;
		}

		m_hPalette = CreatePalette(pLogPalette);

		delete [] pLogPalette;

		if (m_hPalette == NULL)
			return FALSE;
	}

	// Convert the DIB to a DDB

	HDC hDC = GetDC(NULL);
	
	SelectPalette(hDC, m_hPalette, FALSE);
	RealizePalette(hDC);

	BYTE* pDIBBits = (BYTE*) (pBitmapInfo->bmiColors + m_nColors);

	m_hBitmap = CreateDIBitmap(hDC, (BITMAPINFOHEADER*) m_pBitmap, CBM_INIT, pDIBBits,
		(BITMAPINFO*) m_pBitmap, DIB_RGB_COLORS);
	
	ReleaseDC(NULL, hDC);

	return TRUE;
}

BOOL CDibImage::Draw(HDC hDC, int X, int Y)
{
	if (m_hBitmap == NULL)
		return FALSE;

	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP) SelectObject(hMemDC, m_hBitmap);
	BitBlt(hDC, X, Y, GetWidth(), GetHeight(), hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hMemDC);  
	
	return TRUE;
}

int CDibImage::GetWidth()
{
	if (m_pBitmap == NULL)
		return 0;

	BITMAPINFO* pBitmapInfo = (BITMAPINFO*) m_pBitmap;
	return pBitmapInfo->bmiHeader.biWidth;
}

int CDibImage::GetHeight()
{
	if (m_pBitmap == NULL)
		return 0;

	BITMAPINFO* pBitmapInfo = (BITMAPINFO*) m_pBitmap;
	return pBitmapInfo->bmiHeader.biHeight;
}

int CDibImage::GetColorCount()
{
	return m_nColors;
}

HPALETTE CDibImage::GetPalette()
{
	return m_hPalette;
}

