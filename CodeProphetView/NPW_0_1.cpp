// NPW_0_1.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"

#include "MyWizPage.h"
#include "Pages.h"
#include "NewProjWiz.h"

// CNPW_0_1

IMPLEMENT_DYNAMIC(CNPW_0_1, CMFCPropertyPage)

CNPW_0_1::CNPW_0_1()
: CMyWizPage(CNPW_0_1::IDD)
{
	m_psp.dwFlags |= PSP_DEFAULT | PSP_HIDEHEADER;
	m_psp.dwFlags &= ~PSP_HASHELP;
}

CNPW_0_1::~CNPW_0_1()
{
}

BEGIN_MESSAGE_MAP(CNPW_0_1, CMFCPropertyPage)
END_MESSAGE_MAP()

BOOL CNPW_0_1::OnSetActive()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_NEXT);
	
	return CMFCPropertyPage::OnSetActive();
}

LRESULT CNPW_0_1::OnWizardBack()
{
	CNewProjWiz* psheet = (CNewProjWiz*) GetParent();

	return psheet->BackPage();
}

// CNPW_0_1 message handlers


