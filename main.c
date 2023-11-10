#include "sodium.h"

int main(int argc,char **argv) {
    if(argc !=2)
        error("%s: 參數數量無效", argv[0]);
    
    //標記和解析
    user_input = argv[1];
    token = tokenize();
    Function *prog = program();
    
    // 將偏移量分配給局部變數 Assign offsets to local variables
    for (Function *fn = prog; fn; fn = fn->next) {
        int offset =0;
        for (Var *var = prog->locals; var; var = var->next) {
            offset +=8;
            var->offset = offset;
        }
        fn->stack_size = offset;
    }
    // 遍歷 AST 以發出程式集。 Traverse the AST to emit assembly.
    codegen(prog);
    
    return 0;
}