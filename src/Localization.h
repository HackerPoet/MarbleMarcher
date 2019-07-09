#pragma once
#include <map>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class Localization
{
public:
	std::map<std::string, std::map<std::string, std::string>> locales;
	std::string cur_language;

	Localization();

	void LoadLocalsFromFolder(std::string folder);
	void LoadLocalFromFile(fs::path path);
	void SetLanguage(std::string lang);

	std::string operator[](std::string str);
};