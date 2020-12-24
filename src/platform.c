// platform.c

#if __linux__

#include "platform_linux.c"

#endif

#if __APPLE__ || __MACH__

#include "platform_macos.h"

#endif

#if _WIN32 || _WIN64

#error "Windows is not supported"

#endif

#include "renderer.c"
