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

		const fs::wpath& GetSavePath() const;

	public:
		bool Save(const fs::wpath &p);
		bool Load(const fs::wpath &p);

	public:
		inline const std::wstring& GetName() const;
		inline void SetName(const std::wstring &name);

		inline uint32 GetPlayingIdx() const;
		inline void SetPlayingIdx(uint32 idx);

	private:
		SoundInfoList	m_PLList;
		std::wstring	m_Name;
		fs::wpath		m_FromPath;
		uint32			m_PlayingIdx;
	};

	inline const std::wstring& PlayList::GetName() const
	{
		return m_Name;
	}

	inline void PlayList::SetName(const std::wstring &name)
	{
		m_Name = name;
	}

	inline uint32 PlayList::GetPlayingIdx() const
	{
		return m_PlayingIdx;
	}

	inline void PlayList::SetPlayingIdx(uint32 idx)
	{
		m_PlayingIdx = idx;
	}

	inline const fs::wpath& PlayList::GetSavePath() const
	{
		return m_FromPath;
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

		bool ChangePlayListName(const std::wstring &oldName, const std::wstring &newName);

	public:
		typedef std::unordered_map<std::wstring, PlayList>	PlayListMap;
		inline const PlayListMap& GetPlayLists() const;

	private:
		

		PlayListMap		m_PLMap;
		std::wstring	m_CurPLName;
	};

	inline const PlayListMgr::PlayListMap& PlayListMgr::GetPlayLists() const
	{
		return m_PLMap;
	}
}
#endif //_PLAYLISTMGR_H_