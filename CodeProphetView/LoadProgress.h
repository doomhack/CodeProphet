#pragma once


// CLoadProgress dialog

class CLoadProgress : public CDialog
{
	DECLARE_DYNAMIC(CLoadProgress)

public:
	CLoadProgress(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadProgress();

// Dialog Data
	enum { IDD = IDD_LOADPROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
