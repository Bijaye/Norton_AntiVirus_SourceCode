#ifndef SYMCORPUI_CONTROLCOLORER
#define SYMCORPUI_CONTROLCOLORER

#include "stdafx.h"
#include "DialogFont.h"


// Used to set the colors and font of a static text or edit control
// Consequently, the control is not transparent
// Also sends a WM_COMMAND to parent when enter is pressed
class CEditColorFont : public CEdit
{
public:
    HBRUSH CtlColor( CDC* drawDC, UINT controlCode );
    afx_msg void OnNcPaint();
    afx_msg void OnPaint();
    afx_msg void OnKeyUp( UINT characterCode, UINT repeatCount, UINT flags );
    
    DialogFont      displayFont;
    void SetBackgroundColor( COLORREF newBackgroundColor );
    void SetBorderColor( COLORREF newBorderColor );
    HRESULT UpdateFont( void );
    afx_msg void OnSetFocus( CWnd* focusWindow );

    CEditColorFont();
    virtual ~CEditColorFont();
    DECLARE_MESSAGE_MAP()
private:
    COLORREF        backgroundColor;
    CBrush          backgroundBrush;
    COLORREF        borderColor;
    CBrush          borderBrush;
    CFont           displayFontCF;
};

#endif // SYMCORPUI_CONTROLCOLORER