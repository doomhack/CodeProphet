#pragma once

#include "LCPrint.h"

typedef struct itemdata
{
	CString prop;
	CString val;
} itemdata;

// CProfileSummaryView view

class CProfileSummaryView : public CListView
{
	DECLARE_DYNCREATE(CProfileSummaryView)

protected:
	CProfileSummaryView();           // protected constructor used by dynamic creation
	virtual ~CProfileSummaryView();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnInitialUpdate();

	CCodeProphetViewDoc* GetDocument() const;

	afx_msg void OnFilePrintPreview();
	BOOL OnPreparePrinting(CPrintInfo* pInfo);
	void OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/);
	void OnPrint(CDC* /*pDC*/, CPrintInfo* /*pInfo*/);
	void OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/);

	void OnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	itemdata *items[6];

	LCPrinting lcp;

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


