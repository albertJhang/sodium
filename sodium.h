#define _GNU_SOURCE
#include<ctype.h>
#include<stdarg.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//
//標記解析器 Tokenizer.c
//

// 標記 Token
typedef enum{
    TK_RESERVED,//關鍵字或標點符號 Keywords or punctuators
    TK_IDENT,   //身份識別 Identifiers
    TK_NUM,     //整數文字 Integer literals
    TK_EOF,     //文件結束標記 End-of-file markers
} TokenKind;

//標記型態 Token type
typedef struct Token Token;
struct Token{
    TokenKind kind; //標記種類 Token kind
    Token *next;    //下一個輸入標記 Next token
    int val;        //如果kind為TK_NUM，則為數值 If kind is TK_NUM, it's value
    char *str;      //標記文字列 Token string
    int len;        //標記長度 Token length
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
Token *consume(char *op);
Token *consume_ident(void);
void expect(char *op);
long expect_number(void);
char *expect_ident(void);
bool at_eof(void);
Token *tokenize(void);

extern char *user_input;
extern Token *token;

//
// 分析器 parse.c
//

// 區域變數 Local variable
typedef struct Var Var;
struct Var {
    char *name; // 變數名稱 Variable name
    int offset; // 與RBP的偏移 Offset from RBP 
};

typedef struct VarList VarList;
struct VarList {
    VarList *next;
    Var *var;
};

//AST node
typedef enum {
    ND_ADD,         // +
    ND_SUB,         // -
    ND_MUL,         // *
    ND_DIV,         // /
    ND_EQ,          // ==
    ND_NE,          // !=
    ND_LT,          // <
    ND_LE,          // <=
    ND_ASSIGN,      // = 
    ND_RETURN,      // "return"
    ND_IF,          // "if"
    ND_WHILE,       // "while"
    ND_FOR,         // "for"
    ND_BLOCK,       // "block"
    ND_FUNCALL,     // Function call
    ND_EXPR_STMT,   // 表達語句 Expression statement
    ND_VAR,         // 變數 Variable
    ND_NUM,         // 整數
} NodeKind;

//AST 節點類型
typedef struct Node Node;
struct Node {
    NodeKind kind; // 節點種類
    Node *next;    // 下個節點
    Token *tok;    // 代表標記 Representative token

    Node *lhs;     // 左手邊
    Node *rhs;     // 右手邊

    // "if", "while" or "for" 表達式
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    
    //Block
    Node *body;

    // 函數呼叫
    char *funcname;
    Node *args;

    Var *var;      // 如果 kind == ND_VAR 則使用 Used if kind == ND_VAR
    long val;      // 如果 kind == ND_NUM 則使用 Used if kind == ND_NUM
};

typedef struct Function Function;
struct Function {
    Function *next;
    char *name;
    VarList *params;

    Node *node;
    VarList *locals;
    int stack_size;
};

Function *program(void);

//
//指令產生器 codegen.c
//

void codegen(Function *prog);