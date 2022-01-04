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
#include "impl/LowLevelSystemCommon.h"
#include "system/Platform.h"
#include "system/String.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>
#include <fstream>
#include <string>

#ifdef WIN32
#define UNICODE
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#endif

#ifdef ANDROID
#include <android/log.h>
#endif

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// LOG WRITER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

static cLogWriter gLogWriter(_W("hpl.log"));
static cLogWriter gUpdateLogWriter(_W("hpl_update.log"));

//-----------------------------------------------------------------------

cLogWriter::cLogWriter(const tWString& asFileName)
{
	msFileName = cPlatform::GetPlatformPath(asFileName);
}

cLogWriter::~cLogWriter()
{
	if (mpFile) fclose(mpFile);
}

void cLogWriter::Write(const tString& asMessage)
{
	if (!mpFile) ReopenFile();

	if (mpFile)
	{
		fputs(asMessage.c_str(), mpFile);
		fflush(mpFile);
	}
#ifdef ANDROID
	__android_log_print(ANDROID_LOG_INFO, "HPL1", "%s", asMessage.c_str());
#endif
}

void cLogWriter::Clear()
{
	ReopenFile();
	if (mpFile) fflush(mpFile);
}

//-----------------------------------------------------------------------

void cLogWriter::SetFileName(const tWString& asFile)
{
	if (msFileName == asFile) return;

	msFileName = asFile;
	ReopenFile();
}

//-----------------------------------------------------------------------

void cLogWriter::ReopenFile()
{
	if (mpFile) fclose(mpFile);

	#ifdef WIN32
		mpFile = _wfopen(msFileName.c_str(), _W("w"));
	#else
		mpFile = fopen(cString::To8Char(msFileName).c_str(), "w");
	#endif
}

//-----------------------------------------------------------------------

void cScriptOutput::AddMessage(const asSMessageInfo* msg)
{
	char sMess[1024];

	const char* type = "ERR ";
	if (msg->type == asMSGTYPE_WARNING)
		type = "WARN";
	else if (msg->type == asMSGTYPE_INFORMATION)
		type = "INFO";

	snprintf(sMess, 1024, "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);

	msMessage += sMess;
}

void cScriptOutput::Display()
{
	if (msMessage.size() > 500)
	{
		while (msMessage.size() > 500)
		{
			tString sSub = msMessage.substr(0, 500);
			msMessage = msMessage.substr(500);
			Log(sSub.c_str());
		}
		Log(msMessage.c_str());
	}
	else
	{
		Log(msMessage.c_str());
	}
}

void cScriptOutput::Clear()
{
	msMessage = "";
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void SetLogFile(const tWString& asFile)
{
	gLogWriter.SetFileName(asFile);
}

//-----------------------------------------------------------------------

void FatalError(const char* fmt, ...)
{
	char text[2048];
	va_list ap;
	if (fmt == NULL)
		return;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);

	tString sMess = "FATAL ERROR: ";
	sMess += text;
	gLogWriter.Write(sMess);

#ifdef WIN32
	MessageBox(NULL, cString::To16Char(text).c_str(), _W("FATAL ERROR"), MB_ICONERROR);
#endif

	exit(1);
}

void Error(const char* fmt, ...)
{
	char text[2048];
	va_list ap;
	if (fmt == NULL)
		return;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);

	tString sMess = "ERROR: ";
	sMess += text;
	gLogWriter.Write(sMess);
}

void Warning(const char* fmt, ...)
{
	char text[2048];
	va_list ap;
	if (fmt == NULL)
		return;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);

	tString sMess = "WARNING: ";
	sMess += text;
	gLogWriter.Write(sMess);
}

void Log(const char* fmt, ...)
{
	char text[2048];
	va_list ap;
	if (fmt == NULL)
		return;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);

	tString sMess = "";
	sMess += text;
	gLogWriter.Write(sMess);
}

//-----------------------------------------------------------------------

static bool gbUpdateLogIsActive;
void SetUpdateLogFile(const tWString& asFile)
{
	gUpdateLogWriter.SetFileName(asFile);
}

void ClearUpdateLogFile()
{
	if (!gbUpdateLogIsActive) return;

	gUpdateLogWriter.Clear();
}

void SetUpdateLogActive(bool abX)
{
	gbUpdateLogIsActive = abX;
}

void LogUpdate(const char* fmt, ...)
{
	if (!gbUpdateLogIsActive) return;

	char text[2048];
	va_list ap;
	if (fmt == NULL)
		return;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);

	tString sMess = "";
	sMess += text;
	gUpdateLogWriter.Write(sMess);
}

//-----------------------------------------------------------------------

bool IsFileLink(const tWString& asPath)
{
	// Symbolic Links Not Supported under Windows
#ifndef WIN32
	struct stat statbuf;
	if (lstat(cString::To8Char(asPath).c_str(), &statbuf) == 0) {
		return statbuf.st_mode == S_IFLNK;
	}
	else {
		return false;
	}
#else
	return false;
#endif
}

bool LinkFile(const tWString& asPointsTo, const tWString& asLink)
{
	// Symbolic Links Not Supported under Windows
#ifndef WIN32
	return (symlink(cString::To8Char(asPointsTo).c_str(), cString::To8Char(asLink).c_str()) == 0);
#else
	return false;
#endif
}

bool RenameFile(const tWString& asFrom, const tWString& asTo)
{
#ifdef WIN32
	return false;
#else
	return (rename(cString::To8Char(asFrom).c_str(), cString::To8Char(asTo).c_str()) == 0);
#endif
}

}
