
#include "impl/LowLevelSystemAndroid.h"
#include "system/Platform.h"
#include "system/String.h"
#include <sys/stat.h>
#include <fstream>

namespace hpl
{

	tString cPlatform::GetPlatformPath(const tString& asFile)
	{
		if (asFile[0] == '/' || asFile[0] == '\\')
			return asFile;
		static const char *storage = getenv("EXTERNAL_STORAGE");
		return storage + tString("/hpl1/") + asFile;
	}

	tWString cPlatform::GetPlatformPath(const tWString& asFile)
	{
		if (asFile[0] == _W('/') || asFile[0] == _W('\\'))
			return asFile;
		static const char *storage = getenv("EXTERNAL_STORAGE");
		return cString::To16Char(storage) + tWString(_W("/hpl1/")) + asFile;
	}

	bool cPlatform::FileExists(const tWString& asFileName)
	{
		FILE *f = fopen(cString::To8Char(asFileName).c_str(), "r");

		if (f == nullptr)
		{
			return false;
		}

		fclose(f);
		return true;
	}

	void cPlatform::RemoveFile(const tWString& asFilePath)
	{
		remove(cString::To8Char(asFilePath).c_str());
	}

	bool cPlatform::CloneFile(const tWString& asSrcFileName, const tWString& asDestFileName, bool abFailIfExists)
	{
		if (abFailIfExists && FileExists(asDestFileName)) {
			return true;
		}
		std::ifstream IN(cString::To8Char(asSrcFileName).c_str(), std::ios::binary);
		std::ofstream OUT(cString::To8Char(asDestFileName).c_str(), std::ios::binary);
		OUT << IN.rdbuf();
		OUT.flush();
		return true;
	}

	bool cPlatform::CreateFolder(const tWString& asPath)
	{
		return mkdir(cString::To8Char(asPath).c_str(), 0755) == 0;
	}

	bool cPlatform::FolderExists(const tWString& asPath)
	{
		struct stat statbuf;
		return (stat(cString::To8Char(asPath).c_str(), &statbuf) != -1);
	}

	FILE *cPlatform::OpenFile(const tWString& asFileName, const tWString asMode)
	{
		return fopen(cString::To8Char(asFileName).c_str(), cString::To8Char(asMode).c_str());
	}

	static cDate DateFromGMTIme(struct tm* apClock)
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

	cDate cPlatform::FileModifiedDate(const tWString& asFilePath)
	{
		struct tm *pClock;
		struct stat attrib;
		stat(cString::To8Char(asFilePath).c_str(), &attrib);

		pClock = gmtime(&(attrib.st_mtime));	// Get the last modified time and put it into the time structure

		cDate date = DateFromGMTIme(pClock);

		return date;
	}

	typedef long long unsigned int longtime_t;
	unsigned long cPlatform::GetApplicationTime()
	{
		static longtime_t g_PerformanceFrequency;
		static longtime_t g_ClockStart;
		longtime_t CurrentTime;
		struct timespec ts;

		if ( !g_PerformanceFrequency )
		{
			struct timespec res;
			if ( !clock_getres(CLOCK_MONOTONIC, &res) )
				g_PerformanceFrequency = 1000000000LL / res.tv_nsec;
		}
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
	}

	void cPlatform::CreateMessageBox (eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, va_list ap)
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

	void cPlatform::CreateMessageBox ( eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, ...)
	{
		va_list ap;

		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		CreateMessageBox(eType, asCaption, fmt, ap);
		va_end(ap);
	}

	void cPlatform::CreateMessageBox ( const wchar_t* asCaption, const wchar_t *fmt, ...)
	{
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		CreateMessageBox( eMsgBoxType_Default, asCaption, fmt, ap );
		va_end(ap);
	}

	cDate cPlatform::GetDate()
	{
		time_t lTime;
		time(&lTime);

		struct tm* pClock;
		pClock = localtime(&lTime);

		return DateFromGMTIme(pClock);
	}

	// TODO
	void cPlatform::CopyTextToClipboard(const tWString &asText)
	{
	}

	tWString cPlatform::LoadTextFromClipboard()
	{
		return _W("");
	}

	tWString cPlatform::GetSystemSpecialPath(eSystemPath aPathType)
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

	// TODO
	void cPlatform::OpenBrowserWindow(const tWString& asURL)
	{
		//tString asTemp = "./openurl.sh " + cString::To8Char(asURL);
		//system(asTemp.c_str());
	}

}
