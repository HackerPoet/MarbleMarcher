/* This file is part of the Marble Marcher (https://github.com/HackerPoet/MarbleMarcher).
* Copyright(C) 2018 CodeParade
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
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
#include "Res.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

Res::Res(int id) {
  HRSRC src = ::FindResource(NULL, MAKEINTRESOURCE(id), RT_RCDATA);
  ptr = ::LockResource(::LoadResource(NULL, src));
  size = (size_t)::SizeofResource(NULL, src);
}
