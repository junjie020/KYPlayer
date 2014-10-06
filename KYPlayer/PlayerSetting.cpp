#include "stdafx.h"

#include "PlayerSetting.h"
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
		const wchar_t* names[] = { L"AllLoop", L"SingleLoop", L"RandomPlay" };
		return names[pc];		
	}


	PlayerSetting::PlayerSetting()
		: m_pDoc(new tinyxml2::XMLDocument())
		, m_PlayCtrl(PC_AllLoop)
	{
		const fs::wpath settingPath = PathMgr::Inst()->GetSettingPath() / fs::wpath(L"Settings.xml");

		BOOST_ASSERT(fs::exists(settingPath));

		if (tinyxml2::XML_SUCCESS == m_pDoc->LoadFile(Utils::utf16_to_utf8(settingPath.string()).c_str()))
		{
			tinyxml2::XMLElement *pRootElem = m_pDoc->RootElement();
			tinyxml2::XMLElement *pPlayList = pRootElem->FirstChildElement("LastPlayList");
			auto lstName = pPlayList->GetText();
			if (nullptr != lstName)
			{
				m_LastListName = KY::Utils::utf8_to_utf16(lstName);
			}
			
			tinyxml2::XMLElement *pPlayListFolder = pRootElem->FirstChildElement("PLFolder");
			auto lstFolder = pPlayListFolder->GetText();
			if (nullptr != lstFolder)
			{
				m_PlayListFolder = KY::Utils::utf8_to_utf16(lstFolder);
			}
			
			tinyxml2::XMLElement *pPlayCtrl = pRootElem->FirstChildElement("PlayOrder");
			auto ctrlText = pPlayCtrl->GetText();
			if (nullptr != ctrlText)
			{
				m_PlayCtrl = PlayControl(get_play_ctrl(ctrlText));
			}
		}
		else
		{
			delete m_pDoc;
			m_pDoc = nullptr;
		}
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

		pElem->SetValue(KY::Utils::utf16_to_utf8(value).c_str());
	}

	PlayerSetting::~PlayerSetting()
	{
		tinyxml2::XMLElement *pRootElem = m_pDoc->FirstChildElement();

		check_insert_set_value(pRootElem, "LastPlayList", m_LastListName);

		check_insert_set_value(pRootElem, "PLFolder", m_PlayListFolder.string());

		check_insert_set_value(pRootElem, "PlayOrder", get_play_ctrl_name(m_PlayCtrl));

	}


	std::wstring PlayerSetting::GetLastListName() const
	{
		return m_LastListName;
	}

	void PlayerSetting::SetLastListName(const std::wstring &name)
	{
		m_LastListName = name;
	}

	fs::wpath PlayerSetting::GetListFolder() const
	{
		return m_PlayListFolder;
	}

	void PlayerSetting::SetListFolder(const fs::wpath &pp)
	{

	}

	KY::PlayControl PlayerSetting::GetPlayControl() const
	{
		return m_PlayCtrl;
	}

	void PlayerSetting::SetPlayControl(PlayControl pc)
	{

	}


}