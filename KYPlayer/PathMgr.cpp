#include "stdafx.h"

#include "PathMgr.h"

namespace KY
{
	PathMgr::PathMgr()
	{
		const fs::wpath cc = fs::current_path<fs::wpath>();
		//chdir("../../");
		fs::current_path(fs::wpath(L"../../"));

		m_SettingPath = fs::current_path<fs::wpath>();
	}

	PathMgr::~PathMgr()
	{

	}

	fs::wpath PathMgr::GetSettingPath() const
	{
		return m_SettingPath;
	}

}