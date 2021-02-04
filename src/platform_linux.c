// platform_linux.c

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/Xrender.h>

#if RENDERER_OPENGL

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

typedef i32(glSwapInterval_t)(Display* Disp, GLXDrawable Drawable, i32 Interval);

static GLXContext Context;
static glSwapInterval_t* glSwapIntervalEXT;

#endif

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
static double VirtMouseX = 0;
static double VirtMouseY = 0;
static double LastMouseX = 0;
static double LastMouseY = 0;

typedef struct window {
  i32 Width;
  i32 Height;
  framebuffer_size_callback FramebufferSizeCallback;
  Display* Disp;
  i32 Screen;
  i32 Depth;
  Visual* Vis;
  Window Win;
  XVisualInfo* VisualInfo;
  XImage* Image;
  GC Gc;
  Atom AtomWMDelete;
  Cursor WinCursor;

  i32 MouseX;
  i32 MouseY;
  i32 LastWarpedMouseX;
  i32 LastWarpedMouseY;
  cursor_mode CursorMode;
} window;

static window Win;

void PlatformOpenGLInit() {
#if RENDERER_OPENGL
  GLint Attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
  Win.VisualInfo = glXChooseVisual(Win.Disp, 0, Attribs);
  Context = glXCreateContext(Win.Disp, Win.VisualInfo, NULL, GL_TRUE);
  glXMakeCurrent(Win.Disp, Win.Win, Context);

  glSwapIntervalEXT = (glSwapInterval_t*)glXGetProcAddress((u8*)"glXSwapIntervalEXT");
  if (glSwapIntervalEXT) {
    glSwapIntervalEXT(Win.Disp, Win.Win, 0);
  }
  else {
    fprintf(stderr, "Failed to disable vsync\n");
  }
#endif
}

void PlatformWarpCursor(double X, double Y) {
  Win.LastWarpedMouseX = X;
  Win.LastWarpedMouseY = Y;

  XWarpPointer(
      Win.Disp,
      None,
      Win.Win,
      0, 0, 0, 0,
      X, Y
  );

  XFlush(Win.Disp);
}

void PlatformGetCursorPos(double* X, double* Y) {
  if (Win.CursorMode == CURSOR_DISABLED) {
    *X = VirtMouseX;
    *Y = VirtMouseY;
  }
  else {
    *X = Win.MouseX;
    *Y = Win.MouseY;
  }
}

void PlatformSetCursorMode(cursor_mode Mode) {
  Win.CursorMode = Mode;
  if (Win.CursorMode == CURSOR_NORMAL) {
    PlatformEnableCursor();
    PlatformShowCursor();
  }
  else if (Win.CursorMode == CURSOR_DISABLED) {
    PlatformDisableCursor();
    PlatformHideCursor();
  }
}

void PlatformDisableCursor() {
  Window RootWindow;
  u32 Mask;
  LastMouseX = Win.MouseX;
  LastMouseY = Win.MouseY;
  XQueryPointer(Win.Disp, Win.Win, &RootWindow, &RootWindow, &Win.MouseX, &Win.MouseY, &Win.MouseX, &Win.MouseY, &Mask);

  XGrabPointer(Win.Disp, Win.Win, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, None, Win.WinCursor, CurrentTime);
  XWarpPointer(Win.Disp, None, Win.Win, 0, 0, 0, 0, Win.Width / 2, Win.Height / 2);
}

void PlatformEnableCursor() {
  XUngrabPointer(Win.Disp, CurrentTime);
}

void PlatformHideCursor() {
  XDefineCursor(Win.Disp, Win.Win, Win.WinCursor);
}

void PlatformShowCursor() {
  XDefineCursor(Win.Disp, Win.Win, None);
}

void PlatformEnableRawMouseMotion() {
  XIEventMask E;
  u8 Mask[XIMaskLen(XI_RawMotion)] = {0};
  E.deviceid = XIAllMasterDevices;
  E.mask_len = sizeof(Mask);
  E.mask = Mask;
  XISelectEvents(Win.Disp, Win.Win, &E, 1);
}

void PlatformDisableRawMouseMotion() {
  XIEventMask E;
  u8 Mask[] = {0};
  E.deviceid = XIAllMasterDevices;
  E.mask_len = sizeof(Mask);
  E.mask = Mask;
  XISelectEvents(Win.Disp, Win.Win, &E, 1);
}

