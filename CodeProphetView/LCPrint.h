#ifndef G_LC_PRINTING
#define G_LC_PRINTING

class LCPrinting
{
public:
	LCPrinting();//Constructor
	BOOL OnPreparePrinting(CPrintInfo* pInfo, CView * cview, CListCtrl * t_lc);//Guess # of pages
	void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo, CString & t_title, CString & t_date);
	void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

protected:
	//vars
	CListCtrl * lc;//List Control Pointer
	CString TitleStr;
	CString DateStr;
	
	CFont * pOldFont;//Font stuff
	CFont BoldFont;
	CFont lcFont;
	
	CRect page_rc;//Scaling Vars
	int m_nRowHeight;
	int m_nRowsPerPage;
	int m_nMaxRowCount;
	int hc_items;
	unsigned int m_ratiox;//These two are for scaling output to the printer
	unsigned int m_ratioy;
	
	//Protected functions------
	void PrintHeaderControl(CDC *pDC, CPrintInfo *pInfo);
	void PrintHeader(CDC *pDC, CPrintInfo *pInfo);
	void PrintFooter(CDC *pDC, CPrintInfo *pInfo);
	void DrawRow(CDC *pDC, int nItem);
	void draw_line_at(CDC *pDC, unsigned int y);
	void compute_metrics(CDC *pDC);
};

#endif