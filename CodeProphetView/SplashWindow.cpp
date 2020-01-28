/* 
 * Copyright (c) 2004 Apple Enterprises. This code is
 * proprietary and a trade secret of Apple Enterprises.
 *
 * $Workfile: SplashWindow.cpp $
 *
 * $Creator: Chris Apple $
 *
 * $Description: Initial splash screen. $
 *
 * $Log: /AppleEnterprises/CodeExamples/SplashMFC/SplashMfcDialog/SplashWindow.cpp $
 * 
 * 1     4/22/09 3:37p Capple
 * Initial Checkin
 * 
 * 1     11/05/04 1:27p Capple
 * Initial Checkin.
 * 
 * $NoKeywords: $
 */

#include "stdafx.h" 
#include "resource.h"

#include "SplashWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

CSplashWindow* CSplashWindow::m_pSplashWindow;
CString CSplashWindow::m_productNameString;
CString CSplashWindow::m_companyNameString;
CString CSplashWindow::m_versionNumberString;
CString CSplashWindow::m_versionString;
CString CSplashWindow::m_copyrightString;
CString CSplashWindow::m_commentsString;
CString CSplashWindow::m_statusMessage;

int CSplashWindow::m_millisecondsToDisplay;						// 0 ==> until mouse click or keystroke

// create rectangle that product name has to fit in
const int CSplashWindow::m_productNameVerticalOffset = 0;		// empty space between top border and Product Name
const int CSplashWindow::m_productNameVerticalHeight = 300;		// maximum height of Product Name
const int CSplashWindow::m_productNameLeftMargin = 20;			// distance from left side to place name, company, copyright and version
const int CSplashWindow::m_productNameRightMargin = 20;			// distance from right side to place name, company, copyright and version
const CString CSplashWindow::m_productNameFontName = _T("Arial");	// name of font for application name
CSize CSplashWindow::m_productNamePointSize = CSize(-1,-1);		// point size used for the application name, (-1,-1) ==> Calculate point size
COLORREF CSplashWindow::m_productNameTextColor = RGB(109,140,44);// color used for text

const BOOL CSplashWindow::m_displayCompanyName = FALSE;			// true if displaying companyName
const BOOL CSplashWindow::m_displayVersion = TRUE;				// true if displaying version
const BOOL CSplashWindow::m_displayCopyright = FALSE;			// true if displaying copyright
const BOOL CSplashWindow::m_displayComments = FALSE;				// true if displaying comments

// create rectangle that strings in body have to fit in
const int CSplashWindow::m_bodyVerticalOffset = 285;			// empty space between top border and top of body
const int CSplashWindow::m_bodyVerticalHeight = 15;			// maximum height of body
const int CSplashWindow::m_bodyLeftMargin = 0;				// distance from left side to place company name, copyright, version and comment
const int CSplashWindow::m_bodyRightMargin = 300;				// distance from right side to place company name, copyright, version and comment
const CString CSplashWindow::m_bodyFontName = _T("Arial");			// name of font for company name, copyright and version	
CSize CSplashWindow::m_bodyPointSize = CSize(-1,-1);			// point size used for company name, copyright and version, (-1,-1) ==> Calculate point size	
COLORREF CSplashWindow::m_bodyTextColor = RGB(255,255,255);		// color used for company name, copyright and version (-1 ==> use application name color)

// create rectangle for status line string
const int CSplashWindow::m_statusVerticalOffset = 200;			// empty space between top border and top of status string
const int CSplashWindow::m_statusVerticalHeight = 50;			// maximum height of status string
const int CSplashWindow::m_statusLeftMargin = 10;				// distance from left side to place status string
const int CSplashWindow::m_statusRightMargin = 90;				// distance from right side to place status string
const CString CSplashWindow::m_statusMessageFontName = _T("Arial");	// name of font for status message while starting	
CSize CSplashWindow::m_statusMessagePointSize = CSize(-1,-1);	// point size used for status message while starting, (-1,-1) ==> Calculate point size	
COLORREF CSplashWindow::m_statusMessageTextColor = RGB(192,192,255);// color used for status message while starting (-1 ==> use application name color)


CSplashWindow::CSplashWindow()
{
	GetVersionStrings();
}

CSplashWindow::~CSplashWindow()
{
	// Clear the static window pointer.
	ASSERT(m_pSplashWindow == this);
	m_pSplashWindow = NULL;
}

