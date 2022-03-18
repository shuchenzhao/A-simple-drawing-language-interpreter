#ifndef _PARSER_H
#define _PARSER_H

#include "scanner.h"

typedef struct ExprNode ExprNode;

struct ExprNode {
	enum Token_Type OpCode;
	union {
		struct { ExprNode* left, * right; }CaseOperator;
		struct { ExprNode* child; t_func MathFuncPtr; }CaseFunc;
		double CaseConst;
		double* CaseParmPtr;
	}content;
};
typedef struct ExprNode* ExprNode_Ptr;

extern void Parser(const char* file_name);

#ifdef _PARSER_WITH_SEMANTICS
extern double* getTmemory();
#endif // _PARSER_WITH_SEMANTICS

#endif // _PARSER_H
