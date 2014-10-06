#include "stdafx.h"

#include "SoundSystem.h"
#include "PlayListMgr.h"
#include "Utils.h"
#include "FMOD/fmod.hpp"

namespace KY
{
#define KY_MOD_VER(_TOCHECK)	BOOST_VERIFY(FMOD_OK == _TOCHECK)
	SoundSystem::SoundSystem()
		: m_pModSystem(nullptr)
		, m_pCurPlayingSound(nullptr)
		, m_pCurPlayingChannel(nullptr)
		, m_bExist(false)
	{
		KY_MOD_VER(FMOD::System_Create(&m_pModSystem));
		KY_MOD_VER(m_pModSystem->init(48, FMOD_INIT_NORMAL|FMOD_2D, nullptr));

		m_WatchThread = std::thread(WatchThread);
	}

	SoundSystem::~SoundSystem()
	{
		m_bExist = true;
		m_WatchThread.join();
		clearCurPlay();

		if (nullptr != m_pModSystem)
		{
			m_pModSystem->release();
			m_pModSystem = nullptr;
		}
	}	

	bool SoundSystem::IsPlaying() const
	{
		bool bPaly = false;
		if (nullptr != m_pCurPlayingChannel)
		{
			return FMOD_OK == m_pCurPlayingChannel->isPlaying(&bPaly);
		}

		return bPaly;
	}

	void SoundSystem::PlaySound(uint32 idx, bool bPlayImmediately /*= true*/)
	{
		std::lock_guard<std::mutex> guard(m_PlayNextMutex);
		auto pl = PlayListMgr::Inst()->GetCurPlayList();
		auto soundInfo = pl->GetSoundInfo(idx);
		++soundInfo->playTimes;
		pl->Save(pl->GetSavePath());

		PlaySound(soundInfo->fileName, bPlayImmediately);
	}


	void SoundSystem::PlaySound(const std::wstring &fileName, bool bPlayImmediately /*= true*/)
	{
		clearCurPlay();
		KY_MOD_VER(m_pModSystem->createSound(Utils::utf16_to_utf8(fileName).c_str(), FMOD_DEFAULT, 0, &m_pCurPlayingSound));

		KY_MOD_VER(m_pModSystem->update());
		KY_MOD_VER(m_pModSystem->playSound(m_pCurPlayingSound, nullptr, !bPlayImmediately, &m_pCurPlayingChannel));
	}

	void SoundSystem::Repaly()
	{
		BOOST_ASSERT(nullptr != m_pCurPlayingSound);
		BOOST_ASSERT(nullptr == m_pCurPlayingChannel);
		KY_MOD_VER(m_pModSystem->playSound(m_pCurPlayingSound, nullptr, false, &m_pCurPlayingChannel));
	}

	void SoundSystem::StopPlay()
	{
		if (nullptr != m_pCurPlayingChannel)
		{
			m_pCurPlayingChannel->stop();
			m_pCurPlayingChannel = nullptr;
		}
	}

	void SoundSystem::clearCurPlay()
	{
		if (nullptr != m_pCurPlayingChannel)
		{
			m_pCurPlayingChannel->stop();
			m_pCurPlayingChannel = nullptr;
		}

		if (nullptr != m_pCurPlayingSound)
		{
			m_pCurPlayingSound->release();
			m_pCurPlayingSound = nullptr;
		}
	}

	bool SoundSystem::PauseCurPaly(bool bPause)
	{
		if (nullptr != m_pCurPlayingChannel)
		{
			m_pCurPlayingChannel->setPaused(bPause);
			return true;
		}

		return false;
	}

	void SoundSystem::PlayNextSound()
	{
		auto pl = PlayListMgr::Inst()->GetCurPlayList();

		const auto nextIdx = pl->GetPlayingIdx() + 1;

		pl->SetPlayingIdx(nextIdx);
		
		PlaySound(nextIdx, true);		
	}

	void SoundSystem::WatchThread()
	{
		auto pSoundSys = SoundSystem::Inst();
		while (!pSoundSys->IsExist())
		{
			::Sleep(500);

			if (!pSoundSys->IsReady())
				continue;

			pSoundSys->Update();
			
			if (pSoundSys->IsPlaying() )
				continue;

			pSoundSys->PlayNextSound();
		}
	}

	bool SoundSystem::IsReady() const
	{
		return nullptr != m_pCurPlayingSound && nullptr != m_pCurPlayingChannel;
	}

	void SoundSystem::Update()
	{
		KY_MOD_VER(m_pModSystem->update());
	}

}