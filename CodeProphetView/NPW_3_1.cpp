// NPW_3_1.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"

#include "MyWizPage.h"
#include "Pages.h"
#include "NewProjWiz.h"

// CNPW_3_1

IMPLEMENT_DYNAMIC(CNPW_3_1, CMFCPropertyPage)

CNPW_3_1::CNPW_3_1()
	: CMyWizPage(CNPW_3_1::IDD)
{
	m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_psp.dwFlags &= ~PSP_HASHELP;

	m_psp.pszHeaderTitle = _T("New Codeprophet View Project");
	m_psp.pszHeaderSubTitle = _T("Choose project type.");

	m_proj_type = -1;
}

CNPW_3_1::~CNPW_3_1()
{
}

BOOL CNPW_3_1::OnSetActive()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	if(m_proj_type == -1)
		psheet->SetWizardButtons(PSWIZB_BACK);
	else
		psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

   return CMFCPropertyPage::OnSetActive();
}

LRESULT CNPW_3_1::OnWizardBack()
{
	CNewProjWiz* psheet = (CNewProjWiz*) GetParent();

	return psheet->BackPage();
}

LRESULT CNPW_3_1::OnWizardNext()
{
	if(m_proj_type == 0)
		return IDD_NPW_4_1;

	if(m_proj_type == 1)
		return IDD_NPW_4_2;
	
	return 0;
}

BEGIN_MESSAGE_MAP(CNPW_3_1, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_PROF_INSTR, &CNPW_3_1::OnBnClickedProfInstr)
	ON_BN_CLICKED(IDC_PROF_MC, &CNPW_3_1::OnBnClickedProfMc)
	ON_BN_CLICKED(IDC_PROF_NET, &CNPW_3_1::OnBnClickedProfNet)
END_MESSAGE_MAP()



// CNPW_3_1 message handlers


void CNPW_3_1::OnBnClickedProfInstr()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	m_proj_type = 0;
}

void CNPW_3_1::OnBnClickedProfMc()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	m_proj_type = 1;
}

void CNPW_3_1::OnBnClickedProfNet()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	m_proj_type = 2;
}
