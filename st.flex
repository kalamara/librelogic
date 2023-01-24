/*
 *  The scanner definition for Structured Text.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  
 */
%{
#include <parser-st.h>
//#include <stringtab.h>
//#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval st_yylval
#define yylex  st_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT
 so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");
#define MAX_INT 2147483647
char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE st_yylval;

/*
 *  Add Your own definitions here
 */
//int nested_comments=0;
char err_buf[MAX_STR_CONST]; /* to store error messages */
int put_char_to_str(char c)
{
    if(string_buf_ptr < string_buf + MAX_STR_CONST-1){
        *string_buf_ptr++ = c;
//printf("####STRING now is %s######\n", string_buf);
        return 0;
    } else {
//printf("######MAX  STRING######n");
        memset(err_buf,0,MAX_STR_CONST);
        sprintf(err_buf, "String constant too long");
        st_yylval.error_msg=err_buf;
        
        return -1;
     } 
}
%}

/*
 * Define names for regular expressions here.
 

LE              <=
GE              >=
UNEQ            <>
ASS             :=
CHAR            \'.\'
WHITE           [ \t\f\r\v]

DIGIT           [0-9]
OID             [_a-zA-Z0-9]*
TID             [A-Z]*
ADDR            [%][iI][qQ][mM][tT][rR][bB][0-9a-fA-F]*[.][0-9a-fA-F]
LABEL           \:[a-zA-Z]*\:
TRUE            TRUE
FALSE           FALSE

ELSE            ELSE
IF              IF
ENDIF           END_IF
ELSIF           ELSE_IF
THEN            THEN
WHILE           WHILE
END_WHILE       END_WHILE
FOR             FOR
END_FOR         END_FOR
DO              DO
BY              BY
REPEAT          REPEAT
END_REPEAT      END_REPEAT
UNTIL           UNTIL
CASE            CASE
END_CASE        END_CASE
OF              OF
NOT             NOT
EXIT            EXIT
RETURN          RETURN
JMP             JMP

SINGLECHAR      \<|\,|\(|\)|\{|\}|\+|\-|\*|\/|\.|[@~=:]
             
%x LINECOMMENT         
LINECOMMENTSTART ;
LINECOMMENTEND  \n

%x COMMENT
COMMENTSTART        \(\*
COMMENTEND          \*\)

%x STRING
ESCAPE          [ntbf]
*/
%%

.   {
printf("Unmatched Character\n");
        memset(err_buf,0,MAX_STR_CONST);
        sprintf(err_buf, "%c", yytext[0]);
        st_yylval.error_msg=err_buf;
        return ERROR;
    }

%%
