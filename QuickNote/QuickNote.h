#pragma once

#include "resource.h"

#include "Note.h"
#include "Tag.h"

#include <Windowsx.h>

#include <Shellapi.h> //NOTIFYICONDATA
#include <Strsafe.h> //StringCchCopy()

//listview
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")

//file UNICODE
#include <fstream>
#include <locale>	//wof, wif
#include <codecvt>

//GDI+
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
using namespace Gdiplus;

//global variables
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR           gdiplusToken;

NOTIFYICONDATA nid;				 //icon will appeare in notification area

vector<Tag*> listTag;			//save all tag
const int BUFLEN = 255;
BOOL g_isOpeningDlg = FALSE;	//kiểm tra xem nếu đang mở dialog AddNote thì k cho mở tiếp dialog Addnote khác
int g_selectedItemListTags = -1;//lưu index của item trong listview tag đang được chọn
int g_selectedItemListNotes = -2;//lưu index của item trong listview note đang được chọn
bool g_isHookInstalled = false;	//xem cài đặt hook hay chưa

//biến dùng trong phân trang
#define ROWNUMTAG 10 //số dòng trong 1 page
#define ROWNUMNOTE 10
int g_totalPageTag = 0;
int g_curPageTag = 1;//trang bắt đầu là 1
int g_totalPageNote = 0;
int g_curPageNote = 1;

//--------------------- insert item to listbox ------------------------------
void InsertItemListTag(HWND hListview, Tag* tag, int iItem, int stt)
{
	WCHAR szSTT[10] = L"";
	WCHAR szName[255] = L"";
	WCHAR szFreq[10] = L"";

	wcscat(szName, tag->GetName().c_str());
	
	LV_ITEM lv;

	lv.mask = LVIF_TEXT;
	lv.iItem = iItem;
	lv.iSubItem = 0;
	lv.pszText = _itow(stt + 1, szSTT, 10);
	ListView_InsertItem(hListview, &lv);

	ListView_SetItemText(hListview, iItem, 1, szName);
	ListView_SetItemText(hListview, iItem, 2, _itow(tag->GetFreq(), szFreq, 10));
}

void InsertItemToListNote(HWND hListview, Note note, int iItem, int stt)
{
	WCHAR szSTT[10] = L"";
	WCHAR szName[255] = L"";
	WCHAR szDateCreated[17] = L"";

	wcscat(szName, note.GetName().c_str());
	wcscat(szDateCreated, note.GetTimeCreated().c_str());

	LV_ITEM lv;

	lv.mask = LVIF_TEXT;
	lv.iItem = iItem;
	lv.iSubItem = 0;
	lv.pszText = _itow(stt + 1, szSTT, 10);
	ListView_InsertItem(hListview, &lv);

	ListView_SetItemText(hListview, iItem, 1, szName);
	ListView_SetItemText(hListview, iItem, 2, szDateCreated);
}

//--------------------- sort list tag by freq ----------------------
void SortListTags(vector<Tag*> &list)
{
	int size = list.size();
	
	for (int i = 0; i < size - 1; i++)
	{
		for (int j = i + 1; j < size; j++)
		{
			if (list[i]->GetFreq() < list[j]->GetFreq())
			{
				swap(list[i], list[j]);
			}
		}
	}
}

//--------------------- xử lý chuỗi, tính toán đưa tag và note vào listNote -----------------
//return -1: tag chưa tồn tại --> thêm tag mới
//return >= 0: tag tồn tại --> cập nhật tag tại vị trí trả về
int AddNoteToListTag(vector<Tag*> &lTag, Note note, wstring wsTag)
{
	int kq = -1;
	int size = lTag.size();

	for (int i = 0; i < size; i++)
	{
		if (wsTag.compare(lTag[i]->GetName()) == 0)//tìm xem tag nhập vào đã tồn tại chưa, == 0 là tồn tại
		{
			lTag[i]->AddNote(note);
			kq = i;
			return kq;
		}
	}

	//chưa tồn tại
	Tag* tag = new Tag(wsTag);

	lTag.push_back(tag);
	lTag.back()->AddNote(note);
	
	return kq;
}

wstring RemoveFirstSpaceChar(wstring string) //remove first space charater
{
	int i = 0;
	int pos = 0;

	while (string[i])
	{
		if (string[i] != ' ')
			break;

		pos++;
		i++;
	}

	return string.substr(pos);
}

