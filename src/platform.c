// platform.c

#if __linux__

#include "platform_linux.c"

#endif

#if __APPLE__ || __MACH__

#error "Mac OSX is not supported"

#endif

#if _WIN32 || _WIN64

#error "Windows is not supported"

#endif

#include "renderer.c"
