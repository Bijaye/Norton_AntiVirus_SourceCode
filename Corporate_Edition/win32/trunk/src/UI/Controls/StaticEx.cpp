#include "stdafx.h"
#include "StaticEx.h"
#include <StrSafe.h>
#include <new>


BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
    ON_WM_DRAWITEM_REFLECT()
    ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SETTEXT, OnWMSetText)
END_MESSAGE_MAP()


CStaticEx::CStaticEx() : scalingMode(ScaleMode_FitKeepAspectRatio), specifiedTransparencyColor(false), transparencyColor(RGB(0,0,0)), ownResource(true), backgroundControl(NULL)
{
	// Nothing for now
}
CStaticEx::~CStaticEx()
{
	// Dispose of the display font, if we created one
	if ((HFONT) displayFont != NULL)
		displayFont.DeleteObject();
    if (static_cast<HBITMAP>(image) != NULL)
    {
        if (ownResource)
            image.Destroy();
        else
            image.Detach();
    }    
}

void CStaticEx::DrawItem( LPDRAWITEMSTRUCT drawInfo )
{
    CDC         screenDC;
    CRect       controlRect;
	CClientDC   parentDC(GetParent());
    CRect       controlRectInParent;
    CBitmap*    originalBitmap      = NULL;
    int         bitmapXpos          = 0;
    int         bitmapYpos          = 0;
    DWORD       style               = 0;
    CString     windowText;

    // Initialize
    GetClientRect(&controlRect);
    screenDC.Attach(drawInfo->hDC);
    style = GetStyle();
    GetWindowText(windowText);

    // Dialog background is redrawn via the standard ERASEBKGND reflection we receive just before this
    // Redraw background control
    if (backgroundControl != NULL)
    {
        CRect   clipBox;
        CRect   clipBoxScreen;
        CRect   clipBoxBackgroundControl;

        screenDC.GetClipBox(&clipBox);
        clipBoxScreen = clipBox;
        ClientToScreen(clipBoxScreen);
        clipBoxBackgroundControl = clipBoxScreen;
        backgroundControl->ScreenToClient(&clipBoxBackgroundControl);
        backgroundControl->RedrawWindow(clipBoxBackgroundControl);
    }

    // Draw the bitmap
    CDC         memDC;
    CBitmap     newBitmap;
    CBitmap*    oldBitmap   = NULL;
    int         drawXSize   = 0;
    int         drawYSize   = 0;
    if (static_cast<HBITMAP>(image) != NULL)
    {
        memDC.CreateCompatibleDC(&screenDC);
        newBitmap.Attach((HBITMAP) image);
        oldBitmap = memDC.SelectObject(&newBitmap);
        if (scalingMode == ScaleMode_Exact)
        {
            drawXSize = image.GetWidth();
            drawYSize = image.GetHeight();
        }
        else if (scalingMode == ScaleMode_FitKeepAspectRatio)
        {

            drawXSize = controlRect.Width();
            drawYSize = image.GetHeight();
            if ((controlRect.Width() != image.GetWidth()) || (controlRect.Height() != image.GetHeight()))
            {
                // Yes.  First try to fit width
                drawYSize = (image.GetHeight()*controlRect.Width())/image.GetWidth();
                // Does the image fit in the rect?
                if (drawYSize > controlRect.Height())
                {
                    // No, try fitting height instead
                    drawXSize = (image.GetWidth()*controlRect.Height())/image.GetHeight();
                    drawYSize = controlRect.Height();
                }
            }
        }
        else if (scalingMode == ScaleMode_StretchToFit)
        {
            drawXSize = controlRect.Width();
            drawYSize = controlRect.Height();
        }

        // Align picture as requested
        if ((style & SS_CENTERIMAGE) != 0)
        {
            if (drawXSize < controlRect.Width())
                bitmapXpos = (controlRect.Width() - drawXSize)/2;
            if (drawYSize < controlRect.Height())
                bitmapYpos = (controlRect.Height() - drawYSize)/2;
        }

        // Draw it
        if (!specifiedTransparencyColor)
            screenDC.TransparentBlt(bitmapXpos, bitmapYpos, drawXSize, drawYSize, &memDC, 0, 0, image.GetWidth(), image.GetHeight(), memDC.GetPixel(0,0));
        else
            screenDC.TransparentBlt(bitmapXpos, bitmapYpos, drawXSize, drawYSize, &memDC, 0, 0, image.GetWidth(), image.GetHeight(), transparencyColor);

        // Cleanup
        memDC.SelectObject(oldBitmap);
        newBitmap.Detach();
    }

    // Draw the text
    UINT            originalBkMode  = 0;
    COLORREF        originalColor   = 0;
    CFont*          originalFont    = NULL;
    DWORD           drawTextFlags   = 0;
    CRect           clientRect;

    // Initialize
    if (windowText.GetLength() > 0)
    {
        fontInfo.CreateUpdateFont(&screenDC, &displayFont);

        // Configure DC for drawing text
        originalBkMode = screenDC.SetBkMode(TRANSPARENT);
        if ((HFONT) displayFont != NULL)
            originalFont = screenDC.SelectObject(&displayFont);
        if (!fontInfo.useDefaultColor)
            originalColor = screenDC.SetTextColor(fontInfo.textColor);

        // Determine draw text flags
        // Alignment and wrapping
        style |= SS_LEFT;
        switch (style & SS_TYPEMASK)
        {
            case SS_RIGHT: 
                drawTextFlags = DT_RIGHT | DT_WORDBREAK; 
                break; 
            
            case SS_CENTER: 
                drawTextFlags = SS_CENTER | DT_WORDBREAK;
                break;

            case SS_LEFTNOWORDWRAP:
                drawTextFlags = DT_LEFT | DT_SINGLELINE; 
                break;

            default: // treat other types as left
                case SS_LEFT: 
                    drawTextFlags = DT_LEFT | DT_WORDBREAK; 
                    break;
        }   
        // Tab expansion
        if(windowText.Find(_T('\t')) != -1)
            drawTextFlags |= DT_EXPANDTABS;
        /*
        // If centered horizontally, also center veritcally
        if ((style & SS_CENTER) != 0)
        {
            drawTextFlags = DT_CENTER;
            if (windowText.Find(_T("\r\n")) == -1)
            {
                drawTextFlags |= DT_VCENTER;

                // And because DT_VCENTER only works with single lines
                drawTextFlags |= DT_SINGLELINE; 
            }
        }
        */

        // Handle indentation
        if (indentationPercentage != 0)
        {
            int widthReduction = static_cast<int>(controlRect.Width()*indentationPercentage);
            if (drawTextFlags & DT_RIGHT)
            {
                controlRect.right -= widthReduction;
            }
            else if ((drawTextFlags & DT_LEFT) == DT_LEFT)
            {
                controlRect.left += widthReduction;
                controlRect.right -= widthReduction;
            }
        }

        // Draw the text
        screenDC.DrawText(windowText, &controlRect, drawTextFlags);

        // Restore DC's state
        screenDC.SetBkMode(originalBkMode);
        if ((HFONT) displayFont != NULL)
            screenDC.SelectObject(originalFont);
        screenDC.SetTextColor(originalColor);
    }
}

