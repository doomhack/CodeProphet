#include "stdafx.h"
#include "LCPrint.h"

#define LEFT_MARGIN 2
#define RIGHT_MARGIN 4
#define HEADER_HEIGHT 4
#define FOOTER_HEIGHT 3

//Set it all to 0
//********************************CONSTRUCTOR************************************
LCPrinting::LCPrinting()
{
	lc = 0;
	pOldFont = 0;
	TitleStr = "";
	DateStr = "";
	
	page_rc.SetRect(0,0,0,0);
	m_nRowHeight = 0;
	m_nRowsPerPage = 0;
	m_nMaxRowCount = 0;
	m_ratiox = 0;
	m_ratioy = 0;
	hc_items = 0;
}

//Using default for printer guess at # of pages.
//If no printer exists return FALSE;
//************************ONPREPAREPRINTING*******************************
BOOL LCPrinting::OnPreparePrinting(CPrintInfo* pInfo, CView * cview, CListCtrl * t_lc)
{
	if(t_lc==NULL || cview==NULL || pInfo == NULL)
		return FALSE;
	
	lc = t_lc;//Set Pointer to list Control
	
	//Lets make a guess as to how many pages there are based on the default printer.
	CPrintDialog pdlg(FALSE);
	if (!pdlg.GetDefaults())
		return FALSE;//If no defaults then no printer!!
	
	CDC t_pDC;
	t_pDC.Attach(pdlg.GetPrinterDC());
	compute_metrics(&t_pDC);
	
	m_nMaxRowCount = lc->GetItemCount();
	
	if(!m_nMaxRowCount)
		return FALSE;//Get the number of rows
	
	int nMaxPage = m_nMaxRowCount/m_nRowsPerPage + 1;
	pInfo->SetMaxPage(nMaxPage);
	pInfo->m_nCurPage = 1;  // start printing at page# 1

	//If you want to be able to do this remove it.
	pInfo->m_pPD->m_pd.Flags |=PD_HIDEPRINTTOFILE;
	
	return cview->DoPreparePrinting(pInfo);
}

//Call this from your view class OnBeingPrinting function
//*************************ONBEGINGPRINTING*************************
void LCPrinting::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo, CString & t_title, CString & t_date)
{
	if(!pDC || !pInfo)
		return;
	
	TitleStr = t_title;
	DateStr = t_date;
	
	//create lc font, and Bold lc Font
	LOGFONT  lf;
	CFont * lcfont_ptr = lc->GetFont();
	lcfont_ptr->GetLogFont(&lf);
	lcFont.CreateFontIndirect(&lf);
	lf.lfWeight = FW_BOLD;
	lf.lfHeight+=22;//Make it a little bigger
	lf.lfWidth = 0;
	BoldFont.CreateFontIndirect(&lf);
	
	compute_metrics(pDC);
	int nMaxPage = m_nMaxRowCount/m_nRowsPerPage + 1;//Compute this again in case user changed printer
	
	pInfo->SetMaxPage(nMaxPage);
	pInfo->m_nCurPage = 1;  // start printing at page# 1
}

