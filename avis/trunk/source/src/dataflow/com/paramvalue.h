// ParamValue.h: interface for the CParamValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARAMVALUE_H__4ACA6FC1_5162_11D2_B974_0004ACEC31AA__INCLUDED_)
#define AFX_PARAMVALUE_H__4ACA6FC1_5162_11D2_B974_0004ACEC31AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CParamValue : public CObject  
{
public:
    void ResetValues();
	BOOL WriteParamValue(FILE *fp, CString delim = ":");
	void AddParamValue(CString & key, CString & value);
	BOOL GetValue (CString & key, CString & value);
	int ReadParamValue(FILE * fp, BOOL addDefault = FALSE, BOOL updateDefault = FALSE, CString delim = ":=" );
	CStringArray m_Value;
	CStringArray m_Param;
	CStringArray m_DefaultValue;
	CStringArray m_DefaultParam;
	CParamValue();
	virtual ~CParamValue();

private:
	int GetNextParamValue (FILE * fp, CString & param, CString & value, CString & delim);
};

#endif // !defined(AFX_PARAMVALUE_H__4ACA6FC1_5162_11D2_B974_0004ACEC31AA__INCLUDED_)
