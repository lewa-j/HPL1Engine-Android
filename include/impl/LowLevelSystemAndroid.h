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
#ifndef HPL_LOWLEVELSYSTEM_ANDROID_H
#define HPL_LOWLEVELSYSTEM_ANDROID_H

#include "system/LowLevelSystem.h"
#include "impl/LowLevelSystemCommon.h"

#include <angelscript.h>
#include <android/native_window.h>
#include <android_native_app_glue.h>

namespace hpl
{
	extern ANativeWindow *gpAWindow;
	extern android_app *gpAndroidApp;

	class cLowLevelSystemAndroid : public iLowLevelSystem
	{
	public:
		cLowLevelSystemAndroid();
		~cLowLevelSystemAndroid();

		iScript* CreateScript(const tString& asName) override;

		bool AddScriptFunc(const tString& asFuncDecl, void* pFunc, int callConv) override;
		bool AddScriptVar(const tString& asVarDecl, void *pVar) override;

	private:
		asIScriptEngine *mpScriptEngine;
		cScriptOutput *mpScriptOutput;
		int mlHandleCount;
	};
}
#endif // HPL_LOWLEVELSYSTEM_ANDROID_H