BEGIN_MESSAGE_MAP(CSplashWindow, CWnd)
	//{{AFX_MSG_MAP(CSplashWindow)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSplashWindow::ShowSplashScreen(  CWnd* pParentWnd /*= NULL*/, LPCTSTR statusMessage /*= NULL*/, int millisecondsToDisplay /*= 0*/ )
{
	if ( m_statusMessage.IsEmpty() ) {
		m_statusMessage.Empty();
	}

	if ( statusMessage ) {
		m_statusMessage = statusMessage; 
	}

	m_millisecondsToDisplay = millisecondsToDisplay;

	// Allocate a new splash screen, and create the window.
	if ( m_pSplashWindow == NULL ) {
		m_pSplashWindow = new CSplashWindow;
		if (!m_pSplashWindow->Create(pParentWnd)) {
			delete m_pSplashWindow;
			m_pSplashWindow = NULL;
		}
	}

	// Set a timer to destroy the splash screen.
	if ( millisecondsToDisplay ) { 
		m_pSplashWindow->SetTimer( 1, m_millisecondsToDisplay, NULL );
	}

	m_pSplashWindow->ShowWindow ( SW_SHOW );
	m_pSplashWindow->UpdateWindow();
	m_pSplashWindow->InvalidateRect( NULL, FALSE );
	
	// clear message queue
	MSG msg;
    while (PeekMessage(&msg, m_pSplashWindow->m_hWnd,  0, 0, PM_REMOVE)) { 
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}

BOOL CSplashWindow::PreTranslateAppMessage(MSG* pMsg)
{
	if (m_pSplashWindow == NULL)
		return FALSE;

	// If we get a keyboard or mouse message, hide the splash screen.
	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
		pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN)
	{
		m_pSplashWindow->HideSplashScreen();
		return TRUE;	// message handled here
	}

	return FALSE;	// message not handled
}

BOOL CSplashWindow::Create(CWnd* pParentWnd /*= NULL*/)
{
	if (!m_bitmap.LoadBitmap(IDB_SPLASH))
		return FALSE;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	int xPos = 0;
	int yPos = 0;
	int width = bm.bmWidth;
	int height = bm.bmHeight;

	// if parent window, center it on the parent window. otherwise center it on the screen
	CRect parentRect;
	if ( pParentWnd == NULL ) {
		GetDesktopWindow()->GetWindowRect (parentRect );
	} else {
		pParentWnd->GetWindowRect ( parentRect );
	}

	xPos = parentRect.left + (parentRect.right - parentRect.left)/2 - (width/2);
	yPos = parentRect.top + (parentRect.bottom - parentRect.top)/2 - (height/2);


	BOOL result =  CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, xPos, yPos, width, height, pParentWnd->GetSafeHwnd(), NULL);

	if ( !result )
		return FALSE;

	// if no parent window, make it a topmost, so eventual application window will appear under it
	if ( pParentWnd == NULL ) {
		SetWindowPos( &wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	}
	return TRUE;
}

void CSplashWindow::HideSplashScreen()
{
	// Destroy the window, and update the mainframe.
	if ( m_pSplashWindow != NULL ) {
		m_pSplashWindow->DestroyWindow();
		CWnd* mainWnd = AfxGetMainWnd();
		if ( mainWnd && IsWindow(mainWnd->m_hWnd) )
			mainWnd->UpdateWindow();
	}
}

void CSplashWindow::PostNcDestroy()
{
	// Free the C++ class.
	delete this;
}

int CSplashWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Center the window.
	CenterWindow();

	// Set a timer to destroy the splash screen.
	if ( m_millisecondsToDisplay ) {
		SetTimer(1, m_millisecondsToDisplay, NULL);
	}

	return 0;
}

CSize CSplashWindow::FindFontPointSize( CPaintDC &dc, LPCTSTR fontName, const CStringArray &stringsToCheck, CSize maximumSize )
{

	CFont font;
//	int pointSize = 8;
	int pointSize = 10;

	CSize previousLargest = CSize(0,0);
	CSize largest = CSize(0,0);

	int numberOfStringsToCheck = stringsToCheck.GetSize();
	maximumSize.cy /= numberOfStringsToCheck;
/*
	while ( 1 )
	{
		CFont* originalFont = dc.SelectObject(&font);
		font.CreatePointFont( pointSize*10, fontName, &dc);
		dc.SelectObject(&font);

		previousLargest = largest;
		for ( int i=0; i<numberOfStringsToCheck; i++ ) { 
			CSize szProduct = dc.GetTextExtent( stringsToCheck[i] );
			if (  largest.cx < szProduct.cx ) {
				largest = szProduct;
			}
		}

		dc.SelectObject(&originalFont);
		font.DeleteObject();

		if ( largest.cx > maximumSize.cx )
			break;
		if ( largest.cy > maximumSize.cy )
			break;

		pointSize += 2;

	}
*/
//	pointSize -= 2;
	CSize ret = CSize ( pointSize, pointSize );
	return ret;
}

