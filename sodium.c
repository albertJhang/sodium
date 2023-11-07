#include<ctype.h>
#include<stdarg.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//
//Tokenizer
//

//標記種類
typedef enum{
    TK_RESERVED,//符號
    TK_NUM,//整數標記
    TK_EOF,//代表輸入結束的標記
} TokenKind;

typedef struct Token Token;

//標記型態
struct Token{
    TokenKind kind;//標記的型態
    Token *next;//下一個輸入標記
    int val;//kind為TK_NUM時的數值
    char *str; //標記文字列
};

char *user_input;

//正在處理的標記
Token *token;

//處理錯誤的函數
//取和printf相同的引數
void error(char *fmt,...){
    va_list ap;
    va_start(ap,fmt);
    vfprintf(stderr,fmt,ap);
    fprintf(stderr,"\n");
    exit(1);
}

// Reports an error location and exit.
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, ""); // print pos spaces.
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}
//下一個標記為符合預期的符號時，讀入一個標記並往下繼續，
//回傳ture。否則0回傳false。
bool consume(char op){
    if(token->kind!=TK_RESERVED||token->str[0]!=op)
        return false;
    token=token->next;
    return true;
}
//下一個標記為符合預期的符號時，讀入一個標記並往下繼續，
//否則警告為錯誤。
void expect(char op){
    if(token->kind!=TK_RESERVED||token->str[0]!=op)
        error_at(token->str, "不是'%c'", op);
    token=token->next;
}
// 下一個標記為數值時，讀入一個標記並往下繼續，
//否則警告為錯誤。
int expect_number(){
    if(token->kind!=TK_NUM)
        error_at(token->str, "不是數值");
    int val=token->val;
    token=token->next;
    return val;
}

bool at_eof(){
    return token->kind==TK_EOF;
}

//建立一個新的標記，連結到cur
Token *new_token(TokenKind kind,Token *cur,char *str){
    Token *tok=calloc(1,sizeof(Token));
    tok->kind=kind;
    tok->str=str;
    cur->next=tok;
    return tok;
}

//將輸入文字列p作標記解析並回傳標記連結串列
Token *tokenize(){
    char *p = user_input;
    Token head;
    head.next=NULL;
    Token *cur=&head;

    while(*p){
        //跳過空白符號
        if(isspace(*p)){
            p++;
            continue;
        }
        
        //標點符號
        if(strchr("+-*/()", *p)){
            cur=new_token(TK_RESERVED,cur,p++);
            continue;
        }
        
        if(isdigit(*p)){
            cur=new_token(TK_NUM,cur,p);
            cur->val=strtol(p,&p,10);
            continue;
        }
        
        error_at(p, "令牌無效");
    }
    
    new_token(TK_EOF,cur,p);
    return head.next;
}

//
//解析器
//

typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // 整數
} NodeKind;

//AST 節點類型
typedef struct Node Node;
struct Node {
    NodeKind kind; //節點種類
    Node *lhs;     //左手邊
    Node *rhs;     //右手邊
    int val;       //如果 kind == ND_NUM 則使用
};

Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_num(int val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

Node *expr();
Node *mul();
Node *primary();

//expr = mul("+" mul | "-" mul)*
Node *expr(){
    Node *node = mul();

    for(;;) {
       if (consume('+'))
            node = new_binary(ND_ADD, node, mul());
       else if(consume('-'))
            node = new_binary(ND_SUB, node, mul());
       else
            return node;
    }
}

//mul = primary("*" primary | "/" primary)*
Node *mul() {
    Node *node = primary();

    for(;;) {
        if(consume('*'))
            node = new_binary(ND_MUL, node, primary());
        else if(consume('/'))
            node = new_binary(ND_DIV, node, primary());
        else
            return node;
    }
}

//primary = "(" expr ")" | num
Node *primary() {
    if(consume('(')){
        Node *node = expr();
        expect(')');
        return node;
    }

    return new_num(expect_number());
}

//
//Code generator程式碼產生器
//

void gen(Node *node) {
    if(node->kind == ND_NUM) {
        printf("    push %d\n", node->val);
        return ;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch(node->kind) {
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("   imul rax, rdi\n ");
        break;
    case ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;
    }

    printf("    push rax\n");
}

int main(int argc,char **argv)
{
    if(argc !=2)
        error("%s: 參數數量無效", argv[0]);
    //標記解析
    user_input = argv[1];
    token = tokenize();
    Node *node = expr();
    
    //輸出前半部份組合語言指令
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // 遍歷 AST 以發出程式集。
    gen(node);

    // 結果必須位於堆疊頂部，因此將其彈出
    // 到 RAX 使其成為程式退出程式碼。
    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}