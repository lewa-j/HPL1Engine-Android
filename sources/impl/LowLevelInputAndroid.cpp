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
#include "impl/LowLevelSystemAndroid.h"
#include "input/Mouse.h"
#include "input/Keyboard.h"
#include "graphics/LowLevelGraphics.h"

namespace hpl
{
	class cLowLevelInputAndroid;

	class cMouseAndroid : public iMouse
	{
	public:
		cMouseAndroid(cLowLevelInputAndroid *apLowLevelInput, iLowLevelGraphics *apLowLevelGraphics)
		 : iMouse("Android Mouse"), mpLowLevelInput(apLowLevelInput),mpLowLevelGraphics(apLowLevelGraphics)
		{
			for(auto& b : mvMButtonArray){
				b = false;
			}

		}

		void Update()
		{
			cVector2f vScreenSize = mpLowLevelGraphics->GetScreenSize();
			cVector2f vVirtualSize = mpLowLevelGraphics->GetVirtualSize();

			cVector2f vLastMouseAbsPos = mvMouseAbsPos;
			for(const auto &event : mpLowLevelInput->mlstEvents)
			{
				if(event.type != eAInputType_Touch &&
					event.type != eAInputType_Mouse &&
					event.action != AMOTION_EVENT_ACTION_DOWN &&
					event.action != AMOTION_EVENT_ACTION_UP &&
					event.action != AMOTION_EVENT_ACTION_MOVE)
				{
					continue;
				}

				if(event.action == AMOTION_EVENT_ACTION_MOVE)
				{
					mvMouseAbsPos = cVector2f(event.x,event.y);
					mvMouseAbsPos = (mvMouseAbsPos/vScreenSize)*vVirtualSize;
				}
				else
				{
					bool bButtonIsDown = event.action == AMOTION_EVENT_ACTION_DOWN;
					mvMButtonArray[eMButton_Left] = bButtonIsDown;
				}

				if(event.type == eAInputType_Mouse)
				{
					mvMButtonArray[eMButton_Left] = event.key & AMOTION_EVENT_BUTTON_PRIMARY;
					mvMButtonArray[eMButton_Right] = event.key & AMOTION_EVENT_BUTTON_SECONDARY;
					mvMButtonArray[eMButton_Middle] = event.key & AMOTION_EVENT_BUTTON_TERTIARY;
				}
			}

			mvMouseRelPos = mvMouseAbsPos - vLastMouseAbsPos;
			//mvMouseRelPos = cVector2f((float)lX,(float)lY);
			//mvMouseRelPos = (mvMouseRelPos/vScreenSize)*vVirtualSize;
		}

		bool ButtonIsDown(eMButton mButton)
		{
			return mvMButtonArray[mButton];
		}
		cVector2f GetAbsPosition()
		{
			return mvMouseAbsPos;
		}
		cVector2f GetRelPosition()
		{
			cVector2f vPos = mvMouseRelPos;
			mvMouseRelPos = cVector2f(0,0);
			return vPos;
		}
		void Reset()
		{
			mvMouseRelPos = cVector2f(0,0);
		}
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

		bool mvMButtonArray[eMButton_LastEnum];

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
		 : iKeyboard("Android Keyboard"), mpLowLevelInput(apLowLevelInput)
		{
			mvKeyArray.resize(eKey_LastEnum);
			mvKeyArray.assign(mvKeyArray.size(),false);
		}

		void Update()
		{
			mlstKeysPressed.clear();
			for(const auto &event : mpLowLevelInput->mlstEvents)
			{
				if(event.type != eAInputType_Key &&
					event.action != AKEY_EVENT_ACTION_DOWN &&
					event.action != AKEY_EVENT_ACTION_UP)
				{
					continue;
				}

				eKey key = AndroidToKey(event.key);
				mvKeyArray[key] = (event.action == AKEY_EVENT_ACTION_DOWN);

				if(event.action == AKEY_EVENT_ACTION_DOWN)
				{
					mModifier = eKeyModifier_NONE;

					if(event.modifiers & AMETA_CTRL_ON)		mModifier |= eKeyModifier_CTRL;
					if(event.modifiers & AMETA_SHIFT_ON)	mModifier |= eKeyModifier_SHIFT;
					if(event.modifiers & AMETA_ALT_ON)		mModifier |= eKeyModifier_ALT;
					if(event.modifiers & AMETA_META_ON)		mModifier |= eKeyModifier_META;

					mlstKeysPressed.push_back(cKeyPress(key,event.uchar,mModifier));
				}
			}
		}

		//Keyboard specific
		bool KeyIsDown(eKey aKey){return mvKeyArray[aKey];}
		cKeyPress GetKey()
		{
			cKeyPress key = mlstKeysPressed.front();
			mlstKeysPressed.pop_front();
			return key;
		}
		bool KeyIsPressed(){return !mlstKeysPressed.empty();}
		eKeyModifier GetModifier(){return mModifier;}
		tString KeyToString(eKey){return "None";}
		eKey StringToKey(tString){return eKey_NONE;}

