// HotPathTabView.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"
#include "CodeProphetViewDoc.h"
#include "ColumnTreeView.h"
#include "HotPathTabView.h"
#include "HotPathView.h"


// CHotPathTabView

IMPLEMENT_DYNCREATE(CHotPathTabView, CTabView)

CHotPathTabView::CHotPathTabView()
{

}

CHotPathTabView::~CHotPathTabView()
{
}

BEGIN_MESSAGE_MAP(CHotPathTabView, CTabView)
	ON_WM_CREATE()
END_MESSAGE_MAP()


int CHotPathTabView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabView::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetTabControl().ModifyTabStyle(CMFCTabCtrl::STYLE_3D_ONENOTE);
	GetTabControl().SetLocation(CMFCBaseTabCtrl::LOCATION_BOTTOM);
	GetTabControl().SetActiveTabBoldFont(TRUE);

	return 0;
}


void CHotPathTabView::OnInitialUpdate()
{
	CTabView::OnInitialUpdate();

	int num_threads = GetDocument()->m_callgraph.num_threads;


	CCreateContext cc;
	cc.m_pNewViewClass = NULL;
	cc.m_pCurrentDoc = this->GetDocument();
	cc.m_pNewDocTemplate = NULL;
	cc.m_pLastView = NULL;
	cc.m_pCurrentFrame = NULL;

	for(int i = 0; i < num_threads; i++)
	{
		CString tabtxt;

		tabtxt.Format(_T("Thread %d"),i+1);
		this->AddView(RUNTIME_CLASS(CHotPathView),tabtxt, i, &cc);
		CWnd* curview = GetTabControl().GetTabWnd(i);
		curview->SendMessage(WM_SETVIEWID, 0, i);
	}
}


#ifndef _DEBUG  // debug version in CodeProphetAnalystView.cpp
inline CCodeProphetViewDoc* CHotPathTabView::GetDocument() const
{
	return reinterpret_cast<CCodeProphetViewDoc*>(m_pDocument);
}
#endif



// CHotPathTabView diagnostics

#ifdef _DEBUG
void CHotPathTabView::AssertValid() const
{
	CTabView::AssertValid();
}

#ifndef _WIN32_WCE
void CHotPathTabView::Dump(CDumpContext& dc) const
{
	CTabView::Dump(dc);
}
#endif

CCodeProphetViewDoc* CHotPathTabView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCodeProphetViewDoc)));
	return (CCodeProphetViewDoc*)m_pDocument;
}

#endif //_DEBUG


// CHotPathTabView message handlers
