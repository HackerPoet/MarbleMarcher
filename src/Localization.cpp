#include "Localization.h"

Localization::Localization()
{
}

void Localization::LoadLocalsFromFolder(std::string folder)
{
	std::vector<fs::path> files = GetFilesInFolder(folder, ".loc");
	for (int i = 0; i < files.size(); i++)
	{
		LoadLocalFromFile(files[i]);
	}
}

void Localization::LoadLocalFromFile(fs::path path)
{
}

void Localization::SetLanguage(std::string lang)
{
	cur_language = lang;
}

std::string Localization::operator[](std::string str)
{
	return locales[cur_language][str];
}
