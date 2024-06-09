#ifndef _SYSTEM_CONSOLE_H_
#define _SYSTEM_CONSOLE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
/* KEYBOARD.C */
void keyboard_install();
#define KEYBUF_SIZE 128

/* CONSOLE.C */
int32_t is_control_pressed();
int kbhit();
#define KEYBUF_SIZE 128
#define VK_NONE 0xFF
#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_RETURN 0x0D
#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_PAUSE 0x13
#define VK_CAPITAL 0x14
#define VK_SPACE 0x20

#define VK_ESCAPE 0x1B
#define VK_END 0x23
#define VK_HOME 0x24
#define VK_LEFT 128
#define VK_UP 130
#define VK_RIGHT 129
#define VK_DOWN 140
#define VK_PRINT 0x2A
#define VK_INSERT 0x2D
#define VK_DELETE 0x2E

#define VK_KEY_0 0x30
#define VK_KEY_1 0x31
#define VK_KEY_2 0x32
#define VK_KEY_3 0x33
#define VK_KEY_4 0x34
#define VK_KEY_5 0x35
#define VK_KEY_6 0x36
#define VK_KEY_7 0x37
#define VK_KEY_8 0x38
#define VK_KEY_9 0x39

#define VK_KEY_A 0x41
#define VK_KEY_B 0x42
#define VK_KEY_C 0x43
#define VK_KEY_D 0x44
#define VK_KEY_E 0x45
#define VK_KEY_F 0x46
#define VK_KEY_G 0x47
#define VK_KEY_H 0x48
#define VK_KEY_I 0x49
#define VK_KEY_J 0x4A
#define VK_KEY_K 0x4B
#define VK_KEY_L 0x4C
#define VK_KEY_M 0x4D
#define VK_KEY_N 0x4E
#define VK_KEY_O 0x4F
#define VK_KEY_P 0x50
#define VK_KEY_Q 0x51
#define VK_KEY_R 0x52
#define VK_KEY_S 0x53
#define VK_KEY_T 0x54
#define VK_KEY_U 0x55
#define VK_KEY_V 0x56
#define VK_KEY_W 0x57
#define VK_KEY_X 0x58
#define VK_KEY_Y 0x59
#define VK_KEY_Z 0x5A

#define VK_SLEEP 0x5F

#define VK_NUMPAD0 0x60
#define VK_NUMPAD1 0x61
#define VK_NUMPAD2 0x62
#define VK_NUMPAD3 0x63
#define VK_NUMPAD4 0x64
#define VK_NUMPAD5 0x65
#define VK_NUMPAD6 0x66
#define VK_NUMPAD7 0x67
#define VK_NUMPAD8 0x68
#define VK_NUMPAD9 0x69

#define VK_MULTIPLY 0x6A
#define VK_ADD 0x6B

#define VK_F1 141
#define VK_F2 142
#define VK_F3 143
#define VK_F4 144
#define VK_F5 145
#define VK_F6 146
#define VK_F7 147
#define VK_F8 148
#define VK_F9 149
#define VK_F10 0x79
#define VK_F11 0x7A
#define VK_F12 0x7B

#define VGA_BLACK 0x00
#define VGA_BLUE 0x01
#define VGA_GREEN 0x02
#define VGA_CYAN 0x03
#define VGA_RED 0x04
#define VGA_MAGENTA 0x05
#define VGA_BROWN 0x06
#define VGA_LIGHTGREY 0x07
#define VGA_DARKGREY 0x08
#define VGA_LIGHTBLUE 0x09
#define VGA_LIGHTGREEN 0x0A
#define VGA_LIGHTCYAN 0x0B
#define VGA_LIGHTRED 0x0C
#define VGA_LIGHTMAGENTA 0x0D
#define VGA_YELLOW 0x0E
#define VGA_WHITE 0x0F
/* SCRN.C */
void init_video(void);
void cls();
void putch_xy(int x, int y, unsigned char c);
void puts_xy(int x, int y, unsigned char *text);
void disable_cursor();
void enable_cursor();
void gotoxy(int x, int y);
void getxy(int *x, int *y);
void set_text_colour(unsigned char forecolour, unsigned char backcolour);
uint8_t get_foreground_colour();
uint8_t get_background_colour();
void set_background_colour(unsigned char colour);
void set_foreground_colour(unsigned char colour);
int get_cursor_x();
int get_cursor_y();
unsigned short *video_mem_ptr();
void get_screen_size(int *x, int *y);
unsigned short vid_read_charattrib(int x, int y);
char vid_read_char(int x, int y);
unsigned char vid_read_attrib(int x, int y);

#ifdef __cplusplus
}
#endif

#endif