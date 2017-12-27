#pragma once

#include "Note.h"

class Tag
{
	wstring szName;
	vector<Note> listNote; //list Note 
public:
	Tag();
	Tag(wstring);
	~Tag();

	wstring GetName();
	Note GetNote(int);
	vector<Note> GetListNote();
	int GetFreq();

	void SetName(wstring);
	void SetNoteList(vector<Note>);

	void AddNote(Note);
	void RemoveNote(int);
};

