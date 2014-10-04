#ifndef _UTILS_H_
#define _UTILS_H_
namespace KY
{
	class Utils
	{
	public:
		static std::string utf16_to_utf8(const std::wstring &src);
		static std::wstring utf8_to_utf16(const std::string &src);
	};
}
#endif //_UTILS_H_