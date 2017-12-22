#include "stdafx.h"
#include "Tag.h"

Tag::Tag()
{
}


Tag::~Tag()
{}

Tag::Tag(wstring name)
{
	this->szName = name;
}

wstring Tag::GetName()
{
	return szName;
}

Note Tag::GetNote(int viTri)
{
	return this->listNote[viTri];
}

vector<Note> Tag::GetListNote()
{
	return this->listNote;
}

int Tag::GetFreq()
{
	return this->listNote.size();
}

void Tag::SetName(wstring name)
{
	this->szName = name;
}

void Tag::SetNoteList(vector<Note> list)
{
	this->listNote = list;
}

void Tag::AddNote(Note note)
{
	this->listNote.push_back(note);
}

void Tag::RemoveNote(wstring nameNote)
{
	int size = this->listNote.size();

	for (int i = 0; i < size; i++)
	{
		if (nameNote.compare(this->listNote[i].GetName()) == 0)
		{
			listNote.erase(listNote.begin() + i);
			break;
		}
	}
}