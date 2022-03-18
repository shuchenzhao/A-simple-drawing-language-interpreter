#include <stdlib.h>
#include <math.h>

#include "semantics.h"
#include "ui.h"

double
Origin_x = 0, Origin_y = 0,
Scale_x = 1, Scale_y = 1,
rot_angle = 0;

double GetExprValue(ExprNode_Ptr root) {
	if (root == NULL) return 0.0;
	switch (root->OpCode) {
	case PLUS:
		return GetExprValue(root->content.CaseOperator.left) + GetExprValue(root->content.CaseOperator.right);
	case MINUS:
		return GetExprValue(root->content.CaseOperator.left) - GetExprValue(root->content.CaseOperator.right);
	case MUL:
		return GetExprValue(root->content.CaseOperator.left) * GetExprValue(root->content.CaseOperator.right);
	case DIV:
		return GetExprValue(root->content.CaseOperator.left) / GetExprValue(root->content.CaseOperator.right);
	case POWER:
		return pow(GetExprValue(root->content.CaseOperator.left), GetExprValue(root->content.CaseOperator.right));
	case FUNC:
		return (*(root->content.CaseFunc.MathFuncPtr)) (GetExprValue(root->content.CaseFunc.child));
	case CONST_ID:
		return root->content.CaseConst;
	case T:
		return *(root->content.CaseParmPtr);
	default:
		return 0.0;
	}
}

void CalcCoord(ExprNode_Ptr x_tree, ExprNode_Ptr y_tree, double* ptr_x_value, double* ptr_y_value) {
	double x_val, x_temp, y_val;

	x_val = GetExprValue(x_tree);
	y_val = GetExprValue(y_tree);

	x_val *= Scale_x;
	y_val *= Scale_y;

	x_temp = x_val * cos(rot_angle) + y_val * sin(rot_angle);
	y_val = y_val * cos(rot_angle) - x_val * sin(rot_angle);
	x_val = x_temp;

	x_val += Origin_x;
	y_val += Origin_y;

	if (NULL != ptr_x_value) *ptr_x_value = x_val;
	if (NULL != ptr_y_value) *ptr_y_value = y_val;
}

void setOrigin(ExprNode_Ptr x_tree, ExprNode_Ptr y_tree) {
	Origin_x = GetExprValue(x_tree);
	Origin_y = GetExprValue(y_tree);
}

void setScale(ExprNode_Ptr x_tree, ExprNode_Ptr y_tree) {
	Scale_x = GetExprValue(x_tree);
	Scale_y = GetExprValue(y_tree);
}

void setRotate(ExprNode_Ptr tree) {
	rot_angle = GetExprValue(tree);
}

void DrawLoop(ExprNode_Ptr start_tree,
	ExprNode_Ptr end_tree,
	ExprNode_Ptr step_tree,
	ExprNode_Ptr x_tree,
	ExprNode_Ptr y_tree) {
	double x_val, y_val;
	double start_val, end_val, step_val;
	double* p_T_value = getTmemory();
	start_val = GetExprValue(start_tree);
	end_val = GetExprValue(end_tree);
	step_val = GetExprValue(step_tree);
	for (*p_T_value = start_val; *p_T_value <= end_val; *p_T_value += step_val) {
		CalcCoord(x_tree, y_tree, &x_val, &y_val);
		DrawPixel((unsigned long)x_val, (unsigned long)y_val, 0);
	}
}
