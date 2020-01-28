// NPW_4_2.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"
#include "MyWizPage.h"
#include "Pages.h"
#include "NewProjWiz.h"
#include "NPW_4_2.h"


// CNPW_4_2 dialog

IMPLEMENT_DYNAMIC(CNPW_4_2, CMFCPropertyPage)

CNPW_4_2::CNPW_4_2()
	: CMyWizPage(CNPW_4_2::IDD)
	, m_modpath(_T(""))
	, m_prog_path(_T(""))
	, m_cmdline(_T(""))
	, m_just_my_code(FALSE)
	, m_thread_only(FALSE)
{
	m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_psp.dwFlags &= ~PSP_HASHELP;

	m_psp.pszHeaderTitle = _T("New Codeprophet View Project");
	m_psp.pszHeaderSubTitle = _T("Project to Launch.");
}

BOOL CNPW_4_2::OnSetActive()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);

   return CMFCPropertyPage::OnSetActive();
}

LRESULT CNPW_4_2::OnWizardBack()
{
	CNewProjWiz* psheet = (CNewProjWiz*) GetParent();

	return psheet->BackPage();
}

CNPW_4_2::~CNPW_4_2()
{
}

void CNPW_4_2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROGPATH, m_modpath);
	DDX_Text(pDX, IDC_LAUNCHPROG, m_prog_path);
	DDX_Text(pDX, IDC_CMD, m_cmdline);
	DDX_Check(pDX, IDC_JUSTMYCODE, m_just_my_code);
	DDX_Check(pDX, IDC_THREADONLY, m_thread_only);
}


BEGIN_MESSAGE_MAP(CNPW_4_2, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BROWSEPROG, &CNPW_4_2::OnBnClickedBrowseprog)
	ON_BN_CLICKED(IDC_BROWSELAUNCH, &CNPW_4_2::OnBnClickedBrowselaunch)
END_MESSAGE_MAP()


// CNPW_4_2 message handlers

void CNPW_4_2::OnBnClickedBrowseprog()
{
	CFileDialog fd(TRUE,NULL,NULL,4|2,_T("Executable Files (*.exe)|*.exe|Dll Files (*.dll)|*.dll|All Files (*.*)|*||"));

	if(fd.DoModal() == IDOK)
	{
		this->m_modpath = fd.GetPathName();
		this->UpdateData(FALSE);
	}
}

void CNPW_4_2::OnBnClickedBrowselaunch()
{
	CFileDialog fd(TRUE,NULL,NULL,4|2,_T("Executable Files (*.exe)|*.exe|Dll Files (*.dll)|*.dll|All Files (*.*)|*||"));

	if(fd.DoModal() == IDOK)
	{
		this->m_prog_path = fd.GetPathName();
		this->UpdateData(FALSE);
	}
}
