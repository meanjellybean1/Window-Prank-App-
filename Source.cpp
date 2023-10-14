#include "Header.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK NewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

static int windowCount = 0;
HBITMAP hBitmap;
HHOOK keyboardHook = nullptr;
int LAG = 150; // how many windows you want to open
int Mouse = 300; // how much the window runs away from mouse (below 450 -100)

std::vector<HWND> windows;

HBITMAP loadImage(const std::string& imagePath) {
	HBITMAP hBitmap = (HBITMAP)LoadImageA(NULL, imagePath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBitmap == NULL) {
		MessageBox(NULL, "Failed to load the image!", "Error", MB_OK | MB_ICONERROR);
	}
	return hBitmap;
}
 

void createNewWindow(const std::string& windowTitle)
{
	HBITMAP hBitmap = loadImage("D:\\acana\\Desktop\\Timon.bmp");
	if (hBitmap == NULL) return;

	BITMAP bitmap;
	GetObject(hBitmap, sizeof(bitmap), &bitmap);

	int windowWidth = bitmap.bmWidth;
	int windowHeight = bitmap.bmHeight;

	char windowClass[100];
	sprintf_s(windowClass, "NewWindowClass%d", windowCount);

	WNDCLASSA newWc = {};
	newWc.lpfnWndProc = NewWindowProc;
	newWc.hInstance = GetModuleHandle(NULL);
	newWc.lpszClassName = windowClass;

	if (!RegisterClassA(&newWc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	HWND newHwnd = CreateWindowExA(0, windowClass, windowTitle.c_str(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);

	if (newHwnd != NULL) {
		windowCount++;
		ShowWindow(newHwnd, SW_SHOWNORMAL);
	}
	else {
		MessageBox(NULL, "Window Creation Failed!", "Error", MB_OK | MB_ICONERROR);
	}
}


LRESULT handle_WM_CLOSE(HWND hwnd) {
	if (hBitmap) {
		DeleteObject(hBitmap);
		hBitmap = nullptr;  // Reset the bitmap handle to null
	}
	HBITMAP hBitmap = loadImage("D:\\acana\\Desktop\\Timon.bmp"); // called function 

	if (hBitmap == NULL) return 1;

	std::string windowTitle = "Escape Again " + std::to_string(windowCount);
	createNewWindow(windowTitle.c_str());
	
	return 0;
}

LRESULT handle_WM_LBUTTONDOWN(HWND hwnd) {
	int xNew = rand() % (GetSystemMetrics(SM_CXSCREEN) - 200);
	int yNew = rand() % (GetSystemMetrics(SM_CYSCREEN) - 200);
	SetWindowPos(hwnd, NULL, xNew, yNew, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	BOOL playResult = sndPlaySound(TEXT("C:\\Users\\acana\\source\\repos\\Prank App\\duck.wav"), SND_FILENAME | SND_ASYNC);
	return 0;
}

LRESULT handle_WM_PAINT(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	HBITMAP hBitmap = loadImage("D:\\acana\\Desktop\\Timon.bmp"); // called function

	if (hBitmap != NULL) {
		HDC hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBitmap);
		BITMAP bitmap;
		GetObject(hBitmap, sizeof(bitmap), &bitmap);
		BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);
	}

	EndPaint(hwnd, &ps);
	return 0;
}

LRESULT handle_WM_MOUSEMOVE(HWND hwnd) {
	RECT windowRect;
	GetWindowRect(hwnd, &windowRect);

	POINT mousePos;
	GetCursorPos(&mousePos);

	// Calculate distance between mouse and window
	int deltaX = abs(mousePos.x - windowRect.left);
	int deltaY = abs(mousePos.y - windowRect.top);

	// Define a threshold distance (e.g., 50 pixels) for the window to start moving away
	
	double variationPercentage = 0.15; //variation of movement 

	int threshold = Mouse + static_cast<int>(Mouse * variationPercentage);
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (deltaX < threshold && deltaY < threshold) {
		// Calculate new position
		int newPosX = windowRect.left + (mousePos.x > windowRect.left ? -1 : 1)* (threshold - deltaX);
		int newPosY = windowRect.top + (mousePos.y > windowRect.top ? -1 : 1)* (threshold - deltaY);
		
		int randomOffsetX = rand() % (3 * threshold) - threshold; // Random offset between -threshold and +threshold
		int randomOffsetY = rand() % (3 * threshold) - threshold; // Random offset between -threshold and +threshold

		newPosX += randomOffsetX;
		newPosY += randomOffsetY;

		// Ensure the window does not go off the screen
		newPosX = max(0, min(screenWidth - (windowRect.right - windowRect.left), newPosX));
		newPosY = max(0, min(screenHeight - (windowRect.bottom - windowRect.top), newPosY));

		// Set new window position
		SetWindowPos(hwnd, HWND_TOPMOST, newPosX, newPosY, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE: return handle_WM_CLOSE(hwnd); 
	case WM_LBUTTONDOWN: return handle_WM_LBUTTONDOWN(hwnd);
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT: return handle_WM_PAINT(hwnd);
	case WM_MOUSEMOVE:
		return handle_WM_MOUSEMOVE(hwnd);
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}



LRESULT CALLBACK NewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE: return handle_WM_CLOSE(hwnd); 
	case WM_LBUTTONDOWN: return handle_WM_LBUTTONDOWN(hwnd);
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT: return handle_WM_PAINT(hwnd);
	case WM_MOUSEMOVE:
		return handle_WM_MOUSEMOVE(hwnd);
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void prankwindow() {
	MSG msg = {};
	HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

	HBITMAP hBitmap = loadImage("D:\\acana\\Desktop\\Timon.bmp"); // called function 

	if (hBitmap == NULL) return;

	std::string windowTitle = "Prank App ";
	createNewWindow(windowTitle.c_str());
	
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		UnhookWindowsHookEx(keyboardHook);// related to the hook

	}
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	static int counter = 0; // Counter variable to keep track of the number of times the code has run

	if (nCode == HC_ACTION) {
		if (wParam == WM_KEYDOWN) {
			KBDLLHOOKSTRUCT* pKbdStruct = (KBDLLHOOKSTRUCT*)lParam;
			DWORD vkCode = pKbdStruct->vkCode;
			if (vkCode == VK_F4 || vkCode == VK_HOME || vkCode == VK_RMENU || vkCode == VK_MENU || vkCode == VK_RWIN) {
				MessageBox(NULL, "BLOCKED your atl + f4!", "Error", MB_OK | MB_ICONERROR);
				return 1;
			}
			if (vkCode == VK_LCONTROL || vkCode == VK_RCONTROL) {
				counter++; // Increment the counter each time the control key is pressed or held

				for (int i = 0; i < LAG; ++i) {
					HBITMAP hBitmap = loadImage("D:\\acana\\Desktop\\Timon.bmp"); // called function 
					if (hBitmap == NULL) return 1;

					BITMAP bitmap;
					GetObject(hBitmap, sizeof(bitmap), &bitmap);

					int windowWidth = bitmap.bmWidth;
					int windowHeight = bitmap.bmHeight;

					char windowClass[100];
					sprintf_s(windowClass, "NewWindowClass%d", windowCount);

					WNDCLASSA newWc = {};
					newWc.lpfnWndProc = NewWindowProc;
					newWc.hInstance = GetModuleHandle(NULL);
					newWc.lpszClassName = windowClass;

					if (!RegisterClassA(&newWc)) {
						MessageBox(NULL, "Window Registration Failed!", "Error", MB_OK | MB_ICONERROR);
						return 1;
					}

					char windowTitle[100];
					sprintf_s(windowTitle, "YOU WILL NEVER ESCAPE!!! %03d", windowCount);

					HWND newHwnd = CreateWindowExA(0, windowClass, windowTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, NULL, NULL, GetModuleHandle(NULL), NULL);

					if (newHwnd != NULL) {
						windowCount++;  // Increment the window count
						ShowWindow(newHwnd, SW_SHOWNORMAL);
					}
					else {
						MessageBox(NULL, "Window Creation Failed!", "Error", MB_OK | MB_ICONERROR);
					}
				}
				if (counter >= LAG) {
					counter = 0;
				}
			}
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}


void KeyboardHookThread() {
	// Set up the keyboard hook
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

	// Message loop for the hook
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Unhook the keyboard hook before exiting the thread
	UnhookWindowsHookEx(keyboardHook);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	std::thread keyboardHookThread(KeyboardHookThread);

	// Execute the main application logic in the main thread
	prankwindow();

	// Wait for the keyboard hook thread to finish and then join the thread
	if (keyboardHookThread.joinable()) {
		keyboardHookThread.join();
	}

	return 0;
}