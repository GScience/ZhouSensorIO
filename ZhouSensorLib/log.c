
// Zhou Sensor Log

#include <Windows.h>
#include <stdarg.h>
#include <stdio.h>
#include "log.h"

void OutputDebugStringfA(const char* format, ...)
{
	char strBuffer[LOG_BUFFER_SIZE] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, format);
	_vsnprintf_s(strBuffer, LOG_BUFFER_SIZE, sizeof(strBuffer) - 1, format, vlArgs);
	va_end(vlArgs);
	OutputDebugStringA(strBuffer);
}

void OutputDebugStringfW(const wchar_t* format, ...)
{
	wchar_t strBuffer[LOG_BUFFER_SIZE] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, format);
	_vsnwprintf_s(strBuffer, LOG_BUFFER_SIZE, sizeof(strBuffer) - 1, format, vlArgs);
	va_end(vlArgs);
	OutputDebugStringW(strBuffer);
}