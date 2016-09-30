////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// WNDSIZE.H - Created - 09/27/96
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/WNDSIZE.H_V   1.2.1.0   22 Jun 1998 19:27:52   dwilson  $
//
// Description:
//    Header filr for the CWindowResize class.  This class was
//    implemented in order to attempt to make resizing of windows a
//    little bit easier of a chore.  The idea of this class was
//    that you would be able to simply register all of the
//    controls that you needed to be resized or moved with the window
//    resizer class upon initialization of your dialog.  After this,
//    you would simply have to call only one function upon resize
//    with the new width and height, and all of your controls would
//    be resized for you.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/WNDSIZE.H_V  $
// 
//    Rev 1.2.1.0   22 Jun 1998 19:27:52   dwilson
// Branch base for version QAKN
// 
//    Rev 1.2   20 Jan 1998 17:36:10   KSACKIN
// Updated the WndSize header to have a few new function such as
// a RegisterWindow function that takes a Control ID instead of a HWND.  This
// makes coding a little easier.  Also added a function to set the parent of
// the CWindowResizer class so that subsequent calls do not have to supply
// this information.
//
//    Rev 1.1   10 Dec 1997 20:17:18   KSACKIN
// Modified the API to not be exported, and also to not use MFC.  We really
// did not need to use MFC, and were only using it to shortcut some simple
// Windows functions, which we will now just call directly.
//
//    Rev 1.2   09 Dec 1997 20:33:16   KSACKIN
// Added the UseSizeGrip function to the WindowSizer class.  This allows for a
// user of this class to have a size grip in their dialog without having to
// create it themselves.
//
//    Rev 1.1   07 Oct 1997 17:25:26   ksackin
// Removed the AFX_EXT_CLASS definition of these classes.  They are no longer
// part of a DLL, but rather part of a library.
//
//    Rev 1.0   25 Jul 1997 13:33:22   llavall
// Initial revision.
//
//    Rev 1.2   23 May 1997 18:04:52   DBUCHES
// Added AFX_EXT_CLASS to class declarations.
//
//    Rev 1.1   14 May 1997 15:16:40   KSACKIN
// Removed the dependency on StdAfx.h.  This file should only be used for local
// project files, and not for header files.  Every MFC project should have it's
// own StdAfx.h file in it's source area.  This is not a shared header file.
//
//    Rev 1.0   24 Apr 1997 20:10:20   JBRENNA
// Initial revision.
//
//    Rev 1.0   13 Mar 1997 15:10:44   RFULLER
// Initial revision
//
//    Rev 1.1   09 Dec 1996 23:38:54   ksackin
// Added functionality to empty out the list when you are done with it.  This
// is nice if it is in a dialog that may get called several times before it
// destructs.  Otherwise, it will have controls from provious dialogs in it,
// which would assert.
//
//    Rev 1.0   20 Nov 1996 02:13:00   ksackin
// Initial revision.
//
//    Rev 1.0   27 Sep 1996 00:32:36   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__WNDSIZE_H)
#define __WNDSIZE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////////
// #defines.

#define NOT_INITIALIZED -1


/////////////////////////////////////////////////////////////////////
// Structures.

typedef struct tagWndSizeMultiplierStruct
{
    float xMultiplier;   // horizontal movement
    float yMultiplier;   // vertical movement
    float cxMultiplier;  // horizontal resizing
    float cyMultiplier;  // vertical resizing

} WNDSIZE_MULTIPLIER_STRUCT;


/////////////////////////////////////////////////////////////////////
// Function Prototypes.

WNDSIZE_MULTIPLIER_STRUCT *FillMultiplierStruct( WNDSIZE_MULTIPLIER_STRUCT *lprStruct,
                                                 float xMult,
                                                 float yMult,
                                                 float cxMult,
                                                 float cyMult
                                                 );


/////////////////////////////////////////////////////////////////////
// CControlContainer Class.

class CControlContainer
{
private:
    WNDSIZE_MULTIPLIER_STRUCT m_rMultiplierStruct;   // This will be for multiplying location and size
    POINT m_rOPos;  // original position.
    SIZE  m_rOSize; // original size.
    HWND  m_hWnd;
    DWORD m_dwSizeFlags;
    DWORD m_dwSWPFlags;  // SetWindowPos Flags.

public:
    CControlContainer( void );
    CControlContainer( HWND hWnd,
                       WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct,
                       DWORD dwSWPFlags = SWP_NOZORDER
                       );

    void InitSize( HWND hWnd );

    void MapToControl( HWND hWnd,
                       WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct,
                       DWORD dwSWPFlags = SWP_NOZORDER
                       );
    long GetXPos( void ) {return m_rOPos.x;};
    long GetYPos( void ) {return m_rOPos.y;};

    void UpdateControl( int nTotalDeltaX, int nTotalDeltaY );
};


/////////////////////////////////////////////////////////////////////
// CControl Container Node class.

class CControlContainerNode
{
private:
    CControlContainer m_cControlContainer;
    CControlContainerNode *m_lpNextContainerNodeWidth;
    CControlContainerNode *m_lpPrevContainerNodeWidth;
    CControlContainerNode *m_lpNextContainerNodeHeight;
    CControlContainerNode *m_lpPrevContainerNodeHeight;

public:
    CControlContainerNode( void );
    CControlContainerNode( HWND hWnd,
                           WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct,
                           DWORD dwSWPFlags = SWP_NOZORDER
                           );

