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

		std::wstring GetLastListName() const;
		void SetLastListName(const std::wstring &name);

		fs::wpath GetListFolder() const;
		void SetListFolder(const fs::wpath &pp);

		PlayControl GetPlayControl() const;
		void SetPlayControl(PlayControl pc);

	private:
		std::wstring m_LastListName;
		fs::wpath m_PlayListFolder;

		PlayControl	m_PlayCtrl;

		tinyxml2::XMLDocument *m_pDoc;
	};

}
#endif //_PLAYERSETTING_H_