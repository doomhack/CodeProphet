// NPW_4_1.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"
#include "MyWizPage.h"
#include "Pages.h"
#include "NewProjWiz.h"


// CNPW_4_1

IMPLEMENT_DYNAMIC(CNPW_4_1, CMFCPropertyPage)

CNPW_4_1::CNPW_4_1()
	: CMyWizPage(CNPW_4_1::IDD)
{
	m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_psp.dwFlags &= ~PSP_HASHELP;

	m_psp.pszHeaderTitle = _T("New Codeprophet View Project");
	m_psp.pszHeaderSubTitle = _T("Compile module.");

}

CNPW_4_1::~CNPW_4_1()
{
}

BOOL CNPW_4_1::OnSetActive()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

   return CMFCPropertyPage::OnSetActive();
}

LRESULT CNPW_4_1::OnWizardBack()
{
	CNewProjWiz* psheet = (CNewProjWiz*) GetParent();

	return psheet->BackPage();
}

LRESULT CNPW_4_1::OnWizardNext()
{
	return IDD_NPW_5_1;
}

BEGIN_MESSAGE_MAP(CNPW_4_1, CMFCPropertyPage)
END_MESSAGE_MAP()



// CNPW_4_1 message handlers


