#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "errlog.h"
#include "parser.h"

#ifdef _PARSER_WITH_SEMANTICS
#include "semantics.h"
#endif

void program();
void statement();
void for_statement();
void origin_statement();
void rot_statement();
void scale_statement();
ExprNode_Ptr expression();
ExprNode_Ptr term();
ExprNode_Ptr factor();
ExprNode_Ptr component();
ExprNode_Ptr atom();

int InitParser(const char* file_name);
void CloseParser();
void destroy_all_tree();

void FetchToken();
void MatchToken(enum Token_Type expected);
void SyntaxError(int case_of);
void PrintSyntaxTree(ExprNode_Ptr root, int indent);
ExprNode_Ptr MakeExprNode(enum Token_Type opcode, ...);

void enter(char* x);
void back(char* x);
void call_match(char* x);
void tree_trace(ExprNode_Ptr x);

struct t_parser_stuff {
	int	indent;
	ExprNode_Ptr start_ptr,
		end_ptr,
		step_ptr,
		x_ptr,
		y_ptr,
		angle_ptr;
	struct Token curr_token;

	double parameter;
};

static struct t_parser_stuff parser_data;
#define current_token parser_data.curr_token

#ifdef _PARSER_WITH_SEMANTICS
double* getTmemory() { return &(parser_data.parameter); }
#endif

void Parser(const char* file_name) {
	if (InitParser(file_name) == 0)	return;

	FetchToken();
	program();

	CloseParser();
	return;
}

void program() {
	enter("program");
	while (current_token.type != NONTOKEN) {
		statement();
		MatchToken(SEMICO);
	}
	back("program");
}

void statement() {
	enter("statement");
	switch (current_token.type) {
	case ORIGIN: origin_statement(); break;
	case SCALE:	scale_statement(); break;
	case ROT: rot_statement(); break;
	case FOR: for_statement(); break;
	default: SyntaxError(2);
	}
	destroy_all_tree();
	back("statement");
}

void origin_statement() {
	enter("origin_statement");

	MatchToken(ORIGIN);
	MatchToken(IS);
	MatchToken(L_BRACKET);
	parser_data.x_ptr = expression();
	tree_trace(parser_data.x_ptr);
	MatchToken(COMMA);
	parser_data.y_ptr = expression();
	tree_trace(parser_data.y_ptr);
	MatchToken(R_BRACKET);

#ifdef _PARSER_WITH_SEMANTICS
	setOrigin(parser_data.x_ptr, parser_data.y_ptr);
#endif

	back("origin_statement");
}

void scale_statement() {
	enter("scale_statement");

	MatchToken(SCALE);
	MatchToken(IS);
	MatchToken(L_BRACKET);
	parser_data.x_ptr = expression();
	tree_trace(parser_data.x_ptr);
	MatchToken(COMMA);
	parser_data.y_ptr = expression();
	tree_trace(parser_data.y_ptr);
	MatchToken(R_BRACKET);

#ifdef _PARSER_WITH_SEMANTICS 
	setScale(parser_data.x_ptr, parser_data.y_ptr);
#endif

	back("scale_statement");
}

void rot_statement() {
	enter("rot_statement");

	MatchToken(ROT);
	MatchToken(IS);
	parser_data.angle_ptr = expression();
	tree_trace(parser_data.angle_ptr);

#ifdef _PARSER_WITH_SEMANTICS
	setRotate(parser_data.angle_ptr);
#endif

	back("rot_statement");
}

void for_statement() {
	enter("for_statement");

	MatchToken(FOR);
	MatchToken(T);
	MatchToken(FROM);
	parser_data.start_ptr = expression();
	tree_trace(parser_data.start_ptr);
	MatchToken(TO);
	parser_data.end_ptr = expression();
	tree_trace(parser_data.end_ptr);
	MatchToken(STEP);
	parser_data.step_ptr = expression();
	tree_trace(parser_data.step_ptr);
	MatchToken(DRAW);
	MatchToken(L_BRACKET);
	parser_data.x_ptr = expression();
	tree_trace(parser_data.x_ptr);
	MatchToken(COMMA);
	parser_data.y_ptr = expression();
	tree_trace(parser_data.y_ptr);
	MatchToken(R_BRACKET);

#ifdef _PARSER_WITH_SEMANTICS 
	DrawLoop(parser_data.start_ptr, parser_data.end_ptr,
		parser_data.step_ptr,
		parser_data.x_ptr, parser_data.y_ptr);
#endif

	back("for_statement");
}

