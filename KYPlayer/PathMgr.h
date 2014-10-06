#ifndef _PATHMGR_H_
#define _PATHMGR_H_

#include "Singleton.h"
namespace KY
{
	class PathMgr : public Singleton<PathMgr>
	{
	public:
		PathMgr();
		~PathMgr();

		const fs::wpath& GetSettingPath() const;
		const fs::wpath& GetPlayListPath() const;


	private:
		fs::wpath m_SettingPath;
		fs::wpath m_PlayListPath;
	};
}
#endif //_PATHMGR_H_