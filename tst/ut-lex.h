#ifndef _UT_LEX_H_ 
#define _UT_LEX_H_

void ut_st_char() {
    st_fin = fmemopen(NULL, 0, "r"); 
    int result = yylex();
    CU_ASSERT(result == -1);
}

void ut_st_comments() {
}
/*
void ut_number() {
    int result = extract_number(NULL);
    CU_ASSERT(result == PLC_ERR);
    
    char *line = "lol lol";
    result = extract_number(line);
    CU_ASSERT(result == PLC_ERR);
    
    line = "lol52a";
    
    result = extract_number(line + 5);
    CU_ASSERT(result == PLC_ERR);
    
    result = extract_number(line + 4);
    CU_ASSERT(result == 2);
    
    result = extract_number(line + 3);
    CU_ASSERT(result == 52);
    
    result = extract_number(line);
    CU_ASSERT(result == PLC_ERR);
}

void ut_comments() {
    read_line_trunk_comments(NULL);
    char line[MAXSTR];
    memset(line, 0, MAXSTR);
    sprintf(line, "%s", "lol");
    read_line_trunk_comments(line);
    CU_ASSERT(strcmp("lol", line) == 0);
    
    sprintf(line, "%s", "lol ; lala");
    read_line_trunk_comments(line);
    
    CU_ASSERT(strcmp("lol ", line) == 0);
}

void ut_whitespace() {
    char line[MAXSTR];
    memset(line, 0, MAXSTR);
    sprintf(line, "whitespace!!!   \t   \n\r ");
    trunk_whitespace(line);
    CU_ASSERT(strcmp(line, "whitespace!!!") == 0);
    
    sprintf(line, "   \n\t\r  whitespace!!!   \t   \n\r ");
    trunk_whitespace(line);
    CU_ASSERT(strcmp(line, "whitespace!!!") == 0);
}

void ut_label() {
    trunk_label(NULL, NULL, NULL);
    
    char line[MAXSTR];
    char buf[MAXSTR];
    char result[MAXSTR];
    memset(line, 0, MAXSTR);
    memset(buf, 0, MAXSTR);
    memset(result, 0, MAXSTR);

    sprintf(line, "%s", "lol");
    trunk_label(line, buf, result);
    CU_ASSERT(strcmp("lol", buf) == 0);
    CU_ASSERT(strcmp("", result) == 0);
    
    sprintf(line, "%s", "lol:mama");
    trunk_label(line, buf, result);
    CU_ASSERT(strcmp("lol", result) == 0);
    CU_ASSERT(strcmp("mama", buf) == 0);
    
}

void ut_modifier() {
    char line[MAXSTR];
    memset(line, 0, MAXSTR);
    
    PLC_BYTE result = read_modifier(NULL, NULL);
    CU_ASSERT(result == (PLC_BYTE) PLC_ERR);
    
    char *target = "";
    sprintf(line, "%s", "lol");
    result = read_modifier(line, &target);
    CU_ASSERT(result == NOP);
    CU_ASSERT(target == NULL);

    sprintf(line, "%s", "lol(");
    result = read_modifier(line, &target);
    CU_ASSERT(result == IL_PUSH);
    CU_ASSERT(*target == '(');
    CU_ASSERT(target == line + 3);
    
    sprintf(line, "%s", "lo?l");
    result = read_modifier(line, &target);
    CU_ASSERT(result == IL_COND);
    CU_ASSERT(*target == '?');
    
    sprintf(line, "%s", "l!ol");
    result = read_modifier(line, &target);
    CU_ASSERT(result == IL_NEG);
    CU_ASSERT(*target == '!');

    sprintf(line, "%s", "l ol");
    result = read_modifier(line, &target);
    CU_ASSERT(result == IL_NORM);
    CU_ASSERT(*target == ' ');
}

void ut_operator() {
    char line[MAXSTR];
    memset(line, 0, MAXSTR);
    
    PLC_BYTE result = read_operator(NULL, NULL);
    CU_ASSERT(result == (PLC_BYTE) PLC_ERR);
    
    sprintf(line, "%s", "LT");
    result = read_operator(line, NULL);
    CU_ASSERT(result == IL_LT);
    
    sprintf(line, "%s", "XOR lwieufql");
    result = read_operator(line, line + 3);
    CU_ASSERT(result == IL_XOR);
    
    sprintf(line, "%s", "lwieufql");
    result = read_operator(line, line + 3);
    CU_ASSERT(result == N_IL_INSN);
    
}

void ut_arguments() {
    int result = find_arguments(NULL, NULL, NULL, NULL);
    CU_ASSERT(result == PLC_ERR);
    
    char line[MAXSTR];
    memset(line, 0, MAXSTR);
    
    PLC_BYTE operand = 0;
    PLC_BYTE byte = 0;
    PLC_BYTE bit = 0;
    
    sprintf(line, "%s", "aqhsgdf");
    result = find_arguments(line, &operand, &byte, &bit);
    CU_ASSERT(result == PLC_ERR_BADCHAR);
    
    sprintf(line, "%s", "aqh\%9df");
    result = find_arguments(line, &operand, &byte, &bit);
    CU_ASSERT(result == PLC_ERR_BADOPERAND);
    
    sprintf(line, "%s", "aqh\%zdf");
    result = find_arguments(line, &operand, &byte, &bit);
    CU_ASSERT(result == PLC_ERR_BADCHAR);
    
    sprintf(line, "%s", "aqh\%mdf");
    result = find_arguments(line, &operand, &byte, &bit);
    CU_ASSERT(result == PLC_ERR_BADINDEX);
    CU_ASSERT(operand == OP_MEMORY);
    
    sprintf(line, "%s", "aqh\%i25f");
    result = find_arguments(line, &operand, &byte, &bit);
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(operand == OP_INPUT);
    CU_ASSERT(byte == 25);
    CU_ASSERT(bit == BYTESIZE);
    
    sprintf(line, "%s", "\%t25/f");
    result = find_arguments(line, &operand, &byte, &bit);
    CU_ASSERT(result == PLC_ERR_BADINDEX);
    CU_ASSERT(operand == OP_TIMEOUT);
    CU_ASSERT(byte == 25);
    CU_ASSERT(bit == BYTESIZE);
    
    sprintf(line, "%s", "\%q25/9");
    result = find_arguments(line, &operand, &byte, &bit);
    CU_ASSERT(result == PLC_ERR_BADINDEX);
    CU_ASSERT(operand == OP_OUTPUT);
    CU_ASSERT(byte == 25);
    CU_ASSERT(bit == BYTESIZE);
    
    sprintf(line, "%s", "\%b25/7");
    result = find_arguments(line, &operand, &byte, &bit);
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(operand == OP_BLINKOUT);
    CU_ASSERT(byte == 25);
    CU_ASSERT(bit == 7);
    
    sprintf(line, "%s", "\%if0");
    result = find_arguments(line, &operand, &byte, &bit);
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(operand == OP_REAL_INPUT);
    CU_ASSERT(byte == 0);
    
    sprintf(line, "%s", "\%mf1");
    result = find_arguments(line, &operand, &byte, &bit);
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(operand == OP_REAL_MEMORY);
    CU_ASSERT(byte == 1);
    
    sprintf(line, "%s", "\%qf21");
    result = find_arguments(line, &operand, &byte, &bit);
    CU_ASSERT(result == PLC_OK);
    CU_ASSERT(operand == OP_REAL_OUTPUT);
    CU_ASSERT(byte == 21);
}
*/
#endif //_UT_LEX_H_
