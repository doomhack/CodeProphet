#pragma once

#include "LCPrint.h"

// CFunctionListView view

class CFunctionListView : public CListView
{
	DECLARE_DYNCREATE(CFunctionListView)

protected:
	CFunctionListView();           // protected constructor used by dynamic creation
	virtual ~CFunctionListView();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnInitialUpdate();

	afx_msg void OnFilePrintPreview();
	BOOL OnPreparePrinting(CPrintInfo* pInfo);
	void OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/);
	void OnPrint(CDC* /*pDC*/, CPrintInfo* /*pInfo*/);
	void OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/);

	void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg LRESULT OnSetViewID(WPARAM wParam, LPARAM lParam);

	void OnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult);

	unsigned int mythread;

	LCPrinting lcp;

public:
	CCodeProphetViewDoc* GetDocument() const;

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};

//Custom Messages
#define WM_SETVIEWID (WM_USER + 100)
