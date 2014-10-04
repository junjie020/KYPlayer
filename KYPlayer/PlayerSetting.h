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

		std::wstring GetLastListName() const;
		std::wstring GetListFolder() const;

		PlayControl GetPlayControl() const;

	private:
		std::wstring m_LastPlayingListName;
		std::wstring m_PlayListFolder;

		PlayControl	m_PlayCtrl;

		tinyxml2::XMLDocument *m_pDoc;
	};

}
#endif //_PLAYERSETTING_H_