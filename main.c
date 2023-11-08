#include "sodium.h"

int main(int argc,char **argv) {
    if(argc !=2)
        error("%s: 參數數量無效", argv[0]);
    
    //標記和解析
    user_input = argv[1];
    token = tokenize();
    Node *node = program();
    
    // 遍歷 AST 以發出程式集。
    codegen(node);
    return 0;
}