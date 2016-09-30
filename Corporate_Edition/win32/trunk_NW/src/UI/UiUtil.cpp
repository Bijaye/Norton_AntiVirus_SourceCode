#include "stdafx.h"
#include "UiUtil.h"
#include <afxpriv.h>

BOOL CControlParentSheet::OnInitDialog()
{
    BOOL returnValBOOL = FALSE;
    CTabCtrl* tabCtrl;
    returnValBOOL = CPropertySheet::OnInitDialog();
    tabCtrl = GetTabControl();
    tabCtrl->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
    ModifyStyleEx(0, WS_EX_CONTROLPARENT);
    return returnValBOOL;
};

void CControlParentSheet::SetParent( CWnd* newParent )
{
    m_pParentWnd = newParent;
}

void CControlParentSheet::SetCaption( LPCTSTR caption )
{
    m_strCaption = caption;
    m_psh.pszCaption = caption;
    if (caption != NULL)
        m_psh.dwFlags &= ~PSH_PROPTITLE;
    else
        m_psh.dwFlags |= PSH_PROPTITLE; 
}

HRESULT LoadImageResource( HINSTANCE resourceModuleHandle, LPCTSTR resourceName, LPCTSTR resourceType, Gdiplus::Bitmap** gdiplusBitmap )
// Loads a JPG, GIF, PNG, or TIFF file from the specified resource and sets *gdiplusBitmap equal to a Gdiplus::Bitmap object
// representing the specified resource.  Note this does NOT work with bitmap resources stored as type bitmap.
{
    HRSRC       resourceHandle      = NULL;
    DWORD       dataSize            = NULL;
    void*       resourceData        = NULL;
    HGLOBAL     globalDataHandle    = NULL;
    void*       globalDataBuffer    = NULL;
    IStream*    dataStream          = NULL;
    HRESULT     returnValHR         = E_FAIL;

    // Load the resource
    resourceHandle = ::FindResource(resourceModuleHandle, resourceName, resourceType);
    if (resourceHandle == NULL)
        return HRESULT_FROM_WIN32(GetLastError());
    dataSize = ::SizeofResource(resourceModuleHandle, resourceHandle);
    if (dataSize == 0)
        return HRESULT_FROM_WIN32(GetLastError());
    resourceData = ::LockResource(::LoadResource(resourceModuleHandle, resourceHandle));
    if (resourceData == NULL)
        return HRESULT_FROM_WIN32(GetLastError());

    // Allocate Global memory for the IStream
    globalDataHandle  = ::GlobalAlloc(GMEM_MOVEABLE, dataSize);
    if (globalDataHandle != NULL)
    {
        globalDataBuffer = ::GlobalLock(globalDataHandle);
        if (globalDataBuffer != NULL)
        {
            // Copy resource data to the HGLOBAL, create the stream, and finally the Gidplus::Bitmap
            CopyMemory(globalDataBuffer, resourceData, dataSize);

            returnValHR = ::CreateStreamOnHGlobal(globalDataHandle, TRUE, &dataStream);
            if (SUCCEEDED(returnValHR))
            {
                *gdiplusBitmap = Gdiplus::Bitmap::FromStream(dataStream);
                dataStream->Release();
                dataStream = NULL;

                if (*gdiplusBitmap != NULL)
                { 
                    if ((*gdiplusBitmap)->GetLastStatus() == Gdiplus::Ok)
                    {
                        returnValHR = S_OK;
                    }
                    else
                    {
                        delete *gdiplusBitmap;
                        *gdiplusBitmap = NULL;
                    }
                }
                else
                {
                    returnValHR = E_FAIL;
                }
            }
            ::GlobalUnlock(globalDataHandle);
        }
        else
        {
            returnValHR = HRESULT_FROM_WIN32(GetLastError());
        }
        // Do NOT free unless we failed - the Gdiplus::Bitmap owns it now
        // (Or, more likely, the IStream).
        if (FAILED(returnValHR))
            ::GlobalFree(globalDataHandle);
        globalDataHandle = NULL;
    }
    else
    {
        returnValHR = HRESULT_FROM_WIN32(GetLastError());
    }
    return returnValHR;
}