//***********************ONPRINT*************************
void LCPrinting::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	if(!pDC || !pInfo)
		return;
	
	//This has to be in OnPrint() or else PrintPreview goes screwy
	pOldFont = pDC->GetCurrentFont();
	
	//Fit all columns to 1 page, regardless of column number.
	pDC->SetMapMode(MM_ANISOTROPIC);
	
	//For every 1 List Control pixel
	pDC->SetWindowExt(1, 1);
	
	//The printer has ratio more dots
	pDC->SetViewportExt(m_ratiox, m_ratioy);
	
	int nStartRow = (pInfo->m_nCurPage - 1)*m_nRowsPerPage;
	int nEndRow = nStartRow+m_nRowsPerPage;
	
	if(nEndRow > m_nMaxRowCount)
		nEndRow = m_nMaxRowCount;
	
	PrintHeader(pDC, pInfo);     //print the header
	
	pDC->SetWindowOrg(-1*page_rc.left, 0);
	PrintFooter(pDC, pInfo);	  //Print the footer
	pDC->SetWindowOrg(-1*page_rc.left, -1*HEADER_HEIGHT*m_nRowHeight);
	PrintHeaderControl(pDC, pInfo);//Print the header Control, Manually
	pDC->SelectObject(&lcFont);//Use the LC normal font
	pDC->SetTextColor(RGB(0,0,0));//Black text on
	pDC->SetBkColor(RGB(255,255,255));//White paper
	
	CRect rcBounds;
	lc->GetItemRect(nStartRow, &rcBounds, LVIR_BOUNDS);
	
	//offset top margin of rcBounds by ListControl header
	CRect rc;
	lc->GetHeaderCtrl()->GetClientRect(&rc);
	rcBounds.OffsetRect(0, -rc.Height());
	pDC->OffsetWindowOrg(rcBounds.left, rcBounds.top);
	
	//start printing rows
	for(int i = nStartRow; i < nEndRow; i++)
		DrawRow(pDC, i);
	
	//SetWindowOrg back for next page
	pDC->SetWindowOrg(0,0);
	pDC->SelectObject(pOldFont);//Put the old font back
}

//Set the extents after calling this function because it uses printer extents
//He is using a list in here have to figure out what to do.
//********************************PRINT_HEADER************************************
void LCPrinting::PrintHeader(CDC *pDC, CPrintInfo *pInfo)
{
	pDC->SelectObject(&BoldFont);
	pDC->SetTextColor(RGB(0,0,0));//Black text on
	pDC->SetBkColor(RGB(255,255,255));//White paper
	
	CRect rc = page_rc;
	rc.bottom = rc.top+m_nRowHeight;
	
	//print App title on top right margin
	pDC->DrawText(TitleStr, &rc,  DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_RIGHT  | DT_NOCLIP);
}

