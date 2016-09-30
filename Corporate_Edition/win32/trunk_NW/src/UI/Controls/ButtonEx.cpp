#include "stdafx.h"
#include "ButtonEx.h"
#include "UiUtil.h"


BEGIN_MESSAGE_MAP(CButtonEx, CButton)
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_SETCURSOR()
    ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
    ON_WM_ERASEBKGND()
    ON_MESSAGE(BM_SETCHECK, OnSetCheck)
END_MESSAGE_MAP()


CButtonEx::CButtonEx() : currState(ButtonState_Normal), useHoverFont(false), useSelectedFont(false), usePressedFont(false), alignmentStop(0.0), textPushDown(true), type(ButtonType_PushButton), hoverCursor(NULL), checked(false), mouseInClientArea(false), handCursor(NULL), backgroundOldBitmap(NULL), ownResources(true), scalingMode(ScaleMode_Exact), backgroundControl(NULL)
{
    // Load the system hand cursor
    handCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND));
}

CButtonEx::~CButtonEx()
{
    if (ownResources)
    {
        normalImage.Destroy();
        normalSelectedImage.Destroy();
        pressedImage.Destroy();
        hotImage.Destroy();
    }
    else
    {
        normalImage.Detach();
        normalSelectedImage.Detach();
        pressedImage.Detach();
        hotImage.Detach();
    }
    if (handCursor != NULL)
        DestroyCursor(handCursor);
}

HRESULT CButtonEx::LoadBitmaps( UINT normalImageID, UINT normalSelectedImageID, UINT pressedImageID, UINT hotImageID, CButtonEx::ScaleMode drawScalingMode )
{
    HRESULT returnValHR = S_OK;
    HRESULT returnValHRb = E_FAIL;

    // Free any existing bitmap(s)
    if (static_cast<HBITMAP>(normalImage) != NULL)
    {
        if (ownResources)
            normalImage.Destroy();
        else
            normalImage.Detach();
    }
    if (static_cast<HBITMAP>(normalSelectedImage) != NULL)
    {
        if (ownResources)
            normalSelectedImage.Destroy();
        else
            normalSelectedImage.Detach();
    }
    if (static_cast<HBITMAP>(pressedImage) != NULL)
    {
        if (ownResources)
            pressedImage.Destroy();
        else
            pressedImage.Detach();
    }
    if (static_cast<HBITMAP>(hotImage) != NULL)
    {
        if (ownResources)
            hotImage.Destroy();
        else
            hotImage.Detach();
    }

    // Load the new bitmap
	scalingMode = drawScalingMode;
    if (normalImageID != 0)
    {
        normalImage.LoadFromResource(AfxGetResourceHandle(), normalImageID);
        if (static_cast<HBITMAP>(normalImage) != NULL)
            returnValHRb = S_OK;
        else
            returnValHRb = E_FAIL;
        returnValHR = (SUCCEEDED(returnValHR) ? returnValHRb : returnValHR);
    }
    if (normalSelectedImageID != 0)
    {
        normalSelectedImage.LoadFromResource(AfxGetResourceHandle(), normalSelectedImageID);
        if (static_cast<HBITMAP>(normalSelectedImage) != NULL)
            returnValHRb = S_OK;
        else
            returnValHRb = E_FAIL;
        returnValHR = (SUCCEEDED(returnValHR) ? returnValHRb : returnValHR);
    }
    if (pressedImageID != 0)
    {
        pressedImage.LoadFromResource(AfxGetResourceHandle(), pressedImageID);
        if (static_cast<HBITMAP>(pressedImage) != NULL)
            returnValHRb = S_OK;
        else
            returnValHRb = E_FAIL;
        returnValHR = (SUCCEEDED(returnValHR) ? returnValHRb : returnValHR);
    }
    if (hotImageID != 0)
    {
        hotImage.LoadFromResource(AfxGetResourceHandle(), hotImageID);
        if (static_cast<HBITMAP>(hotImage) != NULL)
            returnValHRb = S_OK;
        else
            returnValHRb = E_FAIL;
        returnValHR = (SUCCEEDED(returnValHR) ? returnValHRb : returnValHR);
    }

    return returnValHR;
}

