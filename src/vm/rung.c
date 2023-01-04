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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "instruction.h"
#include "rung.h"

/*****************************rung***********************************/
char* strdup_rc(char *dest, const char *src) {
//strdup with realloc/calloc
    int len = strlen(src);
    if (len == 0) {
        return dest;
    }
    char *r = (!dest) ? (char*) calloc(1, len) : realloc((void*) dest, len);

    memset(r, 0, len);
    sprintf(r, "%s", src);

    return r;
}

opcode_t take(rung_t r) {
    if (r->stack == NULL)
        return &(r->prealloc[0]);
    else if (r->stack->depth < MAXSTACK - 1)
        return &(r->prealloc[r->stack->depth]);
    else
        return NULL;
}

void give(opcode_t head) {
    memset(head, 0, sizeof(struct opcode));
}

int push(PLC_BYTE op, PLC_BYTE t, const data_t val, rung_t r) {
    // push an opcode and a value into stack.
    struct opcode *p = take(r);
    if (!p)
        return PLC_ERR;
    // initialize
    p->operation = op;
    p->value = val;
    p->type = t;
    p->next = r->stack; //*stack;
    p->depth = (r->stack == NULL) ? 1 : r->stack->depth + 1;
    // set stack head pointer to point at it
    r->stack = p;
    return PLC_OK;
}

data_t pop(const data_t val, opcode_t *stack) {
    // retrieve stack heads operation and operand, apply it to val and return result
    data_t r = val; // return value
    opcode_t p;
    if (*stack != NULL) {
        // safety
        r = operate((*stack)->operation, (*stack)->type, (*stack)->value, val); // execute instruction
        p = *stack;
        *stack = (*stack)->next;
        // set stack head to point to next opcode in stack
        give(p);
    }
    return r;
}

int get(const rung_t r, const unsigned int idx, instruction_t *i) {
    if (r == NULL || idx >= r->insno)
        return PLC_ERR;
    *i = r->instructions[idx];
    return PLC_OK;
}

int append(const instruction_t i, rung_t r) {
    if (r == NULL || r->insno == MAXSTACK)
        return PLC_ERR;
    if (i != NULL) {
        if (r->instructions == NULL) { // lazy allocation
            r->instructions = (instruction_t*) calloc(MAXSTACK, sizeof(instruction_t));
        }
        if (lookup(i->label, r) >= 0)
            return PLC_ERR; // don't allow duplicate labels

        instruction_t ins = (instruction_t) calloc(1, sizeof(struct instruction));
        deepcopy(i, ins);

        r->instructions[(r->insno)++] = ins;
    }
    return PLC_OK;
}

codeline_t append_line(const char *l, codeline_t code) {
    if (l == NULL) {
        return code;
    }
    codeline_t r = (codeline_t) calloc(1, sizeof(struct codeline));
    r->line = strdup(l);

    if (code != NULL) {

        codeline_t i = code;
        while (i->next != NULL) {

            i = i->next;
        }
        i->next = r;

        return code;
    }
    return r;
}

codeline_t clear_lines(codeline_t code){
    
    codeline_t i = code;
    codeline_t p = NULL;
    while (i) {
        while (i->next) {
            p = i;
            i = i->next;
        }   
        if(i->line){
            free(i->line);
            i->line = NULL; 
        }
        free(i);
        if(p){
            p->next = NULL;
        } else {
           code = NULL;
        }
        p = NULL;
        i = code;
    }
    return code;
}

void clear_rung(rung_t r) {
    int i = 0;
    if (r != NULL && r->instructions != NULL) {
        for (; i < MAXSTACK; i++) {
            if (r->instructions[i] != NULL){
                free(r->instructions[i]);
                r->instructions[i] == NULL;
            }
        }
        free(r->instructions);
        clear_lines(r->code);
        r->instructions = NULL;
        r->insno = 0;
    }
}

int lookup(const char *label, rung_t r) {
    int ret = PLC_ERR;
    if (label == NULL || r == NULL)
        return ret;

    int i = 0;
    instruction_t ins = NULL;
    for (; i < r->insno; i++) {
        get(r, i, &ins);
        if (strlen(ins->label) > 0 && strcmp(ins->label, label) == 0) {
            ret = i;
            break;
        }
    }
    return ret;
}

int intern(rung_t r) {
    if (r == NULL)
        return PLC_ERR;

    int i = 0;
    instruction_t ins = NULL;
    for (; i < r->insno; i++) {
        get(r, i, &ins);
        if (strlen(ins->lookup) > 0) {
            int l = lookup(ins->lookup, r);
            if (l < 0)
                return PLC_ERR;
            else
                ins->operand = l;
        }
    }
    return PLC_OK;
}

void dump_rung(rung_t r, char *dump) {
    if (r == NULL || dump == NULL)
        return;
    instruction_t ins;
    unsigned int pc = 0;
    char buf[4] = "";
    for (; pc < r->insno; pc++) {
        if (get(r, pc, &ins) < PLC_OK)
            return;
        sprintf(buf, "%d.", pc);
        strcat(dump, buf);
        dump_instruction(ins, dump);
    }
    // printf("%s", dump);
}
