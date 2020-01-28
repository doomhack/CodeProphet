// NewProjWiz.cpp : implementation file
//

#include "stdafx.h"

#include "CodeProphetView.h"

#include "MyWizPage.h"

#include "Pages.h"

#include "NewProjWiz.h"

// CNewProjWiz

IMPLEMENT_DYNAMIC(CNewProjWiz, CMFCPropertySheet)

CNewProjWiz::CNewProjWiz()
{
}

CNewProjWiz::~CNewProjWiz()
{
}


BEGIN_MESSAGE_MAP(CNewProjWiz, CMFCPropertySheet)
END_MESSAGE_MAP()


BOOL CNewProjWiz::AddPages()
{
	SetWizardMode();
	AddPage(&p0_1);
	AddPage(&p1_1);
	AddPage(&p2_1);
	AddPage(&p2_2);
	AddPage(&p3_1);
	AddPage(&p4_1);
	AddPage(&p4_2);
	AddPage(&p5_1);

	m_psh.dwFlags &= ~(PSH_HASHELP);


	m_psh.dwFlags |= PSH_WIZARD97|PSH_WATERMARK|PSH_HEADER;
	m_psh.pszbmWatermark = MAKEINTRESOURCE(IDB_SIDEBAR);
	m_psh.pszbmHeader = MAKEINTRESOURCE(IDB_WIZ_ICON);

	m_psh.hInstance = AfxGetInstanceHandle(); 

	return TRUE;
}

void CNewProjWiz::OnActivatePage(CPropertyPage* pPage)
{
	CMyWizPage* pPage2 = (CMyWizPage*)pPage;

	pages.push( pPage2->DlgResID );
}

LRESULT CNewProjWiz::BackPage()
{
	pages.pop();
	LRESULT res = pages.top();

	pages.pop();

	return res;
}
// CNewProjWiz message handlers


