// allerCalleeTabView.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"
#include "CodeProphetViewDoc.h"
#include "ColumnTreeView.h"
#include "ClassView.h"
#include "ClassTabView.h"


// CallerCalleeTabView

IMPLEMENT_DYNCREATE(CClassTabView, CTabView)

CClassTabView::CClassTabView()
{

}

CClassTabView::~CClassTabView()
{
}

BEGIN_MESSAGE_MAP(CClassTabView, CTabView)
	ON_WM_CREATE()
END_MESSAGE_MAP()


int CClassTabView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabView::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetTabControl().ModifyTabStyle(CMFCTabCtrl::STYLE_3D_ONENOTE);
	GetTabControl().SetLocation(CMFCBaseTabCtrl::LOCATION_BOTTOM);
	GetTabControl().SetActiveTabBoldFont(TRUE);

	return 0;
}



void CClassTabView::OnInitialUpdate()
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
		this->AddView(RUNTIME_CLASS(CClassView),tabtxt, i, &cc);
		CWnd* curview = GetTabControl().GetTabWnd(i);
		curview->SendMessage(WM_SETVIEWID, 0, i);
	}
}






// CallerCalleeTabView diagnostics

#ifdef _DEBUG
void CClassTabView::AssertValid() const
{
	CTabView::AssertValid();
}

#ifndef _WIN32_WCE
void CClassTabView::Dump(CDumpContext& dc) const
{
	CTabView::Dump(dc);
}
#endif
#endif //_DEBUG


// CallerCalleeTabView message handlers



CCodeProphetViewDoc* CClassTabView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCodeProphetViewDoc)));
	return (CCodeProphetViewDoc*)m_pDocument;
}
