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
#ifndef HPL_LOWLEVELINPUT_ANDROID_H
#define HPL_LOWLEVELINPUT_ANDROID_H

#include "input/LowLevelInput.h"

namespace hpl
{
	class iLowLevelGraphics;

	class cLowLevelInputAndroid : public iLowLevelInput
	{
	public:
		cLowLevelInputAndroid(iLowLevelGraphics *apLowLevelGraphics);
		~cLowLevelInputAndroid();

		void LockInput(bool abX);

		void BeginInputUpdate();
		void EndInputUpdate();

		iMouse* CreateMouse();
		iKeyboard* CreateKeyboard();

	private:
		iLowLevelGraphics *mpLowLevelGraphics;
	};
};
#endif // HPL_LOWLEVELINPUT_ANDROID_H

