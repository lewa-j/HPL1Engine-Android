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

#include <stdio.h>
#include <stdlib.h>
#include <clocale>
#include <time.h>

#include <android/log.h>
#include <android_native_app_glue.h>

#include "impl/LowLevelSystemAndroid.h"
#include "impl/LowLevelInputAndroid.h"
#include "impl/SqScript.h"
#include "system/String.h"

#include "impl/scriptstdstring.h"

extern int hplMain(const hpl::tString &asCommandLine);

static void droid_handle_cmd(android_app* app, int32_t cmd)
{
	switch (cmd)
	{
		case APP_CMD_INIT_WINDOW:
			// The window is being shown, get it ready.
			hpl::gpAWindow = app->window;
			if (app->window != nullptr) {
				//init display
			}
			break;
		case APP_CMD_TERM_WINDOW:
			// The window is being hidden or closed, clean it up.
			hpl::gpAWindow = nullptr;
			//destroy display
			break;
		default:
			break;
	}
}

static int32_t droid_handle_input(android_app* app, AInputEvent* event)
{
	using namespace hpl;
	cLowLevelInputAndroid* pInput = (cLowLevelInputAndroid*)app->userData;
	if(!pInput)
		return 0;
	cAndroidInputEvent outEvent{};
	int it = AInputEvent_getType(event);
	int isrc = AInputEvent_getSource(event);
	if (it == AINPUT_EVENT_TYPE_KEY)
	{
		outEvent.type = eAInputType_Key;
		outEvent.key = AKeyEvent_getKeyCode(event);
		outEvent.action = AKeyEvent_getAction(event);
		outEvent.modifiers = AKeyEvent_getMetaState(event);
		//TODO unicode character
		pInput->AddEvent(outEvent);
		return 1;
	}
	else if (it == AINPUT_EVENT_TYPE_MOTION)
	{
		if(isrc == AINPUT_SOURCE_TOUCHSCREEN)
			outEvent.type = eAInputType_Touch;
		else if(isrc == AINPUT_SOURCE_MOUSE || isrc == AINPUT_SOURCE_MOUSE_RELATIVE){
			outEvent.type = eAInputType_Mouse;
			outEvent.key = AMotionEvent_getButtonState(event);
		}else
			return 0;
		outEvent.action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
		outEvent.x = AMotionEvent_getX(event, 0);
		outEvent.y = AMotionEvent_getY(event, 0);
		//TODO Multitouch
		//AMotionEvent_getPointerCount
		//AMotionEvent_getPointerId
		pInput->AddEvent(outEvent);
		return 1;
	}

	return 0;
}

void android_main(android_app *app)
{
	app->onAppCmd = droid_handle_cmd;
	app->onInputEvent = droid_handle_input;

	//wait until we have window
	int ident;
	int events;
	struct android_poll_source* source;
	while ((ident=ALooper_pollAll(-1, nullptr, &events,(void**)&source)) >= 0)
	{
		if (source != nullptr)
		{
			source->process(app, source);
		}
		if (app->destroyRequested != 0)
		{
			//stop engine
			return;
		}
		if(hpl::gpAWindow)
			break;
	}

	hpl::tString cmdline = "";
	/*for (int i=1; i < argc; i++) {
		if (cmdline.length()>0) {
			cmdline.append(" ").append(argv[i]);
		} else {
			cmdline.append(argv[i]);
		}
	}*/

	hpl::gpAndroidApp = app;
	int r = hplMain(cmdline);
	__android_log_print(ANDROID_LOG_INFO,"HPL1test","hplMain returned %d\n",r);

	ANativeActivity_finish(app->activity);

	while ((ident=ALooper_pollAll(-1, nullptr, &events,(void**)&source)) >= 0)
	{
		if (source != nullptr)
		{
			source->process(app, source);
		}
		if (app->destroyRequested != 0)
		{
			//stop engine
			return;
		}
	}
}

namespace hpl
{
	android_app *gpAndroidApp = nullptr;
	ANativeWindow *gpAWindow = nullptr;

