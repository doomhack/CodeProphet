#pragma once


// CFunctionListTabView view

class CFunctionListTabView : public CTabView
{
	DECLARE_DYNCREATE(CFunctionListTabView)

protected:
	CFunctionListTabView();           // protected constructor used by dynamic creation
	virtual ~CFunctionListTabView();
	virtual void CFunctionListTabView::OnInitialUpdate();

	virtual CCodeProphetViewDoc* GetDocument() const;


	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


