// InputDialog.cpp : implementation file
//

#include "stdafx.h"
#include "KYPlayer.h"
#include "InputDialog.h"
#include "afxdialogex.h"


// CInputDialog dialog

IMPLEMENT_DYNAMIC(CInputDialog, CDialog)

CInputDialog::CInputDialog(const std::wstring &dlgName, CWnd* pParent /*=NULL*/)
	: CDialog(CInputDialog::IDD, pParent)
	, m_DlgName(dlgName)
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

BOOL CInputDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_DlgName.c_str());
	return TRUE;
}

void CInputDialog::OnOK()
{
	OnCancel();
}


// CInputDialog message handlers