	cLowLevelSystemAndroid::cLowLevelSystemAndroid()
	{
		mpScriptEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

		mpScriptOutput = hplNew( cScriptOutput, () );
		mpScriptEngine->SetMessageCallback(asMETHOD(cScriptOutput,AddMessage), mpScriptOutput, asCALL_THISCALL);

		RegisterStdString(mpScriptEngine);

		mlHandleCount = 0;

		Log("-------- THE HPL ENGINE LOG ------------\n\n");
	}

	//-----------------------------------------------------------------------

	cLowLevelSystemAndroid::~cLowLevelSystemAndroid()
	{
		/*Release all runnings contexts */

		mpScriptEngine->ShutDownAndRelease();
		hplDelete(mpScriptOutput);

		//perhaps not the best thing to skip :)
		//if(gpLogWriter)	hplDelete(gpLogWriter);
		//gpLogWriter = NULL;
	}
	
	unsigned long cLowLevelSystemAndroid::GetTime()
	{
		return GetApplicationTime();
	}

	//-----------------------------------------------------------------------

	cDate cLowLevelSystemAndroid::GetDate()
	{
		time_t lTime;
		time(&lTime);

		struct tm* pClock;
		pClock = localtime(&lTime);

		return DateFromGMTIme(pClock);
	}

	//-----------------------------------------------------------------------

	iScript* cLowLevelSystemAndroid::CreateScript(const tString& asName)
	{
		return hplNew( cSqScript, (asName,mpScriptEngine,mpScriptOutput,mlHandleCount++) );
	}

	//-----------------------------------------------------------------------

	bool cLowLevelSystemAndroid::AddScriptFunc(const tString& asFuncDecl, void* pFunc, int callConv)
	{
		if(mpScriptEngine->RegisterGlobalFunction(asFuncDecl.c_str(),
												asFUNCTION(pFunc),callConv)<0)
		{
			Error("Couldn't add func '%s'\n",asFuncDecl.c_str());
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cLowLevelSystemAndroid::AddScriptVar(const tString& asVarDecl, void *pVar)
	{
		if(mpScriptEngine->RegisterGlobalProperty(asVarDecl.c_str(),pVar)<0)
		{
			Error("Couldn't add var '%s'\n",asVarDecl.c_str());
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	void cLowLevelSystemAndroid::Sleep ( const unsigned int alMillisecs )
	{
		//TODO
		//SDL_Delay ( alMillisecs );
	}
	
	
	void CreateMessageBoxW (eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, va_list ap)
	{
		wchar_t text[2048];

		if (fmt == NULL)
			return;
		vswprintf(text, 2047, fmt, ap);

		tWString sMess = _W("");

		//TODO proper message box
		sMess += asCaption;
		sMess +=_W("\n\n");
		sMess += text;
		Error("%ls\n\n%ls",asCaption,text);
	}

	void CreateMessageBoxW ( eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, ...)
	{
		va_list ap;

		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		CreateMessageBoxW (eType, asCaption, fmt, ap);
		va_end(ap);
	}

	void CreateMessageBoxW ( const wchar_t* asCaption, const wchar_t *fmt, ...)
	{
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		CreateMessageBoxW( eMsgBoxType_Default, asCaption, fmt, ap );
		va_end(ap);
	}

	//-----------------------------------------------------------------------

	tWString GetSystemSpecialPath(eSystemPath aPathType)
	{
		switch (aPathType)
		{
		case eSystemPath_Personal: {
			return cString::To16Char(tString(gpAndroidApp->activity->externalDataPath));
		}
		default:
			return _W("");
		}
	}

	void SetWindowCaption(const tString &asName)
	{
		//TODO Implement
	}

	typedef long long unsigned int longtime_t;
	unsigned long GetApplicationTime()
	{
		static longtime_t g_PerformanceFrequency;
		static longtime_t g_ClockStart;
		longtime_t CurrentTime;
		struct timespec ts;

		if( !g_PerformanceFrequency )
		{
			struct timespec res;
			if( !clock_getres(CLOCK_MONOTONIC, &res) )
				g_PerformanceFrequency = 1000000000LL/res.tv_nsec;
		}
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return ts.tv_sec*1000 + ts.tv_nsec/1000000;
	}
	
	void CopyTextToClipboard(const tWString &asText)
	{
	}

	tWString LoadTextFromClipboard()
	{
		return _W("");
	}
}

