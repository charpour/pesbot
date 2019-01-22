// ------------------------------------------------------------------
//
//  FILENAME: pesbot.cpp       AUTHOR: JG 2019
//
//      A bot for Pro Evolution Soccer 2019
//
//      Automates progression of Become A Legend, Master League, 
//      and MyClub Divisions SIM game modes.
// 
//      Uses Windows Messages and a ReadFile hook to send input
//      to the game via the PS4 Remote Play app.
//
//      You can download the Remote Play app @
//      https://remoteplay.dl.playstation.net/remoteplay
//
// ------------------------------------------------------------------

#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

#include "pesbot.h"

static BOOLEAN stop_bot = FALSE;


void print_usage() {
    printf("    Usage:\n");
    printf("        pesbot.exe [mode]\n\n");
    printf("    Modes:\n");
    printf("        ml      Master League\n");
    printf("        bal     Become A Lenged\n");
    printf("        sim     My Club Sim\n\n");
    printf("    Example:\n");
    printf("        pesbot.exe bal\n");
}


static size_t djb_hash(const char *cp)
{
    size_t hash = 5381;
    while (*cp)
        hash = 33 * hash ^ (unsigned char) *cp++;
    return hash;
}


BOOLEAN inject_hook(HWND window, HANDLE process)
{
    char dll_path[MAX_PATH];
    GetFullPathNameA("readfilehook.dll", MAX_PATH, (LPSTR)dll_path, NULL);
    DWORD dll_path_len = strlen(dll_path);
    
    LPVOID dll_addr = VirtualAllocEx(process, NULL, dll_path_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if(dll_addr == NULL) {
        printf("VitualAllocEx failure. Error (%lu)\n", GetLastError());
        return FALSE;
    }

    if(!WriteProcessMemory(process, dll_addr, dll_path, dll_path_len, NULL)) {
        printf("WriteProcessMemory failure. Error (%lu)\n", GetLastError());
        return FALSE;
    }
    
    LPVOID load_library = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    if(load_library == NULL) {
        printf("GetProcAddress failure. LoadLibrary not found. Error (%lu)\n", GetLastError());
        return FALSE;
    }

    if(CreateRemoteThread(process, 0, 0, (LPTHREAD_START_ROUTINE)load_library, dll_addr, 0, 0) == NULL) {
        printf("CreateRemoteThread failure. Error (%lu)\n", GetLastError());
        return FALSE;
    }

    return TRUE;
}


BOOLEAN eject_hook(HWND window, HANDLE process, DWORD mod_addr)
{  
    
    LPVOID free_library = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "FreeLibrary");
    if(free_library == NULL) {
        printf("FreeLibrary failure. Error (%lu)\n", GetLastError());
        return FALSE;
    }

    if(CreateRemoteThread(process, 0, 0, (LPTHREAD_START_ROUTINE)free_library, (LPVOID)mod_addr, 0, 0) == NULL) {
        printf("CreateRemoteThread failure. Error (%lu)\n", GetLastError());
        return FALSE;
    }

    return TRUE;
}


RGBQUAD get_pixel(HWND window, DWORD x, DWORD y)
{
    RECT rc;
    GetWindowRect(window, &rc);
    const DWORD image_size = 1 * 1;
    RGBQUAD pixels[image_size];

    HDC hdc = GetDC(window);

    HDC mem_dc = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, 13, 13);
    SelectObject(mem_dc, bmp);

    BitBlt(mem_dc, 0, 0, 1, 1, hdc, (rc.left + x), (rc.top + y), SRCCOPY);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = 1;
    bmi.bmiHeader.biHeight = 1;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    GetDIBits(mem_dc, bmp, 0, 1, pixels, &bmi, DIB_RGB_COLORS);

    ReleaseDC(window, hdc);
    DeleteDC(mem_dc);
    DeleteObject(bmp);

    return pixels[0];
}


BOOLEAN check_pixel(HWND window, DWORD x, DWORD y, int r, int g, int b)
{
    RGBQUAD pixel = get_pixel(window, x, y);

    if( (pixel.rgbRed >= (r - 13) && pixel.rgbRed <= (r + 13)) &&
        (pixel.rgbGreen >= (g - 13) && pixel.rgbGreen <= (g + 13)) && 
        (pixel.rgbBlue >= (b - 13) && pixel.rgbBlue <= (b + 13)))
        return TRUE;
    else
        return FALSE;
}


BOOLEAN on_forward_time(HWND window)
{
    return check_pixel(window, 100, 80, 13, 118, 235);
}


BOOLEAN on_transfer_dialog(HWND window) 
{
    return check_pixel(window, 520, 313, 252, 252, 252);
}