HRESULT LoadImageResource( HINSTANCE resourceModuleHandle, LPCTSTR resourceName, LPCTSTR resourceType, HBITMAP* bitmapHandle )
// As LoadImageResource, but returns an HBITMAP the caller now owns.
{
    Gdiplus::Bitmap*        gdiplusBitmap   = NULL;
    Gdiplus::Status         returnValStatus = Gdiplus::GenericError;
    HRESULT                 returnValHR     = E_FAIL;

    returnValHR = LoadImageResource(resourceModuleHandle, resourceName, resourceType, &gdiplusBitmap);
    if (SUCCEEDED(returnValHR))
    {
        returnValStatus = gdiplusBitmap->GetHBITMAP(RGB(0,0,0), bitmapHandle);
        returnValHR = GdiplusStatusToHresult(returnValStatus);
    }
    return returnValHR;

}

HRESULT LoadImageResource( HINSTANCE resourceModuleHandle, LPCTSTR resourceName, LPCTSTR resourceType, CImage* image )
// As LoadImageResource, but returns a CImage the caller now owns.
{
    HBITMAP                 bitmapHandle    = NULL;
    HRESULT                 returnValHR     = E_FAIL;

    returnValHR = LoadImageResource(AfxGetResourceHandle(), resourceName, resourceType, &bitmapHandle);
    if (SUCCEEDED(returnValHR) && (bitmapHandle != NULL))
        image->Attach(bitmapHandle);
    return returnValHR;

}

HRESULT LoadImageResource( HINSTANCE resourceModuleHandle, UINT resourceID, LPCTSTR resourceType, CImage* image )
// As LoadImageResource, but returns a CImage the caller now owns.
{
    HBITMAP                 bitmapHandle    = NULL;
    HRESULT                 returnValHR     = E_FAIL;

    returnValHR = LoadImageResource(AfxGetResourceHandle(), MAKEINTRESOURCE(resourceID), resourceType, &bitmapHandle);
    if (SUCCEEDED(returnValHR) && (bitmapHandle != NULL))
        image->Attach(bitmapHandle);
    return returnValHR;

}

HRESULT GdiplusStatusToHresult( Gdiplus::Status statusCode )
// Converts the passed GDI+ status code to an HRESULT
{
    HRESULT     returnValHR     = E_FAIL;

    switch (statusCode)
    {
    case Gdiplus::Ok:
        returnValHR = S_OK;
        break;

    case Gdiplus::GenericError:
        returnValHR = E_FAIL;
        break;
    // Note this is returned by constructors when they fail regardless of actual reason
    case Gdiplus::InvalidParameter:
        returnValHR = E_FAIL;
        break;
    case Gdiplus::OutOfMemory:
        returnValHR = E_OUTOFMEMORY;
        break;
    case Gdiplus::ObjectBusy:
        returnValHR = HRESULT_FROM_WIN32(ERROR_BUSY);
        break;
    case Gdiplus::InsufficientBuffer:
        returnValHR = E_FAIL;
        break;
    case Gdiplus::NotImplemented:
        returnValHR = E_NOTIMPL;
        break;
    case Gdiplus::Win32Error:
        returnValHR = E_FAIL;
        break;
    case Gdiplus::WrongState:
        returnValHR = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
    case Gdiplus::Aborted:
        returnValHR = E_ABORT;
        break;
    case Gdiplus::FileNotFound:
        returnValHR = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        break;
    case Gdiplus::ValueOverflow:
        returnValHR = HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);
        break;
    case Gdiplus::AccessDenied:
        returnValHR = E_ACCESSDENIED;
        break;
    case Gdiplus::UnknownImageFormat:
        returnValHR = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
        break;
    case Gdiplus::FontFamilyNotFound:
        returnValHR = E_FAIL;
        break;
    case Gdiplus::FontStyleNotFound:
        returnValHR = E_FAIL;
        break;
    case Gdiplus::NotTrueTypeFont:
        returnValHR = E_FAIL;
        break;
    case Gdiplus::UnsupportedGdiplusVersion:
        returnValHR = E_FAIL;
        break;
    case Gdiplus::GdiplusNotInitialized:
        returnValHR = E_FAIL;
        break;
    case Gdiplus::PropertyNotFound:
        returnValHR = E_FAIL;
        break;
    case Gdiplus::PropertyNotSupported:
        returnValHR = E_FAIL;
        break;
    default:
        returnValHR = E_FAIL;
        break;
    }

    return returnValHR;
}

