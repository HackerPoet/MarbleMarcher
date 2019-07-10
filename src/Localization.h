#pragma once
#include <map>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <Level.h>
#include <locale>
#include <Res.h>
#include <codecvt>

namespace fs = std::filesystem;

class Localization
{
public:
	Localization();

	void LoadLocalsFromFolder(std::string folder);
	void LoadLocalFromFile(fs::path path);
	void SetLanguage(std::string lang);

	std::wstring operator[](std::string str);
	sf::Font& operator()(std::string str);

private:
	std::string cur_language;
	std::map<std::string, std::map<std::string, std::wstring>> locales;
	std::map<std::string, std::map<std::string, sf::Font>> fonts;
};

extern Localization LOCAL;

std::wstring utf8_to_wstring(const std::string& str);