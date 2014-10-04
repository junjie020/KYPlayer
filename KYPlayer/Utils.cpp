#include "stdafx.h"

#include "Utils.h"

namespace KY
{
	

	std::string Utils::utf16_to_utf8(const std::wstring &src)
	{
		auto tmp = src;
		const auto len = ::WideCharToMultiByte(CP_UTF8, 0, &*tmp.begin(), -1, nullptr, 0, nullptr, FALSE);

		std::string result;
		result.resize(len);

		::WideCharToMultiByte(CP_UTF8, 0, &*tmp.begin(), -1, &*result.begin(), len, nullptr, FALSE);

		return result;
	}

	std::wstring Utils::utf8_to_utf16(const std::string &src)
	{
		auto tmp = src;
		const auto len = ::MultiByteToWideChar(CP_UTF8, 0, &*tmp.begin(), -1, nullptr, 0);

		std::wstring result;
		result.resize(len);

		::MultiByteToWideChar(CP_UTF8, 0, &*tmp.begin(), -1, &*result.begin(), len);

		return result;
	}

}