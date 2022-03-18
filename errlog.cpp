#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "errlog.h"
#include "ui.h"

static FILE* fpLog = NULL;
static FILE* fpErr = NULL;

void CloseError() {
	if (NULL != fpLog) fclose(fpLog); fpLog = NULL;
	if (NULL != fpErr) fclose(fpErr); fpErr = NULL;
}

void InitError() {
	fpLog = fopen(LOGFILE_PATH, "w");
	fpErr = fopen(ERRFILE_PATH, "w");
	atexit(CloseError);
}

void error_msg(int line, const char* descrip, const char* string) {
	FILE* fp = (fpErr == NULL) ? stderr : fpErr;
	char msg[1025];
	snprintf(msg, sizeof(msg), "Error in line %d:%s %s\n", line, descrip, string);

	if (fp != stderr) fprintf(fp, msg);

	ShowMessage(1, msg);
}

void logPrint(const char* format, ...) {
	va_list args;
	FILE* fp = (fpLog == NULL) ? stdout : fpLog;

	va_start(args, format);
	vfprintf(fp, format, args);
	va_end(args);
}