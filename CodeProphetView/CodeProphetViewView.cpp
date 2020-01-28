
// CodeProphetViewView.cpp : implementation of the CCodeProphetViewView class
//

#include "stdafx.h"
#include "CodeProphetView.h"

#include "CodeProphetViewDoc.h"
#include "CodeProphetViewView.h"

#include "FunctionListTabView.h"
#include "FunctionListView.h"
#include "ColumnTreeView.h"
#include "CallerCalleeTabView.h"
#include "ProfileSummaryView.h"
#include "HotPathTabView.h"
#include "ClassTabView.h"
#include "ClassView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCodeProphetViewView

IMPLEMENT_DYNCREATE(CCodeProphetViewView, CTabView)

BEGIN_MESSAGE_MAP(CCodeProphetViewView, CTabView)
	// Standard printing commands
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_PRINT, &CTabView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CTabView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCodeProphetViewView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CCodeProphetViewView construction/destruction

CCodeProphetViewView::CCodeProphetViewView()
{
	// TODO: add construction code here
	
}

CCodeProphetViewView::~CCodeProphetViewView()
{
}


void CCodeProphetViewView::OnInitialUpdate()
{
	CTabView::OnInitialUpdate();

	CCreateContext cc;
	cc.m_pNewViewClass = NULL;
	cc.m_pCurrentDoc = this->GetDocument();
	cc.m_pNewDocTemplate = NULL;
	cc.m_pLastView = NULL;
	cc.m_pCurrentFrame = NULL;

	this->AddView(RUNTIME_CLASS(CProfileSummaryView),_T("Profile Summary"),-1,&cc);
	this->AddView(RUNTIME_CLASS(CFunctionListTabView),_T("Functions"), -1, &cc);
	this->AddView(RUNTIME_CLASS(CallerCalleeTabView),_T("Call Graph"), -1, &cc);
	this->AddView(RUNTIME_CLASS(CHotPathTabView),_T("Hot Paths"), -1 ,&cc);
	//this->AddView(RUNTIME_CLASS(CClassTabView),_T("Classes"), -1 ,&cc);
	//this->AddView(RUNTIME_CLASS(CView),_T("Threads"), -1 ,&cc);

}


int CCodeProphetViewView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabView::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetTabControl().ModifyTabStyle(CMFCTabCtrl::STYLE_3D_ONENOTE);
	GetTabControl().SetLocation(CMFCBaseTabCtrl::LOCATION_TOP);
	GetTabControl().SetActiveTabBoldFont(TRUE);

	return 0;
}


BOOL CCodeProphetViewView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CTabView::PreCreateWindow(cs);
}

// CCodeProphetViewView drawing

void CCodeProphetViewView::OnDraw(CDC* pDC)
{
	CCodeProphetViewDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CCodeProphetViewView printing


void CCodeProphetViewView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CCodeProphetViewView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CCodeProphetViewView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CCodeProphetViewView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CCodeProphetViewView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCodeProphetViewView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CCodeProphetViewView diagnostics

#ifdef _DEBUG
void CCodeProphetViewView::AssertValid() const
{
	CTabView::AssertValid();
}

void CCodeProphetViewView::Dump(CDumpContext& dc) const
{
	CTabView::Dump(dc);
}

CCodeProphetViewDoc* CCodeProphetViewView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCodeProphetViewDoc)));
	return (CCodeProphetViewDoc*)m_pDocument;
}
#endif //_DEBUG


// CCodeProphetViewView message handlers
