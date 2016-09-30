// Copyright 1992 - 1994 Symantec, Peter Norton Group
// ************************************************************************
//
// $Header:   S:/NAVWINX5/VCS/NAVVIEW.H_v   1.0   01 Jun 1998 16:08:38   DBuches  $
//
// Description:
//
// See Also:
//
// ************************************************************************
// $Log:   S:/NAVWINX5/VCS/NAVVIEW.H_v  $
// 
//    Rev 1.0   01 Jun 1998 16:08:38   DBuches
// Initial revision.
// 
//    Rev 1.0   01 Mar 1998 18:50:24   pagbabi
// Initial revision.
// ************************************************************************

#ifndef _NAVVIEW_H_
#define _NAVVIEW_H_

class CNavWinView : public CNodeView
{

public:
    CNavWinView();

    // IExtendContextMenu interface
	STDMETHOD(AddMenuItems) (LPDATAOBJECT, LPCONTEXTMENUCALLBACK, long*);
	STDMETHOD(Command) (long, LPDATAOBJECT);

    // Pure override
    STDMETHOD(DoDescription)( LPDATAOBJECT pDataObject );

    // helpers
    CNavWinNode* GetNode() { return dynamic_cast<CNavWinNode*>(m_pNode); }

private:

};

#endif
