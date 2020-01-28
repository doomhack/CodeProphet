#pragma once

#include <stack>

// CNewProjWiz

class CNewProjWiz : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(CNewProjWiz)

public:
	CNewProjWiz();
	virtual ~CNewProjWiz();
	BOOL AddPages();
	
	void OnActivatePage(CPropertyPage* pPage);

	std::stack<LRESULT> pages;

	LRESULT BackPage();

protected:
	DECLARE_MESSAGE_MAP()

	CNPW_0_1 p0_1;
	CNPW_1_1 p1_1;
	CNPW_2_1 p2_1;
	CNPW_2_2 p2_2;
	CNPW_3_1 p3_1;
	CNPW_4_1 p4_1;
	CNPW_4_2 p4_2;
	CNPW_5_1 p5_1;
};