// NPW_2_1.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"

#include "MyWizPage.h"
#include "Pages.h"
#include "NewProjWiz.h"
#include "NPW_2_1.h"

// CNPW_2_1

IMPLEMENT_DYNAMIC(CNPW_2_1, CMFCPropertyPage)

CNPW_2_1::CNPW_2_1()
	: CMyWizPage(CNPW_2_1::IDD)
	, m_res_pdb(0)
	, m_res_none(0)
	, m_res_path(_T(""))
	, m_tfold(FALSE)
	, m_undec(FALSE)
	, m_cpg_path(_T(""))
{
	m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_psp.dwFlags &= ~PSP_HASHELP;

	m_psp.pszHeaderTitle = _T("New Codeprophet View Project");
	m_psp.pszHeaderSubTitle = _T("Import a .cpg file.");
}

CNPW_2_1::~CNPW_2_1()
{
}

void CNPW_2_1::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PDBFILE, m_res_path);
	DDX_Check(pDX, IDC_TFOLD, m_tfold);
	DDX_Check(pDX, IDC_UNDEC, m_undec);
	DDX_Text(pDX, IDC_CPGFILE, m_cpg_path);
	DDX_Control(pDX, IDC_PDBFILE, m_pdb_ctrl);
}

BOOL CNPW_2_1::OnSetActive()
{
	CMFCPropertySheet* psheet = (CMFCPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

   return CMFCPropertyPage::OnSetActive();
}

LRESULT CNPW_2_1::OnWizardBack()
{
	CNewProjWiz* psheet = (CNewProjWiz*) GetParent();

	return psheet->BackPage();
}

BEGIN_MESSAGE_MAP(CNPW_2_1, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BROWSEPDB, &CNPW_2_1::OnBnClickedBrowsepdb)
	ON_BN_CLICKED(IDC_BROWSECPG, &CNPW_2_1::OnBnClickedBrowsecpg)
	ON_BN_CLICKED(IDC_USEPDB, &CNPW_2_1::OnBnClickedUsepdb)
	ON_BN_CLICKED(IDC_USENONE, &CNPW_2_1::OnBnClickedUsenone)
END_MESSAGE_MAP()



// CNPW_2_1 message handlers



void CNPW_2_1::OnBnClickedBrowsepdb()
{

}

void CNPW_2_1::OnBnClickedBrowsecpg()
{

}

void CNPW_2_1::OnBnClickedUsepdb()
{
	// TODO: Add your control notification handler code here
}

void CNPW_2_1::OnBnClickedUsenone()
{
	// TODO: Add your control notification handler code here
}
