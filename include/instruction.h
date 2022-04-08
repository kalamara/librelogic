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

#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#define LABELLEN 4

/**
 *IL modifiers
 */
typedef enum {
  IL_NEG  = 0x1,    /// '!'
  IL_PUSH = 0x2,    /// '(' 
  IL_NORM = 0x4,    /// ' '
  IL_COND = 0x8,    ///'?'
} IL_MODIFIERS;
#define N_IL_MODIFIERS 4
#define IS_MODIFIER(x) (x >= IL_NEG && x < N_IL_MODIFIERS)

#define MAX_LABEL 1024

/**
 * @brief The instruction struct
 */
typedef struct instruction {
  char label[MAX_LABEL];
  char lookup[MAX_LABEL]; // label to lookup (applies to JMP etc.)
  unsigned char operation;
  unsigned char operand;
  unsigned char modifier;
  unsigned char byte;
  unsigned char bit;
} * instruction_t;

/**
 * @brief get type of instruction
 * @convention type is encoded in the instruction
 * the length is given by the bit part, and the scalar / real is defined by
 * the operand (analog / digital)
 * @param instruction
 * @return the type, or error
 */
int get_type(const instruction_t ins);

/**
 *@brief deepcopy instructions
 *@param from
 *@param to
 */
void deepcopy(const instruction_t from, instruction_t to);

void dump_instruction(instruction_t ins, char *dump);

#endif // _INSTRUCTION_H_