BOOL CStaticEx::OnEraseBkgnd( CDC* drawDC )
{
    // Default Windows behavior is for control to FillRect with color from parent, but we
    // want transparency so get parent to redraw this region
    CWnd*   parentWindow = NULL;
    POINT   controlOriginInParent              = {0};
    CRect controlRect;

    // Determine origin in parent logical coordinates
    parentWindow = GetParent();
    MapWindowPoints(parentWindow, &controlOriginInParent, 1);
    OffsetWindowOrgEx(drawDC->m_hDC, controlOriginInParent.x, controlOriginInParent.y, &controlOriginInParent);

    // Set clipping region to this control only
    GetClientRect(&controlRect);
    ClientToScreen(controlRect);
    parentWindow->ScreenToClient(controlRect);
    drawDC->IntersectClipRect(controlRect);
    parentWindow->SendMessage(WM_ERASEBKGND, (WPARAM)drawDC->GetSafeHdc(), 0);

    // Restore origin
    SetWindowOrgEx(drawDC->m_hDC, controlOriginInParent.x, controlOriginInParent.y, NULL);
    return TRUE;
}

LRESULT CStaticEx::OnWMSetText( WPARAM wparam, LPARAM lparam )
{
	// WM_SETTEXT doesn't invalidate the control
	// Force invalidate the whole thing, causing a WM_ERASEBKGND to be sent, fixing control-over-dialog transparency
	Invalidate();
	return this->DefWindowProc(WM_SETTEXT, wparam, lparam);
}

