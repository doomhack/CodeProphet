#pragma once


// CNPW_4_2 dialog

class CNPW_4_2 : public CMyWizPage
{
	DECLARE_DYNAMIC(CNPW_4_2)

public:
	CNPW_4_2();
	virtual ~CNPW_4_2();

// Dialog Data
	enum { IDD = IDD_NPW_4_2 };

	BOOL OnSetActive();
	LRESULT OnWizardBack();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowseprog();
	afx_msg void OnBnClickedBrowselaunch();
	CString m_modpath;
	CString m_prog_path;
	CString m_cmdline;
	BOOL m_just_my_code;
	BOOL m_thread_only;
};
