// CallerCalleeView.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"
#include "CodeProphetViewDoc.h"
#include "ColumnTreeView.h"
#include "ClassView.h"


// CCallerCalleeView

IMPLEMENT_DYNCREATE(CClassView, CColumnTreeView)

CClassView::CClassView()
{

}

CClassView::~CClassView()
{
}

BEGIN_MESSAGE_MAP(CClassView, CColumnTreeView)
	ON_NOTIFY(TVN_SELCHANGED, TreeID, OnTvnSelchanged)
	ON_MESSAGE(WM_SETVIEWID, &CClassView::OnSetViewID)
END_MESSAGE_MAP()


// CCallerCalleeView drawing

void CClassView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

void CClassView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTREEVIEW* pNMTreeView = (NMTREEVIEW*)pNMHDR;

	// find the label of the selected item
	CString strLabel;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if (hItem)
	{
		CTreeCtrl& tree = GetTreeCtrl();
		strLabel = tree.GetItemText(hItem);
		int nPos = strLabel.Find('\t');
		if (nPos >= 0)
			strLabel = strLabel.Left(nPos);
	}

	// display it on the status bar
	GetParentFrame()->SetMessageText(strLabel);

	*pResult = 0;
}


void CClassView::OnInitialUpdate()
{
	CColumnTreeView::OnInitialUpdate();

	CTreeCtrl& tree = GetTreeCtrl();
	CHeaderCtrl& header = GetHeaderCtrl();

	DWORD dwStyle = GetWindowLong(tree, GWL_STYLE);
	dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	SetWindowLong(tree, GWL_STYLE, dwStyle);

	m_ImgList.Create(IDB_CLASS_VIEW, 16, 0, RGB(255,0,0));
	tree.SetImageList(&m_ImgList, TVSIL_NORMAL);

	HDITEM hditem;
	hditem.mask = HDI_TEXT | HDI_WIDTH | HDI_FORMAT;
	hditem.fmt = HDF_LEFT | HDF_STRING;
	hditem.cxy = 200;
	hditem.pszText = _T("Function Name");
	header.InsertItem(0, &hditem);
	hditem.cxy = 100;
	hditem.pszText = _T("Function Address");
	header.InsertItem(1, &hditem);
	hditem.cxy = 100;
	hditem.pszText = _T("Call Count");
	header.InsertItem(2, &hditem);
	hditem.pszText = _T("Inclusive Time (ms)");
	header.InsertItem(3, &hditem);
	hditem.pszText = _T("Inclusive Time (%)");
	header.InsertItem(4, &hditem);
	hditem.pszText = _T("Exclusive Time (ms)");
	header.InsertItem(5, &hditem);
	hditem.pszText = _T("Exclusive Time (%)");
	header.InsertItem(6, &hditem);
	hditem.pszText = _T("Min Time (ms)");
	header.InsertItem(7, &hditem);
	hditem.pszText = _T("Max Time (ms)");
	header.InsertItem(8, &hditem);
	hditem.pszText = _T("Avg Time (ms)");
	header.InsertItem(9, &hditem);
	UpdateColumns();

	CCodeProphetViewDoc *doc = GetDocument();

	CCallTree *calltree = doc->m_callgraph.g_treelist.at(mythread);

	int numchild = calltree->GetNode0()->num_child;

	while(numchild--)
		AddFunctions(calltree->GetNode0()->children.at(numchild), TVI_ROOT);

}

void CClassView::AddFunctions(TreeNode* thisnode, HTREEITEM level)
{
	CTreeCtrl& tree = GetTreeCtrl();
	CString item;

	item.Format(_T("%s\t%s\t%d\t%.3f\t%.3f%%\t%.3f\t%.3f%%\t%.3f\t%.3f\t%.3f"), thisnode->func_name, thisnode->func_addr, thisnode->call_count, thisnode->tot_msec, thisnode->tot_pc, thisnode->exc_msec, thisnode->exc_pc, thisnode->min_msec , thisnode->max_msec , thisnode->avg_msec);
	
	int numchild = thisnode->num_child;

	int image;

	if(numchild > 0)
		image = 1;
	else
		image = 3;

	HTREEITEM newlevel = tree.InsertItem(item, image, image, level);

	while(numchild--)
		AddFunctions(thisnode->children.at(numchild), newlevel);
}


LRESULT CClassView::OnSetViewID(WPARAM wParam, LPARAM lParam)
{
	mythread = lParam;

	return TRUE;
}



// CCallerCalleeView diagnostics

#ifdef _DEBUG
void CClassView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CClassView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG



CCodeProphetViewDoc* CClassView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCodeProphetViewDoc)));
	return (CCodeProphetViewDoc*)m_pDocument;
}

// CCallerCalleeView message handlers
