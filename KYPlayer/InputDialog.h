#pragma once


// CInputDialog dialog

class CInputDialog : public CDialog
{
	DECLARE_DYNAMIC(CInputDialog)

public:
	CInputDialog(const std::wstring &dlgName, CWnd* pParent = NULL);   // standard constructor
	virtual ~CInputDialog();

	virtual void OnCancel() override;
	virtual void OnOK() override;

	virtual BOOL OnInitDialog() override;

// Dialog Data
	enum { IDD = IDD_DIALOG_INPUT_CONTENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	const std::wstring& GetContent() const;

private:
	std::wstring m_DlgName;
	std::wstring m_Content;
};

inline const std::wstring& CInputDialog::GetContent() const
{
	return m_Content;
}
