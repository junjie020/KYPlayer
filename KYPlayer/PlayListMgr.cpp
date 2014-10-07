#include "stdafx.h"

#include "PlayListMgr.h"
#include "Utils.h"

#include "TinyXML/tinyxml2.h"

#include <ctime>

namespace KY
{

	PlayList::PlayList(const std::wstring &name)
		: m_Name(name)
		, m_PlayingIdx(0)
	{
		if (m_Name.empty())
		{
			std::time_t result = std::time(nullptr);

			std::wostringstream oss;			
			oss << std::wstring(L"Default")
				<< result;

			m_Name = oss.str();
		}
	}

	PlayList::PlayList(const fs::wpath &plName)
	{
		Load(plName);
	}

	PlayList::~PlayList()
	{

	}

	const SoundInfoList& PlayList::GetPLInfoList() const
	{
		return m_PLList;
	}

#define LIST_VERSION 0x0001
	bool PlayList::Save(const fs::wpath &plPath)
	{
		m_FromPath = plPath;

		tinyxml2::XMLDocument doc;

		auto pRootElem = doc.NewElement("Root");
		pRootElem->SetAttribute("version", LIST_VERSION);

		auto name = Utils::utf16_to_utf8(m_Name);
		pRootElem->SetAttribute("name", name.c_str());

		for (auto it = m_PLList.begin(); it != m_PLList.end(); ++it)
		{
			auto pFileElem = doc.NewElement("File");

			//{@
			auto pFileNameElem = doc.NewElement("Name");
			pFileNameElem->SetText(Utils::utf16_to_utf8(it->fileName.string()).c_str());

			pFileElem->InsertEndChild(pFileNameElem);

			auto pFilePlayTimes = doc.NewElement("PlayTimes");			
			std::ostringstream oss; oss << it->playTimes;
			pFilePlayTimes->SetText(oss.str().c_str());

			pFileElem->InsertEndChild(pFilePlayTimes);			
			//@}


			pRootElem->InsertEndChild(pFileElem);
		}

		doc.InsertEndChild(pRootElem);

		return tinyxml2::XML_SUCCESS == doc.SaveFile(Utils::utf16_to_utf8(plPath.string()).c_str());
	}

	bool PlayList::Load(const fs::wpath &p)
	{		
		m_FromPath = p;
		tinyxml2::XMLDocument doc;
		const std::string fileName = KY::Utils::utf16_to_utf8(p.string());
		if (tinyxml2::XML_SUCCESS == doc.LoadFile(fileName.c_str()))
		{
			auto pRoot = doc.RootElement();
			m_Name = Utils::utf8_to_utf16(pRoot->Attribute("name"));
			for (auto pChild = pRoot->FirstChildElement();
				nullptr != pChild; pChild = pChild->NextSiblingElement())
			{
				auto pNameElem = pChild->FirstChildElement("Name");
				SoundInfo info;
				auto nameText = pNameElem->GetText();
				BOOST_ASSERT(nullptr != nameText);
				info.fileName = KY::Utils::utf8_to_utf16(nameText);

				auto pTimesElem = pChild->FirstChildElement("PlayTimes");
				if (nullptr != pTimesElem)
				{
					info.playTimes = std::atoi(pNameElem->GetText());
				}

				m_PLList.push_back(info);
			}

			return true;
		}

		return false;
	}

	bool PlayList::AddSound(const SoundInfo &sound)
	{
		m_PLList.push_back(sound);
		return true;
	}

	bool PlayList::AddSound(const fs::wpath &soundPath, uint32 playTimes)
	{
		SoundInfo info = { soundPath, L"", L"", playTimes };
		return AddSound(info);
	}

	const SoundInfo* PlayList::GetSoundInfo(uint32 idx) const
	{
		BOOST_ASSERT(idx < m_PLList.size());

		return &m_PLList[idx];
	}

	SoundInfo* PlayList::GetSoundInfo(uint32 idx)
	{
		BOOST_ASSERT(idx < m_PLList.size());
		return &m_PLList[idx];
	}


	PlayListMgr::PlayListMgr()
	{

	}

	PlayListMgr::~PlayListMgr()
	{

	}

	void PlayListMgr::Init(const fs::wpath &plFolder)
	{
		for (auto it = fs::wdirectory_iterator(plFolder);
			it != fs::wdirectory_iterator(); ++it)
		{
			const auto pp = it->path();
			if (L".pl" != pp.extension())
				continue;

			AddPlayList(PlayList(pp));

		}
	}

	void PlayListMgr::AddPlayList(const PlayList &pl)
	{
		auto name = pl.GetName();

		auto itFound = m_PLMap.find(name);
		if (itFound == m_PLMap.end())
		{
			m_PLMap.insert(std::make_pair(name, pl));
			return;
		}

		BOOST_ASSERT(false && "same name found");
	}

	const PlayList* PlayListMgr::GetCurPlayList() const
	{
		return const_cast<PlayListMgr*>(this)->GetCurPlayList();

	}

	PlayList* PlayListMgr::GetCurPlayList()
	{
		if (!m_CurPLName.empty())
		{
			auto itFound = m_PLMap.find(m_CurPLName);
			if (itFound != m_PLMap.end())
				return &itFound->second;
		}
		return nullptr;
	}

	bool PlayListMgr::SetCurPlayListName(const std::wstring &name)
	{
		m_CurPLName = name;
		return m_PLMap.find(name) != m_PLMap.end();
	}

	bool PlayListMgr::ChangePlayListName(const std::wstring &oldName, const std::wstring &newName)
	{
		if (oldName == newName)
			return false;

		auto itFound = m_PLMap.find(oldName);
		if (itFound == m_PLMap.end())
			return false;

		auto pl = itFound->second;

		pl.SetName(newName);

		m_PLMap.insert(std::make_pair(newName, pl));

		m_PLMap.erase(itFound);

		return true;
	}

}