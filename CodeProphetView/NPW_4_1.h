#pragma once


// CNPW_4_1

class CNPW_4_1 : public CMyWizPage
{
	DECLARE_DYNAMIC(CNPW_4_1)

public:
	CNPW_4_1();
	virtual ~CNPW_4_1();
	enum { IDD = IDD_NPW_4_1 };
	LRESULT OnWizardBack();
	LRESULT OnWizardNext();

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnSetActive();
};


