/*
*  st.y
*              Parser definition for the Structured Text language.
*/
%{
  #include <iostream>
 // #include "st-tree.h"
//  #include "stringtab.h"
//  #include "utilities.h"
  
  extern char *curr_filename;
  #define _GLR_ 1
  /* Locations */
  #define YYLTYPE int           /*  the type of locations */
  #define st_yylloc curr_lineno  /* use the curr_lineno from the lexer
  for the location of tokens */

  #if (__STDC_VERSION__ < 199901 && ! defined __GNUC__ \
       && ! defined inline)
  # define inline
  #endif

    extern int node_lineno;          
    /* set before constructing a tree node
    to whatever you want the line number
    for the tree node to be */
     
#if _GLR_     
      #define YYLLOC_DEFAULT(Current, Rhs, N)         \
      Current = Rhs[1].yystate.yyloc;                 \
      node_lineno = Current;
#else
      
      #define YYLLOC_DEFAULT(Current, Rhs, N)         \
      Current = Rhs[1]; \
      node_lineno = Current;
#endif
    
    #define SET_NODELOC(Current)  \
    node_lineno = Current;
   
    /* IMPORTANT NOTE ON LINE NUMBERS
    *********************************
    * The above definitions and macros cause every terminal in your grammar to 
    * have the line number supplied by the lexer. The only task you have to
    * implement for line numbers to work correctly, is to use SET_NODELOC()
    * before constructing any constructs from non-terminals in your grammar.
    * Example: Consider you are matching on the following very restrictive 
    * (fictional) construct that matches a plus between two integer constants. 
    * (SUCH A RULE SHOULD NOT BE  PART OF YOUR PARSER):
    
    plus_consts	: INT_CONST '+' INT_CONST 
    
    * where INT_CONST is a terminal for an integer constant. Now, a correct
    * action for this rule that attaches the correct line number to plus_const
    * would look like the following:
    
    plus_consts	: INT_CONST '+' INT_CONST 
    {
      // Set the line number of the current non-terminal:
      // ***********************************************
      // You can access the line numbers of the i'th item with @i, just
      // like you acess the value of the i'th exporession with $i.
      //
      // Here, we choose the line number of the last INT_CONST (@3) as the
      // line number of the resulting expression (@$). You are free to pick
      // any reasonable line as the line number of non-terminals. If you 
      // omit the statement @$=..., bison has default rules for deciding which 
      // line number to use. Check the manual for details if you are interested.
      @$ = @3;
      
      
      // Observe that we call SET_NODELOC(@3); this will set the global variable
      // node_lineno to @3. Since the constructor call "plus" uses the value of 
      // this global, the plus node will now have the correct line number.
      SET_NODELOC(@3);
      
      // construct the result node:
      $$ = plus(int_const($1), int_const($3));
    }
    
    */
    
    
    
    void yyerror(char *s);        /*  defined below; called for each parse error */
    extern int yylex();           /*  the entry point to the lexer  */
    
    /************************************************************************/
    /*                DONT CHANGE ANYTHING IN THIS SECTION                  */
    
    Program ast_root;	      /* the result of the parse  */
    Runges parse_results;        /* for use in semantic analysis */
    int omerrs = 0;               /* number of errors in lexing and parsing */
    %}
    
    /* A union of all the types that can be the result of parsing actions. */
    %union {
      Boolean boolean;
      Symbol symbol;
      Program program;
      Rung_ rung_;
      Runges runges;
      Feature feature;
      Features features;
      Formal formal;
      Formals formals;
      Case case_;
      Cases cases;
      Expression expression;
      Expressions expressions;
      char *error_msg;
    }
    
    /* 
    Declare the terminals; a few have types for associated lexemes.
    The token ERROR is never used in the parser; thus, it is a parse
    error when the lexer returns it.
    
    The integer following token declaration is the numeric constant used
    to represent that token internally.  Typically, Bison generates these
    on its own, but we give explicit numbers to prevent version parity
    problems (bison 1.25 and earlier start at 258, later versions -- at
    257)
    */
    %token RUNG 258 ELSE 259 FI 260 IF 261 IN 262 
    %token INHERITS 263 LET 264 LOOP 265 POOL 266 THEN 267 WHILE 268
    %token CASE 269 ESAC 270 OF 271 DARROW 272 NEW 273 ISVOID 274
    %token <symbol>  STR_CONST 275 INT_CONST 276 
    %token <boolean> BOOL_CONST 277
    %token <symbol>  TYPEID 278 VARID 279 
    %token ASSIGN 280 NOT 281 LE 282 ERROR 283
    
    /*  DON'T CHANGE ANYTHING ABOVE THIS LINE, OR YOUR PARSER WONT WORK       */
    /**************************************************************************/
    
    /* Complete the nonterminal list below, giving a type for the semantic
    value of each non terminal. (See section 3.6 in the bison 
    documentation for details). */
    
    /* Declare types for the grammar's non-terminals. */
    %type <program> program
    %type <runges> rung_list
    %type <rung_> rung
    
    /* You will want to change the following line. */
    %type <features> feature_list
    %type <feature> feature
    %type <formals> formal_list
    %type <formal> formal
    %type <cases> case_list
    %type <case_> case_
    %type <expressions> expr_commas
    %type <expressions> expr_semicols
    %type <expression> expr
    %type <expression> init
    %type <expression> init_value
    %type <symbol> init_list
    %type <symbol> init_item
    %type <expression> const_

    %type <expression> branch
    %type <expression> dispatch
    %type <expression> disp_expr
    %type <expression> operand
    %type <expression> operation
   %glr-parser
    /*Precedence declarations go here. */
    %right ASSIGN
    %left NOT
    %nonassoc LE '<' '='
    %left '+' '-'
    %left '*' '/'
    %left ISVOID
    %left '~'
    %left '@'
    %left '.'
 
    
    /*%expect-rr 1
    %expect 20*/
    %%
    /* 
    Save the root of the abstract syntax tree in a global variable.
    */
     program	: 
