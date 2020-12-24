// platform_macos.m

#include <Cocoa/Cocoa.h>
#include <mach-o/dyld.h>
#include <mach/mach_time.h>

#if RENDERER_OPENGL

#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#endif

#include "types.h"
#include "platform.h"
#include "platform_macos.h"

static struct window Win;

struct render_state;

static NSAutoreleasePool* g_autoreleasepool = NULL;

void PlatformOpenGLInit() {
#if RENDERER_OPENGL
#endif
}

i32 WindowOpen(i32 Width, i32 Height, const char* Title, u8 Fullscreen) {
	return 0;
}

i32 WindowWidth() {
  return Win.Width;
}

i32 WindowHeight() {
  return Win.Height;
}

void WindowFocus() {
}

void WindowSwapBuffers(struct render_state* RenderState) {
}

int WindowEvents() {
	return 0;
}

void WindowSetTitle(const char* Title) {
}

void WindowClose() {
}

void WindowDestroyContext() {
}
