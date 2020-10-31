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
#include "impl/SqScript.h"
#include "system/String.h"

#include "impl/stdstring.h"
#include "impl/scriptstring.h"

extern int hplMain(const hpl::tString &asCommandLine);

void android_main(android_app *app)
{
	hpl::tString cmdline = "";
	/*for (int i=1; i < argc; i++) {
		if (cmdline.length()>0) {
			cmdline.append(" ").append(argv[i]);
		} else {
			cmdline.append(argv[i]);
		}
	}*/
	int r = hplMain(cmdline);
	__android_log_print(ANDROID_LOG_INFO,"HPL1test","hplMain returned %d\n",r);
}
namespace hpl
{
	cLowLevelSystemAndroid::cLowLevelSystemAndroid()
	{
		//mpScriptEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

		mpScriptOutput = hplNew( cScriptOutput, () );
		//mpScriptEngine->SetMessageCallback(asMETHOD(cScriptOutput,AddMessage), mpScriptOutput, asCALL_THISCALL);

#ifdef AS_MAX_PORTABILITY
		//RegisterScriptString(mpScriptEngine);
#else
		//RegisterStdString(mpScriptEngine);
#endif

		mlHandleCount = 0;

		Log("-------- THE HPL ENGINE LOG ------------\n\n");
	}

	//-----------------------------------------------------------------------

	cLowLevelSystemAndroid::~cLowLevelSystemAndroid()
	{
		/*Release all runnings contexts */

		//mpScriptEngine->Release();
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
		return nullptr;
		//return hplNew( cSqScript, (asName,mpScriptEngine,mpScriptOutput,mlHandleCount++) );
	}

	//-----------------------------------------------------------------------

	bool cLowLevelSystemAndroid::AddScriptFunc(const tString& asFuncDecl, void* pFunc, int callConv)
	{
		//if(mpScriptEngine->RegisterGlobalFunction(asFuncDecl.c_str(),
		//										asFUNCTION(pFunc),callConv)<0)
		{
			Error("Couldn't add func '%s'\n",asFuncDecl.c_str());
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cLowLevelSystemAndroid::AddScriptVar(const tString& asVarDecl, void *pVar)
	{
		//if(mpScriptEngine->RegisterGlobalProperty(asVarDecl.c_str(),pVar)<0)
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
