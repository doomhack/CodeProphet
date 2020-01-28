// FunctionListView.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"
#include "CodeProphetViewDoc.h"
#include "FunctionListView.h"

int CALLBACK FunctionSortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

// CFunctionListView


IMPLEMENT_DYNCREATE(CFunctionListView, CListView)

CFunctionListView::CFunctionListView()
{
}

CFunctionListView::~CFunctionListView()
{
}

BEGIN_MESSAGE_MAP(CFunctionListView, CListView)
	ON_WM_CREATE()
	
	ON_MESSAGE(WM_SETVIEWID, &CFunctionListView::OnSetViewID)
	
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CFunctionListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFunctionListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFunctionListView::OnFilePrintPreview)

	ON_WM_CONTEXTMENU()

	ON_NOTIFY(HDN_ITEMCLICK, 0, OnItemclickList1)

END_MESSAGE_MAP()

void CFunctionListView::OnFilePrintPreview()
{
	CListView::OnFilePrintPreview();
}

BOOL CFunctionListView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return lcp.OnPreparePrinting(pInfo, this, &this->GetListCtrl());
}

void CFunctionListView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CCodeProphetViewDoc* pDoc = GetDocument();
	CTime time = CTime::GetCurrentTime();
	CString doc_title = pDoc->GetTitle();
	CString date = time.Format( _T("%A, %B %d, %Y") );
	lcp.OnBeginPrinting(pDC, pInfo, doc_title, date);
}

void CFunctionListView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	lcp.OnPrint(pDC, pInfo);
}

void CFunctionListView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	lcp.OnEndPrinting(pDC, pInfo);//Clean up those printing fonts
	CListView::OnEndPrinting(pDC, pInfo);
}

int CFunctionListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetListCtrl().ModifyStyle(0 ,LVS_REPORT); 
	GetListCtrl().SetExtendedStyle( GetListCtrl().GetExtendedStyle() | LVS_REPORT | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	return 0;
}

LRESULT CFunctionListView::OnSetViewID(WPARAM wParam, LPARAM lParam)
{
	mythread = lParam;

	return TRUE;
}


void CFunctionListView::OnInitialUpdate()
{
   CListView::OnInitialUpdate();

   CListCtrl& listCtrl = GetListCtrl();

   // Insert a column. This override is the most convenient.
   listCtrl.InsertColumn(0, _T("Function Name"), LVCFMT_LEFT);
   listCtrl.InsertColumn(1, _T("Function Address"), LVCFMT_RIGHT);
   listCtrl.InsertColumn(2, _T("Call Count"), LVCFMT_RIGHT);
   listCtrl.InsertColumn(3, _T("Inclusive Time (ms)"), LVCFMT_RIGHT);
   listCtrl.InsertColumn(4, _T("Inclusive Time (%)"), LVCFMT_RIGHT);
   listCtrl.InsertColumn(5, _T("Exclusive Time (ms)"), LVCFMT_RIGHT);
   listCtrl.InsertColumn(6, _T("Exclusive Time (%)"), LVCFMT_RIGHT);
   listCtrl.InsertColumn(7, _T("Min Time (ms)"), LVCFMT_RIGHT);
   listCtrl.InsertColumn(8, _T("Max Time (ms)"), LVCFMT_RIGHT);
   listCtrl.InsertColumn(9, _T("Avg Time (ms)"), LVCFMT_RIGHT);

   CCodeProphetViewDoc* doc = GetDocument();
   CString strText;
   CCallTree* tree = doc->m_callgraph.g_treelist.at(mythread);
   unsigned int num_func = tree->function_list.size();

   if(!tree)
	   return;

	for(unsigned int i = 0; i < num_func; i++)
	{
		TreeNode* this_func = tree->function_list.at(i);

		listCtrl.InsertItem(i, this_func->func_name);
		
		listCtrl.SetItemText(i, 1, this_func->func_addr);

		strText.Format(_T("%d"), this_func->call_count);
		listCtrl.SetItemText(i, 2, strText);

		strText.Format(_T("%.3f"), this_func->tot_msec);
		listCtrl.SetItemText(i, 3, strText);

		strText.Format(_T("%.3f%%"), this_func->tot_pc);
		listCtrl.SetItemText(i, 4, strText);

		strText.Format(_T("%.3f"), this_func->exc_msec);
		listCtrl.SetItemText(i, 5, strText);

		strText.Format(_T("%.3f%%"), this_func->exc_pc);
		listCtrl.SetItemText(i, 6, strText);

		strText.Format(_T("%.3f"), this_func->min_msec);
		listCtrl.SetItemText(i, 7, strText);

		strText.Format(_T("%.3f"), this_func->max_msec);
		listCtrl.SetItemText(i, 8, strText);

		strText.Format(_T("%.3f"), this_func->avg_msec);
		listCtrl.SetItemText(i, 9, strText);

		listCtrl.SetItemData(i, (LPARAM)this_func);
	}
	
	// Set reasonable widths for our columns
	listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	listCtrl.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	listCtrl.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
	listCtrl.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
	listCtrl.SetColumnWidth(4, LVSCW_AUTOSIZE_USEHEADER);
	listCtrl.SetColumnWidth(5, LVSCW_AUTOSIZE_USEHEADER);
	listCtrl.SetColumnWidth(6, LVSCW_AUTOSIZE_USEHEADER);
	listCtrl.SetColumnWidth(7, LVSCW_AUTOSIZE_USEHEADER);
	listCtrl.SetColumnWidth(8, LVSCW_AUTOSIZE_USEHEADER);
	listCtrl.SetColumnWidth(9, LVSCW_AUTOSIZE_USEHEADER);
}

