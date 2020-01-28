
// CodeProphetViewView.h : interface of the CCodeProphetViewView class
//


#pragma once


class CCodeProphetViewView : public CTabView
{
protected: // create from serialization only
	CCodeProphetViewView();
	DECLARE_DYNCREATE(CCodeProphetViewView)

// Attributes
public:
	CCodeProphetViewDoc* GetDocument() const;

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

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();


// Implementation
public:
	virtual ~CCodeProphetViewView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in CodeProphetViewView.cpp
inline CCodeProphetViewDoc* CCodeProphetViewView::GetDocument() const
   { return reinterpret_cast<CCodeProphetViewDoc*>(m_pDocument); }
#endif

