
// KYPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KYPlayer.h"
#include "KYPlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "SoundSystem.h"
#include "PathMgr.h"
#include "PlayerSetting.h"

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

	auto pList = GET_LC();

	const auto key = size_t(pList->GetItemData(pNMItemActivate->iItem));

	auto itFound = m_itemInserted.find(key);

	BOOST_ASSERT(itFound != m_itemInserted.end());
	
		
	auto name = KY::Utils::utf16_to_utf8(itFound->second.fullName.string());

	KY::SoundSystem::Inst()->PlaySound(name, true);
	
}

void CKYPlayerDlg::InitSoundListCtrl()
{
	auto pList = GET_LC();
	::CRect rt;
	pList->GetWindowRect(&rt);
	pList->InsertColumn(LCT_Name, L"Name", rt.Width(), rt.Width());
}

void CKYPlayerDlg::InitSettings()
{

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

	typedef std::hash<std::wstring>	WSTRHash;

	for (auto it = paths.begin(); it != paths.end(); ++it)
	{
		ItemInfo info;

		info.fullName = *it;

		const auto key = WSTRHash()(info.fullName.string());

		auto idx = pList->InsertItem(pList->GetItemCount(), info.fullName.basename().c_str());

		auto result = m_itemInserted.insert(std::make_pair(key, info));
		BOOST_VERIFY(result.second);
		pList->SetItemData(idx, DWORD_PTR(key));
	}
}

void CKYPlayerDlg::InitSystems()
{
	KY::PlayerSetting::Create();
	KY::PathMgr::Create();

	KY::SoundSystem::Create();
}
