#pragma once


// CCallerCalleeView view

class CCallerCalleeView : public CColumnTreeView
{
	DECLARE_DYNCREATE(CCallerCalleeView)

protected:
	CCallerCalleeView();           // protected constructor used by dynamic creation
	virtual ~CCallerCalleeView();

	virtual void OnInitialUpdate();
	CImageList m_ImgList;
	void AddFunctions(TreeNode* thisnode, HTREEITEM level);


public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view

	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);

	unsigned int mythread;
	afx_msg LRESULT OnSetViewID(WPARAM wParam, LPARAM lParam);
	CCodeProphetViewDoc* GetDocument() const;

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


//Custom Messages
#define WM_SETVIEWID (WM_USER + 100)
