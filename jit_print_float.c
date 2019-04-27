#include "src\asm.h"

ASM *a;
float f = 1700.50;
int i = 750;
char *str = "Variable i: %d\n";

void funcao() {
    if ((a = asm_New(100)) == NULL)
        return;
    emit_begin(a);
//      emit_func_arg_var_float1 (a, &f);
      emit_mov_var_reg(a,&i,EDX);
      // 48 8b 0c 25 30 7a 40 00 	mov    0x407a30,%rcx
      emit_mov_var_reg(a,&str,RCX);
      emit_call(a,printf,0,0);
    emit_end(a);
    if (asm_SetExecutable_ASM(a,0)==0) {
        asm_Run(a);
    }
    ASM_FREE(a);
}

int main() {
    funcao();
    printf ("Exiting With Sucess:\n");
    return 0;
}


