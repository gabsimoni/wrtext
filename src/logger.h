/*!
	@file logger.h
	@brief logger.h and logger.c contain the code for logging messages on the console.
	These can include errors or just debug information. When the application is compiled in release mode
	these messages will not be printed
*/
#pragma once
#include <stdarg.h>

void log_info(const char* filename,const char *format,...);
void log_err(const char* filename,const char *format,...);