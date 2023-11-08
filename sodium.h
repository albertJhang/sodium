#include<ctype.h>
#include<stdarg.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//
//分詞器 Tokenizer.c
//

typedef enum{
    TK_RESERVED,//關鍵字或標點符號 Keywords or punctuators
    TK_NUM,//整數文字 Integer literals
    TK_EOF,//文件結束標記 End-of-file markers
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
bool consume(char *op);
void expect(char *op);
long expect_number(void);
bool at_eof(void);
Token *tokenize(void);

extern char *user_input;
extern Token *token;

//
//解析器 parse.c
//

typedef enum {
    ND_ADD,     // +
    ND_SUB,     // -
    ND_MUL,     // *
    ND_DIV,     // /
    ND_EQ,      //==
    ND_NE,      //!=
    ND_LT,      //<
    ND_LE,      //<= 
    ND_NUM,     // 整數
} NodeKind;

//AST 節點類型
typedef struct Node Node;
struct Node {
    NodeKind kind; //節點種類
    Node *next;    //下個節點
    Node *lhs;     //左手邊
    Node *rhs;     //右手邊
    int val;       //如果 kind == ND_NUM 則使用
};

Node *program(void);

//
//程式碼產生器 codegen.c
//

void codegen(Node *node);