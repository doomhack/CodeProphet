// FunctionListTabView.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"
#include "CodeProphetViewDoc.h"
#include "FunctionListTabView.h"
#include "FunctionListView.h"

// CFunctionListTabView

IMPLEMENT_DYNCREATE(CFunctionListTabView, CTabView)

CFunctionListTabView::CFunctionListTabView()
{
}

CFunctionListTabView::~CFunctionListTabView()
{
}

BEGIN_MESSAGE_MAP(CFunctionListTabView, CTabView)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CFunctionListTabView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabView::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetTabControl().ModifyTabStyle(CMFCTabCtrl::STYLE_3D_ONENOTE);
	GetTabControl().SetLocation(CMFCBaseTabCtrl::LOCATION_BOTTOM);
	GetTabControl().SetActiveTabBoldFont(TRUE);

	return 0;
}

void CFunctionListTabView::OnInitialUpdate()
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

		if(i > 255)
		{
			if (AfxMessageBox(_T("The file you are trying to load contains more than 256 threads.\nThis may take a long time to load.\nEnabling Thread Folding may reduce this number by merging\nthreads with the same entry point.\n\n256 Threads have been processed. Would you like to continue processing?"), MB_YESNO) == IDNO)
				break;
		}

		tabtxt.Format(_T("Thread %d"),i+1);
		this->AddView(RUNTIME_CLASS(CFunctionListView),tabtxt, i, &cc);
		CWnd* curview = GetTabControl().GetTabWnd(i);
		curview->SendMessage(WM_SETVIEWID, 0, i);
	}
}


#ifndef _DEBUG  // debug version in CodeProphetAnalystView.cpp
inline CCodeProphetViewDoc* CFunctionListTabView::GetDocument() const
{
	return reinterpret_cast<CCodeProphetViewDoc*>(m_pDocument);
}
#endif

// CFunctionListTabView diagnostics

#ifdef _DEBUG
void CFunctionListTabView::AssertValid() const
{
	CTabView::AssertValid();
}

#ifndef _WIN32_WCE
void CFunctionListTabView::Dump(CDumpContext& dc) const
{
	CTabView::Dump(dc);
}
#endif


CCodeProphetViewDoc* CFunctionListTabView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCodeProphetViewDoc)));
	return (CCodeProphetViewDoc*)m_pDocument;
}

#endif //_DEBUG

// CFunctionListTabView message handlers
