// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// Bitmap256.cpp : implementation file
//

#include "stdafx.h"
#include "Bitmap256.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmap256

CBitmap256::CBitmap256() : m_hBitmap(NULL), m_bLoaded(FALSE)
{

}

CBitmap256::~CBitmap256()
{
	if (m_hBitmap) DeleteObject(m_hBitmap); 
	if (m_Palette.m_hObject) m_Palette.DeleteObject();
}


BEGIN_MESSAGE_MAP(CBitmap256, CStatic)
	//{{AFX_MSG_MAP(CBitmap256)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmap256 message handlers

void CBitmap256::OnPaint() 
{
	if (!m_bLoaded) 
	{
		m_bLoaded=TRUE;
		LoadBitmap();
	}

	if (m_hBitmap)
	{
		CPaintDC dc(this); // device context for painting

		Draw(0,0,&dc);
	
	}
	else 
		CStatic::OnPaint();
		
	
}


void CBitmap256::Draw(int x,int y,CDC* pDC)
{
	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(pDC))
		return;

	// select and realize the palette
	CPalette *pOldPal = pDC->SelectPalette(&m_Palette,0);
	pDC->RealizePalette();

	CBitmap* pBitmap = CBitmap::FromHandle(m_hBitmap);

	BITMAP bm;
	pBitmap->GetBitmap(&bm);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(pBitmap);
	pDC->BitBlt(x, y, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	dcImage.SelectObject(pOldBitmap);

	if (pOldPal) pDC->SelectPalette(pOldPal,0);

	dcImage.DeleteDC();
}

BOOL CBitmap256::LoadBitmap(LPCSTR resID)
{
	LPBITMAPINFO pInfo=NULL;
	void* pBits=NULL;
	char sid[MAX_PATH];
	HRSRC hFind;

	if (!resID) // load image defined in control
	{
		ZeroMemory(sid,MAX_PATH);
		GetWindowText(sid,MAX_PATH);
		if ((unsigned char)sid[0]==0xFF)
		{
			long rid = MAKELONG((unsigned char)sid[1],(unsigned char)sid[2]);
			hFind = FindResource(AfxGetResourceHandle(),MAKEINTRESOURCE(rid),RT_BITMAP); 
		}
		else
			hFind = FindResource(AfxGetResourceHandle(),sid,RT_BITMAP); 
	}
	else 
		hFind = FindResource(AfxGetResourceHandle(),resID,RT_BITMAP); 

	if (!hFind) return FALSE;

	// load bitmap info
	HGLOBAL hMem = LoadResource(AfxGetResourceHandle(),hFind);
	if (hMem)
		pInfo =(LPBITMAPINFO)LockResource(hMem);
 
	if (!pInfo) return FALSE;


	if (pInfo->bmiHeader.biBitCount != 8) 
		return FALSE;

	// create device DIB bitmap
	HDC hdc = CreateCompatibleDC(NULL);
	m_hBitmap =  CreateDIBSection(hdc,pInfo,DIB_RGB_COLORS,&pBits,NULL,0);
	if (hdc) DeleteDC(hdc);

	VERIFY(m_hBitmap && pBits);

	if (pBits == NULL || m_hBitmap == NULL) return FALSE;
	
	// may mave to be caluclated 
	if (pInfo->bmiHeader.biSizeImage==0)
	{
		DWORD dwLine = pInfo->bmiHeader.biWidth;
		ASSERT(pInfo->bmiHeader.biHeight>0);// don't support bottom up yet
		// make sure line to word aligned
		while (dwLine % 2) dwLine++;
		pInfo->bmiHeader.biSizeImage = dwLine*pInfo->bmiHeader.biHeight;
	}

	// does not have to be used
	if (pInfo->bmiHeader.biClrUsed==0)
		pInfo->bmiHeader.biClrUsed = 256;
	
	//copy image data
	DWORD dwOffset = pInfo->bmiHeader.biSize + (sizeof(RGBQUAD)*pInfo->bmiHeader.biClrUsed);
	CopyMemory(pBits,(void*)((DWORD)pInfo +dwOffset),pInfo->bmiHeader.biSizeImage);


	// create color table (if needed)
	BYTE pMem[sizeof(LOGPALETTE)+(sizeof(PALETTEENTRY)*256)];
	LPLOGPALETTE p = (LPLOGPALETTE)pMem;
	p->palVersion = 0x300;
	p->palNumEntries=256;
	for (int i=0;i<256;i++)
	{
		p->palPalEntry[i].peRed= pInfo->bmiColors[i].rgbRed;
		p->palPalEntry[i].peGreen= pInfo->bmiColors[i].rgbGreen;
		p->palPalEntry[i].peBlue= pInfo->bmiColors[i].rgbBlue;
		p->palPalEntry[i].peFlags=0 ; 
 	}


	BOOL bOk;
	bOk = m_Palette.CreatePalette(p);

	VERIFY(bOk);

	return TRUE;

}

