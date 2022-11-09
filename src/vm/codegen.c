/*******************************************************************************
 LibreLogic : a free PLC library
 Copyright (C) 2022, Antonis K. (kalamara AT ceid DOT upatras DOT gr)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "data.h"
#include "instruction.h"
#include "rung.h"
#include "plclib.h"
#include "parser-tree.h"
#include "parser-ld.h"
#include "codegen.h"

int gen_expr(const item_t expression, rung_t rung, PLC_BYTE recursive_operation) {
    int rv = PLC_OK;
    
    if (expression == NULL || rung == NULL) {
        return PLC_ERR;
    }

    if (expression->tag != TAG_EXPRESSION) {
        return PLC_ERR;
    }

    PLC_BYTE operator = expression->v.exp.op;
    if (!IS_OPERATION(operator) && operator != IL_LD) {
        return PLC_ERR_BADOPERATOR;
    }

    PLC_BYTE modifier = expression->v.exp.mod;
    if (!IS_MODIFIER(modifier)) {
        return PLC_ERR_BADOPERATOR;
    }
    
    // left operand
    rv = gen_expr_left(expression->v.exp.a, rung, recursive_operation, modifier);
    if (rv < 0) {
        return rv;
    }
    // right operand
    rv = gen_expr_right(expression->v.exp.b, rung, operator, modifier);

    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));
    
    if (rv == PLC_OK && (modifier & IL_PUSH) && IS_OPERATION(operator) && expression->v.exp.b != NULL) {
        ins.operation = IL_POP;
        rv = append(&ins, rung);
    }
    return rv;
}

int gen_expr_left(const item_t left, rung_t rung, PLC_BYTE recursive, PLC_BYTE mod) {
    int rv = PLC_OK;
    if (left == NULL)
        return PLC_ERR_BADOPERAND;
    PLC_BYTE inner = IL_LD;

    if (IS_OPERATION(recursive)) {
        inner = recursive;
        mod |= IL_PUSH;
    } else {
        mod &= (0xff - IL_PUSH);
    }
    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));
    switch (left->tag) {
        case TAG_IDENTIFIER: // LD
            ins.operation = inner;
            ins.operand = left->v.id.operand;
            ins.modifier = mod;
            ins.byte = left->v.id.byte;
            ins.bit = left->v.id.bit;
            rv = append(&ins, rung);
            break;
        case TAG_EXPRESSION:
            // recursion
            rv = gen_expr(left, rung, inner);
            break;
        default:
            rv = PLC_ERR_BADOPERAND;
    }
    return rv;
}

int gen_expr_right(const item_t right, rung_t rung, PLC_BYTE op, PLC_BYTE mod) {
    int rv = PLC_OK;

    if (right != NULL) {
        struct instruction ins;
        memset(&ins, 0, sizeof(struct instruction));
        switch (right->tag) {
            case TAG_IDENTIFIER:
                ins.operation = op;
                ins.modifier = mod;
                ins.operand = right->v.id.operand;
                ins.byte = right->v.id.byte;
                ins.bit = right->v.id.bit;
                rv = append(&ins, rung);
                break;
            case TAG_EXPRESSION:
                //recursion
                rv = gen_expr(right, rung, op);
                break;
            default:
                rv = PLC_ERR_BADOPERAND;
        }
    }
    return rv;
}

int gen_ass(const item_t assignment, rung_t rung) {
    int rv = PLC_OK;
    
    if (rung == NULL || assignment == NULL || assignment->tag != TAG_ASSIGNMENT)
        return PLC_ERR;

    if (assignment->v.ass.left == NULL || assignment->v.ass.left->tag != TAG_IDENTIFIER)
        return PLC_ERR_BADOPERAND;
    
    PLC_BYTE type = assignment->v.ass.type;
    if (!IS_COIL(type))
        return PLC_ERR_BADCOIL;
    
    item_t left = assignment->v.ass.left;
    
    struct instruction ins;
    memset(&ins, 0, sizeof(struct instruction));
    
    if (assignment->v.ass.right == NULL)
        return PLC_ERR_BADOPERATOR;

    item_t right = assignment->v.ass.right;
    switch (right->tag) {
        case TAG_IDENTIFIER:
            ins.operation = IL_LD;
            ins.modifier = IL_NORM;
            ins.operand = right->v.id.operand;
            ins.byte = right->v.id.byte;
            ins.bit = right->v.id.bit;
            rv = append(&ins, rung);
            break;
        case TAG_EXPRESSION:
            rv = gen_expr(right, rung, 0);
            break;
        default:
            return PLC_ERR_BADOPERATOR;
    }

    if (rv == PLC_OK) {
        switch (type) {
            case LD_DOWN:
                ins.operation = IL_ST;
                ins.modifier = IL_NEG;
                break;

            case LD_RESET:
                ins.operation = IL_RESET;
                ins.modifier = IL_COND;
                break;

            case LD_SET:
                ins.operation = IL_SET;
                ins.modifier = IL_COND;
                break;

            default: // coil
                ins.operation = IL_ST;
                ins.modifier = IL_NORM;
        }
        ins.operand = left->v.id.operand;
        ins.byte = left->v.id.byte;
        ins.bit = left->v.id.bit;
        rv = append(&ins, rung);
    }
    return rv;
}
