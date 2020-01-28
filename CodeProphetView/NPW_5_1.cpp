// NPW_5_1.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"
#include "MyWizPage.h"
#include "Pages.h"
#include "NewProjWiz.h"
#include "NPW_5_1.h"



// CNPW_5_1 dialog

IMPLEMENT_DYNAMIC(CNPW_5_1, CMFCPropertyPage)

CNPW_5_1::CNPW_5_1()
	: CMyWizPage(CNPW_5_1::IDD)
	, m_prog_path(_T(""))
	, m_lunch_path(_T(""))
	, m_cmd(_T(""))
	, m_prof_sus(FALSE)
	, m_no_hw_timers(FALSE)
{
	m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_psp.dwFlags &= ~PSP_HASHELP;

	m_psp.pszHeaderTitle = _T("New Codeprophet View Project");
	m_psp.pszHeaderSubTitle = _T("Project to Launch.");
}

CNPW_5_1::~CNPW_5_1()
{
}

BOOL CNPW_5_1::OnSetActive()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);

   return CMFCPropertyPage::OnSetActive();
}

LRESULT CNPW_5_1::OnWizardBack()
{
	CNewProjWiz* psheet = (CNewProjWiz*) GetParent();

	return psheet->BackPage();
}

void CNPW_5_1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROGPATH, m_prog_path);
	DDX_Text(pDX, IDC_LAUNCHPATH, m_lunch_path);
	DDX_Text(pDX, IDC_CMD, m_cmd);
	DDX_Check(pDX, IDC_PROF_SUS, m_prof_sus);
	DDX_Check(pDX, IDC_NO_HW_TIM, m_no_hw_timers);
}


BEGIN_MESSAGE_MAP(CNPW_5_1, CPropertyPage)
	ON_BN_CLICKED(IDC_BROWSEPROG, &CNPW_5_1::OnBnClickedBrowseprog)
	ON_BN_CLICKED(IDC_BROWSELAUNCH, &CNPW_5_1::OnBnClickedBrowselaunch)
END_MESSAGE_MAP()


// CNPW_5_1 message handlers

void CNPW_5_1::OnBnClickedBrowseprog()
{
	CFileDialog fd(TRUE,NULL,NULL,4|2,_T("Executable Files (*.exe)|*.exe|Dll Files (*.dll)|*.dll|All Files (*.*)|*||"));

	if(fd.DoModal() == IDOK)
	{
		this->m_prog_path = fd.GetPathName();
		this->UpdateData(FALSE);
	}
}

void CNPW_5_1::OnBnClickedBrowselaunch()
{
	CFileDialog fd(TRUE,NULL,NULL,4|2,_T("Executable Files (*.exe)|*.exe|Dll Files (*.dll)|*.dll|All Files (*.*)|*||"));

	if(fd.DoModal() == IDOK)
	{
		this->m_lunch_path = fd.GetPathName();
		this->UpdateData(FALSE);
	}
}
