// platform.h

typedef void (*framebuffer_size_callback)(i32 Width, i32 Height);

struct render_state;

static void PlatformOpenGLInit();

static i32 WindowOpen(i32 Width, i32 Height, const char* Title, u8 Fullscreen);

static void WindowSetFramebufferCallback(framebuffer_size_callback FramebufferSizeCallback);

static void WindowSwapBuffers(struct render_state* RenderState);

static void WindowSetTitle(const char* Title);

static void WindowClose();

static void WindowDestroyContext();
