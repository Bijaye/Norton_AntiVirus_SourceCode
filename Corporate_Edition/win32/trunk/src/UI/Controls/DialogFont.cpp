#include "stdafx.h"
#include "DialogFont.h"
#include "StrSafe.h"


DialogFont::DialogFont() : fontSize(0), isBold(false), isItalic(false), isUnderline(false), useDefaultColor(true), textColor(RGB(0,0,0))
{
    // Nothing for now
}

HRESULT DialogFont::LoadFromString( UINT resourceID )
{
    CString     fontDescriptor;
    HRESULT     returnValHR     = E_FAIL;

    try
    {
        if (fontDescriptor.LoadString(resourceID))
            returnValHR = LoadFromString(fontDescriptor);
        else
            returnValHR = HRESULT_FROM_WIN32(ERROR_RESOURCE_NAME_NOT_FOUND);
    }
    catch (std::bad_alloc&)
    {
        returnValHR = E_OUTOFMEMORY;
    }
    catch (CMemoryException*)
    {
        returnValHR = E_OUTOFMEMORY;
    }

    return returnValHR;
}

HRESULT DialogFont::LoadFromString( CString& fontDescriptor )
// Parses the specified font descriptor string.
// 
// Formats:
//      Use dialog font     *,<size scale>[b][i][u][,<red>/<green>/<blue>]
//      Use custom font     <typeface name>,<font point size>[b][i][u][,<red>/<green>/<blue>]
{
    CString     fontString;
    CString     sizeStyleInfo;
    CString     colorInfo;
    int         firstSeparatorPos   = -1;
    int         secondSeparatorPos  = -1;
    int         redValue            = 0;
    int         greenValue          = 0;
    int         blueValue           = 0;
    HRESULT     returnValHR         = E_FAIL;

    // Validate parameters
    returnValHR = S_OK;
    fontString = fontDescriptor;
    fontString.Trim();
    firstSeparatorPos = fontString.Find(_T(","));
    // Are there any separators?
    if (firstSeparatorPos != -1)
    {
        // Yes.
        // Parse typeface name
        if ((firstSeparatorPos == 1) && (fontString[0] == _TXCHAR('*')))
            typefaceName = "";
        else
            typefaceName = fontString.Left(firstSeparatorPos);
        secondSeparatorPos = fontString.Find(_T(","), firstSeparatorPos+1);

        // Parse the size and style information
        if (secondSeparatorPos != -1)
            sizeStyleInfo = fontString.Mid(firstSeparatorPos+1, secondSeparatorPos - firstSeparatorPos);
        else
            sizeStyleInfo = fontString.Mid(firstSeparatorPos+1);
        if (sizeStyleInfo != _T(""))
        {
            // Size - scale for relative fonts, point size for exact fonts
            // If a relative font, auto correct 0 to 1.
            fontSize = _tstof((LPCTSTR) sizeStyleInfo);
            if (fontSize == 0)
            {
                if (typefaceName == _T(""))
                    fontSize = 1;
                else
                    returnValHR = E_FAIL;
            }
            if (fontSize < 0)
            {
                fontSize = 1;
                returnValHR = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
            }

            // Bold, italic, underline
            if ((_tcschr(sizeStyleInfo, _TXCHAR('b')) != NULL) || (_tcschr(sizeStyleInfo, _TXCHAR('B')) != NULL))
                isBold = true;
            if ((_tcschr(sizeStyleInfo, _TXCHAR('i')) != NULL) || (_tcschr(sizeStyleInfo, _TXCHAR('I')) != NULL))
                isItalic = true;
            if ((_tcschr(sizeStyleInfo, _TXCHAR('u')) != NULL) || (_tcschr(sizeStyleInfo, _TXCHAR('U')) != NULL))
                isUnderline = true;
        }
        else
        {
            // Bad grammer, but we know what they mean
        }

        // Parse color information
        if (secondSeparatorPos == -1)
        {
            useDefaultColor = true;
        }
        else
        {
            useDefaultColor = false;
            colorInfo = fontString.Mid(secondSeparatorPos+1);

            if (_stscanf_s(colorInfo, _T("%d/%d/%d"), &redValue, &greenValue, &blueValue) == 3)
                textColor = RGB(redValue, greenValue, blueValue);
            else
                returnValHR = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
        }
    }
    else
    {
        // No.  Is the typeface the default?
        if (fontString == _T("*"))
        {
            // Yes, we're done, just use the current font
            typefaceName = "";
            fontSize = 1;
            isBold = false;
            isItalic = false;
            isUnderline = false;
            useDefaultColor = true;
        }
        else
        {
            returnValHR = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
        }
    }

    return returnValHR;
}

HRESULT DialogFont::CreateFont( CDC *drawContext, CFont *newFont )
// Sets *newFont equal to the font described by this object.  Releases any existing font
// contained by newFont.
{
    LOGFONT         newLogFont      = {0};
    HRESULT         returnValHR     = E_FAIL;

    // Validate parameters
	if (drawContext == NULL)
		return E_POINTER;
    if (newFont == NULL)
        return E_POINTER;

	returnValHR = CreateLOGFONT(drawContext->GetCurrentFont(), drawContext, &newLogFont);
    if (SUCCEEDED(returnValHR))
    {
        // Delete any existing font and create the new one.
        if ((HFONT) newFont != NULL)
            newFont->DeleteObject();
        if (newFont->CreateFontIndirect(&newLogFont))
            returnValHR = S_OK;
        else
            returnValHR = E_FAIL;
    }

    return returnValHR;
}