    void MapToControl( HWND hWnd,
                       WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct,
                       DWORD dwSWPFlags = SWP_NOZORDER
                       );

    CControlContainerNode *GetNextControlNodeWidth( void ) {return m_lpNextContainerNodeWidth;};
    CControlContainerNode *GetNextControlNodeHeight( void ) {return m_lpNextContainerNodeHeight;};
    CControlContainerNode *GetPrevControlNodeWidth( void ) {return m_lpPrevContainerNodeWidth;};
    CControlContainerNode *GetPrevControlNodeHeight( void ) {return m_lpPrevContainerNodeHeight;};

    void SetNextControlNodeWidth( CControlContainerNode *lpNextContainerNodeWidth ) { m_lpNextContainerNodeWidth = lpNextContainerNodeWidth;};
    void SetNextControlNodeHeight( CControlContainerNode *lpNextContainerNodeHeight ) { m_lpNextContainerNodeHeight = lpNextContainerNodeHeight;};
    void SetPrevControlNodeWidth( CControlContainerNode *lpPrevContainerNodeWidth ) { m_lpPrevContainerNodeWidth = lpPrevContainerNodeWidth;};
    void SetPrevControlNodeHeight( CControlContainerNode *lpPrevContainerNodeHeight ) { m_lpPrevContainerNodeHeight = lpPrevContainerNodeHeight;};

    CControlContainer *GetControlContainer( void ) {return &m_cControlContainer; };
};


/////////////////////////////////////////////////////////////////////
// CControlContainerList class.

class CControlContainerList
{
private:
    CControlContainerNode *m_lpcTopNode;
    CControlContainerNode *m_lpcBottomNode;
    CControlContainerNode *m_lpcLeftNode;
    CControlContainerNode *m_lpcRightNode;

public:
    CControlContainerList( void );
    ~CControlContainerList( void );

    void RemoveAllItems( void );

    void AddControl( HWND  hWnd,
                     float xMultiplier,
                     float yMultiplier,
                     float cxMultiplier,
                     float cyMultiplier,
                     DWORD dwSWPFlags = SWP_NOZORDER
                     );

    void AddControl( HWND hWnd,
                     WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct,
                     DWORD dwSWPFlags = SWP_NOZORDER
                     );

    void InsertNodeHorizontal( CControlContainerNode *lpNextContainerNode );
    void InsertNodeVertical( CControlContainerNode *lpNextContainerNode );

    void UpdateControlsHorizontally( int nCurrentDeltaX, int nTotalDeltaX, int nTotalDeltaY );
    void UpdateControlsVertically( int nCurrentDeltaY, int nTotalDeltaY, int nTotalDeltaX );
};


/////////////////////////////////////////////////////////////////////
// CWindowResizer class.

class CWindowResizer
{
private:
    CControlContainerList m_cControlContainerList;
    BOOL m_bInitialized;
    HWND m_hParentWindow;
    HWND m_hSizeGrip;
    BOOL m_bUseSizeGrip;
    int  m_nCurrentWindowWidth;
    int  m_nCurrentWindowHeight;
    int  m_nTotalDeltaX;
    int  m_nTotalDeltaY;

public:
    CWindowResizer( void );

                                        // **** Must call this function ****
                                        // *********************************
                                        // This function will be used to
                                        // subclass the parent dialog so that
                                        // we can take care of certain window
                                        // messages behind the scene without
                                        // having a client call through to us
                                        // continuously.

    void AttachToParent(     // Function to attach this control to the parent.
        HWND hParentWindow   // Parent window.  Usually dialog main window.
        );

    void SetParent(     // Function to set this class's parent window.
        HWND hParentWindow   // Parent window.  Usually dialog main window.
        );

    void UpdateSize( UINT nType, int cx, int cy );

    void RegisterWindow( int   nControlID,
                         float xMultiplier,
                         float yMultiplier,
                         float cxMultiplier,
                         float cyMultiplier,
                         DWORD dwSWPFlags = SWP_NOZORDER
                         );

    void RegisterWindow( HWND  hControlWnd,
                         float xMultiplier,
                         float yMultiplier,
                         float cxMultiplier,
                         float cyMultiplier,
                         DWORD dwSWPFlags = SWP_NOZORDER
                         );

    void RegisterWindow( HWND hControlWnd,
                         WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct,
                         DWORD dwSWPFlags = SWP_NOZORDER
                         );

    void RegisterWindow( int  nControlID,
                         WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct,
                         DWORD dwSWPFlags = SWP_NOZORDER
                         );

     void UseSizeGrip(        // Call to add size grip to dialog.
         BOOL bUseSizeGrip    // Use SizeGrip?
         );

     void ShowSizeGrip( // Call to show or hide the size grip in a dialog.
         BOOL bShow     // Show SizeGrip?
         );

     void SetInitialized( BOOL bNewState );
     BOOL GetInitialized( void ) { return m_bInitialized;};

     void RemoveAllItems( void );   // Call to re-initialize, or called in destructor.

     WNDPROC m_lpfnParentWindowProc;
};


#endif // if !defined(__WNDSIZE_H)
