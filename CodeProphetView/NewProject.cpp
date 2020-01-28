// NewProject.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"
#include "CodeProphetViewDoc.h"
#include "NewProject.h"


// CNewProject dialog

IMPLEMENT_DYNAMIC(CNewProject, CDialog)

CNewProject::CNewProject(CWnd* pParent /*=NULL*/)
	: CDialog(CNewProject::IDD, pParent)
	, m_cpgfiletext(_T(""))
	, m_pdbfiletext(_T(""))
	, m_enabletfold(FALSE)
	, m_undec(FALSE)
{
	this->proj_set = NULL;
}

CNewProject::~CNewProject()
{
}


void CNewProject::SetProjSettings(CNewProjectSettings& proj_set)
{
	this->proj_set = &proj_set;
}

void CNewProject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CPGFILE, m_cpgctrl);
	DDX_Text(pDX, IDC_CPGFILE, m_cpgfiletext);
	DDX_Text(pDX, IDC_PDBFILE, m_pdbfiletext);
	DDX_Check(pDX, IDC_ENABLETFOLD, m_enabletfold);
	DDX_Control(pDX, IDC_PDBFILE, m_pdbctrl);
	DDX_Control(pDX, IDC_BROWSEPDB, m_btnpdb);
	DDX_Check(pDX, IDC_UNDEC, m_undec);
	DDX_Control(pDX, IDOK, m_createproject);
}


BEGIN_MESSAGE_MAP(CNewProject, CDialog)
	ON_BN_CLICKED(IDC_USEPDB, &CNewProject::OnBnClickedUsepdb)
	ON_BN_CLICKED(IDC_USENONE, &CNewProject::OnBnClickedUsenone)
	ON_BN_CLICKED(IDC_BROWSECPG, &CNewProject::OnBnClickedBrowsecpg)
	ON_BN_CLICKED(IDC_BROWSEPDB, &CNewProject::OnBnClickedBrowsepdb)
	ON_BN_CLICKED(IDOK, &CNewProject::OnBnClickedOk)
END_MESSAGE_MAP()


// CNewProject message handlers

void CNewProject::OnBnClickedUsepdb()
{
	this->m_pdbctrl.EnableWindow(TRUE);
	
	this->m_createproject.EnableWindow(TRUE);

	this->m_btnpdb.EnableWindow(TRUE);
	sym_res = symres_pdb;
}

void CNewProject::OnBnClickedUsenone()
{
	this->m_pdbctrl.EnableWindow(FALSE);

	this->m_createproject.EnableWindow(TRUE);

	this->m_btnpdb.EnableWindow(FALSE);
	sym_res = symres_none;
}

void CNewProject::OnBnClickedBrowsecpg()
{
	CFileDialog fd(TRUE,NULL,NULL,4|2,_T("CodeProphet Files (*.cpg)|*.cpg||"));
	
	if(fd.DoModal() == IDOK)
	{
		this->m_cpgfiletext = fd.GetPathName();
		this->UpdateData(FALSE);
	}
}

void CNewProject::OnBnClickedBrowsepdb()
{
	CFileDialog fd(TRUE,NULL,NULL,4|2,_T("Debug Database Files (*.pdb)|*.pdb|Executable Files (*.exe)|*.exe|Dll Files (*.dll)|*.dll|All Files (*.*)|*||"));
	
	if(fd.DoModal() == IDOK)
	{
		this->m_pdbfiletext = fd.GetPathName();
		this->UpdateData(FALSE);
	}
}

void CNewProject::OnBnClickedOk()
{
	if(this->m_cpgfiletext != "")
		OnOK();
	else
		MessageBox(_T("You must open a CodeProphet output file to create a new project."),NULL,MB_OK | MB_ICONSTOP);

	if(this->proj_set)
	{
		this->proj_set->cpgfile = m_cpgfiletext;
		this->proj_set->tfold = m_enabletfold;
		this->proj_set->symbol_res = sym_res;
		this->proj_set->undec = m_undec;

		switch(sym_res)
		{
			case symres_pdb:
				this->proj_set->symres_file = m_pdbfiletext;
				break;
			default:
				this->proj_set->symbol_res = symres_none;
				this->proj_set->symres_file = "";
		}
	}
}
