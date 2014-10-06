#ifndef _PLAYERSETTING_H_
#define _PLAYERSETTING_H_
#include "Singleton.h"
#include "CommonDef.h"

namespace tinyxml2
{
	class XMLDocument;
}


namespace KY
{
	class PlayerSetting : public Singleton<PlayerSetting>
	{
	public:
		PlayerSetting();
		~PlayerSetting();

		void Init();

		PlayControl GetPlayControl() const;
		void SetPlayControl(PlayControl pc);
		uint32 GetLastPlayingIdx() const;

		bool Save();

	private:
		PlayControl	m_PlayCtrl;
		uint32		m_LastPlayIdx;
		tinyxml2::XMLDocument *m_pDoc;
	};

}
#endif //_PLAYERSETTING_H_