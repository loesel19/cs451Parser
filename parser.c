/* parser.c - a parser for simple statements */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

int charClass;
char lexeme [100];
char nextChar;
int lexLen;
int token;
int nextToken;
FILE *in_fp, *fopen();

void addChar();
void getChar();
void getNonBlank();
int lex();
void expr();
void term();
void factor();
void booexpr();
void statement();
void statements();
void comp();
void assign();
void forstmt();
void whilestmt();
void ifstmt();
void starter();

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define IF_TOKEN 30
#define WHILE_TOKEN 31
#define FOR_TOKEN 32
#define ELSE_TOKEN 33
#define GREATER_THAN 40
#define LESS_THAN 41
#define SEMICOLON 50

/******************************************************/
/* main driver */
int main(int *argc, char *argv[]) {
    /* Open the input data file and process its contents */
    if ((in_fp = fopen(argv[1], "r")) == NULL)
    printf("ERROR - cannot open %s \n", argv[2]);
    else {
        getChar();
        do {
            lex();
	    starter();
        } while (nextToken != EOF);
    }
    return 0;
}


void starter(){
if(nextToken < 30){
    /* we go into expr since we are dealing with an expression */
    expr();
}else{
    /* tokens greater than 30 are for statements so we call statements to start */
    statements();
}
}

/*****************************************************/
/* lookup - a function to lookup operators and parentheses
and return the token */
int lookup(char ch) {
    switch (ch) {
        case '(':
        addChar();
        nextToken = LEFT_PAREN;
        break;
        case ')':
        addChar();
        nextToken = RIGHT_PAREN;
        break;
        case '+':
        addChar();
        nextToken = ADD_OP;
        break;
        case '-':
        addChar();
        nextToken = SUB_OP;
        break;
        case '*':
        addChar();
        nextToken = MULT_OP;
        break;
        case '/':
        addChar();
        nextToken = DIV_OP;
        break;
        case '=':
        addChar();
        nextToken = ASSIGN_OP;
        break;
        /* add cases for < > */
        case '>':
        addChar();
        nextToken = GREATER_THAN;
        break;
        case '<':
        addChar();
        nextToken = LESS_THAN;
        break;
        case ';':
        addChar();
        nextToken = SEMICOLON;
        break;
        default:
        addChar();
        nextToken = EOF;
        break;
    }
    return nextToken;
}
/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
    else
    printf("Error - lexeme is too long \n");

}
/*****************************************************/
/* getChar - a function to get the next character of
input and determine its character class */
void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {
        if (isalpha(nextChar))
        charClass = LETTER;
        else if (isdigit(nextChar))
        charClass = DIGIT;
        else charClass = UNKNOWN;
    }
    else
    charClass = EOF;

}
/*****************************************************/
/* getNonBlank - a function to call getChar until it
returns a non-whitespace character */
void getNonBlank() {
    while (isspace(nextChar))
    getChar();
}

/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic expressions */
int lex() {
    lexLen = 0;
    getNonBlank();
    switch (charClass) {
        /* Parse identifiers */
        case LETTER:
        addChar();
        getChar();
        while (charClass == LETTER || charClass == DIGIT) {
            addChar();
            getChar();
        }
        /*we need to check if our lexem is equal to while | for | if | else */
        if(!strcmp(lexeme, "while")){
            nextToken = WHILE_TOKEN;
        }else if(!strcmp(lexeme, "for")){
            nextToken = FOR_TOKEN;
        }else if(!strcmp(lexeme, "if")){
            nextToken = IF_TOKEN;
        }else if(!strcmp(lexeme, "else")){
            nextToken = ELSE_TOKEN;
        }else{
        nextToken = IDENT;
        }
        break;

        /* Parse integer literals */
        case DIGIT:
        addChar();
        getChar();
        while (charClass == DIGIT) {
            addChar();
            getChar();
        }
        nextToken = INT_LIT;
        break;
        /* Parentheses and operators */
        case UNKNOWN:
        lookup(nextChar);
        getChar();
        break;
        /* EOF */
        case EOF:
        nextToken = EOF;
        lexeme[0] = 'E';
        lexeme[1] = 'O';
        lexeme[2] = 'F';
        lexeme[3] = 0;
        break;
    } /* End of switch */

    printf("Next token is: %d, Next lexeme is %s \n",
    nextToken, lexeme);
    return nextToken;
} /* End of function lex */

/* Parses strings in the language generated by the rule:
   <expr> -> <term> {(+ | -) <term>} */
void expr() {
    printf("Enter <expr>\n");
    /* Parse the first term */
    term();
    /* As long as the next token is + or -, get
    the next token and parse the next term */
    while (nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
        term();
    }
    printf("Exit <expr>\n");
}

/* Parses strings in the language generated by the rule:
   <term> -> <factor> {(* | /) <factor>) */
void term() {
    printf("Enter <term>\n");
    /* Parse the first factor */
    factor();
    /* As long as the next token is * or /, get the
    next token and parse the next factor */
    while (nextToken == MULT_OP || nextToken == DIV_OP) {
        lex();
        factor();
    }
    printf("Exit <term>\n");
}

/* Parses strings in the language generated by the rule:
    <factor> -> id | int_constant | ( <expr ) */
