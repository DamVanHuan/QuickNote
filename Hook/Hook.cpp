// DLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <Windowsx.h>

#define EXPORT  __declspec(dllexport)

HWND hwnd;
HHOOK hHook = NULL;
HINSTANCE hinstLib;

#define IDC_POPUP_NEWNOTE 117
#define IDC_POPUP_VIEWSTATISTIC 115

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(hHook, nCode, wParam, lParam);

	//getstate & 0x800 <==> getsate & (1 << 15) --> check hightest bit is set or not
	if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && 
		(GetAsyncKeyState(VK_SHIFT) & 0x8000) && 
		(GetAsyncKeyState(0x4E) & 0x8000)) //0x4E: key "N"
	{
		PostMessage(hwnd, WM_COMMAND, IDC_POPUP_NEWNOTE, 0);//IDC_POPUP_NEWNOTE = 117
	}
	else
	if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && 
		(GetAsyncKeyState(VK_SHIFT) & 0x8000) && 
		(GetAsyncKeyState(0x45) & 0x8000)) //0x45: key "E"
	{
		DestroyWindow(hwnd);//thoát chương trình (CTR SHIFT E)
	}
	else
	if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && 
		(GetAsyncKeyState(VK_SHIFT) & 0x8000) && 
		(GetAsyncKeyState(0x53) & 0x8000)) //0x53: key "S"
	{
		PostMessage(hwnd, WM_COMMAND, IDC_POPUP_VIEWSTATISTIC, 0);//IDC_POPUP_VIEWSTATISTIC = 115
	}
	//else
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

HWND hLabel;

EXPORT void doInitHook(HWND hWnd)
{
	hwnd = hWnd;

	if (hHook)
		return;

	hHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardHookProc, hinstLib, 0);

	//if (hHook != NULL)
	//{
	//	MessageBox(hWnd, L"Setup hook successfully", L"Thông báo", MB_OK);
	//}
	//else MessageBox(hWnd, L"Fail setup hook", L"Thông báo", 0);
}

EXPORT void doRemoveHook(HWND hWnd)
{
	if (hHook == NULL)
		return;

	if (UnhookWindowsHookEx(hHook))
	{
		hHook = NULL;
		//MessageBox(hWnd, L"Remove hook successfully", L"Thông báo", MB_OK);
	}
}