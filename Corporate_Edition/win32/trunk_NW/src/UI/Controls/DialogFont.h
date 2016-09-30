#ifndef SYMCORPUI_DIALOGFONT
#define SYMCORPUI_DIALOGFONT

#include "stdafx.h"

// A font used by the UI.  Needed because most fonts are relative fonts and those can't be resolved until
// OnDraw is called, so we must store the relative info and create the font as needed.  But we might
// need to specify exact fonts instead.
// (The DC for a control returned during OnInitDialog returns a different default font than that returned
// during OnDraw.  The latter returns the correct font).
class DialogFont
{
public:
    CString     typefaceName;           // Use "" to specify the default font
    double      fontSize;               // If default font, this is a multiplier specifying relative size.  Otherwise, it is an exact point size.
    bool        isBold;
    bool        isItalic;
    bool        isUnderline;
    bool        useDefaultColor;        // TRUE to use default color, FALSE to use textColor
    COLORREF    textColor;

    // Loads a font descriptor string from the specified string resource ID.
    // Formats:
    //      Use dialog font     *,<size scale>[b][i][u][,<red>/<green>/<blue>]
    //      Use custom font     <typeface name>,<font point size>[b][i][u][,<red>/<green>/<blue>]
    HRESULT LoadFromString( UINT resourceID );
    HRESULT LoadFromString( CString& fontDescriptor );
    // Sets *newFont equal to the font described by this object.  Releases any existing font
    // contained by newFont.
    HRESULT CreateFont( CDC *drawContext, CFont *newFont );
    // Sets *thisFontInfo equal to a LOGFONT describing the font described by this object.
    HRESULT CreateLOGFONT( CDC* drawContext, LOGFONT* thisFontInfo );
    // Compares the font described by this object to *newFont.  If they are the same, does
    // nothing, if they are different or newFont is not yet attached to a font, releases
    // the font it is attached to and creates a new font for it as described by this object.
    HRESULT CreateUpdateFont( CDC* drawContext, CFont* currFontInUse );
    bool IsLOGFONTEqual( LOGFONT& fontA, LOGFONT& fontB );
    // Returns TRUE if the DialogFont is empty, meaning just a default font, else FALSE
    bool IsNULL( void );

    // Constructor-destructor
    DialogFont();
    bool operator==( const DialogFont& comparatorFont );
    bool operator!=( const DialogFont& comparatorFont );
};

#endif // SYMCORPUI_DIALOGFONT