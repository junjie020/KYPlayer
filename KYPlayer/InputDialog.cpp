// InputDialog.cpp : implementation file
//

#include "stdafx.h"
#include "KYPlayer.h"
#include "InputDialog.h"
#include "afxdialogex.h"


// CInputDialog dialog

IMPLEMENT_DYNAMIC(CInputDialog, CDialog)

CInputDialog::CInputDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInputDialog::IDD, pParent)
{

}

CInputDialog::~CInputDialog()
{
}

void CInputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInputDialog, CDialog)
END_MESSAGE_MAP()

void CInputDialog::OnCancel()
{
	auto pWnd = GetDlgItem(IDC_EDIT_INPUT_CONTENT);
	pWnd->SendMessage(WM_KILLFOCUS, 0, 0);

	CString content;
	pWnd->GetWindowText(content);

	m_Content = content;

	CDialog::OnCancel();
}


// CInputDialog message handlers
