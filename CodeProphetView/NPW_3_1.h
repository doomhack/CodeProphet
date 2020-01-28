#pragma once

// CNPW_3_1

class CNPW_3_1 : public CMyWizPage
{
	DECLARE_DYNAMIC(CNPW_3_1)

public:
	CNPW_3_1();
	virtual ~CNPW_3_1();
	enum { IDD = IDD_NPW_3_1 };
	LRESULT OnWizardBack();
	LRESULT OnWizardNext();

	int m_proj_type;

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnSetActive();
public:
	afx_msg void OnBnClickedProfInstr();
	afx_msg void OnBnClickedProfMc();
	afx_msg void OnBnClickedProfNet();
};


