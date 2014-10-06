
// KYPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KYPlayer.h"
#include "KYPlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "InputDialog.h"

#include "SoundSystem.h"
#include "PathMgr.h"
#include "PlayerSetting.h"
#include "PlayListMgr.h"

#include "Utils.h"


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
	
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	
END_MESSAGE_MAP()


// CKYPlayerDlg dialog

#define GET_LC()	static_cast<CListCtrl*>(GetDlgItem(IDC_SOUND_LIST))
#define GET_PL_CB()	static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_PLAY_LIST_NAME))



CKYPlayerDlg::CKYPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CKYPlayerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKYPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKYPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()		
	ON_NOTIFY(NM_DBLCLK, IDC_SOUND_LIST, &CKYPlayerDlg::OnNMDblclkSoundList)
	ON_NOTIFY(NM_RCLICK, IDC_SOUND_LIST, &CKYPlayerDlg::OnNMRClickSoundList)
	ON_COMMAND(ID_SOUNDLIST_ADDFILEFROMFOLDER, &CKYPlayerDlg::OnSoundlistAddfilefromfolder)		
	ON_COMMAND(ID_MANAGERLIST_SAVE, &CKYPlayerDlg::OnManagerlistSave)
	ON_COMMAND(ID_MANAGERLIST_LOAD, &CKYPlayerDlg::OnManagerlistLoad)
	ON_COMMAND(ID_MANAGERLIST_NEW, &CKYPlayerDlg::OnManagerlistNew)
	ON_CBN_SELCHANGE(IDC_COMBO_PLAY_LIST_NAME, &CKYPlayerDlg::OnCbnSelchangeComboPlayListName)	
	ON_CBN_KILLFOCUS(IDC_COMBO_PLAY_LIST_NAME, &CKYPlayerDlg::OnCbnKillfocusComboPlayListName)
END_MESSAGE_MAP()


// CKYPlayerDlg message handlers

BOOL CKYPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	InitSystems();

	InitSoundListCtrl();	
	InitSettings();

	AfterInit();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKYPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKYPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKYPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

typedef std::list<fs::wpath>	PathList;

static PathList get_all_paths(const fs::wpath &pp)
{
	PathList ll;

	const wchar_t * exts[] = { L".mp3", L".ogg", L".wav" };
	const auto itEnd = fs::wrecursive_directory_iterator();
	for (auto it = fs::wrecursive_directory_iterator(pp);
		it != itEnd; ++it)
	{
		if (std::find(std::begin(exts), std::end(exts), it->path().extension()) != std::end(exts))
		{
			ll.push_back(*it);
		}		
	}

	return ll;
}


void CKYPlayerDlg::OnNMDblclkSoundList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	auto pl = KY::PlayListMgr::Inst()->GetCurPlayList();

	BOOST_ASSERT(nullptr != pl);

	auto soundInfo = pl->GetSoundInfo(pNMItemActivate->iItem);

	BOOST_ASSERT(nullptr != soundInfo);

	KY::SoundSystem::Inst()->PlaySound(KY::Utils::utf16_to_utf8(soundInfo->fileName.string()), true);	
}

void CKYPlayerDlg::InitSoundListCtrl()
{
	auto pList = GET_LC();
	::CRect rt;
	pList->GetWindowRect(&rt);
	pList->InsertColumn(LCT_Name, L"Name", rt.Width(), rt.Width());
}

static bool init_cur_play_list_from_setting()
{
	auto lastListName = KY::PlayerSetting::Inst()->GetLastListName();

	if (!lastListName.empty())
	{
		return KY::PlayListMgr::Inst()->SetCurPlayListName(lastListName);		
	}

	// need create new PlayList
	KY::PlayList pl;
	auto name = pl.GetName();

	KY::PlayListMgr::Inst()->AddPlayList(pl);

	return KY::PlayListMgr::Inst()->SetCurPlayListName(name);
}


void CKYPlayerDlg::FillSoundList()
{
	auto pl = KY::PlayListMgr::Inst()->GetCurPlayList();
	BOOST_ASSERT(nullptr != pl);

	auto plInfo = pl->GetPLInfoList();
	auto pListCtrl = GET_LC();
	for (auto it = plInfo.begin(); it != plInfo.end(); ++it)
	{
		auto soundInfo = *it;
		pListCtrl->InsertItem(pListCtrl->GetItemCount(), soundInfo.fileName.string().c_str());
	}
}


