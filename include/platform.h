// platform.h

struct render_state;

static void PlatformOpenGLInit();

static i32 WindowOpen(i32 Width, i32 Height, const char* Title);

static void WindowSwapBuffers(struct render_state* RenderState);
