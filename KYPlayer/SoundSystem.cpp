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
	{
		KY_MOD_VER(FMOD::System_Create(&m_pModSystem));
		KY_MOD_VER(m_pModSystem->init(48, FMOD_INIT_NORMAL, nullptr));
	}

	SoundSystem::~SoundSystem()
	{
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
			KY_MOD_VER(m_pCurPlayingChannel->isPlaying(&bPaly));
		}

		return bPaly;		
	}

	static FMOD_RESULT F_CALLBACK ChannelCallBack(FMOD_CHANNELCONTROL *channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void *commanddata1, void *commanddata2)
	{
		switch (controltype)
		{
		case FMOD_CHANNELCONTROL_CALLBACK_END:
			SoundSystem::Inst()->PlayNextSound();
			return FMOD_OK;
		}

		return FMOD_OK;
	}

	void SoundSystem::PlaySound(uint32 idx, bool bPlayImmediately /*= true*/)
	{
		auto pl = PlayListMgr::Inst()->GetCurPlayList();
		auto soundInfo = pl->GetSoundInfo(idx);

		PlaySound(soundInfo->fileName, bPlayImmediately);
	}


	void SoundSystem::PlaySound(const std::wstring &fileName, bool bPlayImmediately /*= true*/)
	{
		clearCurPlay();
		KY_MOD_VER(m_pModSystem->createSound(Utils::utf16_to_utf8(fileName).c_str(), FMOD_DEFAULT, 0, &m_pCurPlayingSound));

		KY_MOD_VER(m_pModSystem->playSound(m_pCurPlayingSound, nullptr, !bPlayImmediately, &m_pCurPlayingChannel));

		if (nullptr != m_pCurPlayingChannel)
		{
			m_pCurPlayingChannel->setCallback(ChannelCallBack);
		}
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

		PlaySound(nextIdx, true);
	}

	void SoundSystem::Update()
	{
		KY_MOD_VER(m_pModSystem->update());
	}



}