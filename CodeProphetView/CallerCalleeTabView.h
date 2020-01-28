#pragma once


// CallerCalleeTabView view

class CallerCalleeTabView : public CTabView
{
	DECLARE_DYNCREATE(CallerCalleeTabView)

protected:
	CallerCalleeTabView();           // protected constructor used by dynamic creation
	virtual ~CallerCalleeTabView();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();

	virtual CCodeProphetViewDoc* GetDocument() const;

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


