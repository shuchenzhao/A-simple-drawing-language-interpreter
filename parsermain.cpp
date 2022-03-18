#include <stdio.h>

#include "parser.h"
#include "errlog.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Please input file!\n");
		return 1;
	}
	InitError();

	printf("\n����ִ�в��������ļ�: \n"
		"%s�洢�﷨������������ı�\n"
		"%s�洢����Ĵ�����Ϣ\n\n",
		LOGFILE_PATH, ERRFILE_PATH
	);

	Parser(argv[1]);
	return 0;
}