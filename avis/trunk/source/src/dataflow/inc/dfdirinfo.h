// DFDirInfo.h: interface for the DFDirInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DFDIRINFO_H__87E2C1C5_C5EA_11D2_A4C1_0004ACECC1E1__INCLUDED_)
#define AFX_DFDIRINFO_H__87E2C1C5_C5EA_11D2_A4C1_0004ACECC1E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DFDirInfo  
{
public:
	DFDirInfo(CString dirPath, CTime time);
	virtual ~DFDirInfo();
    CString dirPath;
	CTime   dirTime;
};

#endif // !defined(AFX_DFDIRINFO_H__87E2C1C5_C5EA_11D2_A4C1_0004ACECC1E1__INCLUDED_)

// collection of dir objects
typedef CTypedPtrList<CPtrList, DFDirInfo*> DFDirList;  
