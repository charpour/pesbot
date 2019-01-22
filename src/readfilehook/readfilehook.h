// ----------------------------------------------------------------
//
// 	FILENAME: readfilehook.h	AUTHOR: JG 2019
//
// ----------------------------------------------------------------

#pragma once

// necessary DS4 input codes
// Documented here http://www.psdevwiki.com/ps4/DS4-USB#Report_Structure
#define L1 0x1
#define SQUARE 0x18
#define CROSS 0x28
#define CIRCLE 0x48
#define TRIANGLE 0x88
#define OPTIONS 0x20
#define DPAD_RIGHT 0x2
#define DPAD_LEFT 0x6
#define DPAD_UP 0x0
#define DPAD_DOWN 0x4

// screen ids
typedef enum _SCREEN
{
	KICKOFF,
	HALFTIME,
	HIGHLIGHTS,
	FULLTIME,
	EXP,
	LEVEL_UP,
	RATING_CHANGE,
	GP_REWARDS,
	SCOUT_REWARDS,
	MANAGER, 
	RENEW_NO,
	RENEW_YES,
	STATUS_CHECK,
	ARE_YOU_SURE,
	YES_SURE,
	HAS_RENEWED,
	DAILYBONUS,
	MENU,
	UNKNOWN
}
SCREEN;

// Average RGB values
typedef struct _AVR_RGB
{
	int r, g, b;
}
AVR_RGB;

int get_screen();

void send_input(DWORD windows_msg);

BOOLEAN blue_button(int x, int y);

BOOLEAN on_squad_man();

BOOLEAN on_matchmaking();

BOOLEAN on_match_data();

BOOLEAN on_sim_panel();

BOOLEAN on_match_tab();

void press_button(BYTE button, BYTE *buffer);

// returns the average RGB values for a given RGB array
AVR_RGB get_average_rgb(RGBQUAD pixels[], int image_size);

// checks average RGB values of rectangle pixel snapshot returned by get_pixels()
BOOLEAN check_pixels(DWORD x, DWORD y, int width, int height, int r, int g, int b);

// checks pixel at xy coord against given RGB values
BOOLEAN check_pixel(DWORD x, DWORD y, int r, int g, int b);

// gets rectangle pixel snapshot from given xy(upper left) coords
RGBQUAD *get_pixels(DWORD x, DWORD y, int width, int height);

// restores original ReadFile jump
void restore_jmp();

// allocates memory for trampoline and patches jump to ReadFile
void setup_hook();
