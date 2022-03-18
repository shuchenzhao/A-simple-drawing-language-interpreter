#ifndef SCANNER_H
#define SCANNER_H

enum Token_Type {
	COMMENT,
	ID,

	ORIGIN, SCALE, ROT, IS, TO,
	STEP, DRAW, FOR, FROM,
	T,

	SEMICO, L_BRACKET, R_BRACKET, COMMA,
	PLUS, MINUS, MUL, DIV, POWER,
	FUNC,
	CONST_ID,

	NONTOKEN,
	ERRTOKEN
};

typedef enum Token_Type Token_Type;

#define TOKEN_LEN_MAX 20

struct position {
	unsigned int line;
	unsigned int col;
};

typedef double (*t_func)(double);

struct Token {
	Token_Type type;
	char lexeme[TOKEN_LEN_MAX + 1];
	double value;
	t_func FuncPtr;
	struct position where;
};

//给Parser的接口
int InitScanner(const char* fileName);
struct Token GetToken();
void CloseScanner();

#endif // !SCANNER_H

