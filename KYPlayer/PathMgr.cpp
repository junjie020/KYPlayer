#include "stdafx.h"

#include "PathMgr.h"

namespace KY
{
	PathMgr::PathMgr()
	{
		const auto cc = fs::current_path<fs::wpath>();
		
		fs::current_path(fs::wpath(L"../../"));

		const auto myDir = fs::current_path<fs::wpath>();

		m_SettingPath = myDir / fs::wpath(L"settings");
		BOOST_ASSERT(fs::is_directory(m_SettingPath));
		m_PlayListPath = myDir / fs::wpath(L"PlayLists");
		BOOST_ASSERT(fs::is_directory(m_PlayListPath));

		fs::current_path(cc);
	}

	PathMgr::~PathMgr()
	{

	}

	const fs::wpath& PathMgr::GetSettingPath() const
	{
		return m_SettingPath;
	}

	const fs::wpath& PathMgr::GetPlayListPath() const
	{
		return m_PlayListPath;
	}

}