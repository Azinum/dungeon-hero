// platform_macos.h

#ifndef _PLATFORM_MACOS_H
#define _PLATFORM_MACOS_H

struct window {
	int Width;
	int Height;
	void* Win;  // Of type NSWindow
} window;

static struct window Win;

enum key_codes {
  KEY_A = 0,
  KEY_W = 0,
  KEY_S = 0,
  KEY_D = 0,
  KEY_Z = 0,
  KEY_X = 0,
  KEY_R = 0,
  KEY_H = 0,
  KEY_J = 0,
  KEY_K = 0,
  KEY_L = 0,
  KEY_COMMA = 0,
  KEY_DOT = 0,
  MAX_KEY,
};

static i8 KeyDown[MAX_KEY];
static i8 KeyPressed[MAX_KEY];
static i32 MouseX = 0;
static i32 MouseY = 0;

#endif