void CSplashWindow::OnPaint()
{
	CPaintDC dc(this);

	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(&dc))
		return;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	dcImage.SelectObject(pOldBitmap);

	// calculate height of strings
	int windowHeight = bm.bmHeight;
	int windowWidth = bm.bmWidth;

	// get point sizes of different strings		
	CStringArray stringsToCheck;
	SIZE sectionSize;


	// draw product name
	int productNameLeftMargin = m_productNameLeftMargin;
	int productNameRightMargin = windowWidth - m_productNameRightMargin;
	sectionSize.cx = productNameRightMargin - productNameLeftMargin;
	sectionSize.cy = m_productNameVerticalHeight;

	if ( m_productNamePointSize == CSize(-1,-1) ) {
		stringsToCheck.RemoveAll();
		stringsToCheck.Add(m_productNameString);
		m_productNamePointSize = CSplashWindow::FindFontPointSize( dc, m_productNameFontName, stringsToCheck, sectionSize );
	}

	CFont productNameFont;
	productNameFont.CreatePointFont(m_productNamePointSize.cx*10, m_productNameFontName, &dc);
	CFont* originalFont = dc.SelectObject(&productNameFont);

	int topOfText = m_productNameVerticalOffset; 
	int bottomOfText = topOfText + m_productNameVerticalHeight; 
	CRect productNameRect = CRect(productNameLeftMargin,topOfText, productNameRightMargin, bottomOfText );
	dc.SetTextColor( m_productNameTextColor );
	dc.SetBkMode(TRANSPARENT);
	dc.DrawText(m_productNameString, productNameRect, DT_VCENTER|DT_CENTER|DT_SINGLELINE );

	// draw body
	int bodyLeftMargin = m_bodyLeftMargin;
	int bodyRightMargin = windowWidth - m_bodyRightMargin;
	sectionSize.cx = bodyRightMargin - bodyLeftMargin;
	sectionSize.cy = m_bodyVerticalHeight;
	if ( m_bodyPointSize == CSize(-1,-1) ) {
		stringsToCheck.RemoveAll();
		if ( m_displayCompanyName ) 
			stringsToCheck.Add(m_companyNameString);
		if ( m_displayVersion ) 
			stringsToCheck.Add(m_versionString);
		if ( m_displayCopyright ) 
			stringsToCheck.Add(m_copyrightString);
		if ( m_displayComments ) 
			stringsToCheck.Add(m_commentsString);
		m_bodyPointSize = CSplashWindow::FindFontPointSize( dc, m_bodyFontName, stringsToCheck, sectionSize );
	}

	CFont bodyFont;
	bodyFont.CreatePointFont(m_bodyPointSize.cx*10, m_bodyFontName, &dc);

	dc.SetTextColor( (m_bodyTextColor == -1) ? m_productNameTextColor : m_bodyTextColor );
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(&bodyFont);

	int singleStringOfBodyHeight = m_bodyPointSize.cy;
	topOfText = m_bodyVerticalOffset; 
	bottomOfText = topOfText + singleStringOfBodyHeight; 
	if ( m_displayCompanyName ) {
		CRect companyNameRect = CRect(bodyLeftMargin, topOfText, bodyRightMargin, bottomOfText );
		dc.DrawText(m_companyNameString, companyNameRect, DT_VCENTER|DT_CENTER|DT_SINGLELINE );
		topOfText += singleStringOfBodyHeight; 
		bottomOfText += singleStringOfBodyHeight; 
	}
	if ( m_displayVersion ) {
		CRect versionRect = CRect(bodyLeftMargin, topOfText, bodyRightMargin, bottomOfText );
		dc.DrawText(m_versionString, versionRect, DT_VCENTER|DT_CENTER|DT_SINGLELINE );
		topOfText += singleStringOfBodyHeight; 
		bottomOfText += singleStringOfBodyHeight; 
	}
	if ( m_displayCopyright ) {
		CRect copyrightRect = CRect(bodyLeftMargin, topOfText, bodyRightMargin, bottomOfText );
		dc.DrawText(m_copyrightString, copyrightRect, DT_VCENTER|DT_CENTER|DT_SINGLELINE );
		topOfText += singleStringOfBodyHeight; 
		bottomOfText += singleStringOfBodyHeight; 
	}
	if ( m_displayComments ) {
		CRect commentsRect = CRect(bodyLeftMargin, topOfText, bodyRightMargin, bottomOfText );
		dc.DrawText(m_commentsString, commentsRect, DT_VCENTER|DT_CENTER|DT_SINGLELINE );
		topOfText += singleStringOfBodyHeight; 
		bottomOfText += singleStringOfBodyHeight; 
	}

	// draw status
	if ( !m_statusMessage.IsEmpty() ) {
		int statusLeftMargin = m_statusLeftMargin;
		int statusRightMargin = windowWidth - m_statusRightMargin;
		sectionSize.cx = statusRightMargin - statusLeftMargin;
		sectionSize.cy = m_statusVerticalHeight;

		if ( m_statusMessagePointSize == CSize(-1,-1) ) {
			stringsToCheck.RemoveAll();
			stringsToCheck.Add(m_statusMessage);
			m_statusMessagePointSize = CSplashWindow::FindFontPointSize( dc, m_statusMessageFontName, stringsToCheck, sectionSize );
		}

		topOfText = m_statusVerticalOffset; 
		bottomOfText = topOfText + m_statusVerticalHeight; 
		CRect statusRect = CRect(statusLeftMargin,topOfText, statusRightMargin, bottomOfText );

		CFont statusFont;
		statusFont.CreatePointFont(m_statusMessagePointSize.cx*10, m_statusMessageFontName, &dc);

		dc.SetTextColor( (m_statusMessageTextColor == -1) ? m_productNameTextColor : m_statusMessageTextColor );
		dc.SetBkMode(TRANSPARENT);
		dc.SelectObject(&statusFont);
		dc.DrawText(m_statusMessage, statusRect, DT_VCENTER|DT_CENTER|DT_SINGLELINE );
	}


	dc.SelectObject(&originalFont);
	productNameFont.DeleteObject();
	bodyFont.DeleteObject();
}

