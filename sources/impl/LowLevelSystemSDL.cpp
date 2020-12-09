/*
 * Copyright (C) 2006-2010 - Frictional Games
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
//#include <vld.h>
//Use this to check for memory leaks!

#ifdef WIN32
#pragma comment(lib, "angelscript.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")
#define UNICODE
#include <windows.h>
#include <shlobj.h>
#endif

#ifndef WIN32
// Include FLTK
#include "FL/fl_ask.H"
#endif

#define _UNICODE

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fstream>
#include <string>

#include "impl/LowLevelSystemSDL.h"
#include "impl/SqScript.h"

#include "SDL/SDL.h"

#include "impl/stdstring.h"
#include "impl/scriptstring.h"

#include "system/String.h"

#include <clocale>

extern int hplMain(const hpl::tString &asCommandLine);

#ifdef WIN32
#include <windows.h>
int WINAPI WinMain(	HINSTANCE hInstance,  HINSTANCE hPrevInstance,LPSTR	lpCmdLine, int nCmdShow)
{
	return hplMain(lpCmdLine);
}
#else
int main(int argc, char *argv[])
{
	if(!std::setlocale(LC_CTYPE, "")) {
		fprintf(stderr, "Can't set the specified locale! Check LANG, LC_CTYPE, LC_ALL.\n");
		return 1;
	}


	hpl::tString cmdline = "";
	for (int i=1; i < argc; i++) {
		if (cmdline.length()>0) {
			cmdline.append(" ").append(argv[i]);
		} else {
			cmdline.append(argv[i]);
		}
	}
	return hplMain(cmdline);
}
#endif


namespace hpl
{
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelSystemSDL::cLowLevelSystemSDL()
	{
		mpScriptEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

		mpScriptOutput = hplNew( cScriptOutput, () );
		mpScriptEngine->SetMessageCallback(asMETHOD(cScriptOutput,AddMessage), mpScriptOutput, asCALL_THISCALL);

#ifdef AS_MAX_PORTABILITY
		RegisterScriptString(mpScriptEngine);
#else
		RegisterStdString(mpScriptEngine);
#endif

		mlHandleCount = 0;

		Log("-------- THE HPL ENGINE LOG ------------\n\n");
	}

	//-----------------------------------------------------------------------

	cLowLevelSystemSDL::~cLowLevelSystemSDL()
	{
		/*Release all runnings contexts */

		mpScriptEngine->Release();
		hplDelete(mpScriptOutput);

		//perhaps not the best thing to skip :)
		//if(gpLogWriter)	hplDelete(gpLogWriter);
		//gpLogWriter = NULL;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void CopyTextToClipboard(const tWString &asText)
	{
#ifdef WIN32
		OpenClipboard(NULL);
		EmptyClipboard();

		HGLOBAL clipbuffer;
		wchar_t * pBuffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, (asText.size()+1) *sizeof(wchar_t));
		pBuffer = (wchar_t*)GlobalLock(clipbuffer);
		wcscpy(pBuffer, asText.c_str());
		//GlobalUnlock(clipbuffer);

		SetClipboardData(CF_UNICODETEXT, clipbuffer);

		GlobalUnlock(clipbuffer);

		CloseClipboard();
