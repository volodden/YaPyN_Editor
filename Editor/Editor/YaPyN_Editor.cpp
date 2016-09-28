﻿#include "YaPyN_Editor.h"

// Временные путь для сохранения/загрузки.
const std::string filePathToLoad = "file.txt";
const std::string filePathToSave = "file.txt";

const int YaPyN_Editor::sizeBetweenCells = 10;
const int YaPyN_Editor::marginLeftRightCells = 8;

YaPyN_Editor::YaPyN_Editor()
{
	handleMainWindow = 0;
	changed = false;
	childrensWindow.resize(0);
}

YaPyN_Editor::~YaPyN_Editor()
{
	handleMainWindow = 0;
	changed = false;
	childrensWindow.resize(0);
}

bool YaPyN_Editor::RegisterClass()
{
	WNDCLASSEX windowClass;
	::ZeroMemory(&windowClass, sizeof(windowClass));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = YaPyN_Editor::windowProc;
	windowClass.hInstance = GetModuleHandle(0);
	windowClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	windowClass.lpszClassName = L"YaPyN_Editor";

	return (::RegisterClassEx(&windowClass) != 0);
}

bool YaPyN_Editor::Create()
{
	CreateWindowEx(0, 
		L"YaPyN_Editor",
		L"YaPyN Editor v1.0",
		WS_EX_OVERLAPPEDWINDOW | WS_SIZEBOX | WS_SYSMENU | WS_VSCROLL,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		GetModuleHandle(0),
		this);

	createToolbar();
	checkHandle(handleMainWindow);
	return (handleMainWindow != 0);
}

void YaPyN_Editor::Show(int cmdShow)
{
	ShowWindow(handleMainWindow, cmdShow);
	for( auto window = childrensWindow.begin(); window != childrensWindow.end(); ++window ) {
		window->Show(cmdShow);
	}
}

void YaPyN_Editor::OnNCCreate(HWND hwnd)
{
	handleMainWindow = hwnd;
}

void YaPyN_Editor::OnCreate()
{
	createCell();
	createCell();
	createCell();
	createCell();
	createCell();
}

void YaPyN_Editor::OnSize()
{
	InvalidateRect(handleMainWindow, NULL, FALSE);
	PAINTSTRUCT paintStruct;
	BeginPaint(handleMainWindow, &paintStruct);

	HBRUSH brush;
	brush = CreateSolidBrush(RGB(100, 150, 200));
	FillRect(paintStruct.hdc, &paintStruct.rcPaint, brush);
	EndPaint(handleMainWindow, &paintStruct);

	RECT rect;
	::GetClientRect(handleMainWindow, &rect);

	int currentTop = rect.top + sizeBetweenCells;
	for( auto window = childrensWindow.begin(); window != childrensWindow.end(); ++window ) {
		LONG leftBorder = rect.left + marginLeftRightCells;
		LONG width = rect.right - rect.left - 2 * marginLeftRightCells;
		SetWindowPos(window->getHandle(), HWND_TOP, leftBorder, currentTop, width, window->getHeight(), 0);
		currentTop += sizeBetweenCells + window->getHeight();
	}
}

void YaPyN_Editor::OnDestroy()
{
	PostQuitMessage(SuccessDestroyWindowValue);
}

bool YaPyN_Editor::OnClose()
{
	if( changed ) {
		switch( MessageBox(handleMainWindow, L"Вы ввели текст. Сохранить?", L"Завершение работы", MB_YESNOCANCEL | MB_ICONWARNING ))
		{
			case IDYES:
			{
				saveFile(filePathToSave);
				return true;
			}
			case IDNO:
			{
				return true;
			}
			case IDCANCEL:
			{
				return false;
			}
		}
	} else {
		switch( MessageBox(handleMainWindow, L"Вы действительно хотите выйти?", L"Завершение работы", MB_YESNO | MB_ICONWARNING )) {
			case IDNO:
			{
				return false;
			}
			case IDYES:
			{
				return true;
			}
		}
	}
	return true;
}

