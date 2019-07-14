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

std::string tostring(std::wstring string_to_convert)
{
	//setup converter
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	return converter.to_bytes(string_to_convert);
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
	std::map<std::string, sf::Font> fontmap;

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	int line_num = 0;
	while (std::getline(local_file, line))
	{
		if (line.substr(0, 1) != "#")
		{
			element_text.append(((line_num != 0)?L"\n":L"") + converter.from_bytes(line));
			line_num++;
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
			line_num = 0;
		}
	}
	//last element
	local[element_name] = element_text;

	//Load the font
	sf::Font font;
	std::wstring assets = L"assets/";
	if (!font.loadFromFile(tostring(assets + local["font_1"]))) {
		ERROR_MSG("Unable to load font");
	}
	//Load the mono font
	sf::Font font_mono;
	if (!font_mono.loadFromFile(tostring(assets + local["font_2"]))) {
		ERROR_MSG("Unable to load mono font");
	}

	fontmap["default"] = font;
	fontmap["mono"] = font_mono;

	local_file.close();

	locales[lang] = local;
	fonts[lang] = fontmap;
}

void Localization::SetLanguage(std::string lang)
{
	cur_language = lang;
}

std::wstring Localization::operator[](std::string str)
{
	return locales[cur_language][str];
}

sf::Font & Localization::operator()(std::string str)
{
	if (fonts[cur_language].count(str) != 0)
		return fonts[cur_language][str];
	else
		return sf::Font();
}
