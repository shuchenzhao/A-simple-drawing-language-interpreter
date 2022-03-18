#ifndef _PARSER_WITH_SEMANTICS
//#error "你必须定义宏_PARSER_WITH_SEMANTICS."
#endif

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wingdi.h>
#include <WinUser.h>
#include "ui.h"
#include "errlog.h"

HDC hDC;
char SrcFilePath[2021 + 1];
static char* Name = "A simple drawing language compiler";

void Action() {
	extern void Parser(const char* file_name);

	InitError();
	Parser(SrcFilePath);
	CloseError();
}

int CheckSrcFile(LPSTR lpszCmdParam) {
	FILE* file = NULL;

	if (strlen(lpszCmdParam) == 0) {
		ShowMessage(1, "未指定源程序文件！");
		return 0;
	}
	if ((file = fopen(lpszCmdParam, "r")) == NULL) {
		ShowMessage(1, "打开源程序文件失败！");
		MessageBox(NULL, lpszCmdParam, "文件名", MB_OK);
		return 0;
	}
	else fclose(file);
	return 1;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
	case WM_DESTROY:
		ReleaseDC(hWnd, hDC);
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		PAINTSTRUCT pt;
		BeginPaint(hWnd, &pt);

		Action();

			EndPaint(hWnd, &pt);
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
	}
}

int PrepareWindow(HINSTANCE hInst, HINSTANCE hPrevInstance, int nCmdShow) {
	HWND hWnd;
	WNDCLASS W;

	int x, y;
	int width, height;

	width = 740; height = 490;
	x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	memset(&W, 0, sizeof(WNDCLASS));
	W.style = CS_HREDRAW | CS_VREDRAW;
	W.lpfnWndProc = WndProc;
	W.hInstance = hInst;
	W.hCursor = LoadCursor(NULL, IDC_ARROW);
	W.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	W.lpszClassName = Name;
	RegisterClass(&W);

	hWnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW,
		x,y, width, height,
		NULL, NULL, hInst, NULL);
	if (hWnd == NULL) return 0;

	hDC = GetDC(hWnd);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetCursor(LoadCursor(hInst, IDC_ARROW));

	return 1;
}

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int	nCmdShow)
{
	strcpy(SrcFilePath, lpCmdLine);

	if (PrepareWindow(hInstance, hPrevInstance, nCmdShow) == 0) {
		ShowMessage(1, " 窗口初始化失败!");
		return 1;
	}

	if (CheckSrcFile(lpCmdLine) == 0) return 1;

	Action();

	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

void ShowMessage(int flag, const char* msg) {
	if (flag == 0)
		MessageBox(NULL, msg, "提示", MB_OK | MB_ICONINFORMATION);
	else
		MessageBox(NULL, msg, "ERROR", MB_OK | MB_ICONERROR);
}

void DrawPixel(unsigned long x, unsigned long y, unsigned int color_val) {
	COLORREF color = RGB(255, 0, 0);
	SetPixel(hDC, x, y, color);
	SetPixel(hDC, x + 1, y, color);
	SetPixel(hDC, x, y + 1, color);
	SetPixel(hDC, x + 1, y + 1, color);
}




