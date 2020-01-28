#pragma once

// CNPW_2_2

class CNPW_2_2 : public CMyWizPage
{
	DECLARE_DYNAMIC(CNPW_2_2)

public:
	CNPW_2_2();
	virtual ~CNPW_2_2();
	enum { IDD = IDD_NPW_2_2 };
	LRESULT OnWizardBack();

	int m_local_proj;

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnSetActive();
public:

	afx_msg void OnBnClickedProfLocal();
	afx_msg void OnBnClickedProfDev();
};