BOOLEAN on_info_screen(HWND window)
{
    if(check_pixel(window, 313, 75, 35, 63, 100))
        return TRUE;
    else if(check_pixel(window, 313, 75, 28, 68, 109))
        return TRUE;
    else if(check_pixel(window, 640, 120, 3, 163, 173))
        return TRUE;
    else if(check_pixel(window, 640, 120, 3, 143, 152))
        return TRUE;
    else if(check_pixel(window, 656, 163, 3, 163, 173))
        return TRUE;
    else if(check_pixel(window, 200, 120, 6, 23, 60))
        return TRUE;
    else if(check_pixel(window, 333, 166, 3, 45, 90))
        return TRUE;
    else if(check_pixel(window, 650, 205, 3, 163, 173))
        return TRUE;
    else if(check_pixel(window, 172, 43, 29, 70, 115))
        return TRUE;
    else if(check_pixel(window, 170, 80, 29, 70, 115))
        return TRUE;
    else if(check_pixel(window, 256, 82, 3, 61, 114))
        return TRUE;
    else if(check_pixel(window, 715, 223, 237, 237, 237))
        return TRUE;
    else if(check_pixel(window, 573, 148, 45, 96, 180))
        return TRUE;
    else if(check_pixel(window, 190, 122, 19, 36, 73))
        return TRUE;
    else if(check_pixel(window, 812, 453, 216, 222, 223))
        return TRUE;
    else if(check_pixel(window, 100, 84, 9, 70, 143) && 
        !check_pixel(window, 517, 127, 237, 237, 237))
        return TRUE;
    else
        return FALSE;
}

void send_input(HWND window, DWORD windows_msg)
{   
    PostMessage(window, WM_KEYDOWN, windows_msg, 0);
    Sleep(433);
    PostMessage(window, WM_KEYUP, windows_msg, 0);
    Sleep(433);
}

void run_ml_mode(HWND window)
{
    while(!stop_bot) {
        if(on_forward_time(window)) {
            send_input(window, VK_RETURN);
            send_input(window, VK_DOWN);
            send_input(window, VK_RIGHT);
            send_input(window, VK_RETURN);

            if(on_transfer_dialog(window)) {
                send_input(window, VK_RIGHT);
                send_input(window, VK_RETURN);
            }
        }
        else if(on_info_screen(window)) {
            send_input(window, VK_RIGHT);
            send_input(window, VK_RETURN);
        }
    }
}

void run_bal_mode(HWND window)
{
    while(!stop_bot) {
        send_input(window, VK_RIGHT);
        send_input(window, VK_RETURN);
    }
}

void run_sim_mode(HWND window)
{
    HANDLE file_map = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4, "remote_play_map");
    if(file_map == NULL) {
        printf("Could not create file mapping obect. Error: (%lu)\n", GetLastError());
        exit(-1);
    }

    LPVOID buffer = MapViewOfFile(file_map, FILE_MAP_ALL_ACCESS, 0, 0, 4);
    if(buffer == NULL) {
        printf("Could not map view of file (%lu).\n", GetLastError());
        CloseHandle(file_map);
        exit(-1);
    }

    DWORD process_id;
    GetWindowThreadProcessId(window, &process_id);

    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, 0, process_id);
    if(process == NULL) {
        printf("OpenProcess failure. Error (%lu)\n", GetLastError());
        exit(-1);
    }

    if(!inject_hook(window, process)) {
        puts("ReadFile hook injection failed.");
        exit(-1);
    }

    Sleep(500);

    DWORD mod_addr = *(DWORD*)buffer;

    while(!stop_bot)
        continue;
    
    *(DWORD*)buffer = 1;

    while(*(DWORD*)buffer == 1)
        continue;

    if(!eject_hook(window, process, mod_addr)) {
        puts("ReadFile hook ejection failed.");
        exit(-1);
    }

    CloseHandle(process);
    UnmapViewOfFile(buffer);
    CloseHandle(file_map);
}


void run_bot(HWND window, DWORD mode)
{
    if(mode == ML_MODE)
        run_ml_mode(window);
    else if(mode == BAL_MODE)
        run_bal_mode(window);
    else if(mode == SIM_MODE)
        run_sim_mode(window);
}


int main(int argc, char **argv)
{
    if(argc != 2) {
        print_usage();
        return 1;
    }

    DWORD mode;
    switch(djb_hash(argv[1])) {
        case ML_MODE: 
            mode = ML_MODE;
            break;
        case BAL_MODE:
            mode = BAL_MODE;
            break;
        case SIM_MODE:
            mode = SIM_MODE;
            break;
        default:
            print_usage();
            return 1;
    }

    HWND window = FindWindowA(NULL, "PS4 Remote Play");
    if(window == NULL) {
        printf("Remote Play window not found.\n");
        return 1;
    }

    SetWindowPos(window, HWND_NOTOPMOST, 13, 33, 969, 666, SWP_ASYNCWINDOWPOS);
    Sleep(500);

    std::thread bot(run_bot, window, mode);

    printf("Press Enter to stop the bot.\n");
    getchar();
    
    stop_bot = TRUE;
    bot.join();

    return 0;
}