void DrawTransparent( CDC* drawDC, CBitmap* bitmapToDraw, COLORREF transparencyColor, int drawX, int drawY, int newWidth, int newHeight )
// Draws bitmapToDraw with transparency onto drawDC.  transparencyColor is the key color for transparency.
// drawX and drawY are the start locations to draw in drawDC
// If newWidth or newHeight are 0, uses the values from bitmapToDraw
{
    COLORREF    oldBackgroundColor      = RGB(0,0,0);
    COLORREF    oldTextColor            = RGB(0,0,0);
    CDC         imageDC;
    CDC         maskTransferDC;
    CBitmap*    oldBitmap               = NULL;
    CBitmap     maskTransferBitmap;
    CBitmap*    oldmaskTransferBitmap   = NULL;
    BITMAP      bitmapToDrawInfo        = {0};

    // Initialize, create DCs for image and mask
    oldBackgroundColor = drawDC->SetBkColor(RGB(255,255,255));
    oldTextColor = drawDC->SetTextColor(RGB(0,0,0));
    imageDC.CreateCompatibleDC(drawDC);
    maskTransferDC.CreateCompatibleDC(drawDC);

    // Select the image into the image DC
    oldBitmap = imageDC.SelectObject(bitmapToDraw);

    // Create the mask bitmap
    bitmapToDraw->GetBitmap(&bitmapToDrawInfo);
    if (newWidth == 0)
        newWidth = bitmapToDrawInfo.bmWidth;
    if (newHeight == 0)
        newHeight = bitmapToDrawInfo.bmHeight;
    maskTransferBitmap.CreateBitmap(newWidth, newHeight, 1, 1, NULL);

    // Select the mask bitmap into the appropriate dc
    oldmaskTransferBitmap = maskTransferDC.SelectObject(&maskTransferBitmap);

    // Build mask based on transparent colour
    imageDC.SetBkColor(transparencyColor);
    maskTransferDC.StretchBlt(0, 0, newWidth, newHeight, &imageDC, 0, 0, bitmapToDrawInfo.bmWidth, bitmapToDrawInfo.bmHeight, SRCCOPY);

    // Do the work - True Mask method - cool if not actual display
    drawDC->StretchBlt(drawX, drawY, newWidth, newHeight, &imageDC, 0, 0, bitmapToDrawInfo.bmWidth, bitmapToDrawInfo.bmHeight, SRCINVERT);
    drawDC->BitBlt(drawX, drawY, newWidth, newHeight, &maskTransferDC, 0, 0, SRCAND);
    drawDC->StretchBlt(drawX, drawY, newWidth, newHeight, &imageDC, 0, 0, bitmapToDrawInfo.bmWidth, bitmapToDrawInfo.bmHeight, SRCINVERT);

    // Restore settings
    imageDC.SelectObject(oldBitmap);
    maskTransferDC.SelectObject(oldmaskTransferBitmap);
    drawDC->SetBkColor(oldBackgroundColor);
    drawDC->SetTextColor(oldTextColor);
}

void DrawTransparent( CDC* drawDC, CBitmap* bitmapToDraw, COLORREF transparencyColor, int newWidth, int newHeight )
// As DrawTransparent, but always draws at 0,0
{
    DrawTransparent(drawDC, bitmapToDraw, transparencyColor, 0, 0, newWidth, newHeight);
}

void DrawTransparent( CDC* drawDC, CBitmap* bitmapToDraw, int newWidth, int newHeight )
// As DrawTranspareny, but automatically extracts transparency color from top left of bitmap
// and always draws at 0,0
{
    CDC         memoryDC;
    CBitmap*    oldBitmap           = NULL;
    COLORREF    transparencyColor   = RGB(0,0,0);

    memoryDC.CreateCompatibleDC(drawDC);
    oldBitmap = memoryDC.SelectObject(bitmapToDraw);
    transparencyColor = memoryDC.GetPixel(0, 0);
    memoryDC.SelectObject(oldBitmap);
    DrawTransparent(drawDC, bitmapToDraw, transparencyColor, 0, 0, newWidth, newHeight);
}

void DrawTransparent( CDC* drawDC, CImage* imageToDraw, COLORREF transparencyColor, int drawX, int drawY, int newWidth, int newHeight )
// As DrawTransparent, but works with CImages
{
    CBitmap imageCB;

    imageCB.Attach(*imageToDraw);
    DrawTransparent(drawDC, &imageCB, transparencyColor, drawX, drawY, newWidth, newHeight);
    imageCB.Detach();
}

void DrawTransparent( CDC* drawDC, CImage* imageToDraw, int newWidth, int newHeight )
// As DrawTransparent, but works with CImages, automatically determines the transparency
// color from the 1st pixel in the image, and draws at 0,0
{
    CBitmap imageCB;

    imageCB.Attach(*imageToDraw);
    DrawTransparent(drawDC, &imageCB, imageToDraw->GetPixel(0, 0), newWidth, newHeight);
    imageCB.Detach();
}

CTransparentBackground::CTransparentBackground( CWnd* thisControl ) : control(thisControl), backgroundDColdBitmap(NULL)
{
    // Nothing needed
}

