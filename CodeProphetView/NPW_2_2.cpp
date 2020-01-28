// NPW_2_2.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"

#include "MyWizPage.h"
#include "Pages.h"
#include "NewProjWiz.h"

// CNPW_2_2

IMPLEMENT_DYNAMIC(CNPW_2_2, CMFCPropertyPage)

CNPW_2_2::CNPW_2_2()
	: CMyWizPage(CNPW_2_2::IDD)
{
	m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_psp.dwFlags &= ~PSP_HASHELP;

	m_psp.pszHeaderTitle = _T("New Codeprophet View Project");
	m_psp.pszHeaderSubTitle = _T("Choose program type.");

	m_local_proj = -1;
}

CNPW_2_2::~CNPW_2_2()
{
}

BOOL CNPW_2_2::OnSetActive()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	if(m_local_proj == -1)
		psheet->SetWizardButtons(PSWIZB_BACK);
	else
		psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

   return CMFCPropertyPage::OnSetActive();
}

LRESULT CNPW_2_2::OnWizardBack()
{
	CNewProjWiz* psheet = (CNewProjWiz*) GetParent();

	return psheet->BackPage();
}

BEGIN_MESSAGE_MAP(CNPW_2_2, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_PROF_LOCAL, &CNPW_2_2::OnBnClickedProfLocal)
	ON_BN_CLICKED(IDC_PROF_DEV, &CNPW_2_2::OnBnClickedProfDev)
END_MESSAGE_MAP()



// CNPW_2_2 message handlers

void CNPW_2_2::OnBnClickedProfLocal()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	m_local_proj = 1;
}

void CNPW_2_2::OnBnClickedProfDev()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	m_local_proj = 0;
}
