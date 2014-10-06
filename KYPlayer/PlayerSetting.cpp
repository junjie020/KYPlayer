#include "stdafx.h"

#include "PlayerSetting.h"
#include "PlayListMgr.h"
#include "PathMgr.h"
#include "Utils.h"

#include "TinyXML/tinyxml2.h"

namespace KY
{

	static PlayControl get_play_ctrl(const std::string &value)
	{
		if ("AllLoop" == value)
		{
			return PC_AllLoop;
		}

		if ("SingleLoop" == value)
		{
			return PC_SingleLoop;
		}

		if ("RandomPlay" == value)
		{
			return PC_Random;
		}

		return PC_Error;
	}

	static std::wstring get_play_ctrl_name(PlayControl pc)
	{
		const wchar_t* names[] = { L"SingleLoop", L"AllLoop", L"RandomPlay" };
		return names[pc];		
	}


	static inline fs::wpath get_setting_path()
	{
		return PathMgr::Inst()->GetSettingPath() / fs::wpath(L"Settings.xml");
	}

	PlayerSetting::PlayerSetting()
		: m_pDoc(new tinyxml2::XMLDocument())
		, m_PlayCtrl(PC_AllLoop)
	{

	}

	PlayerSetting::~PlayerSetting()
	{
		Save();
	}

	KY::PlayControl PlayerSetting::GetPlayControl() const
	{
		return m_PlayCtrl;
	}

	void PlayerSetting::SetPlayControl(PlayControl pc)
	{
		m_PlayCtrl = pc;
	}

	static void check_insert_set_value(tinyxml2::XMLElement *pParent, const char* elemName, const std::wstring &value)
	{
		tinyxml2::XMLElement *pElem = pParent->FirstChildElement(elemName);

		if (nullptr == pElem)
		{
			auto pDoc = pParent->GetDocument();
			auto pElem = pDoc->NewElement(elemName);
			pParent->InsertEndChild(pElem);
		}

		pElem->SetText(KY::Utils::utf16_to_utf8(value).c_str());
	}

	bool PlayerSetting::Save()
	{
		tinyxml2::XMLElement *pRootElem = m_pDoc->FirstChildElement();

		auto pl = PlayListMgr::Inst()->GetCurPlayList();

		check_insert_set_value(pRootElem, "LastPlayList", pl->GetName());
		check_insert_set_value(pRootElem, "PlayOrder", get_play_ctrl_name(m_PlayCtrl));
		std::wostringstream oss; oss << pl->GetPlayingIdx();
		check_insert_set_value(pRootElem, "LastSongIdx", oss.str());

		const auto pp = get_setting_path();
		return tinyxml2::XML_SUCCESS == m_pDoc->SaveFile(Utils::utf16_to_utf8(pp).c_str());
	}

	void PlayerSetting::Init()
	{
		const fs::wpath settingPath = get_setting_path();

		BOOST_ASSERT(fs::exists(settingPath));

		if (tinyxml2::XML_SUCCESS == m_pDoc->LoadFile(Utils::utf16_to_utf8(settingPath.string()).c_str()))
		{
			tinyxml2::XMLElement *pRootElem = m_pDoc->RootElement();
			tinyxml2::XMLElement *pPlayList = pRootElem->FirstChildElement("LastPlayList");
			auto lstName = pPlayList->GetText();
			if (nullptr != lstName)
			{
				PlayListMgr::Inst()->SetCurPlayListName(Utils::utf8_to_utf16(lstName));
			}

			tinyxml2::XMLElement *pPlayCtrl = pRootElem->FirstChildElement("PlayOrder");
			auto ctrlText = pPlayCtrl->GetText();
			if (nullptr != ctrlText)
			{
				m_PlayCtrl = PlayControl(get_play_ctrl(ctrlText));
			}

			tinyxml2::XMLElement *pLastSongIdx = pRootElem->FirstChildElement("LastSongIdx");
			if (pLastSongIdx)
			{
				std::istringstream iss;
				auto idx = pLastSongIdx->GetText();
				iss.str(idx);
				auto pl = PlayListMgr::Inst()->GetCurPlayList();
				if (nullptr != pl)
				{
					uint32 cc = 0;
					iss >> cc;				
					pl->SetPlayingIdx(cc);					
				}
			}
		}
		else
		{
			delete m_pDoc;
			m_pDoc = nullptr;
		}
	}


}