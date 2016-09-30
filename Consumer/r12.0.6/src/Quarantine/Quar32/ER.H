/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/er.h_v   1.0   25 Feb 1998 14:07:14   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// er.h - contains inline class definition for CEnsureRelease.  This helper class
//        is used to make sure COM objects are released.
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/er.h_v  $
// 
//    Rev 1.0   25 Feb 1998 14:07:14   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#ifndef _ENSUREREL_H
#define _ENSUREREL_H

class CEnsureRelease
{
public:
    CEnsureRelease( IUnknown* p )
        {
        m_pUnk = p;
        }
    ~CEnsureRelease()
        {
        if( m_pUnk )
            m_pUnk->Release();
        }
private:
    IUnknown* m_pUnk;
};

#endif