void CStaticEx::PreSubclassWindow()
{
    // Control must have SS_OWNERDRAW for DrawItem to be called
    ModifyStyle(NULL, SS_OWNERDRAW);
}

void CStaticEx::SetBackgroundControl( CWnd* newBackgroundControl )
// Set the control that is underneath this control for background transparency.
// This control can overlap at most one other control
{
    backgroundControl = newBackgroundControl;
}


HRESULT CStaticEx::LoadBitmap( UINT resourceID, ScaleMode drawScalingMode )
{
    HRESULT     returnValHR = E_FAIL;

    // Free any existing bitmap
    if (static_cast<HBITMAP>(image) != NULL)
    {
        if (ownResource)
            image.Destroy();
        else
            image.Detach();
    }    

    // Load the new bitmap
	if (resourceID != 0)
	{
		scalingMode = drawScalingMode;
		image.LoadFromResource(AfxGetResourceHandle(), resourceID);
        if (static_cast<HBITMAP>(image) != NULL)
            returnValHR = S_OK;
	}
    else
    {
        returnValHR = S_OK;
    }

    return returnValHR;
}

HRESULT CStaticEx::LoadImage( UINT resourceID, CStaticEx::ScaleMode drawScalingMode, LPCTSTR resourceType, bool imageSpecifyTransparencyColor, COLORREF imageTransparencyColor )
// As above, but supports also JPG, GIF, TIFF, PNG, WMF, EMF, Exif, ICON.
{
    HBITMAP     bitmapHandle = NULL;
    HRESULT     returnValHR     = E_FAIL;

    // Free any existing bitmap
    if (static_cast<HBITMAP>(image) != NULL)
    {
        if (ownResource)
            image.Destroy();
        else
            image.Detach();
    }

    // Load the new one
	if (resourceID != 0)
	{
        returnValHR = LoadImageResource(AfxGetResourceHandle(), MAKEINTRESOURCE(resourceID), _T("BINARY"), &bitmapHandle);
        if (SUCCEEDED(returnValHR))
        {
            image.Attach(bitmapHandle);
            scalingMode = drawScalingMode;
            specifiedTransparencyColor = imageSpecifyTransparencyColor;
            if (specifiedTransparencyColor)
                transparencyColor = imageTransparencyColor;
            else
                transparencyColor = image.GetPixel(0,0);
            returnValHR = S_OK;
        }
	}
    return returnValHR;
}

// Sets the image to use to the specified bitmap.  If ownResource is TRUE, assumes ownership of
// newBitmapHandle, releasing it when destroyed/new image loaded
HRESULT CStaticEx::SetBitmap( HBITMAP newBitmapHandle, ScaleMode drawScalingMode, bool ownBitmapHandle )
{
    // Free any existing bitmap
    if (static_cast<HBITMAP>(image) != NULL)
    {
        if (ownResource)
            image.Destroy();
        else
            image.Detach();
    }

    if (newBitmapHandle != NULL)
    {
        image.Attach(newBitmapHandle);
        scalingMode = drawScalingMode;
        ownResource = ownBitmapHandle;
    }
    return S_OK;
}
