// DFGuiDoc.h : interface of the CDFGuiDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DFGUIDOC_H__A244E3BC_5A46_11D2_A583_0004ACECC1E1__INCLUDED_)
#define AFX_DFGUIDOC_H__A244E3BC_5A46_11D2_A583_0004ACECC1E1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CDFGuiDoc : public CDocument
{
protected: // create from serialization only
	CDFGuiDoc();
	DECLARE_DYNCREATE(CDFGuiDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFGuiDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDFGuiDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDFGuiDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFGUIDOC_H__A244E3BC_5A46_11D2_A583_0004ACECC1E1__INCLUDED_)
