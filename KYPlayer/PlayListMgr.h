#ifndef _PLAYLISTMGR_H_
#define _PLAYLISTMGR_H_
#include "Singleton.h"
namespace KY
{
	struct SoundInfo
	{
		fs::wpath	fileName;
		uint32		playTimes;
	};

	typedef std::vector<SoundInfo>	SoundInfoList;

	class PlayList
	{
	public:
		PlayList(const std::wstring &name = L"");
		PlayList(const fs::wpath &plName);
		~PlayList();

	public:		
		const SoundInfoList& GetPLInfoList() const;
		bool AddSound(const SoundInfo &sound);
		bool AddSound(const fs::wpath &soundPath, uint32 playTimes);

		const SoundInfo* GetSoundInfo(uint32 idx) const;

	public:
		bool Save(const fs::wpath &p) const;
		bool Load(const fs::wpath &p);

	public:
		inline const std::wstring& GetName() const;

	private:
		SoundInfoList	m_PLList;
		std::wstring	m_Name;
	};

	inline const std::wstring& PlayList::GetName() const
	{
		return m_Name;
	}

	class PlayListMgr : public Singleton < PlayListMgr >
	{
	public:
		PlayListMgr();
		~PlayListMgr();

		void Init(const fs::wpath &plFolder);

		void AddPlayList(const PlayList &pl);

		bool SetCurPlayListName(const std::wstring &name);
		const PlayList* GetCurPlayList() const;
		PlayList* GetCurPlayList();



	private:
		typedef std::unordered_map<std::wstring, PlayList>	PlayListMap;

		PlayListMap		m_PLMap;
		std::wstring	m_CurPLName;
	};
}
#endif //_PLAYLISTMGR_H_