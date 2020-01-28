#pragma once


// CHotPathTabView view

class CHotPathTabView : public CTabView
{
	DECLARE_DYNCREATE(CHotPathTabView)

protected:
	CHotPathTabView();           // protected constructor used by dynamic creation
	virtual ~CHotPathTabView();

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


