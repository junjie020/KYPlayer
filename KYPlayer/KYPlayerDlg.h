
// KYPlayerDlg.h : header file
//

#pragma once

namespace KY
{
	class PlayList;
}

// CKYPlayerDlg dialog
class CKYPlayerDlg : public CDialogEx
{
// Construction
public:
	CKYPlayerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_KYPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	enum ListColType
	{
		LCT_Num = 0,
		LCT_Name,
		LCT_Artist,
		LCT_Count,
	};

	void InitSoundListCtrl();	
	void InitSettings();
	void InitSystems();

	void FillSoundList();

	void AfterInit();

	void ResetPlayListCombo();	
	void PlayLastSong();

	void PlaySong(KY::PlayList *pl, KY::uint32 idx);

private:
public:
	afx_msg void OnNMDblclkSoundList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickSoundList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSoundlistAddfilefromfolder();		
	afx_msg void OnManagerlistSave();
	afx_msg void OnManagerlistLoad();
	afx_msg void OnManagerlistNew();
	afx_msg void OnCbnSelchangeComboPlayListName();	
	afx_msg void OnCbnKillfocusComboPlayListName();	
	afx_msg void OnFindUsingname();		
};