wstring SplitTag(wstring &listTag)//xử lí dấu ',' và dấu khoảng trắng
{
	int lenTag = 0;
	wstring tag;

	lenTag = listTag.find(',', 0);
	tag = listTag.substr(0, lenTag);
	tag = RemoveFirstSpaceChar(tag);
	
	if (lenTag >= 0)
		listTag = listTag.substr(lenTag + 1, listTag.length() - lenTag);//loại bỏ tag đã tách ở trên
	else listTag = L"";

	return tag;
}

//--------------------- Load icon to notification area ---------------------
void SetIconNotificationArea(HINSTANCE hInstance, HWND hWnd, NOTIFYICONDATA &nid)
{
	HICON hIcon;
	hIcon = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_SMALL));

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = IDI_SMALL;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.hIcon = hIcon;
	nid.uCallbackMessage = WM_ICONNOTIFICATION;
	wcscat(nid.szTip, L"Quick Note");
	
	Shell_NotifyIcon(NIM_ADD, &nid);
}

//--------------------- Load icon to notification area ---------------------
void ShowPopupMenu(HWND hWnd, int cX, int cY, HMENU hPopupMenu)
{
	//insert item to popup menu
	InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDC_POPUP_NEWNOTE, L"Add note	(CTRL SHIFT + N)");
	InsertMenu(hPopupMenu, 1, MF_BYPOSITION | MF_STRING, IDC_POPUP_VIEWNOTE, L"View notes"); // notes
	InsertMenu(hPopupMenu, 2, MF_BYPOSITION | MF_STRING, IDC_POPUP_VIEWSTATISTIC, L"View statistics	(CTRL SHIFT + S)"); // statistics		
	InsertMenu(hPopupMenu, 3, MF_BYPOSITION | MF_STRING, IDM_EXIT, L"Exit	(CTRL SHIFT + E)");
	
	TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, cX, cY, 0, hWnd, NULL); //show popup menu
}

//--------------------- Listview ---------------------
void InitColumnListNotes(HWND lvwNotes)
{
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 40;
	lvCol.pszText = L"STT";
	ListView_InsertColumn(lvwNotes, 0, &lvCol);

	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 120;
	lvCol.pszText = L"Tên note";
	ListView_InsertColumn(lvwNotes, 1, &lvCol);

	lvCol.fmt = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 110;
	lvCol.pszText = L"Thời gian tạo";
	ListView_InsertColumn(lvwNotes, 2, &lvCol);
}

void InitColumnListTags(HWND lvwTag)
{
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 40;
	lvCol.pszText = L"STT";
	ListView_InsertColumn(lvwTag, 0, &lvCol);

	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 170;
	lvCol.pszText = L"Tên tag";
	ListView_InsertColumn(lvwTag, 1, &lvCol);

	lvCol.fmt = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 60;
	lvCol.pszText = L"Số note";
	ListView_InsertColumn(lvwTag, 2, &lvCol);
}

//--------------------- set system font ------------------------
void SetSystemFont(HWND hwnd)
{
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);

	HFONT hFont = CreateFont(16, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);
	
	SetWindowFont(hwnd, hFont, TRUE);
}

//--------------------- save and load file ---------------------
//STRUCT FILE: 
//	dòng 1: tên tag thứ 1
//	dòng 2: tổng số note trong tag (n)
//	dòng 3: tên note 1
//	dòng 4: nội dung note 1
//	dòng 5: thời gian tạo note 1
//	dòng 6+7+8: cấu trúc note2 như note1
//	tương tự cho các dòng là note và các tag
BOOL SaveTags(vector<Tag*> list)
{
	wofstream wof("Tags.txt", ios::out);

	if (wof.is_open())
	{
		wof.imbue(locale(wof.getloc(), new codecvt_utf16<wchar_t, 0x10ffff, consume_header>));
		
		int sizeListTag = list.size();

		for (int i = 0; i < sizeListTag; i++)
		{
			wof << list[i]->GetName() << endl;	//tên tag thứ i

			int sizeListNote = list[i]->GetFreq(); 
			wof << sizeListNote << endl;	//số note của tag thứ i

			for (int j = 0; j < sizeListNote; j++)
			{
				Note temp = list[i]->GetNote(j);

				wof << temp.GetName() << endl;		//tên note thứ i
				wof << temp.GetContent() << endl;	//nội dung note thứ i
				wof << temp.GetTimeCreated() << endl;//thời gian tạo note thứ i
			}
		}

		wof.close();
		return TRUE;
	}
	else return FALSE;
}

