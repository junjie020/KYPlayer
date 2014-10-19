
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


using KY::uint32;

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
	DDX_Control(pDX, IDC_SOUND_LIST, m_PlayListCtrl);
}

BEGIN_MESSAGE_MAP(CKYPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_SOUND_LIST, &CKYPlayerDlg::OnNMDblclkSoundList)
	ON_NOTIFY(NM_RCLICK, IDC_SOUND_LIST, &CKYPlayerDlg::OnNMRClickSoundList)
	ON_COMMAND(ID_SOUNDLIST_ADDFILEFROMFOLDER, &CKYPlayerDlg::OnSoundlistAddfilefromfolder)		
	ON_COMMAND(ID_MANAGERLIST_SAVE, &CKYPlayerDlg::OnManagerlistSave)
	ON_COMMAND(ID_MANAGERLIST_LOAD, &CKYPlayerDlg::OnManagerlistLoad)
	ON_COMMAND(ID_MANAGERLIST_NEW, &CKYPlayerDlg::OnManagerlistNew)
	ON_CBN_SELCHANGE(IDC_COMBO_PLAY_LIST_NAME, &CKYPlayerDlg::OnCbnSelchangeComboPlayListName)	
	ON_CBN_KILLFOCUS(IDC_COMBO_PLAY_LIST_NAME, &CKYPlayerDlg::OnCbnKillfocusComboPlayListName)	
	ON_COMMAND(ID_FIND_USINGNAME, &CKYPlayerDlg::OnFindUsingname)		
	ON_COMMAND(ID_LISTSORT_MOVETO, &CKYPlayerDlg::OnListsortMoveto)	
	ON_NOTIFY(LVN_BEGINDRAG, IDC_SOUND_LIST, &CKYPlayerDlg::OnLvnBegindragSoundList)
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

	PlaySong(pl, pNMItemActivate->iItem);
}

