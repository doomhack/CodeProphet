#pragma once

// CNPW_1_1

class CNPW_1_1 : public CMyWizPage
{
	DECLARE_DYNAMIC(CNPW_1_1)

public:
	CNPW_1_1();
	virtual ~CNPW_1_1();

	enum { IDD = IDD_NPW_1_1 };
	LRESULT OnWizardBack();

	int m_new_proj;

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnSetActive();
	LRESULT OnWizardNext();

public:
	afx_msg void OnBnClickedImportCpg();
	afx_msg void OnBnClickedNewProf();
};