ExprNode_Ptr expression() {
	ExprNode_Ptr left, right;
	Token_Type lastType;

	enter("expression");
	left = term();
	while (current_token.type == PLUS || current_token.type == MINUS) {
		lastType = current_token.type;
		MatchToken(lastType);
		right = term();
		left = MakeExprNode(lastType, left, right);
	}

	back("expression");
	return left;
}

ExprNode_Ptr term() {
	ExprNode_Ptr left, right;
	Token_Type lastType;

	left = factor();
	while (current_token.type == MUL || current_token.type == DIV) {
		lastType = current_token.type;
		MatchToken(lastType);
		right = factor();
		left = MakeExprNode(lastType, left, right);
	}
	return left;
}

ExprNode_Ptr factor() {
	ExprNode_Ptr left, right;

	if (current_token.type == PLUS) {
		MatchToken(PLUS);
		right = factor();
	}
	else if (current_token.type == MINUS) {
		MatchToken(MINUS);
		right = factor();
		left = (ExprNode_Ptr)malloc(sizeof(ExprNode));
		left->OpCode = CONST_ID;
		left->content.CaseConst = 0.0;
		right = MakeExprNode(MINUS, left, right);
	}
	else right = component();
	return right;
}

ExprNode_Ptr component() {
	ExprNode_Ptr left, right;

	left = atom();
	if (current_token.type == POWER) {
		MatchToken(POWER);
		right = component();
		left = MakeExprNode(POWER, left, right);
	}
	return left;
}

ExprNode_Ptr atom() {
	struct Token t = current_token;
	ExprNode_Ptr ptr = nullptr, tmp;

	switch (current_token.type) {
	case CONST_ID:
		MatchToken(CONST_ID);
		ptr = MakeExprNode(CONST_ID, t.value);
		break;
	case T:
		MatchToken(T);
		ptr = MakeExprNode(T);
		break;
	case FUNC:
		MatchToken(FUNC);
		MatchToken(L_BRACKET);
		tmp = expression();
		ptr = MakeExprNode(FUNC, t.FuncPtr, tmp);
		MatchToken(R_BRACKET);
		break;
	case L_BRACKET:
		MatchToken(L_BRACKET);
		ptr = expression();
		MatchToken(R_BRACKET);
		break;
	default:
		SyntaxError(2);
	}
	return ptr;
}

void destroy_tree(ExprNode_Ptr root) {
	if (NULL == root) return;
	switch (root->OpCode) {
	case PLUS:
	case MINUS:
	case MUL:
	case DIV:
	case POWER:
		destroy_tree(root->content.CaseOperator.left);
		destroy_tree(root->content.CaseOperator.right);
		break;
	case FUNC:
		destroy_tree(root->content.CaseFunc.child);
		break;
	default:
		break;
	}

	free(root);
	return;
}

void destroy_all_tree() {
	destroy_tree(parser_data.start_ptr); parser_data.start_ptr = NULL;
	destroy_tree(parser_data.end_ptr); parser_data.end_ptr = NULL;
	destroy_tree(parser_data.step_ptr); parser_data.step_ptr = NULL;
	destroy_tree(parser_data.x_ptr); parser_data.x_ptr = NULL;
	destroy_tree(parser_data.y_ptr); parser_data.y_ptr = NULL;
	destroy_tree(parser_data.angle_ptr); parser_data.angle_ptr = NULL;
}

void CloseParser() {
	CloseScanner();
	destroy_all_tree();
}