HRESULT DialogFont::CreateFont( CWnd* baseFontWindow, CFont *newFont )
// As above, but uses CWnd's GetFont for the base font
{
    LOGFONT         newLogFont      = {0};
    HRESULT         returnValHR     = E_FAIL;

    // Validate parameters
    if (baseFontWindow == NULL)
        return E_POINTER;
	if (newFont == NULL)
		return E_POINTER;

	returnValHR = CreateLOGFONT(baseFontWindow->GetFont(), baseFontWindow->GetDC(), &newLogFont);
    if (SUCCEEDED(returnValHR))
    {
        // Delete any existing font and create the new one.
        if ((HFONT) newFont != NULL)
            newFont->DeleteObject();
        if (newFont->CreateFontIndirect(&newLogFont))
            returnValHR = S_OK;
        else
            returnValHR = E_FAIL;
    }

    return returnValHR;
}

HRESULT DialogFont::CreateLOGFONT( CFont* baseFont, CDC* baseFontDC, LOGFONT* thisFontInfo )
// Sets *thisFontInfo equal to a LOGFONT describing the font described by this object.
{
    // Validate parameters
    if (baseFont == NULL)
        return E_POINTER;
    if (thisFontInfo == NULL)
        return E_POINTER;

    // Should we use the dialog font as the base font for this control?
    if (typefaceName == _T(""))
    {
        // Yes
        baseFont->GetLogFont(thisFontInfo);
        // Scale only if a scaling value has been set.
        if (fontSize != 0)
            thisFontInfo->lfHeight = static_cast<LONG>(thisFontInfo->lfHeight * fontSize);
        thisFontInfo->lfWidth = 0;
    }
    else
    {
        // No, use a user-specified typeface
        StringCchCopy(thisFontInfo->lfFaceName, LF_FACESIZE, (LPCTSTR) typefaceName);
        thisFontInfo->lfHeight = -MulDiv(static_cast<int>(fontSize), GetDeviceCaps(baseFontDC->GetSafeHdc(), LOGPIXELSY), 72);
    }
    if (isBold)
        thisFontInfo->lfWeight = FW_BOLD;
    if (isItalic)
        thisFontInfo->lfItalic = TRUE;
    if (isUnderline)
        thisFontInfo->lfUnderline = TRUE;

    return S_OK;
}

HRESULT DialogFont::CreateUpdateFont( CDC* drawContext, CFont* currFontInUse )
// Compares the font described by this object to *newFont.  If they are the same, does
// nothing, if they are different or newFont is not yet attached to a font, releases
// the font it is attached to and creates a new font for it as described by this object.
// Returns S_OK if new font created, S_FALSE if fonts equal, else the error code of the failure.
{
    LOGFONT     currFontInUseInfo       = {0};
    CFont       currStateFont;
    LOGFONT     currStateFontInfo       = {0};
    bool        createFont              = false;
    HRESULT     returnValHR             = E_FAIL;

    // Validate parameters
    if (drawContext == NULL)
        return E_POINTER;
    if (drawContext == NULL)
        return E_POINTER;
    if (currFontInUse == NULL)
        return E_POINTER;

    // Assume we have to create a new font
    createFont = true;
    // Is there a current in-use font?
    if ((HFONT) *currFontInUse != NULL)
    {
        // Yes.  Can we get it's LOGFONT descriptor?
        if (currFontInUse->GetLogFont(&currFontInUseInfo))
        {
            // Yes.  Now get a LOGFONT representing our current state
            if (SUCCEEDED(CreateLOGFONT(drawContext->GetCurrentFont(), drawContext, &currStateFontInfo)))
            {
                // Is the font we represent equal to the currently in-use font?
                if (IsLOGFONTEqual(currFontInUseInfo, currStateFontInfo) == true)
                {
                    // Yes, no need to re-create
                    createFont = false;
                }
            }
        }
    }

    if (createFont)
    {
        if (((HFONT) *currFontInUse) != NULL)
            currFontInUse->DeleteObject();
        returnValHR = CreateFont(drawContext, currFontInUse);
    }
    else
    {
        returnValHR = S_FALSE;
    }

    return returnValHR;
}

bool DialogFont::IsLOGFONTEqual( LOGFONT& fontA, LOGFONT& fontB)
// Returns TRUE if fontA == fontB, else FALSE.  Font names comparison is case insensitive.
{
    bool returnValBool = true;

    // Compare the non-string area of the logfont
    returnValBool = (memcmp(&fontA, &fontB, sizeof(LOGFONT) - (sizeof(TCHAR)*LF_FACESIZE)) == 0);
    if (returnValBool)
        returnValBool = (_tcsicmp(fontA.lfFaceName, fontB.lfFaceName) == 0);

    return returnValBool;
}

bool DialogFont::operator==( const DialogFont& comparatorFont )
{
    bool returnValBool = true;

    returnValBool &= (typefaceName.CompareNoCase(comparatorFont.typefaceName) == 0);
    returnValBool &= (fontSize == comparatorFont.fontSize);
    returnValBool &= (isBold == comparatorFont.isBold);
    returnValBool &= (isItalic == comparatorFont.isItalic);
    returnValBool &= (isUnderline == comparatorFont.isUnderline);
    returnValBool &= (useDefaultColor == comparatorFont.useDefaultColor);
    returnValBool &= (textColor == comparatorFont.textColor);

    return returnValBool;
}

bool DialogFont::operator!=( const DialogFont& comparatorFont )
{
    return !operator==(comparatorFont);
}