CTransparentBackground::~CTransparentBackground()
{
    // Cleanup
    if (static_cast<HBITMAP>(backgroundBitmap) != NULL)
        backgroundBitmap.DeleteObject();
    if (static_cast<HDC>(backgroundDC) != NULL)
        backgroundDC.SelectObject(backgroundDColdBitmap);
}

void CTransparentBackground::DrawTransparentBackground( CDC* drawDC )
{
    CClientDC       parentDC(control->GetParent());
    CRect           controlRectInParent;

    // Initialize
    control->GetWindowRect(controlRectInParent);
    control->GetParent()->ScreenToClient(controlRectInParent);
    
    if (backgroundDC.m_hDC == NULL)
    {
        backgroundDC.CreateCompatibleDC(&parentDC);
        backgroundBitmap.CreateCompatibleBitmap(&parentDC, controlRectInParent.Width(), controlRectInParent.Height());
        backgroundDColdBitmap = backgroundDC.SelectObject(&backgroundBitmap);
        backgroundDC.BitBlt(0, 0, controlRectInParent.Width(), controlRectInParent.Height(), &parentDC, controlRectInParent.left, controlRectInParent.top, SRCCOPY);
    }
    drawDC->BitBlt(0, 0, controlRectInParent.Width(), controlRectInParent.Height(), &backgroundDC, 0, 0, SRCCOPY);
}

HRESULT SwitchViewInFrame( CFrameWnd* targetFrameWindow, CRuntimeClass* newViewClass )
// Switches the view displayed in targetFrameWindow from whatever it is to newViewClass
// Active view is destroyed and replaced
{
    CView*          activeView              = NULL;
    CRect           viewPosition;
    CDocument*      currDocument            = NULL;
    BOOL            currDocumentAutoDelete  = FALSE;
    CCreateContext  newViewCreationContext;
    CView*          newView                 = NULL;

    // Validate parameters
    if (targetFrameWindow == NULL)
        return E_POINTER;
    if (newViewClass == NULL)
        return E_POINTER;

    // Is there an active frame that is different from the specified one?
    activeView = targetFrameWindow->GetActiveView();
    if (activeView != NULL)
    {
        if (activeView->IsKindOf(newViewClass))
            return S_FALSE;
    }

    // Initialize
    activeView->GetWindowRect(&viewPosition);
    currDocument = activeView->GetDocument();
    currDocumentAutoDelete = currDocument->m_bAutoDelete;

    // Destroy the current view, making sure the document doesn't auto delete itself
    currDocument->m_bAutoDelete = FALSE;
    activeView->DestroyWindow();
    activeView = NULL;
    currDocument->m_bAutoDelete = currDocumentAutoDelete;

    // Create the new view
    newViewCreationContext.m_pNewDocTemplate    = NULL;
    newViewCreationContext.m_pLastView          = NULL;
    newViewCreationContext.m_pLastView          = NULL;
    newViewCreationContext.m_pNewViewClass      = newViewClass;
    newViewCreationContext.m_pCurrentDoc        = currDocument;
    newView = static_cast<CView*>(targetFrameWindow->CreateView(&newViewCreationContext));

    // Move the new view and update it
    targetFrameWindow->ScreenToClient(&viewPosition);
    newView->MoveWindow(viewPosition, TRUE);
    targetFrameWindow->InitialUpdateFrame(currDocument, TRUE);

    return S_OK;
}

void ScaleSizeFitKeepAspectRatio( CRect sourceRect, CRect targetRect, CRect* scaledRect )
// Takes sourceRect and enlarges/reduces it to fully fit into targetRect while preserving
// it's aspect ratio
{
    int newWidth = 0;
    int newHeight = 0;

    // Initialize
    scaledRect->top     = 0;
    scaledRect->left    = 0;
    scaledRect->right   = targetRect.Width();
    scaledRect->bottom  = sourceRect.Height();

    // Is sourceRect the same size as targetRect?
    if ((targetRect.Width() != sourceRect.Width()) || (targetRect.Height() != sourceRect.Height()))
    {
        // No.  First try maximum width to fit into targetRect
        scaledRect->bottom = (sourceRect.Height()*targetRect.Width())/sourceRect.Width();
        // Does the image fit in the rect?
        if (scaledRect->bottom > targetRect.Height())
        {
            // No, fit to maximum height instead
            scaledRect->right = (sourceRect.Width()*targetRect.Height())/sourceRect.Height();
            scaledRect->bottom = targetRect.Height();
        }
    }
}