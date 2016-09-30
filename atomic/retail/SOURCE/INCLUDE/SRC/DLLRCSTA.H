/////////////////////////////////////////////////////////////////////
// DLLRCSTA.H - Created - 11/12/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/DLLRCSTA.H_v   1.0   24 Apr 1997 20:43:00   JBRENNA  $
//
// Description:
//    This file is used to contain switching information for MFC
//    classes to maintain their module resource states.  These have
//    beein inplemented by way of classes that you can declare that
//    will automatically switch the state for you, and then switch
//    it back when that class goes out of scope.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/DLLRCSTA.H_v  $
// 
//    Rev 1.0   24 Apr 1997 20:43:00   JBRENNA
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:10:42   RFULLER
// Initial revision
// 
//    Rev 1.0   12 Nov 1996 22:53:56   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__DLLRCSTA_H)
#define __DLLRCSTA_H


/////////////////////////////////////////////////////////////////////
// External symbols

                                        // This symbol is used to hold the
                                        // state information for the DLL that
                                        // includes this file.  Every dll that
                                        // includes this, needs to have this
                                        // state variable.

extern  AFX_EXTENSION_MODULE extMyExtension;


/////////////////////////////////////////////////////////////////////
// CDLLResourceState class.


/*******************************************************************************
                          A Note About this section...

    I originally did not want to do this in this method.  I am refering to
    accessing the afxCurrentResourceHandle directly.  The reason why I have
    done this, however, is that the MFC library asserts if you are trying to
    set this resource to NULL, and likewise, when you are getting this value,
    and the current value is NULL.  This is neccessary for us to circumvent
    because we may get called from an EXE that is unaware of MFC, and
    therefore, not have their current resource handle selected.  In this case,
    the library would have asserted.

*******************************************************************************/

class CDLLResourceState
{
public:

	CDLLResourceState()
		{
//        m_hinstOld = AfxGetResourceHandle();
        m_hinstOld = afxCurrentResourceHandle;
        SetToDll();
        };

	~CDLLResourceState()
		{SetBack();};
	void SetBack()
		{afxCurrentResourceHandle = m_hinstOld;};
	void SetToDll()
		{AfxSetResourceHandle(extMyExtension.hResource);};
private:
	HINSTANCE m_hinstOld;
};


#endif // if !defined(__DLLRCSTA_H)
