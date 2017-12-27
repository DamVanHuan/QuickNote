// QuickNote.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "QuickNote.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class names

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	AddNote(HWND, UINT, WPARAM, LPARAM); 
INT_PTR CALLBACK	ViewStatistic(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_QUICKNOTE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QUICKNOTE));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QUICKNOTE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_3DFACE+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_QUICKNOTE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   SetIconNotificationArea(hInstance, hWnd, nid); //load icon to notification area
   //ShowWindow(hWnd, SW_HIDE);//hide main form
   ShowWindow(hWnd, SW_NORMAL);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//handle listview, textbox,...
HWND hLvwTags = NULL; //handle listbox tags
HWND hLvwNotes = NULL; //handle listview notes
HWND hTxtContent = NULL; //handle textbox content of note
HWND hBtnDel = NULL;
HWND hBtnViewStatistic = NULL;
HWND hBtnNewNote = NULL;

HWND hCbbCurPageTag = NULL;
HWND hTotalPageTag = NULL;
HWND hCbbCurPageNote = NULL;
HWND hTotalPageNote = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
	{
		INITCOMMONCONTROLSEX icex;
		// Ensure that the common control DLL is loaded. 
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&icex);

		//group tags: listbox tags
		HWND groupTags = CreateWindow(L"BUTTON", L"Danh sách tất cả tag", 
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX | BS_CENTER,
			25, 10, 300, 300, hWnd, (HMENU)0, hInst, 0);

		hLvwTags = CreateWindow(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | 
			WS_VSCROLL | LVS_REPORT | WS_TABSTOP | LVS_SHOWSELALWAYS,
			40, 50, 270, 210, hWnd, (HMENU)ID_LVW_LISTTAGS, hInst, 0);
		InitColumnListTags(hLvwTags);
		ListView_SetExtendedListViewStyle(hLvwTags, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		HWND trang = CreateWindow(L"Static", L"Trang", WS_CHILD | WS_VISIBLE,
			45, 275, 50, 30, hWnd, (HMENU)0, hInst, 0);
		SetSystemFont(trang);
					  
		hCbbCurPageTag = CreateWindow(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_SIMPLE | CBS_HASSTRINGS,
			95, 270, 40, 200, hWnd, (HMENU)ID_CBB_CURPAGETAG, hInst, 0);

		HWND temp = CreateWindow(L"Static", L"/", WS_CHILD | WS_VISIBLE, 
			140, 275, 5, 30, hWnd, (HMENU)0, hInst, 0);
		SetSystemFont(temp);

		hTotalPageTag = CreateWindow(L"Static", L"0", WS_CHILD | WS_VISIBLE,
			145, 275, 50, 30, hWnd, (HMENU)0, hInst, 0);

		//group notes: listview notes
		HWND groupNotes = CreateWindow(L"BUTTON", L"Danh sách note", 
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX | BS_CENTER,
			350, 10, 300, 300, hWnd, (HMENU)0, hInst, 0);

		hLvwNotes = CreateWindow(WC_LISTVIEW, L"",WS_CHILD | WS_VISIBLE | WS_BORDER |
			WS_VSCROLL | LVS_REPORT | WS_TABSTOP | LVS_SHOWSELALWAYS,
			365, 50, 270, 210, hWnd, (HMENU)ID_LVW_LISTNOTES, hInst, NULL);
		InitColumnListNotes(hLvwNotes);
		ListView_SetExtendedListViewStyle(hLvwNotes, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		HWND trang2 = CreateWindow(L"Static", L"Trang", WS_CHILD | WS_VISIBLE, 
			365, 275, 50, 30, hWnd, (HMENU)0, hInst, 0);
		SetSystemFont(trang2);

		hCbbCurPageNote = CreateWindow(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_SIMPLE,
			415, 270, 40, 200, hWnd, (HMENU)ID_CBB_CURPAGENOTE, hInst, 0);
		ComboBox_AddString(hCbbCurPageNote, L"0");

		HWND temp2 = CreateWindow(L"Static", L"/", WS_CHILD | WS_VISIBLE, 
			460, 275, 5, 30, hWnd, (HMENU)0, hInst, 0);
		SetSystemFont(temp2);

		hTotalPageNote = CreateWindow(L"Static", L"0", WS_CHILD | WS_VISIBLE,
			465, 275, 50, 30, hWnd, (HMENU)0, hInst, 0);

		//group content: textbox content
		HWND groupContent = CreateWindow(L"BUTTON", L"Nội dung note",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX | BS_CENTER,
			675, 10, 300, 300, hWnd, (HMENU)0, hInst, 0);

		hTxtContent = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL |
			WS_VSCROLL | ES_MULTILINE | WS_BORDER | ES_READONLY,
			700, 50, 270, 240, hWnd, (HMENU)IDC_TXT_CONTENT, hInst, 0);

		//button delete
		hBtnDel = CreateWindow(L"BUTTON", L"Xóa", WS_CHILD,
			200, 350, 100, 30, hWnd, (HMENU)ID_BTN_DEL, hInst, 0);

		hBtnViewStatistic = CreateWindow(L"BUTTON", L"Biểu đồ thống kê", WS_CHILD | WS_VISIBLE,
			350, 350, 150, 30, hWnd, (HMENU)ID_BTN_VIEWSTATISTIC, hInst, 0);
					 
		hBtnNewNote = CreateWindow(L"BUTTON", L"Thêm note mới", WS_CHILD | WS_VISIBLE,
			550, 350, 150, 30, hWnd, (HMENU)ID_BTN_NEWNOTE, hInst, 0);

		//set font
		SetSystemFont(groupTags);
		SetSystemFont(groupNotes);
		SetSystemFont(groupContent);
		SetSystemFont(hLvwTags);
		SetSystemFont(hLvwNotes);
		SetSystemFont(hTxtContent);
		SetSystemFont(hTotalPageNote);
		SetSystemFont(hCbbCurPageNote);
		SetSystemFont(hTotalPageTag);
		SetSystemFont(hCbbCurPageTag);
		SetSystemFont(hBtnDel);
		SetSystemFont(hBtnViewStatistic);
		SetSystemFont(hBtnNewNote);

		if (LoadTags(listTag))
		{
			int totalRecord = listTag.size();
			g_totalPageTag = totalRecord / ROWNUMTAG + (totalRecord % ROWNUMTAG == 0 ? 0 : 1);

			PhanTrangLvwTags(hLvwTags, listTag, g_totalPageTag, g_curPageTag);	//phân trang listview tag
			AddDataCbb(hCbbCurPageTag, g_totalPageTag, g_curPageTag);				//thêm data vô combobox
			SetWindowText(hTotalPageTag, to_wstring(g_totalPageTag).c_str());
		}

		InstallHook(hWnd);
	}
		break;//WM_CREATE
		
	case WM_ICONNOTIFICATION:
	{
		wmId = LOWORD(lParam);

		switch (wmId)
		{
		case WM_LBUTTONDBLCLK: //left double click on icon will show main window
			ShowWindow(hWnd, SW_NORMAL);
			break;

		case WM_RBUTTONDOWN: //right click on icon will show 3 options: view note, view statistic & exit
		{
								 POINT  curMousePos;
								 HMENU hPopupMenu;

								 GetCursorPos(&curMousePos);
								 hPopupMenu = CreatePopupMenu();
								 ShowPopupMenu(hWnd, curMousePos.x, curMousePos.y, hPopupMenu);
		}
			break;
		}
	
	}
		break;//WM_ICONNOTIFICATION

	case WM_NOTIFY:
	{
		wmId = LOWORD(wParam);
					  
		switch (wmId)
		{
		case ID_LVW_LISTTAGS:
		{
			LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)lParam;
			NMHDR info = pnmtv->hdr;

			if (NM_CLICK == info.code)
			{
				int prevSelectedItem = g_selectedItemListTags;// tránh trường hợp người dùng 
				//click vào item trong listview tag mà k có dữ liệu

				g_selectedItemListTags = ListView_GetNextItem(hLvwTags, -1, LVNI_SELECTED);

				if (g_selectedItemListTags >= 0)
				{
					//tham chiếu tương ứng index trong vector
					g_selectedItemListTags += ROWNUMTAG*(g_curPageTag - 1);

					vector<Note> listNote = listTag[g_selectedItemListTags]->GetListNote();
					int totalRecord = listNote.size();
					g_totalPageNote = totalRecord / ROWNUMNOTE + (totalRecord % ROWNUMNOTE == 0 ? 0 : 1);

					PhanTrangLvwNotes(hLvwNotes, listNote, g_totalPageNote, g_curPageNote);	//phân trang listview note
					AddDataCbb(hCbbCurPageNote, g_totalPageNote, g_curPageNote);			//thêm data vô combobox
					SetWindowText(hTotalPageNote, to_wstring(g_totalPageNote).c_str());
					SetWindowText(hTxtContent, L"");
					ShowWindow(hBtnDel, SW_NORMAL);
				}
				else
				{
					g_selectedItemListTags = prevSelectedItem;
					ShowWindow(hBtnDel, SW_HIDE);
				}
			}
		}
			break; //case: ID_LVW_LISTTAGS

		case ID_LVW_LISTNOTES:
		{
			LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)lParam;
			NMHDR info = pnmtv->hdr;

			if (g_selectedItemListTags == -1)
			{
				break;//nếu bên listtag chưa chọn thì bỏ qua
			}
			//nếu đã chọn tag
			if (NM_CLICK == info.code)
			{
				g_selectedItemListNotes = ListView_GetNextItem(hLvwNotes, -1, LVNI_SELECTED);

				if (g_selectedItemListNotes >= 0)
				{
					//tham chiếu tương ứng index trong vector
					g_selectedItemListNotes += ROWNUMNOTE*(g_curPageNote - 1);

					Note note;
									
					note = listTag[g_selectedItemListTags]->GetNote(g_selectedItemListNotes);
					SetWindowText(hTxtContent, note.GetContent().c_str());
					ShowWindow(hBtnDel, SW_NORMAL);
				}
				else ShowWindow(hBtnDel, SW_HIDE);
			}
		}
			break;//case ID_LVW_LISTNOTES
		}
	}
		break;//WM_NOTIFY

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// Parse the menu selections:
		switch (wmId)
		{
		case ID_BTN_DEL:
		{
			if (g_selectedItemListNotes >= 0) // đang chọn note nghĩa là muốn xóa note
			{
				listTag[g_selectedItemListTags]->RemoveNote(g_selectedItemListNotes);//xóa trong listTag
				
				if (listTag[g_selectedItemListTags]->GetFreq() == 0)//đã xóa hết note thì xóa luôn tag đi
				{
					g_selectedItemListNotes = -1;
					PostMessage(hWnd, WM_COMMAND, ID_BTN_DEL, 0);//gửi thông điệp xóa tag
				}
				else//còn note thì update lại listview note
				{
					ListView_DeleteItem(hLvwNotes, g_selectedItemListNotes);//xóa trong listview

					vector<Note> listNote = listTag[g_selectedItemListTags]->GetListNote();
					
					int totalRecordNote = listNote.size();
					int prevTotalPageNote = g_totalPageNote;
					g_totalPageTag = totalRecordNote / ROWNUMNOTE + (totalRecordNote % ROWNUMNOTE == 0 ? 0 : 1);

					if (g_curPageNote != prevTotalPageNote)//nếu số trang giảm thì update lại chỉ số trong combobox
					{
						AddDataCbb(hCbbCurPageNote, g_totalPageNote, g_curPageNote);			//thêm data vô combobox
						SetWindowText(hTotalPageNote, to_wstring(g_totalPageNote).c_str());
					}

					PhanTrangLvwNotes(hLvwNotes, listNote, g_totalPageNote, g_curPageNote);	//phân trang listview note
					SetWindowText(hTxtContent, L"");
					
					//update lại số note listview tag
					WCHAR szCountNote[10];
					_itow(listTag[g_selectedItemListTags]->GetFreq(), szCountNote, 10);
					ListView_SetItemText(hLvwTags, g_selectedItemListTags, 2, szCountNote);//update lại text

					g_selectedItemListNotes = -1;
				}
				ShowWindow(hBtnDel, SW_HIDE);//xóa xong thì ẩn nút Xóa đi luôn
			}
			else//nếu không chọn note, thì có nghĩa đang chọn tag --> muốn xóa tag 
				//(bởi vì chỉ khi chọn tag hoặc note mới hiện nút Xóa lên thôi)
			{
				delete listTag[g_selectedItemListTags];//giải phóng bộ nhớ 
				listTag.erase(listTag.begin() + g_selectedItemListTags);//xóa trong vector
				g_selectedItemListTags -= ROWNUMTAG*(g_curPageTag - 1);//tham chiếu cho đúng trên list view
				ListView_DeleteItem(hLvwTags, g_selectedItemListTags);

				g_selectedItemListTags = -1;
				ListView_DeleteAllItems(hLvwNotes);
				SetWindowText(hTxtContent, L"");
				ShowWindow(hBtnDel, SW_HIDE);//xóa xong thì ẩn nút Xóa đi luôn

				//update lại listview
				int totalRecord = listTag.size();
				int prevTotalPageTag = g_totalPageTag;
				g_totalPageTag = totalRecord / ROWNUMTAG + (totalRecord % ROWNUMTAG == 0 ? 0 : 1);

				if (g_totalPageTag != prevTotalPageTag)//nếu có giảm trang thì update lại bảng trang
				{
					PhanTrangLvwTags(hLvwTags, listTag, g_totalPageTag, g_curPageTag);	//phân trang listview tag
					AddDataCbb(hCbbCurPageTag, g_totalPageTag, g_curPageTag);			//thêm data vô combobox
					SetWindowText(hTotalPageTag, to_wstring(g_totalPageTag).c_str());
				}
			}
		}
			break;//case ID_BTN_DEL

		case ID_BTN_VIEWSTATISTIC:
			PostMessage(hWnd, WM_COMMAND, IDC_POPUP_VIEWSTATISTIC, 0);
			break;

		case ID_BTN_NEWNOTE:
			PostMessage(hWnd, WM_COMMAND, IDC_POPUP_NEWNOTE, 0);
			break;

		case ID_CBB_CURPAGETAG://select page to show
			if (wmEvent == CBN_SELCHANGE)
			{
				int prevPage = g_curPageTag;

				g_curPageTag = SendMessage(hCbbCurPageTag, CB_GETCURSEL, 0, 0) + 1;//+ 1 tại vì index của page bắt đầu từ 1

				if (g_curPageTag != prevPage)
				{
					PhanTrangLvwTags(hLvwTags, listTag, g_totalPageTag, g_curPageTag);
					ListView_DeleteAllItems(hLvwNotes);//xóa danh sách bên note
				}
			}
				break;

		case IDC_POPUP_NEWNOTE:
			if (!g_isOpeningDlg)
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ADDNOTE), hWnd, AddNote);
			}
			break;

		case IDC_POPUP_VIEWNOTE:
			ShowWindow(hWnd, SW_NORMAL);
			break;

		case IDC_POPUP_VIEWSTATISTIC:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_STATISTIC), hWnd, ViewStatistic);
			break;

		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;//case WM_COMMAND
		
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;

	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		break;

	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &nid);//xóa icon ở khu vực thông báo
		SaveTags(listTag);//save data
		RemoveHook(hWnd);
		//giải phóng bộ nhớ
		for (int i = 0; i < listTag.size(); i++)
		{
			delete listTag[i];
			listTag[i] = NULL;
		}
		listTag.clear();

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK AddNote(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		g_isOpeningDlg = TRUE;
		return (INT_PTR)TRUE;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case ID_BTN_SAVE:
		{
							WCHAR szNote[BUFLEN];
							WCHAR szTag[BUFLEN];
							WCHAR szContent[BUFLEN];

							SYSTEMTIME sysTime;

							int lenNote = GetDlgItemText(hDlg, IDC_TXT_NOTE, szNote, BUFLEN);
							int lenTag = GetDlgItemText(hDlg, IDC_TXT_TAG, szTag, BUFLEN);
							int lenContent = GetDlgItemText(hDlg, IDC_TXT_CONTENT, szContent, BUFLEN);

							if (lenNote < 1 || lenTag < 1 || lenContent < 1)
							{
								MessageBox(0, L"Thông tin không được bỏ trống!!!", L"Thông báo", 0);
								break;
							}

							GetLocalTime(&sysTime);
							Note note(szNote, szContent, sysTime);

							bool isAddNoteSuccessful = false;
							wstring wsListTag = szTag;

							while (wsListTag.length() > 0)
							{
								wstring tag;
								tag = SplitTag(wsListTag);

								if (!tag.empty())
								{
									isAddNoteSuccessful = true;
									AddNoteToListTag(listTag, note, tag);
								}
							}
							
							if (isAddNoteSuccessful)
							{
								//sau khi thêm xong thì sắp xếp lại
								SortListTags(listTag);
								//update lại listview
								int totalRecord = listTag.size();
								int prevTotalPageTag = g_totalPageTag;
								g_totalPageTag = totalRecord / ROWNUMTAG + (totalRecord % ROWNUMTAG == 0 ? 0 : 1);

								PhanTrangLvwTags(hLvwTags, listTag, g_totalPageTag, g_curPageTag);	//phân trang listview tag
								
								if (g_totalPageTag != prevTotalPageTag)
								{
									AddDataCbb(hCbbCurPageTag, g_totalPageTag, g_curPageTag);			//thêm data vô combobox
									SetWindowText(hTotalPageTag, to_wstring(g_totalPageTag).c_str());
								}
								
								MessageBox(0, L"Thêm thành công", L"", 0);
								EndDialog(hDlg, LOWORD(wParam));
								break;
							}
							else
							{
								MessageBox(0, L"Thông tin tag không được để trống!!!", L"Thông báo", 0);
								break;
							}
		}
			break;//cae ID_BTN_SAVE

		case ID_BTN_CANCEL:
			PostMessage(hDlg, WM_DESTROY, 0, 0);
			break;

		default:
			break;
		}
	}
		break;//WM_COMMAND

	case WM_CLOSE:
		PostMessage(hDlg, WM_DESTROY, 0, 0);
		return (INT_PTR)TRUE;

	case WM_DESTROY:
		EndDialog(hDlg, LOWORD(wParam));
		g_isOpeningDlg = FALSE;//cho phep mo dialog khac
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ViewStatistic(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		return (INT_PTR)TRUE;

	case WM_PAINT:
	{
					 PAINTSTRUCT ps;
					 HDC hdc;

					 hdc = BeginPaint(hDlg, &ps);

					 //// TODO: Add any drawing code here..		
					 RECT rect;
					 GetClientRect(hDlg, &rect);

					 DrawPieChart(listTag, hdc, hDlg);
					 
					 ReleaseDC(hDlg, hdc);
					 EndPaint(hDlg, &ps);
	}
		break;//case paint
	case WM_CLOSE:
		GdiplusShutdown(gdiplusToken);
		EndDialog(hDlg, LOWORD(wParam));
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}
