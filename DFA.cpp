#define NULL０
#include "Scanner.h"

//PART I 状态转移及定义

#define CK_CHAR (0<<16)
#define CK_LETTER (1U<<16)
#define CK_DIGIT (2U<<16)
#define CK_NULL (0x80U<<16)

typedef unsigned int t_key;
struct t_state_trans {
	/*定义一个状态转移，包括
	*起始状态-矩阵行下标
	*字符-矩阵列下标
	*目标状态-矩阵单元格值
	*/
	t_key idx;
	/*move(s,ch)参数合成的查询关键字：
	* 第1字节：行下标
	* 第2字节：字符种类
	* 第3-4字节：字符值
	*/
	int state_to;//目标状态
};

#define MK_IDX(from,c) ((t_key)(from<<24)|(c))
#define MOVE(from,c,to) {MK_IDX(from,c),to}
#define TRANS_END MK_IDX(255,CK_NULL)

struct t_state_trans myTransfers[] = {
	MOVE(0, CK_LETTER, 1),
	MOVE(0, CK_DIGIT, 2),
	MOVE(0, '*', 4),
	MOVE(0, '/', 6),
	MOVE(0, '-', 7),
	MOVE(0, '+', 8),
	MOVE(0, ',', 9),
	MOVE(0, ';', 10),
	MOVE(0, '(', 11),
	MOVE(0, ')', 12),
	MOVE(1, CK_LETTER, 1),
	MOVE(1, CK_DIGIT, 1),
	MOVE(2, CK_DIGIT, 2),
	MOVE(2, '.', 3),
	MOVE(3, CK_DIGIT, 3) ,
	MOVE(4,'*', 5),
	MOVE(6,'/', 13),
	MOVE(7, '-', 13),

	{TRANS_END,	255}
};

//PART II 终态及定义

struct t_final_state {
	int state;
	Token_Type kind;
};

struct t_final_state myFinalStates[] = {
	{1,ID},
	{2,CONST_ID},
	{3,CONST_ID},

	{4,MUL},
	{5,POWER},
	{6,DIV},
	{7,MINUS},

	{8,PLUS},
	{9,COMMA},
	{10,SEMICO},
	{11,L_BRACKET},
	{12,R_BRACKET},

	{13,COMMENT},

	{-1,ERRTOKEN}
};

//PART III DFA定义

struct DFA_defination {
	int start_state;
	struct t_final_state* final_state;
	struct t_state_trans* transfers;
};

struct DFA_defination myDFA = {
	0,myFinalStates,myTransfers
};

//PART IV DFA为记号识别提供的接口

int get_start_state() {
	return myDFA.start_state;
}

enum Token_Type state_is_final(int state) {
	struct t_final_state* p = myDFA.final_state;
	for (; p->state > 0; ++p) {
		if (p->state == state)
			return p->kind;
	}
	return ERRTOKEN;
}

int move(int state_src, char ch) {
	int ck_of_ch = CK_CHAR;
	if ('0' <= ch && ch <= '9') {
		ck_of_ch = CK_DIGIT;
		ch = 0;
	}
	else if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z')) {
		ck_of_ch = CK_LETTER;
		ch = 0;
	}
	else ck_of_ch = CK_CHAR;

	t_key key = MK_IDX(state_src, ck_of_ch | ch);

	struct t_state_trans* pTransfer = myDFA.transfers;
	for (; pTransfer->idx != TRANS_END; ++pTransfer) {
		if (pTransfer->idx == key)
			return pTransfer->state_to;
	}
	return -1;
}