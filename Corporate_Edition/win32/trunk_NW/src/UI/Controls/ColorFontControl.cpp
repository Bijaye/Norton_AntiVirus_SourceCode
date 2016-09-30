#include "stdafx.h"
#include "ColorFontControl.h"


BEGIN_MESSAGE_MAP(CEditColorFont, CEdit)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_NCPAINT()
    ON_WM_PAINT()
    ON_WM_CHAR()
    ON_WM_KEYUP()
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()


HBRUSH CEditColorFont::CtlColor( CDC* drawDC, UINT controlCode )
{
    drawDC->SetBkColor(backgroundColor);

    if (!displayFont.useDefaultColor)
        drawDC->SetTextColor(displayFont.textColor);
    return (HBRUSH) backgroundBrush;
}

void CEditColorFont::OnKeyUp( UINT characterCode, UINT repeatCount, UINT flags )
{
    // Send WM_COMMAND to parent when enter is pressed
    if (characterCode == VK_RETURN)
        GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), 0), (LPARAM) GetSafeHwnd());
}


void CEditColorFont::OnPaint()
{
    CRect controlRect;

    // Let the edit control draw, then redraw it's frame with the color we want
    CEdit::OnPaint();

    // Redraw frame with the color we want
    if ((HBRUSH) borderBrush != NULL)
    {
        GetWindowRect(&controlRect);
        ScreenToClient(&controlRect);
        GetDC()->FrameRect(controlRect, &borderBrush);
    }
}

void CEditColorFont::OnNcPaint()
{
    CRect controlRect;

    CEdit::OnNcPaint();
    // Redraw frame with the color we want
    if ((HBRUSH) borderBrush != NULL)
    {
        GetWindowRect(&controlRect);
        ScreenToClient(&controlRect);
        GetDC()->FrameRect(controlRect, &borderBrush);
    }
}

void CEditColorFont::OnSetFocus( CWnd* focusWindow )
{
    if (!GetModify())
        SetWindowText(_T(""));
    CEdit::OnSetFocus(focusWindow);
}


HRESULT CEditColorFont::UpdateFont( void )
// Updates the font used to the one described by displayFont
{
    HRESULT returnValHR = E_FAIL;

    returnValHR = displayFont.CreateUpdateFont(GetDC(), &displayFontCF);
    SetFont(&displayFontCF);
    return returnValHR;
}

void CEditColorFont::SetBackgroundColor( COLORREF newBackgroundColor )
{
    backgroundColor = newBackgroundColor;
    if ((HBRUSH) backgroundBrush)
        backgroundBrush.DeleteObject();
    backgroundBrush.CreateSolidBrush(newBackgroundColor);
}

void CEditColorFont::SetBorderColor( COLORREF newBorderColor )
{
    borderColor = newBorderColor;
    if ((HBRUSH) borderBrush)
        borderBrush.DeleteObject();
    borderBrush.CreateSolidBrush(newBorderColor);
}

CEditColorFont::CEditColorFont() : backgroundColor(RGB(0,0,0)), borderColor(RGB(255,255,255))
{
    // Nothing needed
}

CEditColorFont::~CEditColorFont()
{
    // Cleanup
    if ((HBRUSH) backgroundBrush)
        backgroundBrush.DeleteObject();
    if (((HFONT) displayFontCF) != NULL)
        displayFontCF.DeleteObject();
}