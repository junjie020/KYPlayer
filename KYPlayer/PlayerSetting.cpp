#include "stdafx.h"

#include "PlayerSetting.h"
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
	PlayerSetting::PlayerSetting()
		: m_pDoc(nullptr)
		, m_PlayCtrl(PC_AllLoop)
	{
		m_pDoc = new tinyxml2::XMLDocument();

		if (tinyxml2::XML_NO_ERROR != m_pDoc->LoadFile("Settings.xml"))
		{
			delete m_pDoc;
			m_pDoc = nullptr;
		}


		tinyxml2::XMLElement *pRootElem = m_pDoc->FirstChildElement();
		tinyxml2::XMLElement *pPlayList = pRootElem->FirstChildElement("LastPlayList");
		m_LastPlayingListName = KY::Utils::utf8_to_utf16(pPlayList->Value());

		tinyxml2::XMLElement *pPlayListFolder = pRootElem->FirstChildElement("PLFolder");
		m_PlayListFolder = KY::Utils::utf8_to_utf16(pPlayListFolder->Value());

		tinyxml2::XMLElement *pPlayCtrl = pRootElem->FirstChildElement("AllLoop");
		m_PlayCtrl = PlayControl(get_play_ctrl(pPlayCtrl->Value()));




	}

	std::wstring PlayerSetting::GetLastListName() const
	{
		return m_LastPlayingListName;
	}



}