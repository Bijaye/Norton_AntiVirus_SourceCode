#ifndef PROTECTIONPROVIDER_UIUTIL
#define PROTECTIONPROVIDER_UIUTIL

#include "stdafx.h"
#include "gdiplus.h"
#include "atlimage.h"


// ** CLASSES **
// PropertySheet that sets WS_EX_CONTROLPARENT on itself so ActiveX controls in child
// pages will work correctly.  Note that pages and the controls must also have
// WS_EX_CONTROLPARENT (all the way up the chain, control->top level parent window).
class CControlParentSheet : public CPropertySheet
{
public:
    virtual BOOL OnInitDialog();
    void SetParent( CWnd* newParent );
    void SetCaption( LPCTSTR caption );
};

// Implements transparent background support for a control.  Redraws a control's rect with what it's parent displayed
// at startup.
// To use:
//      1.  Instantiate a member of this in your control class
//      2.  Call DrawTransparentBackground before performing any other drawing in your DrawItem/OnPaint handler
class CTransparentBackground
{
public:
    // Constructor-destructor
    CTransparentBackground( CWnd* thisControl );
    virtual ~CTransparentBackground();
    
    // Gets what was on the screen in the area occupied by this control before it was first drawn, stores it, and re-renders
    // it each subsequent time so the control appears to draw transparently on it's parent dialog
    void DrawTransparentBackground( CDC* drawDC );
private:
    CWnd*       control;
    CDC         backgroundDC;
    CBitmap     backgroundBitmap;
    CBitmap*    backgroundDColdBitmap;
};


// ** FUNCTIONS **
// Loads a JPG, GIF, PNG, or TIFF file from the specified resource and sets *gdiplusBitmap equal to a Gdiplus::Bitmap object
// representing the specified resource.  Note this does NOT work with bitmap resources stored as type bitmap.
HRESULT LoadImageResource( HINSTANCE resourceModuleHandle, LPCTSTR resourceName, LPCTSTR resourceType, Gdiplus::Bitmap** gdiplusBitmap );
// As LoadImageResource, but returns an HBITMAP the caller now owns.
HRESULT LoadImageResource( HINSTANCE resourceModuleHandle, LPCTSTR resourceName, LPCTSTR resourceType, HBITMAP* bitmapHandle );
// As LoadImageResource, but returns a CImage the caller now owns.
HRESULT LoadImageResource( HINSTANCE resourceModuleHandle, LPCTSTR resourceName, LPCTSTR resourceType, CImage* image );
// As LoadImageResource, but returns a CImage the caller now owns.
HRESULT LoadImageResource( HINSTANCE resourceModuleHandle, UINT resourceID, LPCTSTR resourceType, CImage* image );
// Converts the passed GDI+ status code to an HRESULT
HRESULT GdiplusStatusToHresult( Gdiplus::Status statusCode );

// Draws bitmapToDraw with transparency onto drawDC.  transparencyColor is the key color for transparency.
// drawX and drawY are the start locations to draw in drawDC
// If newWidth or newHeight are 0, uses the values from bitmapToDraw
void DrawTransparent( CDC* drawDC, CBitmap* bitmapToDraw, COLORREF transparencyColor, int drawX, int drawY, int newWidth, int newHeight );
// As DrawTransparent, but always draws at 0,0
void DrawTransparent( CDC* drawDC, CBitmap* bitmapToDraw, COLORREF transparencyColor, int newWidth, int newHeight );
// As DrawTranspareny, but automatically extracts transparency color from top left of bitmap
// and always draws at 0,0
void DrawTransparent( CDC* drawDC, CBitmap* bitmapToDraw, int newWidth, int newHeight );
// As DrawTransparent, but works with CImages
void DrawTransparent( CDC* drawDC, CImage* imageToDraw, COLORREF transparencyColor, int drawX, int drawY, int newWidth, int newHeight );
// As DrawTransparent, but works with CImages, automatically determines the transparency
// color from the 1st pixel in the image, and draws at 0,0
void DrawTransparent( CDC* drawDC, CImage* imageToDraw, int newWidth, int newHeight );

// Takes sourceRect and enlarges/reduces it to fully fit into targetRect while preserving
// it's aspect ratio
void ScaleSizeFitKeepAspectRatio( CRect sourceRect, CRect targetRect, CRect* scaledRect );

// Switches the view displayed in targetFrameWindow from whatever it is to viewClass
// Active view is destroyed and replaced
HRESULT SwitchViewInFrame( CFrameWnd* targetFrameWindow, CRuntimeClass* viewClass );

#endif // PROTECTIONPROVIDER_UIUTIL