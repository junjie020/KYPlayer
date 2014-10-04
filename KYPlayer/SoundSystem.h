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
		void PlaySound(const std::string &fullName, bool bPlayImmediately = true);
		void StopPlay();
		void Repaly();
		bool PauseCurPaly(bool bPause);

	private:
		void clearCurPlay();

	private:
		FMOD::System *m_pModSystem;
		FMOD::Sound  *m_pCurPlayingSound;
		FMOD::Channel*m_pCurPlayingChannel;
	};
}
#endif //_SOUNDSYSTEM_H_