#pragma once

#define LOG_BUFFER_SIZE		1024

#define LOGW(format, ...) OutputDebugStringfW(format, __VA_ARGS__)
#define LOGA(format, ...) OutputDebugStringfA(format, __VA_ARGS__)

#ifdef UNICODE
#define LOG(format, ...) LOGW(format, __VA_ARGS__)
#else
#define LOG(format, ...) LOGA(format, __VA_ARGS__)
#endif

void OutputDebugStringfA(const char* format, ...);
void OutputDebugStringfW(const wchar_t* format, ...);