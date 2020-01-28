
class CMyWizPage : public CMFCPropertyPage
{
	public:
	LRESULT DlgResID;

	CMyWizPage::CMyWizPage(LRESULT DlgResID2)
		:CMFCPropertyPage(DlgResID2)
	{
		DlgResID = DlgResID2;
	}
};
