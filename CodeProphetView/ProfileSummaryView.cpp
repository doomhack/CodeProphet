// ProfileSummaryView.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"
#include "CodeProphetViewDoc.h"
#include "ProfileSummaryView.h"

int CALLBACK SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

// CProfileSummaryView

IMPLEMENT_DYNCREATE(CProfileSummaryView, CListView)

CProfileSummaryView::CProfileSummaryView()
{

}

CProfileSummaryView::~CProfileSummaryView()
{
	for (int i = 0; i<6; i++)
		if(items[i])
			delete items[i];
}

BEGIN_MESSAGE_MAP(CProfileSummaryView, CListView)
	ON_WM_CREATE()
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnItemclickList1)
	
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CProfileSummaryView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CProfileSummaryView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CProfileSummaryView::OnFilePrintPreview)
END_MESSAGE_MAP()


void CProfileSummaryView::OnFilePrintPreview()
{
	CListView::OnFilePrintPreview();
}

BOOL CProfileSummaryView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return lcp.OnPreparePrinting(pInfo, this, &this->GetListCtrl());
}

void CProfileSummaryView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CCodeProphetViewDoc* pDoc = GetDocument();
	CTime time = CTime::GetCurrentTime();
	CString doc_title = pDoc->GetTitle();
	CString date = time.Format( _T("%A, %B %d, %Y") );
	lcp.OnBeginPrinting(pDC, pInfo, doc_title, date);
}

void CProfileSummaryView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	lcp.OnPrint(pDC, pInfo);
}

void CProfileSummaryView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	lcp.OnEndPrinting(pDC, pInfo);//Clean up those printing fonts
	CListView::OnEndPrinting(pDC, pInfo);
}


int CProfileSummaryView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetListCtrl().ModifyStyle(0 ,LVS_REPORT); 
	GetListCtrl().SetExtendedStyle( GetListCtrl().GetExtendedStyle() | LVS_REPORT /*| LVS_EX_GRIDLINES*/ | LVS_EX_FULLROWSELECT);

	return 0;
}


void CProfileSummaryView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();
	CListCtrl& listCtrl = GetListCtrl();
	
	// Insert a column. This override is the most convenient.
	listCtrl.InsertColumn(0, _T("Property"), LVCFMT_LEFT);
	listCtrl.InsertColumn(1, _T("Value"), LVCFMT_LEFT);

	CCodeProphetViewDoc* doc = GetDocument();

	items[0] = new itemdata;
	items[0]->prop  = _T("Platform");

	switch(doc->m_callgraph.arch)
	{
		case x86_pc:
			items[0]->val = _T("Windows X86 PC");
		break;

		case x64_pc:
			items[0]->val = _T("Windows X64 PC");
		break;

		case wm5_arm:
			items[0]->val = _T("Windows Mobile 5 ARM");
		break;

		default:
			items[0]->val = _T("Unknown platform");
	}

	items[1] = new itemdata;
	items[1]->prop = _T("Thread Count");
	items[1]->val.Format(_T("%d"),doc->m_callgraph.num_threads);

	items[2] = new itemdata;
	items[2]->prop = _T("Time");
	items[2]->val = doc->m_callgraph.prof_time.Format( "%A, %B %d, %Y" );

	items[3] = new itemdata;
	items[3]->prop = _T("Pointer Length");
	items[3]->val.Format(_T("%d bits"), doc->m_callgraph.ptrlen * 8);

	items[4] = new itemdata;
	items[4]->prop = _T("Timer Frequency");
	if(doc->m_callgraph.timer_freq < 1000000)
		items[4]->val.Format(_T("%.2f khz"), (double)doc->m_callgraph.timer_freq / 1000);
	else if(doc->m_callgraph.timer_freq < 1000000000)
		items[4]->val.Format(_T("%.2f mhz"), (double)doc->m_callgraph.timer_freq / 1000000);
	else
		items[4]->val.Format(_T("%.2f ghz"), (double)doc->m_callgraph.timer_freq / 1000000000);

	items[5] = new itemdata;
	items[5]->prop = _T("Profiler Version");
	items[5]->val.Format(_T("%d.%d.%d"), ((doc->m_callgraph.version & 0xff000000) >> 24), ((doc->m_callgraph.version & 0xff0000) >> 16), (doc->m_callgraph.version & 0xffff));

	for(int i = 0; i < 6; i++)
	{
		listCtrl.InsertItem(i, items[i]->prop);
		listCtrl.SetItemText(i, 1, items[i]->val);
		listCtrl.SetItemData(i, (LPARAM)items[i]);
	}

	listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	listCtrl.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

}

void CProfileSummaryView::OnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW *pLV = (NMLISTVIEW *) pNMHDR;
	
	CListCtrl& listCtrl = GetListCtrl();
	listCtrl.SortItems(SortFunc, pLV->iItem);
	
	*pResult = 0;
}

int CALLBACK SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int nRetVal;

	itemdata* pData1 = (itemdata*)lParam1;
	itemdata* pData2 = (itemdata*)lParam2;


	switch(lParamSort)
	{
	case 0:	// Last Name
		nRetVal = wcscmp(pData1->prop, pData2->prop);
		break;

	case 1:	// First Name
		nRetVal = wcscmp(pData1->val, pData2->val);
		break;

	default:
		break;
	}

	return nRetVal;
}

#ifndef _DEBUG  // debug version in CodeProphetAnalystView.cpp
inline CCodeProphetViewDoc* CProfileSummaryView::GetDocument() const
{
	return reinterpret_cast<CCodeProphetViewDoc*>(m_pDocument);
}
#endif


// CProfileSummaryView diagnostics

#ifdef _DEBUG
void CProfileSummaryView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CProfileSummaryView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif


CCodeProphetViewDoc* CProfileSummaryView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCodeProphetViewDoc)));
	return (CCodeProphetViewDoc*)m_pDocument;
}

#endif //_DEBUG


// CProfileSummaryView message handlers