HRESULT CButtonEx::LoadImages( UINT normalImageID, UINT normalSelectedImageID, UINT pressedImageID, UINT hotImageID, CButtonEx::ScaleMode drawScalingMode, LPCTSTR resourceType )
// Same as LoadBitmaps, but uses BINARY resource type
{
    HBITMAP bitmapHandle    = NULL;
    HRESULT returnValHR     = S_OK;
    HRESULT returnValHRb    = E_FAIL;

    // Free any existing bitmap(s)
    if (static_cast<HBITMAP>(normalImage) != NULL)
    {
        if (ownResources)
            normalImage.Destroy();
        else
            normalImage.Detach();
    }
    if (static_cast<HBITMAP>(normalSelectedImage) != NULL)
    {
        if (ownResources)
            normalSelectedImage.Destroy();
        else
            normalSelectedImage.Detach();
    }
    if (static_cast<HBITMAP>(pressedImage) != NULL)
    {
        if (ownResources)
            pressedImage.Destroy();
        else
            pressedImage.Detach();
    }
    if (static_cast<HBITMAP>(hotImage) != NULL)
    {
        if (ownResources)
            hotImage.Destroy();
        else
            hotImage.Detach();
    }

    // Load the new images as bitmaps
	scalingMode = drawScalingMode;
    if (normalImageID != 0)
    {
        returnValHRb = LoadImageResource(AfxGetResourceHandle(), MAKEINTRESOURCE(normalImageID), resourceType, &bitmapHandle);
        if (SUCCEEDED(returnValHRb))
            normalImage.Attach(bitmapHandle);
        returnValHR = (SUCCEEDED(returnValHR) ? returnValHRb : returnValHR);
    }
    bitmapHandle = NULL;
    if (normalSelectedImageID != 0)
    {
        returnValHRb = LoadImageResource(AfxGetResourceHandle(), MAKEINTRESOURCE(normalSelectedImageID), resourceType, &bitmapHandle);
        if (SUCCEEDED(returnValHRb))
            normalSelectedImage.Attach(bitmapHandle);
        returnValHR = (SUCCEEDED(returnValHR) ? returnValHRb : returnValHR);
    }
    bitmapHandle = NULL;
    if (pressedImageID != 0)
    {
        returnValHRb = LoadImageResource(AfxGetResourceHandle(), MAKEINTRESOURCE(pressedImageID), resourceType, &bitmapHandle);
        if (SUCCEEDED(returnValHRb))
            pressedImage.Attach(bitmapHandle);
        returnValHR = (SUCCEEDED(returnValHR) ? returnValHRb : returnValHR);
    }
    bitmapHandle = NULL;
    if (hotImageID != 0)
    {
        returnValHRb = LoadImageResource(AfxGetResourceHandle(), MAKEINTRESOURCE(hotImageID), resourceType, &bitmapHandle);
        if (SUCCEEDED(returnValHRb))
            hotImage.Attach(bitmapHandle);
        returnValHR = (SUCCEEDED(returnValHR) ? returnValHRb : returnValHR);
    }

    return returnValHR;
}

void CButtonEx::SetBackgroundControl( CWnd* newBackgroundControl )
// Set the control that is underneath this control for background transparency.
// This control can overlap at most one other control
{
    backgroundControl = newBackgroundControl;
}

// Message map functions
void CButtonEx::PreSubclassWindow()
{
    ModifyStyle(NULL, BS_OWNERDRAW);
}

