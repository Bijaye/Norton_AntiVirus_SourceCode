// apwUtilStressDoc.h : interface of the CapwUtilStressDoc class
//


#pragma once

class CapwUtilStressDoc : public CDocument
{
protected: // create from serialization only
	CapwUtilStressDoc();
	DECLARE_DYNCREATE(CapwUtilStressDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CapwUtilStressDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


