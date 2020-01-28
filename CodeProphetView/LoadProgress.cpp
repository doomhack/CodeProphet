// LoadProgress.cpp : implementation file
//

#include "stdafx.h"
#include "CodeProphetView.h"
#include "LoadProgress.h"


// CLoadProgress dialog

IMPLEMENT_DYNAMIC(CLoadProgress, CDialog)

CLoadProgress::CLoadProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadProgress::IDD, pParent)
{
	this->Create(CLoadProgress::IDD, pParent);
	this->ShowWindow(SW_SHOW);
	this->SetForegroundWindow();
}

CLoadProgress::~CLoadProgress()
{
}

void CLoadProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoadProgress, CDialog)
END_MESSAGE_MAP()


// CLoadProgress message handlers
