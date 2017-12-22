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
HWND hCurPage = NULL;
HWND hTotalPage = NULL;

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
					  HWND groupTags = CreateWindow(L"BUTTON", L"List of tags", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
						  25, 10, 280, 440, hWnd, (HMENU)0, hInst, 0);

					  hLvwTags = CreateWindow(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | 
						  WS_VSCROLL | LVS_REPORT | WS_TABSTOP | LVS_SHOWSELALWAYS,
						  40, 35, 250, 200, hWnd, (HMENU)ID_LVW_LISTTAGS, hInst, 0);
					  InitColumnListTags(hLvwTags);
					  ListView_SetExtendedListViewStyle(hLvwTags, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

					  hCurPage = CreateWindow(L"Static", L"", WS_CHILD | WS_VISIBLE,
						  45, 250, 50, 30, hWnd, (HMENU)0, hInst, 0);

					  hTotalPage = CreateWindow(L"Static", L"", WS_CHILD | WS_VISIBLE,
						  100, 250, 50, 30, hWnd, (HMENU)0, hInst, 0);

					  //group notes: listview notes
					  HWND groupNotes = CreateWindow(L"BUTTON", L"List of notes", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
						  325, 10, 280, 440, hWnd, (HMENU)0, hInst, 0);

					  hLvwNotes = CreateWindow(WC_LISTVIEW, L"",WS_CHILD | WS_VISIBLE | WS_BORDER |
						  WS_VSCROLL | LVS_REPORT | WS_TABSTOP | LVS_SHOWSELALWAYS,
						  340, 35, 250, 385, hWnd, (HMENU)ID_LVW_LISTNOTES, hInst, NULL);
					  InitColumnListNotes(hLvwNotes);
					  ListView_SetExtendedListViewStyle(hLvwNotes, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

					  //group content: textbox content
					  HWND groupContent = CreateWindow(L"BUTTON", L"Content of note", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
						  625, 10, 280, 440, hWnd, (HMENU)0, hInst, 0);

					  hTxtContent = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL |
						  WS_VSCROLL | ES_MULTILINE | WS_BORDER | ES_READONLY,
						  640, 35, 260, 385, hWnd, (HMENU)IDC_TXT_CONTENT, hInst, 0);
					 
					  //set font
					  SetSystemFont(groupTags);
					  SetSystemFont(groupNotes);
					  SetSystemFont(groupContent);
					  SetSystemFont(hLvwTags);
					  SetSystemFont(hLvwNotes);
					  SetSystemFont(hTxtContent);

					  if (LoadTags(listTag))
					  {
						  //SortListViewTag(hLvwTags, listTag);
						  int totalRecord = listTag.size();
						  g_totalPage = totalRecord / ROWNUM + (totalRecord % ROWNUM == 0 ? 0 : 1);

						  PhanTrangLvwTags(hLvwTags, hTotalPage, hCurPage, listTag, g_totalPage, g_curPage);
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

								if (g_selectedItemListTags >= 0 &&
									g_selectedItemListTags < listTag.size())
								{
									InsertItemToListNote(hLvwNotes, listTag[g_selectedItemListTags]->GetListNote());
									SetWindowText(hTxtContent, L"");
								}
								else g_selectedItemListTags = prevSelectedItem;
							}
					  }
						  break;

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
								int iSelectedNote = ListView_GetNextItem(hLvwNotes, -1, LVNI_SELECTED);

								if (iSelectedNote >= 0 &&
									iSelectedNote < listTag[g_selectedItemListTags]->GetFreq())
								{
									Note note = listTag[g_selectedItemListTags]->GetNote(iSelectedNote);
													   
									SetWindowText(hTxtContent, note.GetContent().c_str());
								}
							}
					  }
						  break;
					  }
	}
		break;//WM_NOTIFY

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
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
		break;
		
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
								//SortListViewTag(hLvwTags, listTag);

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