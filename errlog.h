#ifndef _ERRLOG_H
#define _ERRLOG_H

#define LOGFILE_PATH "parser.log"
#define ERRFILE_PATH "error.log"

void InitError();
void CloseError();
void error_msg(int line, const char* descrip, const char* string);
void logPrint(const char* format, ...);

#endif // !_ERRLOG_H