void CButtonEx::DrawItem( LPDRAWITEMSTRUCT drawInfo )
{
    CDC         screenDC;
    CRect       controlRect;
    CBitmap*    originalBitmap      = NULL;
    CString     windowText;
    DWORD       originalBackgroundMode      = 0;
    CFont*      originalFont                = NULL;
    COLORREF    originalColor               = 0;
    DWORD       style                       = 0;
    DWORD       drawTextFlags               = 0;
    CImage*     currBackgroundBitmap        = NULL;
    DialogFont* fontToUse                   = NULL;

    // Initialize
    GetClientRect(&controlRect);
    screenDC.Attach(drawInfo->hDC);
    GetWindowText(windowText);
    style = GetStyle();

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

    // Determine the correct background bitmap to use
    // If Normal and checked, translate state to NormalSelected
    if (type == ButtonType_RadioButton)
    {
        if ((currState == ButtonState_Normal) && checked)
            currState = ButtonState_NormalSelected;
    }
    fontToUse = &defaultFont;
    switch (currState)
    {
    case ButtonState_NormalSelected:
        currBackgroundBitmap = &normalSelectedImage;
        if (useSelectedFont)
            fontToUse = &selectedFont;
        break;
    case ButtonState_Pressed:
        currBackgroundBitmap = &pressedImage;
        if (usePressedFont)
            fontToUse = &pressedFont;
        break;
    case ButtonState_Hover:
        currBackgroundBitmap = &hotImage;
        if (useHoverFont)
            fontToUse = &hoverFont;
        break;
    case ButtonState_Normal:
    default:
        currBackgroundBitmap = &normalImage;
        break;
    }

    // Draw the bitmap
    CDC         memDC;
    CBitmap     newBitmap;
    CBitmap*    oldBitmap       = NULL;
    int         bitmapXpos      = 0;
    int         bitmapYpos      = 0;
    int         drawXSize       = 0;
    int         drawYSize       = 0;

    if (static_cast<HBITMAP>(*currBackgroundBitmap) != NULL)
    {
        memDC.CreateCompatibleDC(&screenDC);
        newBitmap.Attach((HBITMAP) *currBackgroundBitmap);
        oldBitmap = memDC.SelectObject(&newBitmap);

        if (scalingMode == ScaleMode_Exact)
        {
            drawXSize = currBackgroundBitmap->GetWidth();
            drawYSize = currBackgroundBitmap->GetHeight();
        }
        else if (scalingMode == ScaleMode_FitKeepAspectRatio)
        {

            drawXSize = controlRect.Width();
            if ((controlRect.Width() != currBackgroundBitmap->GetWidth()) || (controlRect.Height() != currBackgroundBitmap->GetHeight()))
            {
                // Yes.  First try to fit width
                drawYSize = (currBackgroundBitmap->GetHeight()*controlRect.Width())/currBackgroundBitmap->GetWidth();
                // Does the image fit in the rect?
                if (drawYSize > controlRect.Height())
                {
                    // No, try fitting height instead
                    drawXSize = (currBackgroundBitmap->GetWidth()*controlRect.Height())/currBackgroundBitmap->GetHeight();
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
        if ((style & BS_CENTER) != 0)
        {
            if (drawXSize < controlRect.Width())
                bitmapXpos = (controlRect.Width() - drawXSize)/2;
            if (drawYSize < controlRect.Height())
                bitmapYpos = (controlRect.Height() - drawYSize)/2;
        }

        // Draw it
        screenDC.TransparentBlt(bitmapXpos, bitmapYpos, drawXSize, drawYSize, &memDC, 0, 0, currBackgroundBitmap->GetWidth(), currBackgroundBitmap->GetHeight(), memDC.GetPixel(0,0));
        memDC.SelectObject(oldBitmap);
        newBitmap.Detach();
    }

    // Draw text
    if (windowText.GetLength() > 0)
    {
        // Select font
        fontToUse->CreateUpdateFont(&screenDC, &displayFont);

        // Configure DC for drawing text
        originalBackgroundMode = screenDC.SetBkMode(TRANSPARENT);
        if ((HFONT) displayFont != NULL)
            originalFont = screenDC.SelectObject(&displayFont);
        if (!fontToUse->useDefaultColor)
            originalColor = screenDC.SetTextColor(fontToUse->textColor);

        // Determine draw text flags
        // Alignment and wrapping
        switch (style & (BS_LEFT | BS_CENTER | BS_RIGHT))
        {
            case BS_LEFT:
                drawTextFlags |= (DT_LEFT | DT_SINGLELINE);
                break;
            case BS_RIGHT:
                drawTextFlags |= (DT_RIGHT | DT_WORDBREAK);
                break; 
            case BS_CENTER: 
            default:
                drawTextFlags |= (DT_CENTER | DT_WORDBREAK);
                break;
        }   
        // Tab expansion
        if(windowText.Find(_T('\t')) != -1)
            drawTextFlags |= DT_EXPANDTABS;
        if (style & BS_VCENTER)
            drawTextFlags |= DT_VCENTER | DT_SINGLELINE;
        else if (style & BS_TOP)
            drawTextFlags |= DT_TOP;
        else if (style & BS_BOTTOM)
            drawTextFlags |= DT_BOTTOM;
        else if (!(style & (BS_VCENTER | BS_TOP | BS_BOTTOM)))
            drawTextFlags |= DT_VCENTER | DT_SINGLELINE;

        // Draw the text
        if (textPushDown)
        {
            if (currState == ButtonState_Pressed)
            {
                if ((style & BS_VCENTER))
                    controlRect.top += 1;
                else
                    controlRect.top += 2;

                if ((style & BS_CENTER))
                    controlRect.left += 1;
                else
                    controlRect.left += 2;
            }
        }
        if (alignmentStop != 0)
        {
            int widthReduction = static_cast<int>(controlRect.Width()*alignmentStop);
            if (drawTextFlags & DT_RIGHT)
            {
                controlRect.right -= widthReduction;
                if (textPushDown && (currState == ButtonState_Pressed))
                    controlRect.right += 1;
            }
            else if ((drawTextFlags & DT_LEFT) == DT_LEFT)
            {
                controlRect.left += widthReduction;
                controlRect.right -= widthReduction;
            }
        }
        screenDC.DrawText(windowText, &controlRect, drawTextFlags);

        // Restore DC's state
        screenDC.SetBkMode(originalBackgroundMode);
        if ((HFONT) displayFont != NULL)
            screenDC.SelectObject(originalFont);
        screenDC.SetTextColor(originalColor);
    }

    screenDC.Detach();
}

BOOL CButtonEx::OnEraseBkgnd( CDC* drawDC )
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

void CButtonEx::OnMouseMove( UINT nFlags, CPoint point )
{
    TRACKMOUSEEVENT pointerTrack;

    // Redraw using hot image
    if (!mouseInClientArea)
    {
        currState = ButtonState_Hover;
        Invalidate();
    }
    // Ask for notification of when the mouse leaves
    ZeroMemory(&pointerTrack, sizeof(pointerTrack));
    pointerTrack.cbSize = sizeof(pointerTrack);
    pointerTrack.hwndTrack = GetSafeHwnd();
    pointerTrack.dwFlags = TME_LEAVE;
    TrackMouseEvent(&pointerTrack);
    mouseInClientArea = true;
}

LRESULT CButtonEx::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
    mouseInClientArea = false;
    // NEED TO DETERMINE SELECTED OR NOT FOR RADIO TYPE
    currState = ButtonState_Normal;
    Invalidate();
    SetCursor(NULL);

    return 0;
}

void CButtonEx::OnLButtonDown( UINT nFlags, CPoint point )
{
    currState = ButtonState_Pressed;
    Invalidate();
    CButton::OnLButtonDown(nFlags, point);
}

void CButtonEx::OnLButtonUp( UINT nFlags, CPoint point )
{
    CRect controlRect;

    GetClientRect(&controlRect);
    if (controlRect.PtInRect(point))
        currState = ButtonState_Hover;
    else
        currState = ButtonState_Normal;
    Invalidate();
    CButton::OnLButtonUp(nFlags, point);
}

BOOL CButtonEx::OnClicked()
{
    CWnd* radioGroupStart = NULL;
    CWnd* radioGroupEnd = NULL;

    checked = !checked;
    // Owner draw buttons have custom appearance *and* behavior, but default to checkbox behavior
    // Also, the OWNERDRAW and AUTORADIO styles can't be used together.
    // If user wants a radio, give them a radio
    if (type == ButtonType_RadioButton)
    {
        radioGroupStart = GetFirstButtonInGroup();
        radioGroupEnd = GetLastButtonInGroup();
        GetParent()->CheckRadioButton(radioGroupStart->GetDlgCtrlID(), radioGroupEnd->GetDlgCtrlID(), GetDlgCtrlID());
    }

    //return CButton::OnClicked();
    // This is post-LButtonUp so we don't know if the mouse is in window or not
    // Let draw figure out when we're in Normal_Selected
    return FALSE;
}

LRESULT CButtonEx::OnSetCheck( WPARAM wparam, LPARAM lparam )
{
    if (wparam == BST_CHECKED)
    {
        checked = true;
        if (currState == ButtonState_Normal)
            currState = ButtonState_NormalSelected;
    }
    else
    {
        // No 3-state support
        checked = false;
        if (currState == ButtonState_NormalSelected)
            currState = ButtonState_Normal;
    }

    Invalidate();
    UpdateWindow();
    RedrawWindow();
    return ERROR_SUCCESS;
}

BOOL CButtonEx::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
    // Direct Windows to use our custom cursor
    if (nHitTest == HTCLIENT)
    {
        ::SetCursor(handCursor);
        return TRUE;
    }
    return CButton::OnSetCursor(pWnd, nHitTest, message);
}

CWnd* CButtonEx::GetFirstButtonInGroup( void )
// Returns the first control in the same group as this control (for radio buttons)
{
    CWnd* currControl = NULL;
    CWnd* lastControl = NULL;

    // Base case
    if (GetStyle() & WS_GROUP)
        return this;

    // Scan back to the beginning of the dialog for the first control with WS_GROUP
    lastControl = this;
    currControl = GetParent()->GetNextDlgGroupItem(this, true);
    while (((currControl->GetStyle() & WS_GROUP) == 0) && (currControl->GetDlgCtrlID() < lastControl->GetDlgCtrlID()))
    {
        lastControl = currControl;
        // This will wrap to the last control
        currControl = GetParent()->GetNextDlgGroupItem(currControl, true);
    }

    // We will normally find a control with WS_GROUP set, but
    // regardless make sure we didn't wrap
    if (currControl->GetDlgCtrlID() > lastControl->GetDlgCtrlID())
        currControl = lastControl;
    return currControl;
}

CWnd* CButtonEx::GetLastButtonInGroup( void )
// Returns the last control in the same group as this control (for radio buttons)
{
    CWnd* currControl = NULL;
    CWnd* lastControl = NULL;

    // Scan forward, looking for the next control with WS_GROUP set
    lastControl = this;
    currControl = GetParent()->GetNextDlgGroupItem(this, false);
    while (((currControl->GetStyle() & WS_GROUP) == 0) && (currControl->GetDlgCtrlID() > lastControl->GetDlgCtrlID()))
    {
        currControl->GetDlgCtrlID();
        lastControl = currControl;
        // This will wrap!!
        currControl = GetParent()->GetNextDlgGroupItem(currControl, false);
    }

    // Use the control before the last one if either the currControl
    // is a new group or if currControl was wrapped to before this
    if (currControl->GetStyle() & WS_GROUP)
        currControl = lastControl;
    else if (currControl->GetDlgCtrlID() < lastControl->GetDlgCtrlID())
        currControl = lastControl;
    return currControl;
}