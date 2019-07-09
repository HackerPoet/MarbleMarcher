#include "Localization.h"

Localization LOCAL;

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

std::wstring utf8_to_wstring(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.from_bytes(str);
}

void Localization::LoadLocalFromFile(fs::path path)
{
	std::ifstream local_file(path);

	int element = 0;

	std::string line;
	std::string element_name;
	std::wstring element_text;
	std::string lang;

	std::map<std::string, std::wstring> local;

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	while (std::getline(local_file, line))
	{
		if (line.substr(0, 1) != "#")
		{
			element_text.append(converter.from_bytes(line) + L"\n");
		}
		else
		{
			if (element != 0)
			{
				local[element_name] = element_text;
			}
			line.erase(std::remove(line.begin(), line.end(), '#'), line.end());
			element_name = line;
			if (element == 0)
			{
				lang = element_name;
			}
			element_text.clear();
			element++;
		}
	}
	//last element
	local[element_name] = element_text;

	local_file.close();

	locales[lang] = local;
}

void Localization::SetLanguage(std::string lang)
{
	cur_language = lang;
}

std::wstring Localization::operator[](std::string str)
{
	return locales[cur_language][str];
}
