/* This file is part of the Marble Marcher (https://github.com/HackerPoet/MarbleMarcher).
* Copyright(C) 2018 CodeParade
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include <fstream>
#include <sstream>

class Settings {
public:
  Settings() :
    mute(false),
    mouse_sensitivity(0) {
  }

  void Load(const std::string& fname) {
    std::ifstream fin(fname, std::ios::binary);
    if (!fin) { return; }
    fin.read((char*)this, sizeof(this));
  }
  void Save(const std::string& fname) {
    std::ofstream fout(fname, std::ios::binary);
    if (!fout) { return; }
    fout.write((char*)this, sizeof(this));
  }

  bool   mute;
  int    mouse_sensitivity;
};

class AdditionalSettings
{
public:
	int screenshot_width;
	int screenshot_height;

// TODO
//	bool PBR_rendering;
//	bool shadows;

	AdditionalSettings() :
		screenshot_width(1920),
		screenshot_height(1080)
	{}

	void Load(const std::string& fname) {
		int increment = 0;
		std::ifstream config;
		config.open(fname);
		if (config.fail())
		{
			return;
		}
		std::string line;
		while (getline(config, line))
		{
			if (line.substr(0, 1) != "#")
			{
				increment++;
				std::istringstream iss(line);
				float num;
				while ((iss >> num))
				{
					switch (increment)
					{
					case 1:
						screenshot_width = num;
						break;
					case 2:
						screenshot_height = num;
						break;

					//TODO
					case 3:
					
						break;
					case 4:
						
						break;
					default:
						break;
					}
				}
			}
		}
	}
};