BOOL LoadTags(vector<Tag*> &list)
{
	wifstream wif("Tags.txt", ios::in);

	if (wif.is_open())
	{
		// apply BOM-sensitive UTF-16 facet
		wif.imbue(locale(wif.getloc(), new codecvt_utf16<wchar_t, 0x10ffff, consume_header>));

		wstring wsNameTag;
		
		while (getline(wif, wsNameTag))//lấy tên tag
		{
			Tag* tag = new Tag(wsNameTag);
			list.push_back(tag);

			int countNote = 0;
			wstring wsNote;

			getline(wif, wsNote);
			countNote = _wtoi(wsNote.c_str());

			for (int i = 0; i < countNote; i++)
			{
				Note note;
				wstring wsNameNote;
				wstring wsContentNote;
				wstring wsDateCreated;

				getline(wif, wsNameNote);
				getline(wif, wsContentNote);
				getline(wif, wsDateCreated);

				note.SetName(wsNameNote);
				note.SetContent(wsContentNote);
				note.SetTimeCreated(wsDateCreated);

				list.back()->AddNote(note);
			}
		}
		wif.close();

		return TRUE;
	}
	else return FALSE;
}

//--------------------- draw chart ------------------------
int GetDecimalPart(float num)
{
	return (int)(num * 100) % 100;
}

int CountNotes(vector<Tag*> list)
{
	int count = 0;

	for (int i = 0; i < list.size(); i++)
	{
		count += list[i]->GetFreq();
	}

	return count;
}

void DrawPieChart(vector<Tag*> list, HDC hdc, HWND hDlg)
{
	if (list.empty())
	{
		return;
	}

	Graphics graphic(hdc);

	////tạo brush để fill pie chart
	SolidBrush bBlue(Color(255, 51, 255, 255));		//xanh
	SolidBrush bRed(Color(255, 255, 0, 0));			//red
	SolidBrush bYellow(Color(255, 255, 255, 0));	//yellow
	SolidBrush bGreen(Color(255, 0, 0, 255));		//green
	SolidBrush bOrange(Color(255, 255, 165, 0));	//orange
	SolidBrush bPurple(Color(255, 128, 0, 128));	//purple
	SolidBrush bPink(Color(255, 255, 105, 180));	//pink

	Brush *listBrush[7] = { 
		new SolidBrush(Color(255, 51, 255, 255)),	//blue
		new SolidBrush(Color(255, 255, 0, 0)),		//red
		new SolidBrush(Color(255, 255, 255, 0)),	//yellow
		new SolidBrush(Color(255, 0, 0, 255)),		//green
		new SolidBrush(Color(255, 255, 165, 0)),	//orange
		new SolidBrush(Color(255, 128, 0, 128)),	//purple
		new SolidBrush(Color(255, 255, 105, 180))};	//pink

	int idTxtExplication[14] = { IDC_NOTE1, IDC_NOTE2, IDC_NOTE3, IDC_NOTE4, IDC_NOTE5, IDC_NOTE6, IDC_NOTE7,
		IDC_NOTE8, IDC_NOTE9, IDC_NOTE10, IDC_NOTE11, IDC_NOTE12, IDC_NOTE13, IDC_NOTE14 };

	//vẽ chú thích
	int xPieChart = 50;
	int yPieChart = 50;
	int widthPieChart = 300;
	int heightPieChart = 300;

	int x3left = 130;
	int y3left = 380;
	int x3right = 450;
	int lineWidth = 10;
	int lineLength = 25;

	double startAngle = 0;
	double sweepAngle = 0;

	int countTags = list.size();
	float totalNotes = (float)CountNotes(list);
	int i = 0;
	float degree;

	WCHAR szExplication1[BUFLEN] = L"";
	WCHAR szExplication2[BUFLEN] = L"";
	int xExPlication = 500;
	int yExplication = 110;

	for (i; i < countTags && i < 6; i++)
	{
		degree = float(list[i]->GetFreq()) / totalNotes * 100;

		//fill pie chart
		sweepAngle = degree * 3.6;
		graphic.FillPie(listBrush[i], xPieChart, yPieChart, widthPieChart, heightPieChart, startAngle, sweepAngle);
		startAngle += sweepAngle;

		//set explication
		graphic.FillPie(listBrush[i], xExPlication, yExplication, 60, 60, -45, 45);
		
		wsprintf(szExplication1, L"TAG: %s", list[i]->GetName().c_str());
		wsprintf(szExplication2, L"%d note (%d.%d %%)", list[i]->GetFreq(), (int)degree, GetDecimalPart(degree));
		SetDlgItemText(hDlg, idTxtExplication[i * 2], szExplication1);
		SetDlgItemText(hDlg, idTxtExplication[i*2 + 1], szExplication2);

		yExplication += 40;
	}

	if (countTags > 6)
	{
		int notes = 0;
		//tính số note còn lại 
		for (i = 6; i < countTags; i++)
		{
			notes += list[i]->GetFreq();
		}

		//fill pie chart
		graphic.FillPie(listBrush[6], xPieChart, yPieChart, widthPieChart, heightPieChart, startAngle, 360.f - startAngle);
		
		//set explication
		graphic.FillPie(listBrush[6], xExPlication, yExplication, 60, 60, -45, 45);

		degree = (float)notes / totalNotes * 100;
		wsprintf(szExplication1, L"Các tag khác");
		wsprintf(szExplication2, L"%d note (%d.%d %%)", notes, (int)degree, GetDecimalPart(degree));
		SetDlgItemText(hDlg, idTxtExplication[12], szExplication1);
		SetDlgItemText(hDlg, idTxtExplication[13], szExplication2);
	}

	WCHAR szTotalNotes[10];
	WCHAR szTotalTag[10];

	_itow((int)totalNotes, szTotalNotes, 10);
	_itow(countTags, szTotalTag, 10);

	SetDlgItemText(hDlg, IDC_LB_TONGNOTE, szTotalNotes);
	SetDlgItemText(hDlg, IDC_LB_TONGTAG, szTotalTag);
}

