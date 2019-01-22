// ----------------------------------------------------------------
//
// 	FILENAME: pesbot.h       AUTHOR: JG 2019
//
// ----------------------------------------------------------------

#define ML_MODE 5861028
#define BAL_MODE 193415146
#define SIM_MODE 193433778

void print_usage();

static size_t djb_hash(const char *cp);

BOOLEAN inject_hook(HWND window, HANDLE process);

BOOLEAN eject_hook(HWND window, HANDLE process, DWORD mod_addr);

RGBQUAD get_pixel(HWND window, DWORD x, DWORD y);

BOOLEAN check_pixel(HWND window, DWORD x, DWORD y, int r, int g, int b);

BOOLEAN on_forward_time(HWND window);

BOOLEAN on_transfer_dialog(HWND window);

BOOLEAN on_info_screen(HWND window);

void send_input(HWND window, DWORD windows_msg);

void run_ml_mode(HWND window);

void run_bal_mode(HWND window);

void run_sim_mode(HWND window);

void run_bot(HWND window, DWORD mode);
