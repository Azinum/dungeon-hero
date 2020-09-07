// window.c

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct window {
  Display* Disp;
  Window Win;
  XImage* Image;
  GC Gc;
  Atom AtomWMDelete;
} window;

static window Win;

static i32 WindowOpen(u32 Width, u32 Height, const char* Title) {
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
  
  if (XPending(Win.Disp)) {
    XNextEvent(Win.Disp, &E);
    switch (E.type) {
      case KeyPress: {
        switch (E.xkey.keycode) {
          case 9: { // TEMP!
            return -1;
          }
        }
        break;
      }
    }
  }
  return 0;
}

static void WindowClose() {
  XDestroyImage(Win.Image);
  XFreeGC(Win.Disp, Win.Gc);
  XDestroyWindow(Win.Disp, Win.Win);
  XCloseDisplay(Win.Disp);
}
