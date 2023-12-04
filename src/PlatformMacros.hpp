
#pragma once

#define OS_GNULINUX 1
#define OS_WINDOWS  2
#define OS_MACOS    3

#if defined(__gnu_linux__)
#   define PLATFORM_NAME OS_GNULINUX
#elif defined(_WIN64)
#   define PLATFORM_NAME OS_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__) && defined(TARGET_OS_MAC)
#   define PLATFORM_NAME OS_MACOS
#else
#   error "Unsupported platform"
#endif
