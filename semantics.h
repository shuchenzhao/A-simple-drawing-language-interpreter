#ifndef _SEMANTICS_H
#define _SEMANTICS_H

#include "parser.h"

void setOrigin(ExprNode_Ptr x_tree, ExprNode_Ptr y_tree);
void setScale(ExprNode_Ptr x_tree, ExprNode_Ptr y_tree);
void setRotate(ExprNode_Ptr tree);

void DrawLoop(
	ExprNode_Ptr start_tree,
	ExprNode_Ptr end_tree,
	ExprNode_Ptr step_tree,
	ExprNode_Ptr x_tree,
	ExprNode_Ptr y_tree);

#endif // !_SEMANTICS_H

