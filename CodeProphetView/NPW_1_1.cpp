// NPW_1_1.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"

#include "MyWizPage.h"
#include "Pages.h"
#include "NewProjWiz.h"

// CNPW_1_1

IMPLEMENT_DYNAMIC(CNPW_1_1, CMFCPropertyPage)

CNPW_1_1::CNPW_1_1()
	: CMyWizPage(CNPW_1_1::IDD)
{
	m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_psp.dwFlags &= ~PSP_HASHELP;

	m_psp.pszHeaderTitle = _T("New Codeprophet View Project");
	m_psp.pszHeaderSubTitle = _T("Create or import profiling data.");

	m_new_proj = -1;
}

CNPW_1_1::~CNPW_1_1()
{
}


BEGIN_MESSAGE_MAP(CNPW_1_1, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_IMPORT_CPG, &CNPW_1_1::OnBnClickedImportCpg)
	ON_BN_CLICKED(IDC_NEW_PROF, &CNPW_1_1::OnBnClickedNewProf)
END_MESSAGE_MAP()

BOOL CNPW_1_1::OnSetActive()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();

	if(m_new_proj == -1)
		psheet->SetWizardButtons(PSWIZB_BACK);
	else
		psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

   return CMFCPropertyPage::OnSetActive();
}

LRESULT CNPW_1_1::OnWizardBack()
{
	CNewProjWiz* psheet = (CNewProjWiz*) GetParent();

	return psheet->BackPage();
}


// CNPW_1_1 message handlers



void CNPW_1_1::OnBnClickedImportCpg()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	m_new_proj = FALSE;
}

void CNPW_1_1::OnBnClickedNewProf()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	m_new_proj = TRUE;
}

LRESULT CNPW_1_1::OnWizardNext()
{
	if(m_new_proj == 1)
		return IDD_NPW_2_2;
	
	return 0;
}