void CSplashWindow::OnTimer(UINT nIDEvent)
{
	// Destroy the splash screen window.
	HideSplashScreen();
}
void CSplashWindow::GetVersionStrings()
{	
	DWORD dwHandle;			// ignored 
	char *buf;				// pointer to buffer to receive file-version info.
	TCHAR buf1[1024];				// pointer to buffer to store version string.
	CString nameStr;
	nameStr.Format(_T("%s.EXE"), AfxGetApp()->m_pszExeName);
	TCHAR *name = (TCHAR *)((const TCHAR *)nameStr);

	// Get the size of the version information.
	DWORD verSize = GetFileVersionInfoSize(
		name,	// pointer to filename string
		&dwHandle		// pointer to variable to receive zero
	);

	if (verSize != 0)
	{
		buf = new char[verSize + 1];

		BOOL res = GetFileVersionInfo(
				name,	// pointer to filename string
				NULL,			// ignored 
				verSize,		// size of buffer
				buf 			// pointer to buffer to receive file-version info.
		);
		ASSERT(res);

		UINT ver = (UINT)verSize;

		LPVOID lplpBuffer;
		BOOL doThis = FALSE;
/*
		LPVOID lplpBuffer;
		BOOL doThis = VerQueryValue (buf, TEXT("\\StringFileInfo\\000904E4\\ProductName"), &lplpBuffer, &ver);
		if ( doThis ) {
			wcsncpy(buf1, (TCHAR *)lplpBuffer, ver);
			m_productNameString = (CString)buf1;
		}

		doThis = VerQueryValue (buf, TEXT("\\StringFileInfo\\000904E4\\CompanyName"), &lplpBuffer, &ver);
		if ( doThis ) {
			wcsncpy(buf1, (TCHAR *)lplpBuffer, ver);
			m_companyNameString = (CString)buf1;
		}

		doThis = VerQueryValue (buf, TEXT("\\StringFileInfo\\000904E4\\LegalCopyright"), &lplpBuffer, &ver);
		if ( doThis ) {
			wcsncpy(buf1, (TCHAR *)lplpBuffer, ver);
			m_copyrightString = (CString)buf1;
		}

		doThis = VerQueryValue (buf, TEXT("\\StringFileInfo\\000904E4\\Comments"), &lplpBuffer, &ver);
		if ( doThis ) {
			wcsncpy(buf1, (TCHAR *)lplpBuffer, ver);
			m_commentsString = (CString)buf1;
		}
*/
		doThis = VerQueryValue (buf, TEXT("\\StringFileInfo\\000904E4\\ProductVersion"), &lplpBuffer, &ver);
		if ( doThis ) {
			wcsncpy(buf1, (TCHAR *)lplpBuffer, ver);
/*
			int versionNumbers[4];
			int numberOfValues = wscanf ( buf1, "%d,%d,%d,%d", &versionNumbers[0],&versionNumbers[1], &versionNumbers[2], &versionNumbers[3] );
			CString number;
			m_versionNumberString = "";
			for ( int i=0; i<numberOfValues; i++ ) {
				if ( m_versionNumberString.IsEmpty() && (versionNumbers[i] == 0) )
					continue;
				if ( !m_versionNumberString.IsEmpty() )
					m_versionNumberString += ".";
				number.Format(_T("%d"), versionNumbers[i] );
				m_versionNumberString += number; 
			}
*/
//			m_versionString = _T("Version ") + m_versionNumberString;
			m_versionString.Format(_T("Version: %s"),buf1);
		}
		delete buf;
	}
}

