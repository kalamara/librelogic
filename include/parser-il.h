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

#ifndef _PARSER_IL_H_
#define _PARSER_IL_H_

/**
 * @brief read an unsigned integer value from string
 * starting @ position start
 * @param line
 * @return number read (positive value) or error (negative value)
 */
int extract_number(const char *line);

/**
 * @brief extract bit and byte from X/Y expression starting @ start
 * @param the string
 * @param the byte
 * @param the bit
 * @return OK or error
 */
int extract_arguments(const char *buf, PLC_BYTE *byte, PLC_BYTE *bit);

/**
 * @brief read optional descriptor (currently only f) 
 * from line at index
 * and parse grammatically
 * @param line
 * @param current operand
 * @param c index
 * @return IL operand symbol
 */
PLC_BYTE read_type(const char *line, PLC_BYTE *operand, unsigned int index);

/**
 * @brief read operand  
 * from line at index
 * and parse grammatically
 * @param line
 * @param c index
 * @return IL operand symbol
 */
PLC_BYTE read_operand(const char *line, unsigned int index);

/**
 * @brief read up to ';' or /n
 * @param the line to trunkate comments from
 */
void read_line_trunk_comments(char *line);

/**
 * @brief find last ':', truncate up to there, store label.
 * @param buf a place to store labelless lines
 * @param label_buf the extracted label
 */
void trunk_label(const char *line, char *buf, char *label_buf);

/**
 * @brief trunkate all whitespaces left and right
 * @param the line to trunkate whitespaces from
 * @return trunkated line
 */
char* trunk_whitespace(char *line);

/**
 * @brief find first ' ','!','('. store modifier (0 if not found)
 * @param buf a string
 * @param pos points to where was the modifier found
 * @return the modifier
 */
PLC_BYTE read_modifier(const char *buf, char **pos);

/**
 * @brief read operator from beginning to modifier, check if invalid. 
 * @param buf a string
 * @param stop points to end of operator
 * @return error code if invalid, otherwise the operator
 */
PLC_BYTE read_operator(const char *buf, const char *stop);

/**
 * @brief check modifier for correctness
 * @param an instruction
 * @return error code if not correct, 0 otherwise
 */
int check_modifier(const instruction_t op);

/**
 * @brief check operand for correctness
 * @param an instruction
 * @return error code if not correct, 0 otherwise
 */
int check_operand(instruction_t op);

/**
 *@brief find arguments
 * @param a line
 * @param operand
 * @param byte
 * @param bit
 * @return error code on failure
 */
int find_arguments(const char *buf, PLC_BYTE *operand, PLC_BYTE *byte, PLC_BYTE *bit);

/**
 * @brief parse IL line and generate microcode
 * line format:[label:]<operator>[<modifier>[%<operand><byte>[/<bit>]]|<label>][;comment]
 * @param a line of IL code
 * @param the rung to generate micorcode in
 * @return OK or error
 */
int parse_il_line(const char *line, rung_t r);

#endif /* _PARSER-IL_H */