	private:
		eKey AndroidToKey(int alKey)
		{
			switch(alKey)
			{
				case AKEYCODE_DEL: return eKey_BACKSPACE;
				case AKEYCODE_TAB: return  eKey_TAB;
				case AKEYCODE_CLEAR: return eKey_CLEAR;
				case AKEYCODE_ENTER: return eKey_RETURN;
				//case SDLK_PAUSE: return eKey_PAUSE;
				case AKEYCODE_ESCAPE: return eKey_ESCAPE;
				case AKEYCODE_SPACE: return eKey_SPACE;
				//case SDLK_EXCLAIM: return eKey_EXCLAIM;
				//case SDLK_QUOTEDBL: return eKey_QUOTEDBL;
				case AKEYCODE_POUND: return eKey_HASH;
				//case SDLK_DOLLAR: return eKey_DOLLAR;
				//case SDLK_AMPERSAND: return eKey_AMPERSAND;
				case AKEYCODE_APOSTROPHE: return eKey_QUOTE;
				//case AKEYCODE_NUMPAD_LEFT_PAREN: return eKey_LEFTPAREN;
				//case AKEYCODE_NUMPAD_RIGHT_PAREN: return eKey_RIGHTPAREN;
				case AKEYCODE_STAR: return eKey_ASTERISK;
				case AKEYCODE_PLUS: return eKey_PLUS;
				case AKEYCODE_COMMA: return eKey_COMMA;
				case AKEYCODE_MINUS: return eKey_MINUS;
				case AKEYCODE_PERIOD: return eKey_PERIOD;
				case AKEYCODE_SLASH: return eKey_SLASH;
				case AKEYCODE_0: return eKey_0;
				case AKEYCODE_1: return eKey_1;
				case AKEYCODE_2: return eKey_2;
				case AKEYCODE_3: return eKey_3;
				case AKEYCODE_4: return eKey_4;
				case AKEYCODE_5: return eKey_5;
				case AKEYCODE_6: return eKey_6;
				case AKEYCODE_7: return eKey_7;
				case AKEYCODE_8: return eKey_8;
				case AKEYCODE_9: return eKey_9;
				//case SDLK_COLON: return eKey_COLON;
				case AKEYCODE_SEMICOLON: return eKey_SEMICOLON;
				//case SDLK_LESS: return eKey_LESS;
				case AKEYCODE_EQUALS: return eKey_EQUALS;
				//case SDLK_GREATER: return eKey_GREATER;
				//case SDLK_QUESTION: return eKey_QUESTION;
				case AKEYCODE_AT: return eKey_AT;
				case AKEYCODE_LEFT_BRACKET: return eKey_LEFTBRACKET;
				case AKEYCODE_BACKSLASH: return eKey_BACKSLASH;
				case AKEYCODE_RIGHT_BRACKET: return eKey_RIGHTBRACKET;
				//case SDLK_CARET: return eKey_CARET;
				//case SDLK_UNDERSCORE: return eKey_UNDERSCORE;
				case AKEYCODE_GRAVE: return eKey_BACKQUOTE;
				case AKEYCODE_A: return eKey_a;
				case AKEYCODE_B: return eKey_b;
				case AKEYCODE_C: return eKey_c;
				case AKEYCODE_D: return eKey_d;
				case AKEYCODE_E: return eKey_e;
				case AKEYCODE_F: return eKey_f;
				case AKEYCODE_G: return eKey_g;
				case AKEYCODE_H: return eKey_h;
				case AKEYCODE_I: return eKey_i;
				case AKEYCODE_J: return eKey_j;
				case AKEYCODE_K: return eKey_k;
				case AKEYCODE_L: return eKey_l;
				case AKEYCODE_M: return eKey_m;
				case AKEYCODE_N: return eKey_n;
				case AKEYCODE_O: return eKey_o;
				case AKEYCODE_P: return eKey_p;
				case AKEYCODE_Q: return eKey_q;
				case AKEYCODE_R: return eKey_r;
				case AKEYCODE_S: return eKey_s;
				case AKEYCODE_T: return eKey_t;
				case AKEYCODE_U: return eKey_u;
				case AKEYCODE_V: return eKey_v;
				case AKEYCODE_W: return eKey_w;
				case AKEYCODE_X: return eKey_x;
				case AKEYCODE_Y: return eKey_y;
				case AKEYCODE_Z: return eKey_z;
				case AKEYCODE_FORWARD_DEL: return eKey_DELETE;
				case AKEYCODE_NUMPAD_0: return eKey_KP0;
				case AKEYCODE_NUMPAD_1: return eKey_KP1;
				case AKEYCODE_NUMPAD_2: return eKey_KP2;
				case AKEYCODE_NUMPAD_3: return eKey_KP3;
				case AKEYCODE_NUMPAD_4: return eKey_KP4;
				case AKEYCODE_NUMPAD_5: return eKey_KP5;
				case AKEYCODE_NUMPAD_6: return eKey_KP6;
				case AKEYCODE_NUMPAD_7: return eKey_KP7;
				case AKEYCODE_NUMPAD_8: return eKey_KP8;
				case AKEYCODE_NUMPAD_9: return eKey_KP9;
				case AKEYCODE_NUMPAD_DOT: return eKey_KP_PERIOD;
				case AKEYCODE_NUMPAD_DIVIDE: return eKey_KP_DIVIDE;
				case AKEYCODE_NUMPAD_MULTIPLY: return eKey_KP_MULTIPLY;
				case AKEYCODE_NUMPAD_SUBTRACT: return eKey_KP_MINUS;
				case AKEYCODE_NUMPAD_ADD: return eKey_KP_PLUS;
				case AKEYCODE_NUMPAD_ENTER: return eKey_KP_ENTER;
				case AKEYCODE_NUMPAD_EQUALS: return eKey_KP_EQUALS;
				case AKEYCODE_DPAD_UP: return eKey_UP;
				case AKEYCODE_DPAD_DOWN: return eKey_DOWN;
				case AKEYCODE_DPAD_RIGHT: return eKey_RIGHT;
				case AKEYCODE_DPAD_LEFT: return eKey_LEFT;
				case AKEYCODE_INSERT: return eKey_INSERT;
				case AKEYCODE_MOVE_HOME: return eKey_HOME;
				case AKEYCODE_MOVE_END: return eKey_END;
				case AKEYCODE_PAGE_UP: return eKey_PAGEUP;
				case AKEYCODE_PAGE_DOWN: return eKey_PAGEDOWN;
				case AKEYCODE_F1: return eKey_F1;
				case AKEYCODE_F2: return eKey_F2;
				case AKEYCODE_F3: return eKey_F3;
				case AKEYCODE_F4: return eKey_F4;
				case AKEYCODE_F5: return eKey_F5;
				case AKEYCODE_F6: return eKey_F6;
				case AKEYCODE_F7: return eKey_F7;
				case AKEYCODE_F8: return eKey_F8;
				case AKEYCODE_F9: return eKey_F9;
				case AKEYCODE_F10: return eKey_F10;
				case AKEYCODE_F11: return eKey_F11;
				case AKEYCODE_F12: return eKey_F12;
				//case SDLK_F13: return eKey_F13;
				//case SDLK_F14: return eKey_F14;
				//case SDLK_F15: return eKey_F15;
				case AKEYCODE_NUM_LOCK: return eKey_NUMLOCK;
				case AKEYCODE_CAPS_LOCK: return eKey_CAPSLOCK;
				case AKEYCODE_SCROLL_LOCK: return eKey_SCROLLOCK;
				case AKEYCODE_SHIFT_RIGHT: return eKey_RSHIFT;
				case AKEYCODE_SHIFT_LEFT: return eKey_LSHIFT;
				case AKEYCODE_CTRL_RIGHT: return eKey_RCTRL;
				case AKEYCODE_CTRL_LEFT: return eKey_LCTRL;
				case AKEYCODE_ALT_RIGHT: return eKey_RALT;
				case AKEYCODE_ALT_LEFT: return eKey_LALT;
				//case SDLK_RMETA: return eKey_RMETA;
				//case SDLK_RMETA: return eKey_LMETA;
				case AKEYCODE_META_LEFT: return eKey_LSUPER;
				case AKEYCODE_META_RIGHT: return eKey_RSUPER;
				//case SDLK_MODE: return eKey_MODE;
				case AKEYCODE_HELP: return eKey_HELP;
				//case SDLK_PRINT: return eKey_PRINT;
				case AKEYCODE_SYSRQ: return eKey_SYSREQ;
				case AKEYCODE_BREAK: return eKey_BREAK;
				case AKEYCODE_MENU: return eKey_MENU;
				case AKEYCODE_POWER: return eKey_POWER;
				//case SDLK_EURO: return eKey_EURO;
			}

			return eKey_NONE;
		}
		eKeyModifier mModifier;
		std::vector<bool> mvKeyArray;
		std::list<cKeyPress> mlstKeysPressed;
		cLowLevelInputAndroid *mpLowLevelInput;
	};


	cLowLevelInputAndroid::cLowLevelInputAndroid(iLowLevelGraphics *apLowLevelGraphics)
	{
		mpLowLevelGraphics = apLowLevelGraphics;

		gpAndroidApp->userData = this;
	}

	cLowLevelInputAndroid::~cLowLevelInputAndroid()
	{
		gpAndroidApp->userData = nullptr;
	}

	void cLowLevelInputAndroid::LockInput(bool abX)
	{

	}

	void cLowLevelInputAndroid::BeginInputUpdate()
	{
		int ident;
		int events;
		android_poll_source* source;

		while ((ident=ALooper_pollAll(0, nullptr, &events,(void**)&source)) >= 0)
		{
			// Process this event.
			if (source != nullptr) {
				source->process(gpAndroidApp, source);
			}
		}
	}

	void cLowLevelInputAndroid::AddEvent(const cAndroidInputEvent &aEvent)
	{
		mlstEvents.push_back(aEvent);
	}

	void cLowLevelInputAndroid::EndInputUpdate()
	{
		mlstEvents.clear();
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