void CFunctionListView::OnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl& listCtrl = GetListCtrl();
	NMLISTVIEW *pLV = (NMLISTVIEW *) pNMHDR;
	
	listCtrl.SortItems(FunctionSortFunc, pLV->iItem);
	
	*pResult = 0;
}

int CALLBACK FunctionSortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int nRetVal;
	
	TreeNode *pData1 = (TreeNode*)lParam1;
	TreeNode *pData2 = (TreeNode*)lParam2;

	switch(lParamSort)
	{
		case 0:	//Function name
			nRetVal = wcscmp(pData1->func_name, pData2->func_name);
			break;
		
		case 1:	//Func addr
			nRetVal = wcscmp(pData1->func_addr, pData2->func_addr);
		break;
		
		case 2: //Call count
			nRetVal = (BOOL)(pData1->call_count < pData2->call_count);
			break;

		case 3: //Total Msec
			nRetVal = (BOOL)(pData1->tot_msec < pData2->tot_msec);
			break;

		case 4: //Total %
			nRetVal = (BOOL)(pData1->tot_pc < pData2->tot_pc);
			break;

		case 5: //Exclusive Msec
			nRetVal = (BOOL)(pData1->exc_msec < pData2->exc_msec);
			break;

		case 6: //Exclusive %
			nRetVal = (BOOL)(pData1->exc_pc < pData2->exc_pc);
			break;

		case 7: //Min Msec
			nRetVal = (BOOL)(pData1->min_msec < pData2->min_msec);
			break;

		case 8: //Max Msec
			nRetVal = (BOOL)(pData1->max_msec < pData2->max_msec);
			break;

		case 9: //Avg Msec
			nRetVal = (BOOL)(pData1->avg_msec < pData2->avg_msec);
			break;
		
		default:
			break;
	}

	return nRetVal;
}

void CFunctionListView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CFunctionListView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


#ifndef _DEBUG  // debug version in CodeProphetAnalystView.cpp
inline CCodeProphetViewDoc* CFunctionListView::GetDocument() const
{
	return reinterpret_cast<CCodeProphetViewDoc*>(m_pDocument);
}
#endif

// CFunctionListView diagnostics

#ifdef _DEBUG
void CFunctionListView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CFunctionListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif

CCodeProphetViewDoc* CFunctionListView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCodeProphetViewDoc)));
	return (CCodeProphetViewDoc*)m_pDocument;
}

#endif //_DEBUG


// CFunctionListView message handlers