void CKYPlayerDlg::InitSoundListCtrl()
{
	auto pList = GET_LC();
	auto oldStyle = pList->GetExtendedStyle();
	pList->SetExtendedStyle(oldStyle | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	::CRect rt;
	pList->GetWindowRect(&rt);
	const uint32 numColWidth = uint32(rt.Width() * 0.1f);
	pList->InsertColumn(LCT_Num, L"Num", numColWidth, numColWidth);
	const uint32 nameColWidth = uint32(rt.Width() * 0.75f);
	pList->InsertColumn(LCT_Name, L"Name", nameColWidth, nameColWidth);

	const uint32 artistColoWidth = rt.Width() - nameColWidth - numColWidth;
	pList->InsertColumn(LCT_Artist, L"Artist", artistColoWidth, artistColoWidth);


	m_DragHelper.SetListCtrl(pList);
}

static bool init_cur_play_list_from_setting()
{
	auto curPl = KY::PlayListMgr::Inst()->GetCurPlayList();
	if (nullptr != curPl)
	{
		return true;
	}

	// need create new PlayList
	KY::PlayList pl;
	auto name = pl.GetName();

	KY::PlayListMgr::Inst()->AddPlayList(pl);

	if (KY::PlayListMgr::Inst()->SetCurPlayListName(name))
	{
		KY::PlayerSetting::Inst()->Save();
		return true;
	}

	return false;
}


void CKYPlayerDlg::FillSoundList()
{
	m_PlayListCtrl.DeleteAllItems();
	auto pl = KY::PlayListMgr::Inst()->GetCurPlayList();
	BOOST_ASSERT(nullptr != pl);

	auto plInfo = pl->GetPLInfoList();
	
	uint32 idx = 1;
	for (auto it = plInfo.begin(); it != plInfo.end(); ++it, ++idx)
	{
		auto soundInfo = *it;

		LVITEM item = { 0 };
		item.iItem = m_PlayListCtrl.GetItemCount();
		item.mask = LVIF_TEXT;

		
		std::wstring tmp;
		std::wostringstream oss; oss << idx;
		tmp = oss.str();

		item.pszText = &*tmp.begin();
		item.iSubItem = LCT_Num;

		m_PlayListCtrl.InsertItem(&item);

		item.iSubItem = LCT_Name;
		tmp = soundInfo.fileName.string();
		item.pszText = &*tmp.begin();
		m_PlayListCtrl.SetItem(&item);		
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

	auto pSetting = KY::PlayerSetting::Create();	
	auto plMgr = KY::PlayListMgr::Create();

	KY::SoundSystem::Create();

	pSetting->Init();
	plMgr->Init(pathMgr->GetPlayListPath());

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
			CInputDialog inDlg(L"Save Play List Name");
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
	KY::PlayerSetting::Inst()->Save();
}


void CKYPlayerDlg::AfterInit()
{
	ResetPlayListCombo();	
	PlayLastSong();
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

void CKYPlayerDlg::PlayLastSong()
{
	auto pl = KY::PlayListMgr::Inst()->GetCurPlayList();
	if (nullptr != pl && !pl->GetPLInfoList().empty() )
	{
		const auto idx = KY::PlayerSetting::Inst()->GetLastPlayingIdx();
		PlaySong(pl, idx);
		
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

	KY::PlayerSetting::Inst()->Save();
}

void CKYPlayerDlg::PlaySong(KY::PlayList *pl, KY::uint32 idx)
{
	pl->SetPlayingIdx(idx);

	KY::SoundSystem::Inst()->PlaySound(idx);

	auto soundInfo = pl->GetSoundInfo(idx);
	if (nullptr != soundInfo)
	{
		SetWindowText((L"KYPlayer - " + soundInfo->fileName.string()).c_str());
	}
}

void CKYPlayerDlg::OnFindUsingname()
{
	// TODO: Add your command handler code here
	CInputDialog dlg(L"Find Song By Name");
	dlg.DoModal();

	auto content = dlg.GetContent();

	if (content.empty())
		return;

	auto pl = KY::PlayListMgr::Inst()->GetCurPlayList();

	auto idx = pl->FindFirstSongByName(content);

	if (idx < 0)
		return;

	auto pList = GET_LC();

	pList->SetItemState(idx, LVIS_SELECTED, LVIS_SELECTED);
	pList->EnsureVisible(idx, FALSE);
}

void CKYPlayerDlg::OnDestroy()
{
	KY::PlayerSetting::Inst()->Save();

	KY::SoundSystem::Destory();
	KY::PlayListMgr::Destory();
	KY::PlayerSetting::Destory();	
	KY::PathMgr::Destory();

	CDialogEx::OnDestroy();
}


void CKYPlayerDlg::OnListsortMoveto()
{
	// TODO: Add your command handler code here

	auto pList = GET_LC();

	const auto idx = pList->GetSelectionMark();
	if (idx < 0)
		return;

	CInputDialog inDlg(L"Move To...");

	inDlg.DoModal();

	auto content = inDlg.GetContent();

	std::wistringstream iss(content);

	uint32 toIdx = 0;
	iss >> toIdx;

	if (toIdx == idx)
		return;

	auto pl = KY::PlayListMgr::Inst()->GetCurPlayList();

	pl->MoveSongTo(idx, toIdx);
}





void CKYPlayerDlg::OnLvnBegindragSoundList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	m_DragHelper.BeginDrag(pNMLV->iItem, pNMLV->ptAction);

	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CKYPlayerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_DragHelper.IsDragging())
	{
		//CPoint pt(point);
		//ClientToScreen(&pt); //convert to screen coordinates

		UINT flags = 0;
		const auto hitIdx = m_PlayListCtrl.HitTest(point, &flags);

		const auto topIdx = m_PlayListCtrl.GetTopIndex();

		const auto bottomIdx = topIdx + m_PlayListCtrl.GetCountPerPage();

		if (topIdx == hitIdx && topIdx != 0)
			SetTimer(TE_DragOnTop, 33, NULL);
		else if (bottomIdx == hitIdx && bottomIdx != m_PlayListCtrl.GetItemCount())
			SetTimer(TE_DragOnBottom, 33, NULL);
		else
		{
			KillTimer(TE_DragOnTop);
			KillTimer(TE_DragOnBottom);
		}
			
	}


	m_DragHelper.Dragging(point);
	CDialogEx::OnMouseMove(nFlags, point);

}

void CKYPlayerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	const auto dropIdx = m_DragHelper.GetDropIdx();
	m_DragHelper.EndDrag();

	KY::PlayList::IdxList	idxList;
	auto pos = m_PlayListCtrl.GetFirstSelectedItemPosition();
	do 
	{
		auto idx = m_PlayListCtrl.GetNextSelectedItem(pos);
		idxList.push_back(KY::uint32(idx));

		m_PlayListCtrl.SetItemState(idx, 0, LVIS_SELECTED); // remove selection
	} while (pos);

	auto pl = KY::PlayListMgr::Inst()->GetCurPlayList();

	pl->MoveSongTo(idxList, dropIdx);

	FillSoundList();


	KillTimer(TE_DragOnTop);
	KillTimer(TE_DragOnBottom);
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CKYPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
#ifdef min
#undef min
#endif // min

#ifdef max
#undef max
#endif // max
	switch (nIDEvent)
	{
		case TE_DragOnTop:
		{
			const auto topIdx = std::max(0, m_PlayListCtrl.GetTopIndex() - 1);

			m_PlayListCtrl.EnsureVisible(topIdx, FALSE);
		}
		break;
		case TE_DragOnBottom:
		{
			const auto topIdx = m_PlayListCtrl.GetTopIndex();
			const auto bottomIdx = std::min(m_PlayListCtrl.GetItemCount(), topIdx + m_PlayListCtrl.GetCountPerPage() + 1);
			m_PlayListCtrl.EnsureVisible(bottomIdx, FALSE);
		}
		break;

	}
}
