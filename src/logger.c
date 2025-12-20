#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

void
log_info(const char *filename, const char *format, ...)
{
#ifdef DEBUG
	va_list args;
	va_start(args, format);
	printf("\033[0m[%s] ", filename);
	vprintf(format, args);
	printf("\n");
	va_end(args);
#endif
}
void
log_err(const char *filename, const char *format, ...)
{
#ifdef DEBUG
	va_list args;
	va_start(args, format);
	printf("\033[31m[%s] ", filename);
	vprintf(format, args);
	printf("\n");
	va_end(args);
#endif
}