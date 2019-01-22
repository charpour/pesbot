// ------------------------------------------------------------------
//
//  FILENAME: readfilehook.cpp		AUTHOR: JG 2019
//
// ------------------------------------------------------------------
#include "stdafx.h"

#include <string>

HWND window = NULL;
HANDLE file_map = NULL;
LPVOID shared_buffer = NULL;
BYTE *tramp;

typedef BOOL(WINAPI* _ReadFile)(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped);

_ReadFile og_read_file;

BOOL WINAPI read_file(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped)
{
	BOOL ret = og_read_file(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

	BYTE *og_buffer = (BYTE*)lpBuffer;
	if (og_buffer[0] != 0x1)
		return ret;
	
	DWORD *buffer = (DWORD*)shared_buffer;
	if (*buffer == 1) {
		restore_jmp();
		CloseHandle(file_map);
		*buffer = 0;
		return ret;
	}

	static DWORD count = 0, pause = 0;

	static BOOLEAN in_menu = FALSE, match_data = FALSE;

	if (count % 33 == 0) {
		if (in_menu) {
			if (on_matchmaking())
				in_menu = FALSE;
			if (!on_match_tab())
				press_button(L1, og_buffer);
			else if (!on_sim_panel())
				press_button(DPAD_RIGHT, og_buffer);
			else
				press_button(CROSS, og_buffer);
		}
		else {
			int screen = get_screen();
			/*std::string test1 = std::to_string(screen);
			MessageBoxA(NULL, test1.c_str(), "testx", MB_OK);*/
			if (!on_match_data()) {
				if (screen == MENU) {
					in_menu = TRUE;
					match_data = FALSE;
				}
				else if (screen == KICKOFF || screen == HALFTIME || screen == FULLTIME || screen == SCOUT_REWARDS ||
					screen == LEVEL_UP || screen == RATING_CHANGE || screen == GP_REWARDS || screen == EXP ||
					screen == MANAGER || screen == RENEW_YES || screen == STATUS_CHECK || screen == YES_SURE ||
					screen == HAS_RENEWED)
					press_button(CROSS, og_buffer);
				else if (screen == RENEW_NO || screen == ARE_YOU_SURE)
					press_button(DPAD_RIGHT, og_buffer);
				else if (!match_data) {
					if (!on_match_data())
						press_button(TRIANGLE, og_buffer);
					else
						match_data = TRUE;
				}
			}
		}
	}

	count++;

	return ret;
}

int get_screen()
{
	if (on_squad_man())
		return MENU;
	else if (check_pixels(416, 26, 119, 26, 124, 127, 125))
		return KICKOFF;
	else if (check_pixels(109, 413, 49, 33, 177, 175, 175))
		return HALFTIME;
	else if (check_pixels(195, 414, 48, 32, 183, 179, 179))
		return FULLTIME;
	else if (check_pixels(57, 65, 206, 32, 85, 95, 107))
		return EXP;
	else if (check_pixels(342, 62, 121, 36, 109, 116, 124))
		return LEVEL_UP;
	else if (check_pixels(372, 64, 200, 33, 81, 92, 107))
		return RATING_CHANGE;
	else if (check_pixels(390, 99, 175, 104, 203, 209, 225))
		return GP_REWARDS;
	else if (check_pixels(381, 120, 183, 29, 190, 196, 209))
		return SCOUT_REWARDS;
	else if (check_pixels(57, 64, 109, 35, 101, 111, 122))
		return MANAGER;
	else if (check_pixels(131, 65, 152, 32, 93, 103, 115))
		return STATUS_CHECK;
	else if (blue_button(235, 289) && blue_button(719, 289))
		return HAS_RENEWED;
	else if (blue_button(679, 366))
		return RENEW_YES;
	else if (blue_button(272, 366))
			return RENEW_NO;
	else if (blue_button(285, 301))
		return ARE_YOU_SURE;
	else if (blue_button(557, 300))
		return YES_SURE;
	else
		return UNKNOWN;
}

void send_input(DWORD windows_msg)
{
	PostMessage(window, WM_KEYDOWN, windows_msg, 0);
}


BOOLEAN blue_button(int x, int y)
{
	RGBQUAD pixel = *get_pixels(x, y, 1, 1);

	if (pixel.rgbRed < 47)
		return TRUE;

	return FALSE;
}


BOOLEAN on_squad_man()
{
	if (check_pixel(241, 89, 53, 32, 92))
		return TRUE;

	return FALSE;
}


BOOLEAN on_matchmaking()
{
	if (check_pixels(390, 65, 175, 92, 81, 95, 104))
		return TRUE;

	return FALSE;
}


BOOLEAN on_match_data()
{
	if (check_pixels(413, 7, 64, 15, 53, 60, 70))
		return TRUE;

	return FALSE;
}

 
BOOLEAN on_sim_panel()
{
	if (check_pixels(622, 170, 236, 142, 129, 116, 149))
		return TRUE;

	return FALSE;
}


BOOLEAN on_match_tab()
{
	if (check_pixel(75, 92, 60, 36, 94))
		return TRUE;

	return FALSE;
}


void press_button(BYTE button, BYTE *buffer)
{
	if (button == L1 || button == OPTIONS)
		buffer[6] = button;
	else
		buffer[5] = button;
}


AVR_RGB get_average_rgb(RGBQUAD pixels[], int image_size)
{
	double r = 0, g = 0, b = 0;

	for (int i = 0; i < image_size - 1; i++) {
		r += pixels[i].rgbRed * pixels[i].rgbRed;
		g += pixels[i].rgbGreen * pixels[i].rgbGreen;
		b += pixels[i].rgbBlue * pixels[i].rgbBlue;
	}

	AVR_RGB rgb = { (int)sqrt(r / image_size), (int)sqrt(g / image_size), (int)sqrt(b / image_size) };

	return rgb;
}


BOOLEAN check_pixels(DWORD x, DWORD y, int width, int height, int r, int g, int b)
{
	RGBQUAD *pixels = get_pixels(x, y, width, height);

	AVR_RGB rgb = get_average_rgb(pixels, width * height);

	free(pixels);

	if ((rgb.r >= (r - 9) && rgb.r <= (r + 9)) &&
		(rgb.g >= (g - 9) && rgb.g <= (g + 9)) &&
		(rgb.b >= (b - 9) && rgb.b <= (b + 9)))
		return TRUE;
	else
		return FALSE;
}


BOOLEAN check_pixel(DWORD x, DWORD y, int r, int g, int b)
{
	RGBQUAD *pixels = get_pixels(x, y, 1, 1);
	RGBQUAD pixel = *pixels;
	free(pixels);

	if ((pixel.rgbRed >= (r - 13) && pixel.rgbRed <= (r + 13)) &&
		(pixel.rgbGreen >= (g - 13) && pixel.rgbGreen <= (g + 13)) &&
		(pixel.rgbBlue >= (b - 13) && pixel.rgbBlue <= (b + 13)))
		return TRUE;
	else
		return FALSE;
}


RGBQUAD *get_pixels(DWORD x, DWORD y, int width, int height)
{
	RECT rc;
	GetWindowRect(window, &rc);
	const DWORD image_size = width * height;
	RGBQUAD *pixels = (RGBQUAD*)malloc(sizeof(RGBQUAD) * image_size);

	HDC hdc = GetDC(window);
	HDC mem_dc = CreateCompatibleDC(hdc);
	HBITMAP bmp = CreateCompatibleBitmap(hdc, width, height);
	SelectObject(mem_dc, bmp);

	BitBlt(mem_dc, 0, 0, width, height, hdc, (rc.left + x), (rc.top + y), SRCCOPY);

	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	GetDIBits(mem_dc, bmp, 0, height, pixels, &bmi, DIB_RGB_COLORS);


	ReleaseDC(window, hdc);
	DeleteDC(mem_dc);
	DeleteObject(bmp);

	return pixels;
}


void restore_jmp()
{
	BYTE *og_func = (LPBYTE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "ReadFile");
	
	DWORD og_protect, protect, read_file_offset;

	VirtualProtect(og_func, 5, PAGE_EXECUTE_READWRITE, &og_protect);

	read_file_offset = ((DWORD)GetProcAddress(GetModuleHandleA("Kernelbase.dll"), "ReadFile") - (DWORD)og_func) - 5;

	*og_func = 0xE9;
	*((DWORD *)(og_func + 0x1)) = read_file_offset;

	VirtualProtect(og_func, 5, og_protect, &protect);
}


void setup_hook()
{
	BYTE *og_func = (LPBYTE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "ReadFile");
	
	DWORD og_protect, protect, tramp_offset, hook_offset;

	tramp = (BYTE*)VirtualAlloc(NULL, 15, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	VirtualProtect(og_func, 5, PAGE_EXECUTE_READWRITE, &og_protect);

	tramp_offset = ((DWORD)(tramp)-(DWORD)og_func) - 5;

	*og_func = 0xE9;
	*((DWORD *)(og_func + 0x1)) = tramp_offset;

	VirtualProtect(og_func, 5, og_protect, &protect);

	hook_offset = ((DWORD)read_file - (DWORD)(tramp)) - 5;

	*tramp = 0xE9;
	*((DWORD *)(tramp + 0x1)) = hook_offset;

	og_read_file = (_ReadFile)GetProcAddress(GetModuleHandleA("Kernelbase.dll"), "ReadFile");
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		window = FindWindowA(NULL, "PS4 Remote Play");
		if (window == NULL)
			return FALSE;

		file_map = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "remote_play_map");
		if (file_map == NULL)
			return FALSE;

		shared_buffer = MapViewOfFile(file_map, FILE_MAP_ALL_ACCESS, 0, 0, 4);
		if (shared_buffer == NULL) {
			CloseHandle(file_map);
			return FALSE;
		}

		*(DWORD*)shared_buffer = (DWORD)hModule;

		setup_hook();
	}
	return TRUE;
}
