// platform.h

typedef enum cursor_mode {
  CURSOR_NORMAL = 0,
  CURSOR_DISABLED,
} cursor_mode;

typedef void (*framebuffer_size_callback)(i32 Width, i32 Height);

struct render_state;

void PlatformOpenGLInit();

void PlatformWarpCursor(double X, double Y);

void PlatformGetCursorPos(double* X, double* Y);

void PlatformSetCursorMode(cursor_mode Mode);

void PlatformDisableCursor();

i32 WindowOpen(i32 Width, i32 Height, const char* Title, u8 Fullscreen);

i32 WindowWidth();

i32 WindowHeight();

void WindowFocus();

void WindowSwapBuffers(struct render_state* RenderState);

void WindowSetTitle(const char* Title);

void WindowClose();

void WindowDestroyContext();
