#ifndef _SOUNDSYSTEM_H_
#define _SOUNDSYSTEM_H_

#include "Singleton.h"

namespace FMOD
{
	class System;
	class Sound;
	class Channel;
}

namespace KY
{
	class SoundSystem : public Singleton<SoundSystem>
	{
	public:
		SoundSystem();
		~SoundSystem();

		bool IsPlaying() const;
		void PlaySound(uint32 idx, bool bPlayImmediately = true);
		
		void StopPlay();
		void Repaly();
		bool PauseCurPaly(bool bPause);

		void PlayNextSound();

	private:
		void PlaySound(const std::wstring &fileName, bool bPlayImmediately = true);
		void clearCurPlay();


	private:
		static void WatchThread();

		void Update();
		bool IsReady() const;
		bool IsExist() const;

	private:
		FMOD::System *m_pModSystem;
		FMOD::Sound  *m_pCurPlayingSound;
		FMOD::Channel*m_pCurPlayingChannel;

	private:
		std::mutex	m_PlayNextMutex;
		bool		m_bExist;
		std::thread m_WatchThread;
	};

	inline bool SoundSystem::IsExist() const
	{
		return m_bExist;
	}
}
#endif //_SOUNDSYSTEM_H_