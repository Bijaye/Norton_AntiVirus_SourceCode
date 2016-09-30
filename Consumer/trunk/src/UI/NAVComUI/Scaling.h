////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//******************************************************************************
#pragma once

//#define SCALEX(argX) ((int) ((argX) * CScaling::scaleX) )
//#define SCALEY(argY) ((int) ((argY) * CScaling::scaleY) )

#define SCALEX(argX) (CScaling::ScaleX(argX))
#define SCALEY(argY) (CScaling::ScaleY(argY))

//------------------------------------------------------------------------------
//  @Author: CBROWN
//  @Created: 12:4:2002   21:10
//
//  @Purpose: For high DPI scaling support.
//------------------------------------------------------------------------------
class CScaling
{
public:
    CScaling(){ InitScaling(); }

    static void InitScaling()
    {
        HDC screen = GetDC(0);
        scaleX = GetDeviceCaps(screen, LOGPIXELSX) / 96.0;
        scaleY = GetDeviceCaps(screen, LOGPIXELSY) / 96.0;
        ReleaseDC(0, screen);
    }

    static int ScaleX( int arg )
    {
        double dMultiplier = scaleX;

        BOOL bResult = FALSE;

        HIGHCONTRAST hc = { sizeof(hc) };
        bResult = SystemParametersInfo( SPI_GETHIGHCONTRAST, 0, (PVOID)&hc, 0 );

        if( bResult && (hc.dwFlags & HCF_HIGHCONTRASTON)  )
            dMultiplier += 0.10;

        return (int)(arg*dMultiplier);
    }

    static int ScaleY( int arg )
    {
        double dMultiplier = scaleY;

        BOOL bResult = FALSE;

        HIGHCONTRAST hc = { sizeof(hc) };
        bResult = SystemParametersInfo( SPI_GETHIGHCONTRAST, 0, (PVOID)&hc, 0 );

        if( bResult && (hc.dwFlags & HCF_HIGHCONTRASTON) )
            dMultiplier += 0.10;

        return (int)(arg*dMultiplier);
    }

protected:

    static double scaleX;
    static double scaleY;
};

extern CScaling scaling;

