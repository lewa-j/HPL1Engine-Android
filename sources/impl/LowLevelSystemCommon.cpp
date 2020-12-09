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
	msFileName = GetPlatformPath(asFileName);
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

	tString type = "ERR ";
	if (msg->type == asMSGTYPE_WARNING)
		type = "WARN";
	else if (msg->type == asMSGTYPE_INFORMATION)
		type = "INFO";

	sprintf(sMess, "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type.c_str(), msg->message);

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

cDate DateFromGMTIme(struct tm* apClock)
{
	cDate date;

	date.seconds = apClock->tm_sec;
	date.minutes = apClock->tm_min;
	date.hours = apClock->tm_hour;
	date.month_day = apClock->tm_mday;
	date.month = apClock->tm_mon;
	date.year = 1900 + apClock->tm_year;
	date.week_day = apClock->tm_wday;
	date.year_day = apClock->tm_yday;

	return date;
}

//-----------------------------------------------------------------------

void OpenBrowserWindow(const tWString& asURL)
{
#ifdef WIN32
	ShellExecute(NULL, _W("open"), asURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__linux__)
	tString asTemp = "./openurl.sh " + cString::To8Char(asURL);
	system(asTemp.c_str());
#elif defined(__APPLE__)
	tString asTemp = "open " + cString::To8Char(asURL);
	system(asTemp.c_str());
#endif
}

//-----------------------------------------------------------------------

bool FileExists(const tWString& asFileName)
{
#ifdef WIN32
	FILE* f = _wfopen(asFileName.c_str(), _W("r"));
#else
	FILE* f = fopen(cString::To8Char(asFileName).c_str(), "r");
#endif
	if (f == NULL)
	{
		return false;
	}

	fclose(f);
	return true;
}

//-----------------------------------------------------------------------

void RemoveFile(const tWString& asFilePath)
{
#ifdef WIN32
	_wremove(asFilePath.c_str());
#else
	remove(cString::To8Char(asFilePath).c_str());
#endif
}

//-----------------------------------------------------------------------

bool CloneFile(const tWString& asSrcFileName, const tWString& asDestFileName,
	bool abFailIfExists)
{
#ifdef WIN32
	return CopyFile(asSrcFileName.c_str(), asDestFileName.c_str(), abFailIfExists) == TRUE;
#else
	if (abFailIfExists && FileExists(asDestFileName)) {
		return true;
	}
	std::ifstream IN(cString::To8Char(asSrcFileName).c_str(), std::ios::binary);
	std::ofstream OUT(cString::To8Char(asDestFileName).c_str(), std::ios::binary);
	OUT << IN.rdbuf();
	OUT.flush();
	return true;
#endif
}

//-----------------------------------------------------------------------

bool CreateFolder(const tWString& asPath)
{
#ifdef WIN32
	return CreateDirectory(asPath.c_str(), NULL) == TRUE;
#else
	return mkdir(cString::To8Char(asPath).c_str(), 0755) == 0;
#endif
}

bool FolderExists(const tWString& asPath)
{
#ifdef WIN32
	return GetFileAttributes(asPath.c_str()) == FILE_ATTRIBUTE_DIRECTORY;
#else
	struct stat statbuf;
	return (stat(cString::To8Char(asPath).c_str(), &statbuf) != -1);
#endif
}

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

//-----------------------------------------------------------------------

cDate FileModifiedDate(const tWString& asFilePath)
{
	struct tm* pClock;
#ifdef WIN32
	struct _stat attrib;
	_wstat(asFilePath.c_str(), &attrib);
#else
	struct stat attrib;
	stat(cString::To8Char(asFilePath).c_str(), &attrib);
#endif

	pClock = gmtime(&(attrib.st_mtime));	// Get the last modified time and put it into the time structure

	cDate date = DateFromGMTIme(pClock);

	return date;
}

//-----------------------------------------------------------------------

cDate FileCreationDate(const tWString& asFilePath)
{
	struct tm* pClock;
#ifdef WIN32
	struct _stat attrib;
	_wstat(asFilePath.c_str(), &attrib);
#else
	struct stat attrib;
	stat(cString::To8Char(asFilePath).c_str(), &attrib);
#endif

	pClock = gmtime(&(attrib.st_ctime));	// Get the last modified time and put it into the time structure

	cDate date = DateFromGMTIme(pClock);

	return date;
}

//-----------------------------------------------------------------------

tString GetPlatformPath(const tString& asFile)
{
#ifdef ANDROID
	if (asFile[0] == '/' || asFile[0] == '\\')
		return asFile;
	static const char* storage = getenv("EXTERNAL_STORAGE");
	return storage + tString("/hpl1/") + asFile;
#else
	return asFile;
#endif
}

tWString GetPlatformPath(const tWString& asFile)
{
#ifdef ANDROID
	if (asFile[0] == _W('/') || asFile[0] == _W('\\'))
		return asFile;
	static const char* storage = getenv("EXTERNAL_STORAGE");
	return cString::To16Char(storage) + tWString(_W("/hpl1/")) + asFile;
#else
	return asFile;
#endif
}

}