/*1*/    rung_list	{ @$ = @1; ast_root = program($1); }
     ;
    
     rung_list: 
/*2*/    rung			/* single rung */
    { 
        $$ = single_Runges($1);
        parse_results = $$; 
    }
/*3*/    | rung_list rung	/* several runges */
    { 
        $$ = append_Runges($1,single_Runges($2)); 
        parse_results = $$; 
    }
    ;
    
    /* If no parent is specified, the rung inherits from the Object rung. */
   rung	: 
/*4*/ RUNG TYPEID '{' feature_list '}' ';'
    { 
        $$ = rung_($2,idtable.add_string("Object"),$4,
                    stringtable.add_string(curr_filename)); 
    }
    | RUNG TYPEID '{' error '}' ';'
    {}
/*5*/    | RUNG TYPEID INHERITS TYPEID '{' feature_list '}' ';'
    { 
        $$ = rung_($2,$4,$6,stringtable.add_string(curr_filename)); 
    }
    | RUNG TYPEID INHERITS TYPEID '{' error '}' ';'
    { }
    | RUNG TYPEID error '{' feature_list '}' ';'
    { }
    
    /* Feature list may be empty, but no empty features in list. */
  feature_list:		
/*6*/      %dprec 3/* empty */
    {  
        $$ = nil_Features(); 
    }
/*7*/    | feature   %dprec 2
    {/*single feature*/
        $$ = single_Features($1);
    }
/*8*/    | feature_list feature %dprec 1
    {/*Multiple features*/
        $$ = append_Features($1,single_Features($2));
    } 
    
    
    
    feature: 
/*9*/   VARID '(' formal_list ')' ':' TYPEID '{' expr '}' ';'
    {/*Method*/
        $$ = method($1,$3,$6,$8);
    }
    | error '(' formal_list ')' ':' TYPEID '{' expr '}' ';'
    {/**/
    }
/*10*/    | VARID ':' TYPEID ';'
    {/*uninitialized attribute*/
        $$ = attr($1,$3,no_expr());
    }
    | error ':' TYPEID ';'
    {/**/
    }
/*11*/    | VARID ':' TYPEID ASSIGN expr ';'
    {/*itialized attribute*/
        $$ = attr($1,$3,$5);
    }
    | error ';'
    {}
    
   formal_list:
/*12*/     /* empty */
    {  
        $$ = nil_Formals(); 
    }
/*13*/    | 		formal    
    {/*single formal*/
        $$ = single_Formals($1);
    }
/*14*/    | formal_list ',' formal 
    {/*Multiple formals*/
        $$ = append_Formals($1,single_Formals($3));
    } 
    
    formal:
/*15*/     VARID ':' TYPEID
    {
        $$ = formal($1,$3);
    }
    
    case_list:
/*16*/     case_ 
    {/*single case*/
        $$ = single_Cases($1);
    }
/*17*/    | case_list  case_ 
    {/*Multiple case*/
        $$ = append_Cases($1,single_Cases($2));
    } 
    
    case_:
/*18*/     VARID ':' TYPEID DARROW expr ';'
    {
        $$ = branch($1,$3,$5);
    }
    
    expr_commas:
/*19*/    /* empty */
    {  
        $$ = nil_Expressions(); 
    }
/*20*/     | expr    
    {/*single expr*/
        $$ = single_Expressions($1);
    }
/*21*/     | expr_commas ',' expr 
    {/*Multiple expr*/
        $$ = append_Expressions($1,single_Expressions($3));
    } 
    
    expr_semicols:
/*22*/     expr ';'   
    {/*single expr*/
        $$ = single_Expressions($1);
    }