int InitParser(const char* file_name) {
	parser_data.indent = 0;
	parser_data.parameter = 0;
	parser_data.start_ptr = NULL;
	parser_data.end_ptr = NULL;
	parser_data.step_ptr = NULL;
	parser_data.x_ptr = NULL;
	parser_data.y_ptr = NULL;
	parser_data.angle_ptr = NULL;

	atexit(CloseParser);

	if (InitScanner(file_name) == 0) {
		logPrint("打开文件[%s]失败!\n", file_name);
		return 0;
	}

	logPrint("分析文件[%s]...\n", file_name);
	return 1;
}

void FetchToken() {
	current_token = GetToken();
	if (current_token.type == ERRTOKEN)	SyntaxError(1);
}

void MatchToken(enum Token_Type expected) {
	if (current_token.type != expected) SyntaxError(2);
	else {
		int i;
		for (i = 0; i < parser_data.indent; i++) logPrint(" ");
		logPrint("matchtoken %s\n", current_token.lexeme);
	}
	FetchToken();
}

void SyntaxError(int case_of) {
	switch (case_of) {
	case 1:
		error_msg(current_token.where.line, "非法单词", current_token.lexeme);
		break;
	case 2:
		error_msg(current_token.where.line, current_token.lexeme, "不是预期记号");
		break;
	}
	exit(1);
}

void PrintSyntaxTree(ExprNode_Ptr root, int indent) {
	int L;

	if (NULL == root) return;
	L = indent + parser_data.indent;

	for (; L > 0; --L) logPrint(" ");
	switch (root->OpCode) {
	case PLUS:
		logPrint("+\n"); break;
	case MINUS:
		logPrint("-\n"); break;
	case MUL:
		logPrint("*\n"); break;
	case DIV:
		logPrint("/\n"); break;
	case POWER:
		logPrint("**\n"); break;
	case T:
		logPrint("T\n");
		return;
	case FUNC:
		logPrint("%p\n", root->content.CaseFunc.MathFuncPtr);
		PrintSyntaxTree(root->content.CaseFunc.child, indent + 2);
		return;
	case CONST_ID:
		logPrint("%1f\n", root->content.CaseConst);
		return;
	default:
		logPrint("非法的树节点\n");
		return;
	}

	PrintSyntaxTree(root->content.CaseOperator.left, indent + 2);
	PrintSyntaxTree(root->content.CaseOperator.right, indent + 2);
	return;
}

ExprNode_Ptr MakeExprNode(enum Token_Type opcode, ...) {
	ExprNode_Ptr ptr = (ExprNode_Ptr)malloc(sizeof(ExprNode));
	ptr->OpCode = opcode;
	va_list arg_ptr;
	va_start(arg_ptr, opcode);
	switch (opcode) {
	case CONST_ID:
		ptr->content.CaseConst = (double)va_arg(arg_ptr, double);
		break;
	case T:
		ptr->content.CaseParmPtr = &(parser_data.parameter);
		break;
	case FUNC:
		ptr->content.CaseFunc.MathFuncPtr = (t_func)va_arg(arg_ptr, t_func);
		ptr->content.CaseFunc.child = (ExprNode_Ptr)va_arg(arg_ptr, ExprNode_Ptr);
		break;
	default:
		ptr->content.CaseOperator.left = (ExprNode_Ptr)va_arg(arg_ptr, ExprNode_Ptr);
		ptr->content.CaseOperator.right = (ExprNode_Ptr)va_arg(arg_ptr, ExprNode_Ptr);
		break;
	}
	va_end(arg_ptr);
	return ptr;
}

void enter(char* x) {
	int i;
	for (i = 0; i < parser_data.indent; ++i) logPrint(" ");
	logPrint("enter in %s\n", x);
	parser_data.indent += 2;
}

void back(char* x) {
	int i;
	parser_data.indent -= 2;
	for (i = 0; i < parser_data.indent; ++i) logPrint(" ");
	logPrint("exit from %s\n", x);
}

void tree_trace(ExprNode_Ptr x) {
	/*
	int L = parser_data.indent;
	for (; L > 0; --L) 1ogPrint(" ");
	logPrint("TREE:\n");
	*/
	PrintSyntaxTree(x, 0);
}

//#endif //_PARSER_WITH_SEMANTICS