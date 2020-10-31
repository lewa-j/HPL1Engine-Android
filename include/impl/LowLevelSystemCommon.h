#pragma once

#include "system/LowLevelSystem.h"
#include <angelscript.h>

namespace hpl
{
//------------------------------------------------------

class cLogWriter
{
public:
	cLogWriter(const tWString& asDefaultFile);
	~cLogWriter();

	void Write(const tString& asMessage);
	void Clear();

	void SetFileName(const tWString& asFile);

private:
	void ReopenFile();

	FILE* mpFile;
	tWString msFileName;
};

//------------------------------------------------------

class cScriptOutput// : public  asIOutputStream
{
public:
	cScriptOutput() : msMessage("") {}
	~cScriptOutput() {}

	void AddMessage(const asSMessageInfo* msg);
	void Display();
	void Clear();

private:
	tString msMessage;
};

//------------------------------------------------------

cDate DateFromGMTIme(struct tm* apClock);

}
