/*
 * Copyright (C) 2020 - lewa_j
 *
 * This file is part of HPL1 Engine.
 *
 * HPL1 Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HPL1 Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPL1 Engine.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "system/LowLevelSystem.h"
#include <angelscript.h>

namespace hpl
{
//------------------------------------------------------

class cLogWriter
{
public:
	cLogWriter(const tWString& asDefaultFile);
	~cLogWriter();

	void Write(const tString& asMessage);
	void Clear();

	void SetFileName(const tWString& asFile);

private:
	void ReopenFile();

	FILE* mpFile;
	tWString msFileName;
};

//------------------------------------------------------

class cScriptOutput// : public  asIOutputStream
{
public:
	cScriptOutput() : msMessage("") {}
	~cScriptOutput() {}

	void AddMessage(const asSMessageInfo* msg);
	void Display();
	void Clear();

private:
	tString msMessage;
};

}
