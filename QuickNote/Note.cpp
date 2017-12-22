#include "stdafx.h"
#include "Note.h"


Note::Note()
{
}

Note::Note(wstring name, wstring content, SYSTEMTIME sysTime)
{
	wsName = name;
	wsContent = content;

	wsprintf(szTimeCreated, L"%02d:%02d %02d/%02d/%d", 
			sysTime.wHour, sysTime.wMinute, sysTime.wDay, sysTime.wMonth, sysTime.wYear);
}

wstring Note::GetName()
{
	return wsName;
}

wstring Note::GetContent()
{
	return wsContent;
}

wstring Note::GetTimeCreated()
{
	wstring wsTimeCreated = this->szTimeCreated;
	
	return wsTimeCreated;
}

void Note::SetName(wstring name)
{
	this->wsName = name;
}

void Note::SetContent(wstring content)
{
	this->wsContent = content;
}

void Note::SetTimeCreated(SYSTEMTIME sysTime)
{
	wsprintf(szTimeCreated, L"%02d:%02d %02d/%02d/%d",
			sysTime.wHour, sysTime.wMinute, sysTime.wDay, sysTime.wMonth, sysTime.wYear);
}

void Note::SetTimeCreated(wstring time)
{
	wsprintf(szTimeCreated, L"%s", time.c_str());
}