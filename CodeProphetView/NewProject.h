#pragma once
#include "afxwin.h"


// CNewProject dialog

class CNewProject : public CDialog
{
	DECLARE_DYNAMIC(CNewProject)

public:
	CNewProject(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewProject();

// Dialog Data
	enum { IDD = IDD_NEWPROJECT };

	void SetProjSettings(CNewProjectSettings& proj_set);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
private:
	CEdit m_cpgctrl;
	CString m_cpgfiletext;
	CString m_pdbfiletext;
	BOOL m_enabletfold;
	afx_msg void OnBnClickedUsepdb();
	CEdit m_pdbctrl;
	afx_msg void OnBnClickedUsenone();
	CStatic m_stsymbolres;
	CNewProjectSettings* proj_set;
	ESymbolResolution sym_res;

public:
	afx_msg void OnBnClickedBrowsecpg();
	afx_msg void OnBnClickedBrowsepdb();
	afx_msg void OnBnClickedOk();
	CButton m_btnpdb;
	BOOL m_undec;
	afx_msg void OnBnClickedUndec();
	CButton m_createproject;
};