// StringHelper.h: interface for the CStringHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGHELPER_H__BC0DB3C8_FA46_458F_8D8B_F503FA3D97C3__INCLUDED_)
#define AFX_STRINGHELPER_H__BC0DB3C8_FA46_458F_8D8B_F503FA3D97C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStringHelper  
{
public:
	LPCTSTR LoadString ( UINT uID );
	CStringHelper( HINSTANCE hResourceInstance);
	virtual ~CStringHelper();

protected:
    char m_szTemp [256];
    HINSTANCE m_hResourceInstance;
};

#endif // !defined(AFX_STRINGHELPER_H__BC0DB3C8_FA46_458F_8D8B_F503FA3D97C3__INCLUDED_)
