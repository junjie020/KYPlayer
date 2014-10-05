#ifndef _PLAYLISTMGR_H_
#define _PLAYLISTMGR_H_
#include "Singleton.h"
namespace KY
{
	struct PLInfo
	{
		fs::wpath	fileName;
		uint32		playTimes;
	};

	typedef std::list<PLInfo>	PLInfoList;

	class PlayList
	{
	public:
		PlayList(const fs::wpath &file);
		~PlayList();

	public:
		const PLInfoList& GetPLInfoList() const;

	private:
		PLInfoList	m_PLList;

	};

	class PlayListMgr : Singleton < PlayListMgr >
	{
	public:
		PlayListMgr();
		~PlayListMgr();

		void Init(const fs::wpath &plFolder);

		void AddPlayList(const fs::wpath &plName);

	private:
		typedef unordered_map<fs::wpath, PlayList>	PlayListMap;

		PlayListMap	m_PLMap;
	};
}
#endif //_PLAYLISTMGR_H_