void YaPyN_Editor::OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if( HIWORD(wParam) == 0 ) {
		switch( LOWORD(wParam) ) {
			case ID_FILE_EXIT:
			{
				SendMessage(hWnd, WM_CLOSE, wParam, lParam);
				break;
			}
			case ID_FILE_SAVE:
			{
				saveFile(filePathToSave);
				break;
			}
			case ID_FILE_OPEN:
			{
				loadFile(filePathToLoad);
				break;
			}
			case ID_CELL_ADD:
			{
				createCell();
			}
			case ID_CELL_DELETE:
			{
				if( MessageBox(handleMainWindow, L"Вы действительно хотите удалить ячейку?",
					L"Удаление ячейки", MB_YESNO | MB_ICONWARNING) == IDYES ) {
					deleteCell();
				}
			}
			default:
			{
				break;
			}
		}
	} else {
		switch( HIWORD(wParam) ) {
			case EN_CHANGE:
			{
				changed = true;
				break;
			}
			case EN_UPDATE:
			{
				resizeCell(wParam, lParam);
			}
			// Здесь будет меню, но пока его нет.
			// Здесь будет акселлератор, но пока его нет.
		}
	}
}

void YaPyN_Editor::saveFile(std::string pathToFile)
{
}

void YaPyN_Editor::loadFile(std::string pathToFile)
{
}

void YaPyN_Editor::createToolbar()
{
	/*
	TBBUTTON tbb[] = {
		{ STD_FILENEW, ID_FILE_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0 },
		{ STD_FILEOPEN, ID_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0 },
		{ STD_FILESAVE, ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0 },
	};

	hWndToolBar = CreateToolbarEx(handle, WS_CHILD | WS_VISIBLE | CCS_TOP, 1,
		0, HINST_COMMCTRL, IDB_STD_SMALL_COLOR, tbb, _countof(tbb), 0, 0, 0, 0, sizeof(TBBUTTON));

	//DWORD backgroundColor = GetSysColor(COLOR_BTNFACE);
	//COLORMAP colorMap;
	//colorMap.from = RGB(100, 100, 100);
	//colorMap.to = backgroundColor;
	HBITMAP hbm = CreateMappedBitmap(::GetModuleHandle(0), IDB_BITMAP1, 0, NULL, 1);
	TBADDBITMAP tb;
	tb.hInst = NULL;
	tb.nID = reinterpret_cast<UINT_PTR>(hbm);
	int imageIndex = SendMessage(hWndToolBar, TB_ADDBITMAP, 0, reinterpret_cast<LPARAM>(&tb));
	TBBUTTON tbButtonsAdd[] =
	{
		{ imageIndex, ID_CELL_ADD, TBSTATE_ENABLED, BTNS_BUTTON },
		{ STD_DELETE, ID_CELL_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0 },
	};
	SendMessage(hWndToolBar, TB_ADDBUTTONS, _countof(tbButtonsAdd), reinterpret_cast<LPARAM>(tbButtonsAdd));
	*/
}

void YaPyN_Editor::createCell()
{
	childrensWindow.emplace_back(CellWindow());
	CellWindow* cell = &childrensWindow.back();
	cell->Create(handleMainWindow);
	cellsAndHandles.insert(std::make_pair(cell->getHandle(), cell));
}

void YaPyN_Editor::deleteCell()
{
	HWND hwnd = GetFocus();
	checkHandle(hwnd);
	auto cellAndHandle = cellsAndHandles.find(hwnd);
	SendMessage(hwnd, WM_DESTROY, 0, 0);
	childrensWindow.remove(*cellAndHandle->second);
	cellsAndHandles.erase(cellAndHandle);
}

void YaPyN_Editor::resizeCell(WPARAM wParam, LPARAM lParam)
{
	HWND handleCell = reinterpret_cast<HWND>(lParam);
	CellWindow* cell = cellsAndHandles.find(handleCell)->second;
	cell->increaseHeight();
	SendMessage(handle, WM_SIZE, 0, 0);

	//MessageBox(NULL, L"Тест", L"Тест", MB_OK);
}

LRESULT YaPyN_Editor::windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_NCCREATE ) {
		YaPyN_Editor* window = reinterpret_cast<YaPyN_Editor*> ((reinterpret_cast<CREATESTRUCT*>(lParam))->lpCreateParams);
		SetLastError(0);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG>(window));
		if( GetLastError() ) {
			return GetLastError();
		}
		window->OnNCCreate(hwnd);
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	YaPyN_Editor* window = reinterpret_cast<YaPyN_Editor*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	switch( message ) {
		case WM_CREATE:
		{
			window->OnCreate();
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		case WM_SIZE:
		{
			window->OnSize();
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		case WM_CLOSE:
		{
			if( window->OnClose() ) {
				::PostQuitMessage(0);
				return DefWindowProc(hwnd, message, wParam, lParam);
			} else {
				return 0;
			}
		}
		case WM_COMMAND:
		{
			window->OnCommand(hwnd, message, wParam, lParam);
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		case WM_DESTROY:
		{
			window->OnDestroy();
			return SuccessDestroyWindowValue;
		}
		default:
		{
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
	}
}
