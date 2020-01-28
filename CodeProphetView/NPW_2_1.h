#pragma once
#include "afxwin.h"

// CNPW_2_1

class CNPW_2_1 : public CMyWizPage
{
	DECLARE_DYNAMIC(CNPW_2_1)

public:
	CNPW_2_1();
	virtual ~CNPW_2_1();

	enum { IDD = IDD_NPW_2_1 };
	LRESULT OnWizardBack();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnSetActive();
public:
	afx_msg void OnBnClickedBrowsepdb();
	afx_msg void OnBnClickedBrowsecpg();
	int m_res_pdb;
	int m_res_none;
	CString m_res_path;
	BOOL m_tfold;
	BOOL m_undec;
	CString m_cpg_path;
	afx_msg void OnBnClickedUsepdb();
	afx_msg void OnBnClickedUsenone();
	CEdit m_pdb_ctrl;
};


