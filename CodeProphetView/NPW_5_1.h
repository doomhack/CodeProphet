#pragma once


// CNPW_5_1 dialog

class CNPW_5_1 : public CMyWizPage
{
	DECLARE_DYNAMIC(CNPW_5_1)

public:
	CNPW_5_1();
	virtual ~CNPW_5_1();

// Dialog Data
	enum { IDD = IDD_NPW_5_1 };

	BOOL OnSetActive();
	LRESULT OnWizardBack();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowseprog();
	afx_msg void OnBnClickedBrowselaunch();
	CString m_prog_path;
	CString m_lunch_path;
	CString m_cmd;
	BOOL m_prof_sus;
	BOOL m_no_hw_timers;
};
