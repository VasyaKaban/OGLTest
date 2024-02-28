#pragma once

#include <string_view>

constexpr std::string_view trim_spaces_front(std::string_view str) noexcept
{
	for(std::size_t i = 0; i < str.size(); i++)
	{
		if(!(str[i] == ' ' || str[i] == '\n'))
			return std::string_view(str.begin() + i, str.end());
	}

	return {};
}

constexpr std::string_view trim_spaces_back(std::string_view str) noexcept
{
	if(str.empty())
		return {};

	for(auto it = str.rbegin(); it != str.rend(); it++)
	{
		if(*it.base() != ' ' || *it.base() != '\n')
			return std::string_view(str.begin(), it.base() + 1);
	}

	return {};
}
