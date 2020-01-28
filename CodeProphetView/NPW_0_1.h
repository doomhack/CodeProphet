#pragma once

// CNPW_0_1

class CNPW_0_1 : public CMyWizPage
{
	DECLARE_DYNAMIC(CNPW_0_1)

public:
	CNPW_0_1();
	virtual ~CNPW_0_1();

	enum { IDD = IDD_NPW_0_1 };
	LRESULT OnWizardBack();

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnSetActive();
};


