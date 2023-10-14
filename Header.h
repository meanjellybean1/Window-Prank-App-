#define _CRT_SECURE_NO_WARNINGS
#define _WIN32_WINNT 0x0501
#include <Windows.h>
#include <handleapi.h>
#include <fileapi.h>
#include <string>
#include <shlobj.h>
#include <stdio.h>
#include <time.h>
#include <wininet.h>
#include <thread>
#include <codecvt>
#include <tchar.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <mmsyscom.h>

//#pragma comment(lib, "Wininet.lib")
//#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "winmm.lib") // Link with Winmm.lib to use PlaySound

#define WCHAR_MAXPATH (MAX_PATH * sizeof(WCHAR))
