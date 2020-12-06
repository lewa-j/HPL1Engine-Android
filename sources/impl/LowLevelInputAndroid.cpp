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
#include "impl/LowLevelInputAndroid.h"
#include "input/Mouse.h"
#include "input/Keyboard.h"

namespace hpl
{
	class cLowLevelInputAndroid;

	class cMouseAndroid : public iMouse
	{
	public:
		cMouseAndroid(cLowLevelInputAndroid *apLowLevelInput, iLowLevelGraphics *apLowLevelGraphics)
		 : iMouse("Android Mouse")
		{}

		bool ButtonIsDown(eMButton){return false;}
		void Update(){}
		cVector2f GetAbsPosition(){return {0.0f,0.0f};}
		cVector2f GetRelPosition(){return {0.0f,0.0f};}
		void Reset(){}
		/**
		 * Sets how much smoothening there will be in the RelPosition.
		 * The percentages are just ratios, so min/max 1/10 equals 0.1/1
		 * \param afMinPercent The influence of the oldest value
		 * \param afMaxPercent The influence of the newest value
		 * \param alBufferSize The number of values recorded
		 */
		void SetSmoothProperties(float afMinPercent,
			float afMaxPercent,unsigned int alBufferSize){}

	private:
		cVector2f mvMouseAbsPos;
		cVector2f mvMouseRelPos;

		std::vector<bool> mvMButtonArray;

		tVector2fList mlstMouseCoord;

		float mfMaxPercent;
		float mfMinPercent;
		int mlBufferSize;

		cLowLevelInputAndroid *mpLowLevelInput;
		iLowLevelGraphics *mpLowLevelGraphics;

		bool mbWheelUpMoved;
		bool mbWheelDownMoved;
	};
	
	class cKeyboardAndroid : public iKeyboard
	{
	public:
		cKeyboardAndroid(cLowLevelInputAndroid *apLowLevelInput)
		 : iKeyboard("Android Keyboard")
		{}

		void Update(){}

		//Keyboard specific
		bool KeyIsDown(eKey aKey){return false;}
		cKeyPress GetKey(){return {};}
		bool KeyIsPressed(){return false;}
		eKeyModifier GetModifier(){return 0;}
		tString KeyToString(eKey){return "";}
		eKey StringToKey(tString){return eKey_NONE;}

	private:
		eKeyModifier mModifier;
		std::vector<bool> mvKeyArray;
		std::list<cKeyPress> mlstKeysPressed;
		cLowLevelInputAndroid *mpLowLevelInput;
	};
	
	
	cLowLevelInputAndroid::cLowLevelInputAndroid(iLowLevelGraphics *apLowLevelGraphics)
	{
		mpLowLevelGraphics = apLowLevelGraphics;
	}
	
	cLowLevelInputAndroid::~cLowLevelInputAndroid()
	{
		
	}

	void cLowLevelInputAndroid::LockInput(bool abX)
	{
		
	}

	void cLowLevelInputAndroid::BeginInputUpdate()
	{
		//pollEvents
	}
	
	void cLowLevelInputAndroid::EndInputUpdate()
	{
		
	}

	iMouse* cLowLevelInputAndroid::CreateMouse()
	{
		return hplNew( cMouseAndroid,(this,mpLowLevelGraphics));
	}
	
	iKeyboard* cLowLevelInputAndroid::CreateKeyboard()
	{
		return hplNew( cKeyboardAndroid,(this) );
	}
}

