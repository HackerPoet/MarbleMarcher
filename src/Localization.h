#pragma once
#include <map>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <Level.h>
#include <locale>
#include <codecvt>

namespace fs = std::filesystem;

class Localization
{
public:
	std::map<std::string, std::map<std::string, std::wstring>> locales;
	std::string cur_language;

	Localization();

	void LoadLocalsFromFolder(std::string folder);
	void LoadLocalFromFile(fs::path path);
	void SetLanguage(std::string lang);

	std::wstring operator[](std::string str);
};

extern Localization LOCAL;