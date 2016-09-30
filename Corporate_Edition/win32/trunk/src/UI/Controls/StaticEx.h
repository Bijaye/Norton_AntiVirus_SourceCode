#ifndef SYMCORPUI_STATICEX
#define SYMCORPUI_STATICEX

#include "stdafx.h"
#include "DialogFont.h"
#include "atlimage.h"
#include "UiUtil.h"

// Transparent static control
class CStaticEx : public CStatic
{
public:
    // ** CONSTANTS **
    typedef enum {ScaleMode_Exact, ScaleMode_FitKeepAspectRatio, ScaleMode_StretchToFit} ScaleMode;

	// Image functions
	// Loads the specified bitmap for display in the static control
	// Specify resourceID 0 to delete the image
	HRESULT LoadBitmap( UINT resourceID, ScaleMode drawScalingMode = CStaticEx::ScaleMode_FitKeepAspectRatio );
    // As above, but supports also JPG, GIF, TIFF, PNG, WMF, EMF, Exif, ICON.
    HRESULT LoadImage( UINT resourceID, ScaleMode drawScalingMode = CStaticEx::ScaleMode_FitKeepAspectRatio, LPCTSTR resourceType = _T("BINARY"), bool specifyTransparencyColor = false, COLORREF imageTransparencyColor = RGB(0,0,0) );
    // Sets the image to use to the specified bitmap.  If ownResource is TRUE, assumes ownership of
    // newBitmapHandle, releasing it when destroyed/new image loaded
    HRESULT SetBitmap( HBITMAP newBitmapHandle, ScaleMode drawScalingMode = CStaticEx::ScaleMode_FitKeepAspectRatio, bool ownBitmapHandle = true );
    // Set the control that is underneath this control for background transparency.
    // This control can overlap at most one other control
    void SetBackgroundControl( CWnd* newBackgroundControl );

	// Display font
    DialogFont  fontInfo;
    double      indentationPercentage;      // Percentage of client area that text is indented from

    // Constructor-destructor
    CStaticEx();
    virtual ~CStaticEx();

    // Message Map functions
    virtual void PreSubclassWindow();
    virtual void DrawItem( LPDRAWITEMSTRUCT drawInfo );
    afx_msg BOOL OnEraseBkgnd( CDC* pDC );
	afx_msg LRESULT OnWMSetText( WPARAM wparam, LPARAM lparam );
    DECLARE_MESSAGE_MAP()
private:
	// ** DATA MEMBERS **
	// Image
    ScaleMode   scalingMode;
    CImage      image;
    bool        specifiedTransparencyColor;
    COLORREF    transparencyColor;
    bool        ownResource;
	// Font
    DialogFont  lastFontCreated;
    CFont       displayFont;
    // Miscellaneous
    CWnd*       backgroundControl;
};

#endif // SYMCORPUI_STATICEX