//print footer with a line and date, and page number
//****************************PRINT_FOOTER****************************************
void LCPrinting::PrintFooter(CDC *pDC, CPrintInfo *pInfo)
{
	CRect rc = page_rc;
	rc.top = rc.bottom - FOOTER_HEIGHT*m_nRowHeight;
	rc.bottom = rc.top + m_nRowHeight;
	draw_line_at(pDC, rc.top);     //draw line
	
	//draw page number
	CString sTemp ;
	rc.OffsetRect(0, m_nRowHeight/2);
	sTemp.Format(_T("%d"), pInfo->m_nCurPage);
	pDC->DrawText(sTemp,-1,rc, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
	
	//Now draw the DateStr at bottom of page
	pDC->DrawText(DateStr,-1,rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
}


//Do the cleanup
//********************ONEND_PRINTING*****************
void LCPrinting::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	lcFont.DeleteObject();
	BoldFont.DeleteObject();
}

//This function sets alls of the row and metric member vars
//************************COMPUTE_METRICS*********************
void LCPrinting::compute_metrics(CDC *pDC)
{
	//This includes width for all columns
	CRect row_rc;
	lc->GetItemRect(0, &row_rc, LVIR_BOUNDS);
	
	//Get the list control window DC
	CDC  *pCtlDC = lc->GetDC();
	if(NULL == pCtlDC)
		return;
	
	//so we can get the avg character width
	TEXTMETRIC tm;
	pCtlDC->GetTextMetrics(&tm);

	//Lets get the ratios for scaling to printer DC
	//Fit all columns to 1 page, regardless of column number.
	m_ratiox = pDC->GetDeviceCaps(HORZRES)/(row_rc.Width() + (LEFT_MARGIN+RIGHT_MARGIN)*tm.tmAveCharWidth);
	
	//width of pDC/whats got to fit into it in lcDC units
	m_ratioy = pDC->GetDeviceCaps(LOGPIXELSY)/pCtlDC->GetDeviceCaps(LOGPIXELSY);
	
	lc->ReleaseDC(pCtlDC);
	
	//Set up a page rc in list control units that accounts for left and right margins
	page_rc.SetRect(0,0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
	page_rc.bottom = page_rc.bottom/m_ratioy;//Convert units to List Control
	page_rc.right = page_rc.right/m_ratiox;
	page_rc.left = LEFT_MARGIN*tm.tmAveCharWidth;//adjust sides for magins
	page_rc.right -= RIGHT_MARGIN*tm.tmAveCharWidth;
	
	m_nRowHeight = row_rc.Height();//Get the height of a row.
	int pRowHeight = (int)(m_nRowHeight*m_ratioy);//Get RowHeight in printer units.
	m_nRowsPerPage = pDC->GetDeviceCaps(VERTRES)/pRowHeight;//How many rows will fit on page?
	m_nRowsPerPage -= (HEADER_HEIGHT+FOOTER_HEIGHT);//After header and footer rows
	m_nRowsPerPage -= 1; //After header Control row
}

//You can't just have the header control print itself. 1st of all it looks crappy.
//2nd if part of header control is off screen does not print itself.
//So we will manually print it.
//************************PRINTHEADERCONTROL****************************
void LCPrinting::PrintHeaderControl(CDC *pDC, CPrintInfo *pInfo)
{
	UINT dtFlags = DT_SINGLELINE|DT_NOPREFIX|DT_VCENTER|DT_LEFT;//drawing flags
	CHeaderCtrl* hc = lc->GetHeaderCtrl();
	hc_items = hc->GetItemCount();
	
	if (hc_items < 1)
		return; //Remember that hc_items is also used to draw rows.
	
	int order_array[30];//Shouln't have more than 30 columns
	hc->GetOrderArray(order_array, hc_items);
	
	wchar_t temp_str[1024];
	HDITEM  phi;
	phi.mask = HDI_TEXT | HDI_WIDTH ;
	phi.cchTextMax = 1024;
	phi.pszText = temp_str;
	
	CRect rc(0,0,0,m_nRowHeight);
	
	for (int i = 0; i < hc_items; i++)
	{
		hc->GetItem(order_array[i], &phi);//Get in viewed order
		rc.right += phi.cxy;
		pDC->DrawText(temp_str, -1, rc, dtFlags);
		rc.left += phi.cxy;
	}
	
	//Now draw the line below header control
	draw_line_at(pDC, rc.bottom);
}

//*************************************DRAWROW********************************************
void LCPrinting::DrawRow(CDC *pDC, int nItem)
{
	if (hc_items < 1) //Then nothing to print
		return;
	
	int order_array[30];//Shouln't have more than 30 columns
	lc->GetColumnOrderArray(order_array, hc_items);
	
	CString temp_str;
	LV_COLUMN lvc;
	lvc.mask = LVCF_WIDTH;
	
	CRect rc; lc->GetItemRect(nItem, rc, LVIR_LABEL);
	int offset = pDC->GetTextExtent(_T(" "), 1).cx;//Returns CSIZE so get cx member of CSIZE object.
	rc.left += offset/2;//This makes it so that label will be over a little bit
	rc.right -= offset;//Just keep this stuff it DOES look better.
	
	for (int i = 0; i < hc_items; i++)
	{
		lc->GetColumn(order_array[i], &lvc);//Get in viewed order
		temp_str = lc->GetItemText(nItem, order_array[i]);
		rc.right += lvc.cx;
		pDC->DrawText(temp_str, -1, rc, DT_SINGLELINE|DT_NOPREFIX|DT_VCENTER|DT_LEFT);
		draw_line_at(pDC, rc.bottom);//draw a line below each row
		rc.left += lvc.cx;
	}
}

//Just pass this function a y position to draw the line at.
//*************************DRAW_LINE_AT************************************
void LCPrinting::draw_line_at(CDC *pDC, unsigned int y)
{
	pDC->MoveTo(0, y);
	pDC->LineTo(page_rc.right, y);//Use the page_rc to figure out the width of the line
}