/*23*/     | expr_semicols expr ';'
    {/*Multiple expr*/
        $$ = append_Expressions($1,single_Expressions($2));
    } 
    | error ';'
    {/**/
     
    } 

    init_value:
   /*empty*/ 
    {
        $$ = no_expr();
    }
    | ASSIGN expr[value] 
    {
        $$ = $value;
    }
   
    init_item:
    VARID ':' TYPEID init_value
    {
        $$ = $1;
    }
    
    init_list:
    init_item
    {
        $$=$1;
    }
    | init_list ',' init_item
    {
        $$ = $3;
    }
    
    init:
    VARID ':' 
    {
        $<symbol>s = $1;
    }[s] TYPEID 
    {
        $<symbol>t = $4;
    }[t]
    init_value[e] IN expr[body] %dprec 2
    {
         $$ = let($<symbol>s,$<symbol>t,$e,$body);
    }
    | init_list ',' VARID ':' 
    {
        $<symbol>s = $3;
    }[s] TYPEID 
    {
        $<symbol>t = $6;
    }[t]
    init_value[e] IN expr[body] %dprec 1
    {
          $$ = let($<symbol>s,$<symbol>t,$e,$body);
    }
    | error ',' VARID ':' 
    {
        $<symbol>s = $3;
    }[s] TYPEID 
    {
        $<symbol>t = $6;
    }[t]
    init_value[e] IN expr[body] %dprec 1
    {
          $$ = let($<symbol>s,$<symbol>t,$e,$body);
    }
    
    expr:
    /*const dispatch isvoid new and identifier included in operation*/
    VARID ASSIGN expr %dprec 3
    {/*assignment*/
        $$ = assign($1,$3);
    }
    | IF operation THEN branch ELSE branch FI
    {/*if*/
        $$ = cond($2,$4,$6);
    }
    | WHILE operation LOOP expr POOL
    {/*while*/   
        $$ = loop($2,$4);
    }
    | '{' expr_semicols '}'
    {/*block*/
        $$ = block($2);
    }
    | '{' error '}'
    {/**/
    }
    | CASE expr OF case_list ESAC
    {/*case*/
        $$ = typcase($2,$4);
    }
    |'(' expr ')' %dprec 2
    { /*parentheses*/
        $$ = $2;
    }
    | operation %dprec 1
    {
        $$ = $1;
    }
  
    block:
    '}'
    {
    }
    expr ';' '}'  
    {/*single expr*/
        $$ = single_Expressions($1);
    }
    | block ';' expr ';' '}'
    {/*Multiple expr*/
        $$ = append_Expressions($1,single_Expressions($2));
    } 
    | error ';' 
    {/**/
    } 
    
    const_:
    INT_CONST
    {/*integer*/
        $$ = int_const($1);
    }
    | STR_CONST
    {/*string*/ 
        $$ = string_const($1);
    }
    | BOOL_CONST
    {/*bool*/
        $$ = bool_const($1);
    }
    
    disp_expr:
    VARID
    {/*identifier*/
        $$ = object($1);
    }
    | dispatch
    {
        $$ = $1;
    }
    |'(' disp_expr ')' 
    { /*parentheses*/
        $$ = $2;
    }
    
    
    dispatch:
    disp_expr '@' TYPEID '.' VARID '(' expr_commas ')' %dprec 1
    {/*static dispatch*/
        $$ = static_dispatch($1,$3,$5,$7); 
    }
    | disp_expr '.' VARID '(' expr_commas ')' %dprec 1
    {/*dispatch*/
        $$ = dispatch($1,$3,$5);
    }
    | VARID '(' expr_commas ')' %dprec 1
    {/*self dispatch*/
        $$ = dispatch($$, $1,$3);
    }
    
    branch:
    expr
    {
        $$ = $1;
    }
    
    operand:
    const_
    {/*constant*/
        $$ = $1;
    }
    | VARID
    {/*identifier*/
        $$ = object($1);
    }
    | VARID ASSIGN operand 
    {/*assignment*/
        $$ = assign($1,$3);
    }
    | dispatch
    {
        $$ = $1;
    }
 
    
    
    operation:
    operand
    {
        $$ = $1;
    }
    | operation '+' operation
    {/*addition*/
        $$ = plus($1,$3);
    }
    | operation '-' operation
    {/*substraction*/
        $$ = sub($1,$3);
    }
    | operation '*' operation
    {/*multiplication*/
        $$ = mul($1,$3);
    }
    | operation '/' operation
    {/*division*/
        $$ = divide($1,$3);
    }
    | operation '<' operation
    {
        $$ = lt($1,$3);
    }
    | operation LE operation
    {/*less or equal*/
        $$ = leq($1,$3);
    }
    | operation '=' operation
    {/*equals*/
        $$ = eq($1,$3);
    }
    | NOT operation
    {/*binary complement*/
        $$ = comp($2);
    }
    | '~' operation
    {/*integer negation*/
        $$ = neg($2);
    }
    | ISVOID operation
    {/*isvoid*/
        $$ = isvoid($2);
    }
    |'(' operation ')' 
    { /*parentheses*/
        $$ = $2;
    }
    
    
  
    /* end of grammar */
    %%
    
    /* This function is called automatically when Bison detects a parse error. */
    void yyerror(char *s)
    {
      extern int curr_lineno;
      
      cerr << "\"" << curr_filename << "\", line " << curr_lineno << ": " \
      << s << " at or near ";
      print_st_token(yychar);
      cerr << endl;
      omerrs++;
      
      if(omerrs>50) {fprintf(stdout, "More than 50 errors\n"); exit(1);}
    }
    
    
