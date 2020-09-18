// window.c

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

enum key_codes {
  KEY_SPACE         = XK_space,
  KEY_APOSTROPHE    = XK_apostrophe,
  KEY_COMMA         = XK_comma,
  KEY_MINUS         = XK_minus,
  KEY_PERIOD        = XK_period,
  KEY_SLASH         = XK_slash,

  KEY_0             = XK_0,
  KEY_1             = XK_1,
  KEY_2             = XK_2,
  KEY_3             = XK_3,
  KEY_4             = XK_4,
  KEY_5             = XK_5,
  KEY_6             = XK_6,
  KEY_7             = XK_7,
  KEY_8             = XK_8,
  KEY_9             = XK_9,

  KEY_SEMICOLON     = XK_semicolon,
  KEY_EQUAL         = XK_equal,

  KEY_A             = XK_a,
  KEY_B             = XK_b,
  KEY_C             = XK_c,
  KEY_D             = XK_d,
  KEY_E             = XK_e,
  KEY_F             = XK_f,
  KEY_G             = XK_g,
  KEY_H             = XK_h,
  KEY_I             = XK_i,
  KEY_J             = XK_j,
  KEY_K             = XK_k,
  KEY_L             = XK_l,
  KEY_M             = XK_m,
  KEY_N             = XK_n,
  KEY_O             = XK_o,
  KEY_P             = XK_p,
  KEY_Q             = XK_q,
  KEY_R             = XK_r,
  KEY_S             = XK_s,
  KEY_T             = XK_t,
  KEY_U             = XK_u,
  KEY_V             = XK_v,
  KEY_W             = XK_w,
  KEY_X             = XK_x,
  KEY_Y             = XK_y,
  KEY_Z             = XK_z,

// TODO(lucas): Moarrr key codes!
  MAX_KEY = 255,
};

static i8 KeyDown[MAX_KEY];
static i8 KeyPressed[MAX_KEY];
static i32 MouseX = 0;
static i32 MouseY = 0;

typedef struct window {
  i32 Width;
  i32 Height;
  Display* Disp;
  Window Win;
  XImage* Image;
  GC Gc;
  Atom AtomWMDelete;
} window;

static window Win;

static i32 WindowOpen(i32 Width, i32 Height, const char* Title) {
  Win.Width = Width;
  Win.Height = Height;
  Win.Disp = XOpenDisplay(0);
  if (!Win.Disp)
    return -1;

  Win.Win = XCreateSimpleWindow(
    Win.Disp,
    DefaultRootWindow(Win.Disp),
    0, 0, Width, Height, 0, 0, 0
  );

  if (!Win.Win)
    return -1;

  XSizeHints Hints;
  memset(&Hints, 0, sizeof(Hints));
  Hints.flags = PSize | PMinSize | PMaxSize;
  Hints.min_width = Hints.max_width = Hints.base_width = Width;
  Hints.min_height = Hints.max_height = Hints.base_height = Height;

  XSetWMNormalHints(Win.Disp, Win.Win, &Hints);

  Win.AtomWMDelete = XInternAtom(Win.Disp, "WM_DELETE_WINDOW", True);
  XSetWMProtocols(Win.Disp, Win.Win, &Win.AtomWMDelete, 1);
  XFlush(Win.Disp);

  XSelectInput(Win.Disp, Win.Win, StructureNotifyMask | KeyPressMask | KeyReleaseMask);

  XStoreName(Win.Disp, Win.Win, Title);
  XMapWindow(Win.Disp, Win.Win);

  Win.Gc = XCreateGC(Win.Disp, Win.Win, 0, NULL);

  if (!Win.Gc)
    return -1;

  Win.Image = XCreateImage(Win.Disp, NULL, 24, ZPixmap, 0, 0, Width, Height, 32, 0);

  if (!Win.Image)
    return -1;

  return 0;
}

static void WindowSwapBuffers(framebuffer* FrameBuffer) {
  Win.Image->data = (void*)FrameBuffer->Data;
  XPutImage(Win.Disp, Win.Win, Win.Gc, Win.Image, 0, 0, 0, 0, FrameBuffer->Width, FrameBuffer->Height);
  Win.Image->data = NULL;
}

static i32 WindowEvents() {
  XEvent E;
  for (u8 KeyIndex = 0; KeyIndex < MAX_KEY; ++KeyIndex) {
    KeyPressed[KeyIndex] = 0;
  }

  while (XPending(Win.Disp)) {
    XNextEvent(Win.Disp, &E);
    switch (E.type) {
      case KeyPress: {
        i64 KeyCode = XLookupKeysym(&E.xkey, 0);
        switch (KeyCode) {
          case XK_Escape:
            return -1;
          default: {
            if (KeyCode < MAX_KEY) {
              KeyPressed[KeyCode] = 1;
              KeyDown[KeyCode] = 1;
            }
            break;
          }
        }
        break;
      }
      case KeyRelease: {
        u8 IsHoldingDown = 0;
        if (XEventsQueued(Win.Disp, QueuedAfterReading)) {
          XEvent NewEvent;
          XPeekEvent(Win.Disp, &NewEvent);
          if (NewEvent.type == KeyPress && NewEvent.xkey.time == E.xkey.time && NewEvent.xkey.keycode == E.xkey.keycode) {
            XNextEvent(Win.Disp, &E);
            IsHoldingDown = 1;
          }
        }
        if (!IsHoldingDown) {
          i64 KeyCode = XLookupKeysym(&E.xkey, 0);
          if (KeyCode < MAX_KEY) {
            KeyDown[KeyCode] = 0;
          }
        }
        break;
      }
      default:
        break;
    }
  }
  Window RootWindow;
  u32 Mask;
  XQueryPointer(Win.Disp, Win.Win, &RootWindow, &RootWindow, &MouseX, &MouseY, &MouseX, &MouseY, &Mask);
  return 0;
}

static void WindowSetTitle(const char* Title) {
  XStoreName(Win.Disp, Win.Win, Title);
}

static void WindowClose() {
  XDestroyImage(Win.Image);
  XFreeGC(Win.Disp, Win.Gc);
  XDestroyWindow(Win.Disp, Win.Win);
  XCloseDisplay(Win.Disp);
}
