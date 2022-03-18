#include <stdio.h>

#include "parser.h"
#include "errlog.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Please input file!\n");
		return 1;
	}
	InitError();

	printf("\n本次执行产生两个文件: \n"
		"%s存储语法分析器的输出文本\n"
		"%s存储输出的错误信息\n\n",
		LOGFILE_PATH, ERRFILE_PATH
	);

	Parser(argv[1]);
	return 0;
}