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

#ifndef _PARSER_TREE_H_
#define _PARSER_TREE_H_

/*
 *  Node type codes
 */
typedef enum {
    TAG_IDENTIFIER,
    TAG_EXPRESSION,
    TAG_ASSIGNMENT,
    N_TAGS
} item_tag_t;

typedef struct identifier {
    BYTE operand;
    BYTE byte;
    BYTE bit;
} *identifier_t;

typedef struct expression {
    struct item *a;
    struct item *b;
    BYTE op;
    BYTE mod;
} *expression_t;

typedef struct assignment {
    struct item *left;
    struct item *right;
    BYTE type; // contact, down, set, reset
} *assignment_t;

/**
 * a syntax tree node
 */
typedef struct item {
    item_tag_t tag;
    union {
        struct identifier id;
        struct expression exp;
        struct assignment ass;
    } v;
} *item_t;

/**
 * @brief make an identifier node
 * @param operand
 * @param byte
 * @param bit
 * @return a newly allocated tree node 
 */
item_t mk_identifier(const BYTE operand, const BYTE byte, const BYTE bit);

/**
 * @brief make an expression node
 * @param left part
 * @param right part
 * @param operation
 * @param modifier
 * @return a newly allocated tree node 
 */
item_t mk_expression(const item_t a, const item_t b, const BYTE op, const BYTE mod);

/**
 * @brief make an assignment node
 * @param left part
 * @param right part
 * @param type of assignment (normal, negative, set, reset)
 * @return a newly allocated tree node 
 */
item_t mk_assignment(const item_t identifier, const item_t expression, const BYTE type);

/**
 * @brief recursively clear (deallocate) 
 * tree nodes starting from a root node
 */
item_t clear_tree(item_t root);

#endif /* _PARSER_TREE_H_ */
