#include "sodium.h"

// 傳回給定文件的內容 Returns the contents of a given file.
static char *read_file(char *path) {
    // 開啟並讀取文件 Open and read the file.
    FILE *fp = fopen(path, "r");
    if (!fp)
        error("cannot open %s: %s", path, strerror(errno));

    int filemax = 10 * 1024 * 1024;
    char *buf = malloc(filemax);
    int size = fread(buf, 1, filemax - 2, fp);
    if (!feof(fp))
        error("%s: file too large");

    // 確保字串以"\n\0"結尾 Make sure that the string ends with "\n\0".
    if (size == 0 || buf[size - 1] != '\n')
        buf[size++] = '\n';
    buf[size] = '\0';
    return buf;
}

int align_to(int n, int align) {
    return (n + align - 1)& ~(align - 1);
}

int main(int argc,char **argv) {
    if(argc !=2)
        error("%s: 參數數量無效", argv[0]);
    
    //標記和解析
    filename = argv[1];
    user_input = read_file(argv[1]);
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