void CKYPlayerDlg::InitSettings()
{
	auto result = init_cur_play_list_from_setting();

	if (result)
	{
		FillSoundList();
	}
}


void CKYPlayerDlg::OnNMRClickSoundList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CMenu menu;
	menu.LoadMenu(IDR_MENU_SOUND_LIST);

	::POINT pt = { 0 };
	::GetCursorPos(&pt);

	menu.GetSubMenu(0)->TrackPopupMenu(0, pt.x, pt.y, this);
}

void CKYPlayerDlg::OnSoundlistAddfilefromfolder()
{
	// TODO: Add your command handler code here

	::CFolderPickerDialog dlg;
	dlg.DoModal();

	const auto p = dlg.GetFolderPath();

	const auto paths = get_all_paths(fs::wpath(LPCTSTR(p)));

	auto pList = GET_LC();

	auto pl = KY::PlayListMgr::Inst()->GetCurPlayList();
	if (nullptr != pl)
	{
		for (auto it = paths.begin(); it != paths.end(); ++it)
		{
			pl->AddSound(*it, 0);
		}

		FillSoundList();

		return; 
	}

	MessageBox(L"no PlayList found");


}

void CKYPlayerDlg::InitSystems()
{
	auto pathMgr = KY::PathMgr::Create();	// must be first

	KY::PlayerSetting::Create();
	
	auto plMgr = KY::PlayListMgr::Create();
	plMgr->Init(pathMgr->GetPlayListPath());

	KY::SoundSystem::Create();


}


void CKYPlayerDlg::OnManagerlistSave()
{
	// TODO: Add your command handler code here

	auto pl = KY::PlayListMgr::Inst()->GetCurPlayList();

	if (nullptr != pl)
	{
		fs::wpath savePath = pl->GetSavePath();

		if (savePath.empty())
		{
			CInputDialog inDlg;
			inDlg.DoModal();

			savePath = (KY::PathMgr::Inst()->GetPlayListPath() / fs::wpath(inDlg.GetContent() + L".pl"));
		}
	
		const auto result = pl->Save(savePath);

		if (!result)
		{
			MessageBox((L"save file failed :" + savePath.string()).c_str());
		}
	}
}

void CKYPlayerDlg::OnManagerlistLoad()
{
	// TODO: Add your command handler code here
}


void CKYPlayerDlg::OnManagerlistNew()
{
	// TODO: Add your command handler code here

	// should popup a dialog to let user to set the name, 
	// or give them a chance to modify the name after the list have been created
	KY::PlayList pl;	// default constructor to use the rand name

	auto name = pl.GetName();

	KY::PlayListMgr::Inst()->AddPlayList(pl);
}

void CKYPlayerDlg::OnCbnSelchangeComboPlayListName()
{
	// TODO: Add your control notification handler code here
	auto pCombo = GET_PL_CB();

	const auto idx = pCombo->GetCurSel();

	CString curName;
	pCombo->GetLBText(idx, curName);

	KY::PlayListMgr::Inst()->SetCurPlayListName(LPCTSTR(curName));
}


void CKYPlayerDlg::AfterInit()
{
	ResetPlayListCombo();	
}

void CKYPlayerDlg::ResetPlayListCombo()
{
	auto pPLCB = GET_PL_CB();

	pPLCB->ResetContent();

	auto pls = KY::PlayListMgr::Inst()->GetPlayLists();

	for (auto it = pls.begin(); it != pls.end(); ++it)
	{
		const auto &name = it->first;
		pPLCB->AddString(name.c_str());
	}

	auto pl = KY::PlayListMgr::Inst()->GetCurPlayList();

	if (nullptr != pl)
	{
		pPLCB->SelectString(0, pl->GetName().c_str());
	}
}


void CKYPlayerDlg::OnCbnKillfocusComboPlayListName()
{
	// TODO: Add your control notification handler code here

	// TODO: Add your control notification handler code here
	auto pCombo = GET_PL_CB();
	CString curName;
	pCombo->GetWindowText(curName);
	const std::wstring sNewName(curName);

	auto pl = KY::PlayListMgr::Inst()->GetCurPlayList();
	auto oldName = pl->GetName();
	if (sNewName == oldName)
		return;

	KY::PlayListMgr::Inst()->ChangePlayListName(oldName, sNewName);
	KY::PlayListMgr::Inst()->SetCurPlayListName(sNewName);

	ResetPlayListCombo();
}
