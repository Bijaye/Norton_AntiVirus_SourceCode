#ifndef SYMCORPUI_BUTTONEX
#define SYMCORPUI_BUTTONEX

#include "stdafx.h"
#include "DialogFont.h"
#include "atlimage.h"

// An improved CButton that supports multiple transparent image backgrounds, fonts, and different fonts based on state
// Notes:
// 1.  Mark as BS_OWNERDRAW in resources.  Although this class does automatically add that style
//     if missing, in practice the initial draw isn't with this code and so it looks wrong
//     until it's refreshed.
// 2.  BS_OWNERDRAW doesn't mean just draw, it means *behavior* too.  Same field is used to specify
//     checkbox, radio, etc., so if you want an owner-draw radio, you cannot configure it by marking
//     it as both BS_OWNERDRAW and BS_RADIOBUTTON
// 3.  Transparency - what you want determines what you must do.
//          
//          Over Dialog background only     0.  Dialog must draw background in WM_ERASEBKGND, NOT OnPaint
//                                          1.  Mark this control as WS_EX_TRANSPARENT
//                                          2.  Do NOT mark the dialog as WS_EX_TRANSPARENT
//
//          Over another control and dialog 0.  Dialog must draw background in WM_ERASEBKGND, NOT OnPaint
//                                          1.  Do NOT mark this control as WS_EX_TRANSPARNET
//                                          2.  Do NOT mark the dialog as WS_EX_TRANSPARENT
//                                          3.  The other control must use an identical drawing
//                                              protocol or the dialog background won't show through.
//                                          4.  Call SetBackgroundControl to tell this control
//                                              what control is in it's background.
//      While untested, if the "Over another control" rules are followed, there is no reason
//      why this shouldn't work with multiple overlapping controls - just make sure the BackgroundControl
//      relationships are setup according to the Z order you want.
class CButtonEx : public CButton
{
public:
    // ** DATA TYPES **
    typedef enum {ButtonType_PushButton, ButtonType_RadioButton} ButtonType;
    typedef enum {ScaleMode_Exact, ScaleMode_FitKeepAspectRatio, ScaleMode_StretchToFit} ScaleMode;

    // Initializes the button with the specified bitmaps
    HRESULT LoadBitmaps( UINT normalImageID, UINT normalSelectedImageID, UINT pressedImageID, UINT hotImageID, ScaleMode drawScalingMode = CButtonEx::ScaleMode_FitKeepAspectRatio );
    // As above, but supports also JPG, GIF, TIFF, PNG, WMF, EMF, Exif, ICON.
    HRESULT LoadImages( UINT normalImageID, UINT normalSelectedImageID, UINT pressedImageID, UINT hotImageID, ScaleMode drawScalingMode = CButtonEx::ScaleMode_FitKeepAspectRatio, LPCTSTR resourceType = _T("BINARY") );
    // Set the control that is underneath this control for background transparency.
    // This control can overlap at most one other control
    void SetBackgroundControl( CWnd* newBackgroundControl );

    // Display font
    DialogFont  defaultFont;            // Font for Normal state and all others, unless an override has been set
    bool        useHoverFont;           // Set to TRUE to use hoverFont when in Hover state
    DialogFont  hoverFont;              // Font to use when hovering
    bool        useSelectedFont;        // Set to TRUE to use selectedFont when in Normal_Selected state
    DialogFont  selectedFont;           // Font to use in Normal_selected state (checkbox or radio)
    bool        usePressedFont;         // Set to TRUE to use pressedFont when in Pressed state
    DialogFont  pressedFont;            // Font to use in Pressed state
    double      alignmentStop;          // If right or left aligned, the percentage of the client rect to exclude from text drawing

    // Button configuration
    bool        textPushDown;           // TRUE if text should be moved down/right on push, else FALSE
    ButtonType  type;

    // Constructor-destructor
    CButtonEx();
    ~CButtonEx();

    // Message map functions
    virtual void PreSubclassWindow();
    virtual void DrawItem( LPDRAWITEMSTRUCT drawInfo );
    afx_msg BOOL OnEraseBkgnd( CDC* pDC );
    afx_msg void OnMouseMove( UINT nFlags, CPoint point );
    afx_msg LRESULT OnMouseLeave( WPARAM wparam, LPARAM lparam );
    afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
    afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
    afx_msg BOOL OnClicked();
    LRESULT OnSetCheck( WPARAM wparam, LPARAM lparam );
    afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );
    DECLARE_MESSAGE_MAP()

private:
    // ** DATA TYPES
    typedef enum {ButtonState_Normal, ButtonState_NormalSelected, ButtonState_Pressed, ButtonState_Hover} ButtonState;

    // ** FUNCTION MEMBERS **
    // Returns the first control in the same group as this control (for radio buttons)
    CWnd* GetFirstButtonInGroup( void );
    // Returns the last control in the same group as this control (for radio buttons)
    CWnd* GetLastButtonInGroup( void );

    // ** DATA MEMBERS **
    // Image
    CImage      normalImage;            // Normal button state - mouse not hovering, not pressed.
    CImage      normalSelectedImage;    // Normal button state - style is radio button, mouse not hovering, button is selected
    CImage      pressedImage;           // Pressed button image - mouse left button down
    CImage      hotImage;               // Hot button image - mouse hovering over
    bool        ownResources;
    ScaleMode   scalingMode;
    // Button management
    bool        checked;                // If radio or checkbox
    ButtonState currState;              // Describes the current button state
    bool        mouseInClientArea;
    HCURSOR     handCursor;
    // Miscelaneous
    CFont       displayFont;
    HCURSOR     hoverCursor;            // Custom hover cursor to use, if any
    // Transparent background support
    CDC         backgroundDC;
    CBitmap     backgroundBitmap;
    CBitmap*    backgroundOldBitmap;
    CWnd*       backgroundControl;

};

#endif // SYMCORPUI_BUTTONEX