#endif
	}

	tWString LoadTextFromClipboard()
	{
#ifdef WIN32
		tWString sText=_W("");
		OpenClipboard(NULL);

		HGLOBAL clipbuffer = GetClipboardData(CF_UNICODETEXT);

		wchar_t *pBuffer = (wchar_t*)GlobalLock(clipbuffer);

		if(pBuffer != NULL) sText = pBuffer;

		GlobalUnlock(clipbuffer);

		CloseClipboard();

		return sText;
#else
		return _W("");
#endif
	}

	//-----------------------------------------------------------------------


	void CreateMessageBox ( eMsgBoxType eType, const char* asCaption, const char *fmt, ...)
	{
		/*char text[2048];
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		tString sMess = "";
		sMess += text;


		#ifdef WIN32

		UINT lType = MB_OK;

		switch (eType)
		{
		case eMsgBoxType_Info:
			lType += MB_ICONINFORMATION;
			break;
		case eMsgBoxType_Error:
			lType += MB_ICONERROR;
			break;
		case eMsgBoxType_Warning:
			lType += MB_ICONWARNING;
			break;
		default:
			break;
		}


		MessageBox( NULL, sMess.c_str(), asCaption, lType );

		#endif*/
	}

	void CreateMessageBox ( const char* asCaption, const char *fmt, ...)
	{
		/*char text[2048];
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		tString sMess = "";
		sMess += text;

		CreateMessageBox( eMsgBoxType_Default, asCaption, sMess.c_str() );*/
	}


	void CreateMessageBoxW (eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, va_list ap)
	{
		wchar_t text[2048];

		if (fmt == NULL)
			return;
		vswprintf(text, 2047, fmt, ap);

		tWString sMess = _W("");

		#ifdef WIN32
		sMess += text;

		UINT lType = MB_OK;

		switch (eType)
		{
		case eMsgBoxType_Info:
			lType += MB_ICONINFORMATION;
			break;
		case eMsgBoxType_Error:
			lType += MB_ICONERROR;
			break;
		case eMsgBoxType_Warning:
			lType += MB_ICONWARNING;
			break;
		default:
			break;
		}

		MessageBox( NULL, sMess.c_str(), asCaption, lType );
		#else
		sMess += asCaption;
		sMess +=_W("\n\n");
		sMess += text;
		fl_alert("%ls\n\n%ls",asCaption,text);
		#endif
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
#if defined(WIN32)
		int type;
		switch (aPathType)
		{
		case eSystemPath_Personal:	type = CSIDL_PERSONAL;
			break;
		default: return _W("");
		}

		TCHAR sPath[1024];
		if (SUCCEEDED(SHGetFolderPath(NULL,
			type | CSIDL_FLAG_CREATE,
			NULL, 0, sPath)))
		{
			return tWString(sPath);
		}
		else
		{
			return _W("");
		}
#else
		switch (aPathType)
		{
		case eSystemPath_Personal: {
			const char* home = getenv("HOME");
			if (!home)
				return _W("");
			return cString::To16Char(tString(home));
		}
		default:
			return _W("");
		}
#endif
	}

	//-----------------------------------------------------------------------

	void SetWindowCaption(const tString &asName)
	{
		SDL_WM_SetCaption(asName.c_str(),"");
	}

	//-----------------------------------------------------------------------

	bool HasWindowFocus(const tWString &asWindowCaption)
	{
		#ifdef WIN32
			HWND pWindowHandle = FindWindow(NULL, asWindowCaption.c_str());
			return (pWindowHandle == GetFocus());
		#endif
		return true;
	}

	//-----------------------------------------------------------------------

	unsigned long GetApplicationTime()
	{
		return SDL_GetTicks();
	}

	//-----------------------------------------------------------------------

	unsigned long cLowLevelSystemSDL::GetTime()
	{
		return SDL_GetTicks();
	}

	//-----------------------------------------------------------------------

	cDate cLowLevelSystemSDL::GetDate()
	{
		time_t lTime;
		time(&lTime);

		struct tm* pClock;
		pClock = localtime(&lTime);

		return DateFromGMTIme(pClock);
	}

	//-----------------------------------------------------------------------

	iScript* cLowLevelSystemSDL::CreateScript(const tString& asName)
	{
		return hplNew( cSqScript, (asName,mpScriptEngine,mpScriptOutput,mlHandleCount++) );
	}

	//-----------------------------------------------------------------------

	bool cLowLevelSystemSDL::AddScriptFunc(const tString& asFuncDecl, void* pFunc, int callConv)
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

	bool cLowLevelSystemSDL::AddScriptVar(const tString& asVarDecl, void *pVar)
	{
		if(mpScriptEngine->RegisterGlobalProperty(asVarDecl.c_str(),pVar)<0)
		{
			Error("Couldn't add var '%s'\n",asVarDecl.c_str());
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	void cLowLevelSystemSDL::Sleep ( const unsigned int alMillisecs )
	{
		SDL_Delay ( alMillisecs );
	}

	//-----------------------------------------------------------------------
	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// STATIC PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

}
