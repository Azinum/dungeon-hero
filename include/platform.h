// platform.h

typedef enum cursor_mode {
  CURSOR_NORMAL = 0,  // Normal visible cursor that is confined to the screen
  CURSOR_DISABLED,    // Hides and grabs cursor, uses virtual cursor positions
} cursor_mode;

typedef void (*framebuffer_size_callback)(i32 Width, i32 Height);

struct render_state;

void PlatformOpenGLInit();

void PlatformWarpCursor(double X, double Y);

void PlatformGetCursorPos(double* X, double* Y);

void PlatformSetCursorMode(cursor_mode Mode);

void PlatformDisableCursor();

void PlatformEnableCursor();

void PlatformHideCursor();

void PlatformShowCursor();

void PlatformEnableRawMouseMotion();

void PlatformDisableRawMouseMotion();

i32 WindowOpen(i32 Width, i32 Height, const char* Title, u8 Fullscreen);

i32 WindowWidth();

i32 WindowHeight();

void WindowSwapBuffers(struct render_state* RenderState);

void WindowSetTitle(const char* Title);

void WindowClose();

void WindowDestroyContext();
