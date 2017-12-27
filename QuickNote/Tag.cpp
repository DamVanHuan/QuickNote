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

void Tag::RemoveNote(int index)
{
	listNote.erase(listNote.begin() + index);
}