//--------------------- hook -----------------------------
//Bắt sự kiện "CTRL SHIFT N" để mở dialog AddNote
//"CTRL SHIFT S" để mở dialog Statistic (thống kê)
//"CTRL SHIFT E" để thoát chương trình
void InstallHook(HWND hwnd)
{
	typedef VOID(*MYPROC)(HWND);
	MYPROC ProcAdd;

	HINSTANCE hinstLib = LoadLibrary(L"Hook.dll");

	if (hinstLib)
	{
		ProcAdd = (MYPROC)GetProcAddress(hinstLib, "doInitHook");

		if (ProcAdd)
		{
			ProcAdd(hwnd);

			g_isHookInstalled = true;
		}
	}
}

void RemoveHook(HWND hwnd)
{
	typedef VOID(*MYPROC)(HWND);
	MYPROC ProcAdd;

	HINSTANCE hinstLib = LoadLibrary(L"Hook.dll");

	if (hinstLib)
	{
		ProcAdd = (MYPROC)GetProcAddress(hinstLib, "doRemoveHook");

		if (ProcAdd)
			ProcAdd(hwnd);
	}

	g_isHookInstalled = false;
}

//---------------------- Phân trang list view --------------
void PhanTrangLvwTags(HWND hlvwTag, vector<Tag*> list, int iTotalPage, int iCurPage)
{
	ListView_DeleteAllItems(hlvwTag);

	int sumRecord = list.size();
	int indexList = ROWNUMTAG*(iCurPage - 1);
	
	for (int i = 0; (i < ROWNUMTAG) && (indexList < sumRecord); i++)
	{
		InsertItemListTag(hlvwTag, list[indexList], i, indexList);
		indexList++;
	}
}

void PhanTrangLvwNotes(HWND lvwNote, vector<Note> list, int iTotalPage, int iCurPage)
{
	ListView_DeleteAllItems(lvwNote);

	int sumRecord = list.size();
	int indexList = ROWNUMNOTE*(iCurPage - 1);
	
	for (int i = 0; (i < ROWNUMNOTE) && (indexList < sumRecord); i++)
	{
		InsertItemToListNote(lvwNote, list[indexList], i, indexList);
		indexList++;
	}
}

void AddDataCbb(HWND hCbb, int totalPage, int indexPageCur)
{
	int countItemCBB = ComboBox_GetCount(hCbb);
	
	for (int i = 0; i < countItemCBB; i++)
	{
		ComboBox_DeleteString(hCbb, 0);
	}
	
	for (int i = 0; i < totalPage; i++)
	{
		ComboBox_AddString(hCbb, to_wstring(i + 1).c_str());
	}
	ComboBox_SetCurSel(hCbb, indexPageCur - 1);
}