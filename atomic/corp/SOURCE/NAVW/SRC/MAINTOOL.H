// Copyright 1998 Symantec Corporation. 
//*************************************************************************
//
// $Header:   S:/NAVW/VCS/maintool.h_v   1.0   01 Apr 1998 14:43:04   DALLEE  $
//
// Description:
//
// Contains:
//      Declaration CNavMainToolBarCtrl()
//*************************************************************************
// $Log:   S:/NAVW/VCS/maintool.h_v  $
// 
//    Rev 1.0   01 Apr 1998 14:43:04   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __MAINTOOL_H
#define __MAINTOOL_H

#ifndef __TOOLBAR_H
#include "ToolBar.h"
#endif

//
// CNavMainToolBarCtrl
//

typedef struct tagNAVTBBUTTON
{
    HINSTANCE   hInst;                  // Handle to resource module
    UINT        uBitmap;                // Normal bitmap ID
    UINT        uHotBitmap;             // Hot bitmap ID
    UINT        uString;                // Text string ID
    UINT        uID;                    // Command ID
    DWORD       dwUserData;             // Application data
} NAVTBBUTTON, *PNAVTBBUTTON, FAR *LPNAVTBBUTTON;


class CNavMainToolBarCtrl : public CNVToolBarCtrl
{
// Construction
public:
    CNavMainToolBarCtrl();

// Operations
public:
    int AddButton( LPNAVTBBUTTON lpButton );
    BOOL SetBitmapSize( int nWidth, int nHeight );

// Implementation
public:
    virtual ~CNavMainToolBarCtrl();

protected:
    // Image lists

    HIMAGELIST  m_hNormalImageList;
    HIMAGELIST  m_hHotImageList;

    SIZE    m_sizeImage;

    BOOL CreateImageLists();
    void DestroyImageLists();
};

#endif // !__MAINTOOL_H
