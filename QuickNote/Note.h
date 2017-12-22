#pragma once

#include <iostream>
#include <vector>
using namespace std;

class Note
{
	wstring wsName;
	wstring wsContent;
	WCHAR szTimeCreated[17];// type: "hh:mm dd/MM/yyyy"

public:
	Note();
	Note(wstring name, wstring content, SYSTEMTIME sysTime);
	
	wstring GetName();
	wstring GetContent();
	wstring GetTimeCreated();

	void SetName(wstring name);
	void SetContent(wstring content);
	void SetTimeCreated(SYSTEMTIME sysTime);
	void SetTimeCreated(wstring);
};