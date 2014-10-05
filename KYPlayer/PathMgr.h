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

		fs::wpath GetSettingPath() const;

	private:
		fs::wpath m_SettingPath;
	};
}
#endif //_PATHMGR_H_