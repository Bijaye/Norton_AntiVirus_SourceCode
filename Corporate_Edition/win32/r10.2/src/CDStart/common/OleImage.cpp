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

#include "stdafx.h"
#include "OleImage.h"

COleImage::COleImage()
{
    m_hRes = NULL;
    m_pPicture = NULL; 
    m_lWidth = 0; 
    m_lHeight; 
    m_cx = 0;
    m_cy = 0; 
}

COleImage::~COleImage()
{
	if (m_pPicture != NULL)
		m_pPicture->Release();

	if (m_hRes != NULL)
	 	DeleteObject(m_hRes);
}

BOOL COleImage::Load(UINT nResourceID)
{
	return Load(MAKEINTRESOURCE(nResourceID));
}

BOOL COleImage::Load(LPCTSTR szResourceName)
{
	// Find the resource and save a pointer to it

    HRSRC res = FindResource(GetModuleHandle(NULL),szResourceName,"BINARY");
    if (res)
    {
        m_pPicture = LoadPicture(res);
        if (!m_pPicture)
        {
            return FALSE;
        }
    }

	return TRUE;
}


#define HIMETRIC_PER_INCH         2540
#define MAP_PIX_TO_LOGHIM(x,ppli) ((HIMETRIC_PER_INCH*(x)+((ppli)>>1))/(ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli) (((ppli)*(x)+HIMETRIC_PER_INCH/2)/HIMETRIC_PER_INCH)

IPicture *COleImage::LoadPicture(HRSRC res)
{
  IPicture *pic = NULL;

  m_hRes = LoadResource(NULL, res);
  if (m_hRes)
  {
      void *data = LockResource(m_hRes);
      size_t len = SizeofResource(NULL, res);

      HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, len);
      if (hGlobal)
      {
          LPVOID pvData = GlobalLock( hGlobal );
          memcpy(pvData,data,len);
          GlobalUnlock(hGlobal);

          LPSTREAM pStream = NULL;
          CreateStreamOnHGlobal( hGlobal, TRUE, &pStream );
          if (pStream)
          {
              OleLoadPicture(pStream, 0, TRUE, IID_IPicture, (void **)&pic);
              if (pic)
              {
                  pic->get_Width(&m_cx);
                  pic->get_Height(&m_cy);

                  HDC dcPictureLoad = GetDC(NULL);
                  m_lWidth = MAP_LOGHIM_TO_PIX(m_cx, GetDeviceCaps(dcPictureLoad, LOGPIXELSX));
                  m_lHeight = MAP_LOGHIM_TO_PIX(m_cy, GetDeviceCaps(dcPictureLoad, LOGPIXELSX));
                  ReleaseDC(NULL,dcPictureLoad);
              }

              pStream->Release();
          }

          GlobalFree(hGlobal);
      }
  }

#ifdef _DEBUG
    if (!pic)
    {
        MessageBox(NULL, "Unable to load image", "DEBUG", MB_OK );
    }
#endif

  return pic;
}

BOOL COleImage::Draw(HDC hDC, int X, int Y)
{
	if (m_hRes == NULL)
		return FALSE;

    RECT bounds;
    bounds.top = Y;
    bounds.bottom = Y + m_lHeight;
    bounds.left = X;
    bounds.right = X + m_lWidth;
    m_pPicture->Render(hDC, 
        bounds.left, bounds.bottom, bounds.right - bounds.left, bounds.top - bounds.bottom, 
        0, 0, m_cx, m_cy, 
        NULL);

	return TRUE;
}

int COleImage::GetWidth()
{
	return m_lWidth;
}

int COleImage::GetHeight()
{
	return m_lHeight;
}

/*
int COleImage::GetColorCount()
{
	return m_nColors;
}
*/


HPALETTE COleImage::GetPalette()
{
    OLE_HANDLE hPal = NULL;
    if (m_pPicture)
    {
	    m_pPicture->get_hPal(&hPal);
    }

    return (HPALETTE)hPal;
}