void factor() {
    printf("Enter <factor>\n");
    /* Determine which RHS */
    if (nextToken == IDENT || nextToken == INT_LIT){
        printf("Identifier or int literal\n");
        /* Get the next token */
        lex();
    /* If the RHS is ( <expr>), call lex to pass over the left parenthesis,
       call expr, and check for the right parenthesis */
    }else {
        if (nextToken == LEFT_PAREN) {
            lex();
            expr();
            if (nextToken == RIGHT_PAREN)
                lex();
            else
                printf("Error\n");
        } /* End of if (nextToken == ... */
        /* It was not an id, an integer literal, or a left parenthesis */
        else
            printf("Error\n");
    } /* End of else */
    printf("Exit <factor>\n");;
}

/* parses strings in the language generated by the rule:
    <comp> -> < | > */
void comp(){
    printf("Enter <comp>\n");
if (nextToken == LESS_THAN)
{
    printf("Less Than.\n");
}else if(nextToken == GREATER_THAN){
    printf("Greater Than\n");
}else{
    printf("Error\n");
}

    printf("Exit <comp>\n");
}

/* parses strings in the language generated by the rule:
    <booexpr> -> id <comp> int_constant */
void booexpr(){
    printf("Enter <booexpr>\n");
    lex();
    /* first check LHS is an id */
    if (nextToken == IDENT)
    {
        printf("Identifier\n");
        /* call lex to get next token, and then call comp to make sure it is < or >. then lex again and check 
        if it is an int_contant */
        lex();
        comp();
        lex();
        if(nextToken == INT_LIT){
            printf("Int Literal\n");
            lex();
        }else{
            printf("Error\n");
        }
    }else{
        printf("Error\n");
    }

    printf("Exit <booexpr>\n");
    
}

/* parses strings in the language generated by the rule:
    <asgnstmt> -> id = <expr> */

void assign(){
    printf("Enter <assignstmt>\n");
    if(nextToken == IDENT){
        lex();
        if(nextToken == ASSIGN_OP){
            lex();
            expr();
        }else{
            printf("Error\n");
        }
    }else{
        printf("Error\n");
    }
    printf("Exit <assignstmt>\n");
}

/* parses strings in the language generated by the rule:
    <stmts> -> <stmt> {; <stmt>} */
void statements(){
    printf("Enter <stmts>\n");
    statement();
    lex();
    printf("{;\n");
    statement();
    lex();
    printf("}");
    printf("Exit <stmts>\n");

}
/* parses strings in the language generated by the rule:
    <stmt> -> <ifstmt> | <whilestmt> | <forstmt> | <asgnstmt> */
void statement(){
/* check next token to see what we are doing next */
if(strcmp(lexeme, "EOF")){
    printf("Enter <stmt>\n");
    if(nextToken == IF_TOKEN){
        ifstmt();
    }else if(nextToken == WHILE_TOKEN){
        whilestmt();
    }else if(nextToken == FOR_TOKEN){
        forstmt();
    }else if(nextToken == ELSE_TOKEN){
        /*this is used to catch in2.txt */
        statements();
    }else{
        assign();
    }
    printf("Exit <stmt>\n");
}
}

/* parses strings in the language generated by the rule:
    <forstmt> -> for (<assignstmt> ; <boolexpr> ; <assignstmt> ;) <stmts> */
void forstmt(){
    printf("Enter <forstmt>\n");
    if(nextToken == FOR_TOKEN){
        printf("FOR\n");
        lex();
        if(nextToken == LEFT_PAREN){
            lex();
            assign();
            printf(";\n");
            booexpr();
            printf(";\n");
            lex();
            assign();
            if(nextToken == RIGHT_PAREN){
                lex();
                statements();
            }else{
                printf("Error\n");
            }
        }else{
            printf("Error\n");
        }
    }else{
        printf("Error\n");
    }
    printf("Exit <forstmt>\n");
}

/* parses strings in the language generated by the rule:
    <whilestmt> -> while (<boolexpr>) <stmts> */
void whilestmt(){
    printf("Enter <whilestmt>\n");
    if(nextToken == WHILE_TOKEN){
        printf("WHILE\n");
        lex();
        if(nextToken == LEFT_PAREN){
            booexpr();
            if(nextToken == RIGHT_PAREN){
                lex();
                statements();
            }else{
                printf("Error\n");
            }
        }else{
            printf("Error\n");
        }
    }else{
        printf("Error\n");
    }
    printf("Exit <whilestmt>\n");
}

/* parses strings in the language generated by the rule:
    <ifstmt> -> if (<boolexpr>) <stmts> [else <stmts>] */
void ifstmt(){
    printf("Enter <ifstmt>\n");
    if(nextToken == IF_TOKEN){
        printf("IF\n");
        lex();
        if(nextToken == LEFT_PAREN){
            booexpr();
            if(nextToken == RIGHT_PAREN){
                lex();
                statements();
                lex();
                if(nextToken == ELSE_TOKEN){
                    lex();
                    statements();
                    lex();
            }else{
                printf("No else\n");
            }
            }else{
                printf("Error\n");
            }
            
        }else{
            printf("Error\n");
        }
    }else{
        printf("Error\n");
    }
    printf("Exit <ifstmt>\n");
}