i32 WindowOpen(i32 Width, i32 Height, const char* Title, u8 Fullscreen) {
  Win.Width = Width;
  Win.Height = Height;
  Win.FramebufferSizeCallback = NULL;

  Win.Disp = XOpenDisplay(0);
  if (!Win.Disp)
    return -1;

  XSetWindowAttributes Attribs = {0};
  Win.Screen = XDefaultScreen(Win.Disp);
  Win.Depth = XDefaultDepth(Win.Disp, Win.Screen);
  Win.Vis = XDefaultVisual(Win.Disp, Win.Screen);
  Window Parent = XRootWindow(Win.Disp, Win.Screen);
  Win.Win = XCreateWindow(
    Win.Disp,
    Parent,
    0,
    0,
    Win.Width,
    Win.Height,
    0,
    Win.Depth,
    InputOutput,
    Win.Vis,
    CWBackPixel | CWBorderPixel | CWBitGravity | CWEventMask | CWColormap,
    &Attribs
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

  XSelectInput(Win.Disp, Win.Win, StructureNotifyMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | FocusChangeMask | EnterWindowMask | LeaveWindowMask);

  if (Fullscreen) {
    Atom Atoms[2] = {
      XInternAtom(Win.Disp, "_NET_WM_STATE_FULLSCREEN", False),
      False
    };

    XChangeProperty(
      Win.Disp,
      Win.Win,
      XInternAtom(Win.Disp, "_NET_WM_STATE", False),
      XA_ATOM,
      32,
      PropModeReplace,
      (u8*)Atoms,
      1
    );
  }

  XStoreName(Win.Disp, Win.Win, Title);
  XMapWindow(Win.Disp, Win.Win);
  XMapRaised(Win.Disp, Win.Win);

#ifndef RENDERER_OPENGL
  Win.Gc = XCreateGC(Win.Disp, Win.Win, 0, NULL);
  if (!Win.Gc) {
    return -1;
  }
  Win.Image = XCreateImage(Win.Disp, NULL, 24, ZPixmap, 0, 0, Win.Width, Win.Height, 32, 0);

  if (!Win.Image) {
    return -1;
  }
#endif

  // NOTE(lucas): To painfully hide that damn cursor, we use a fully transparent pixelmap
  Pixmap CursorBitmap;
  XColor CursorColor = { .red = 0, .green = 0, .blue = 0 };
  static const i8 NoData[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  CursorBitmap = XCreateBitmapFromData(Win.Disp, Win.Win, (const char*)NoData, 8, 8);
  Win.WinCursor = XCreatePixmapCursor(Win.Disp, CursorBitmap, CursorBitmap, &CursorColor, &CursorColor, 0, 0);
  XFreePixmap(Win.Disp, CursorBitmap);

  Win.MouseX = Win.MouseY = 0;
  Win.LastWarpedMouseX = Win.LastWarpedMouseY = 0;
  Win.CursorMode = CURSOR_NORMAL;
  XSync(Win.Disp, False);
  return 0;
}

i32 WindowWidth() {
  return Win.Width;
}

i32 WindowHeight() {
  return Win.Height;
}

void WindowSetFramebufferCallback(framebuffer_size_callback FramebufferSizeCallback) {
  Win.FramebufferSizeCallback = FramebufferSizeCallback;
}

void WindowSwapBuffers(render_state* RenderState) {
#if RENDERER_OPENGL
  (void)RenderState;
  glXSwapBuffers(Win.Disp, Win.Win);
#else
  Win.Image->data = (void*)RenderState->FrameBuffer.Data;
  XPutImage(Win.Disp, Win.Win, Win.Gc, Win.Image, 0, 0, 0, 0, RenderState->FrameBuffer.Width, RenderState->FrameBuffer.Height);
  Win.Image->data = NULL;
  XSync(Win.Disp, False);
#endif
}

i32 WindowEvents() {
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
      case FocusIn: {
        if (Win.CursorMode == CURSOR_DISABLED) {
          PlatformDisableCursor();
        }
        break;
      }
      case EnterNotify: {
        const i32 X = E.xcrossing.x;
        const i32 Y = E.xcrossing.y;
        VirtMouseX = Win.MouseX = X;
        VirtMouseY = Win.MouseY = Y;
        break;
      }
      case MotionNotify: {
        const i32 X = E.xmotion.x;
        const i32 Y = E.xmotion.y;
        if (X != Win.LastWarpedMouseX || Y != Win.LastWarpedMouseY) {
          if (Win.CursorMode == CURSOR_DISABLED) {
            const i32 Dx = X - LastMouseX;
            const i32 Dy = Y - LastMouseY;
            VirtMouseX += Dx;
            VirtMouseY += Dy;
          }
          else {
            Win.MouseX = X;
            Win.MouseY = Y;
          }
        }
        LastMouseX = X;
        LastMouseY = Y;
        break;
      }
      default:
        break;
    }
  }
  if (Win.CursorMode == CURSOR_DISABLED) {
    if (VirtMouseX != (Win.Width / 2.0f) || VirtMouseY != (Win.Height / 2.0f)) {
      PlatformWarpCursor(Win.Width / 2.0f, Win.Height / 2.0f);
    }
  }
  XFlush(Win.Disp);
  return 0;
}

void WindowSetTitle(const char* Title) {
  XStoreName(Win.Disp, Win.Win, Title);
}

void WindowClose() {
  if (Win.WinCursor != None) {
    XFreeCursor(Win.Disp, Win.WinCursor);
  }
  XUnmapWindow(Win.Disp, Win.Win);
  XDestroyWindow(Win.Disp, Win.Win);
  XCloseDisplay(Win.Disp);
}

void WindowDestroyContext() {
#ifndef RENDERER_OPENGL
  XDestroyImage(Win.Image);
  XFreeGC(Win.Disp, Win.Gc);
#endif
}
