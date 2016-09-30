// apwUtilStressView.h : interface of the CapwUtilStressView class
//


#pragma once


class CapwUtilStressView : public CView
{
protected: // create from serialization only
	CapwUtilStressView();
	DECLARE_DYNCREATE(CapwUtilStressView)

// Attributes
public:
	CapwUtilStressDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CapwUtilStressView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in apwUtilStressView.cpp
inline CapwUtilStressDoc* CapwUtilStressView::GetDocument() const
   { return reinterpret_cast<CapwUtilStressDoc*>(m_pDocument); }
#endif

