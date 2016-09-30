////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//***************************************************************************
// [CBROWN 7/22/2002] 
//***************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************
// Helper Class to select and unselect a GDI Objects in 
// Construction and Deconstruction
//***************************************************************************
template <typename Type>
class CPushGDI
{
public:
    CPushGDI(HDC hDC, Type Object) : m_hDC(hDC), 
        m_hOldObject(static_cast<Type>(::SelectObject(m_hDC, Object)))
    {
    }
    
    ~CPushGDI()
    {
        ::SelectObject(m_hDC, m_hOldObject);
    }
private:
    HDC     m_hDC;
    Type    m_hOldObject;
};