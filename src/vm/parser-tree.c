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
#include "parser-tree.h"

#include "rung.h"

#include "plclib.h"

// TODO: memory optimization: create a factory to allocate different size per node type

item_t mk_identifier(const PLC_BYTE operand, const PLC_BYTE byte, const PLC_BYTE bit) {
    item_t r = (item_t) calloc(1, sizeof(struct item));

    r->tag = TAG_IDENTIFIER;
    r->v.id.operand = operand;
    r->v.id.byte = byte;
    r->v.id.bit = bit;
    return r;
}

item_t mk_expression(const item_t a, const item_t b, const PLC_BYTE op, const PLC_BYTE mod) {
    item_t r = (item_t) calloc(1, sizeof(struct item));

    r->tag = TAG_EXPRESSION;
    r->v.exp.op = op;
    r->v.exp.mod = mod;
    r->v.exp.a = a;
    r->v.exp.b = b;
    return r;
}

item_t mk_assignment(const item_t identifier, const item_t expression, const PLC_BYTE type) {
    item_t r = (item_t) calloc(1, sizeof(struct item));

    r->tag = TAG_ASSIGNMENT;
    r->v.ass.left = identifier;
    r->v.ass.right = expression;
    r->v.ass.type = type;
    return r;
}

item_t clear_tree(item_t root) {
    item_t r = root;
    if (root) {
        switch (root->tag) {
            case TAG_EXPRESSION:
                r->v.exp.a = clear_tree(root->v.exp.a);
                r->v.exp.b = clear_tree(root->v.exp.b);

                break;

            case TAG_ASSIGNMENT:
                r->v.ass.left = clear_tree(root->v.ass.left);
                r->v.ass.right = clear_tree(root->v.ass.right);

                break;

            default:
                break;
        }
        // tree leaves
        if (r != NULL) {
            //free(r);
            memset(r, 0, sizeof(struct item));
            r = (item_t) NULL;
        }
    }
    return r;
}
