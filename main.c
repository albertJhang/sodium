#include "sodium.h"

int align_to(int n, int align) {
    return (n + align - 1)& ~(align - 1);
}

int main(int argc,char **argv) {
    if(argc !=2)
        error("%s: 參數數量無效", argv[0]);
    
    //標記和解析
    user_input = argv[1];
    token = tokenize();
    Program *prog = program();
    
    // 將偏移量分配給局部變數 Assign offsets to local variables
    for (Function *fn = prog->fns; fn; fn = fn->next) {
        int offset =0;
        for (VarList *vl = fn->locals; vl; vl = vl->next) {
            Var *var = vl->var;
            offset += var->ty->size;
            var->offset = offset;
        }
        fn->stack_size = align_to(offset, 8);
    }
    // 遍歷 AST 以發出程式集。 Traverse the AST to emit assembly.
    codegen(prog);
    
    return 0;
}