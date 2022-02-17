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

#ifndef _CODEGEN_H_
#define _CODEGEN_H_

/**
 * @brief generate code from an expression
 * @param expression
 * @param the rung to insert the code to
 * @param the operation to generate if called in a recursion
 * @return ok or error code
 */
int gen_expr(const item_t expression, rung_t rung, BYTE recursive); 

/**
 * @brief generate code for the left side of an expression
 * @param expression or identifier
 * @param the rung to insert the code to
 * @param the operation to generate if called in a recursion
 * @return ok or error code
 */
int gen_expr_left(const item_t left, rung_t rung, BYTE recursive); 

/**
 * @brief generate code for the right side of an expression
 * @param expression or identifier
 * @param the rung to insert the code to
 * @param the operation to generate 
 * @param the modifier 
 * @return ok or error code
 */
int gen_expr_right(const item_t right, rung_t rung, BYTE op, BYTE mod); 

/**
 * @brief generate code from an assignment statement
 * @param an assignment statement
 * @param the rung to insert the code to
 * @return ok or error code
 */
int gen_ass(const item_t assignment, rung_t rung);
 
#endif //_CODEGEN_H_
