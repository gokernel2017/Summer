//-------------------------------------------------------------------
//
// Fast JIT (just-in-time):
//
// TANKS TO:
// ----------------------------------------------
//
//   01: God the creator of the heavens and the earth in the name of Jesus Christ.
//
//   02 - Fabrice Bellard: www.bellard.org
//
// ----------------------------------------------
//
// FILE:
//   asm.c
//
// START DATE:
//   23/08/2017 - 11:45
//
// BY Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "asm.h"

#define STR_ERRO_SIZE   1024

float asmFvalue;

// global:
int erro;

static char strErro[STR_ERRO_SIZE];
static void asm_change_jump (ASM *a);

ASM *asm_new (unsigned long size) {
    ASM *a = (ASM*)malloc(sizeof(ASM));

    if (a && (a->code=(UCHAR*)malloc(size)) != NULL) {
        a->p     = a->code;
        a->label = NULL;
        a->jump  = NULL;
        a->size  = size;
        return a;
    }
    return NULL;
}
void asm_free (ASM *a) {
    if (a) {
        free (a->code);

        // free ASM_label:
        while (a->label != NULL) {
            ASM_label *temp = a->label->next;
            if (a->label->name)
                free (a->label->name);
            free (a->label);
            a->label = temp;
        }
        // free ASM_jump:
        while (a->jump != NULL) {
            ASM_jump *temp = a->jump->next;
            if (a->jump->name)
                free (a->jump->name);
            free (a->jump);
            a->jump = temp;
        }

        free (a);
    }
}
void asm_reset (ASM *a) {

    a->p = a->code;

    // reset ASM_label:
    while (a->label != NULL) {
        ASM_label *temp = a->label->next;
        if (a->label->name)
            free (a->label->name);
        free (a->label);
        a->label = temp;
    }
    // reset ASM_jump:
    while (a->jump != NULL) {
        ASM_jump *temp = a->jump->next;
        if (a->jump->name)
            free(a->jump->name);
        free (a->jump);
        a->jump = temp;
    }

    a->label = NULL;
    a->jump  = NULL;
}
//-------------------------------------------------------------------
// This function use the code of Fabrice Bellard:
//
//   LIB:  tcc-0.9.25
//   FILE: libtcc.c
//   FUNC: void set_pages_executable (void *ptr, unsigned long length);
//   LINE: 400
//
// Set executable: a->code
//
//-------------------------------------------------------------------
int asm_set_executable (void *ptr, unsigned long len) {

    if (asm_ErroGet()) return 1;

#ifdef WIN32
    unsigned long old_protect;

    if (!VirtualProtect(ptr, len, PAGE_EXECUTE_READWRITE, &old_protect)) {
        asm_Erro ("ERROR: asm_set_executable() ... NOT FOUND - VirtualProtect()\n");
        return 1; // erro
    }
#endif

#ifdef __linux__
    unsigned long start, end, PageSize;

    PageSize = sysconf (_SC_PAGESIZE);
    start = (unsigned long)ptr & ~(PageSize - 1);
    end = (unsigned long)ptr + len;
    end = (end + PageSize - 1) & ~(PageSize - 1);
    if (mprotect((void *)start, end - start, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
        asm_Erro ("ERROR: asm_set_executable() ... NOT FOUND - mprotec()\n");
        return 1; // erro
    }
#endif

    return 0; // no erro
}
void asm_label (ASM *a, char *name) {
    if (name) {
        ASM_label *lab;
        ASM_label *l = a->label;

        // find if exist:
        while (l) {
            if (!strcmp(l->name, name)) {
                printf ("Label Exist: '%s'\n", l->name);
                return;
            }
            l = l->next;
        }
        
        if ((lab = (ASM_label*)malloc (sizeof(ASM_label))) != NULL) {

            lab->name = strdup (name);
            lab->pos  = (a->p - a->code); // the index

            // add on top:
            lab->next = a->label;
            a->label = lab;
        }
    }
}

void asm_Erro (char *s) {
    erro++;
    if ((strlen(strErro) + strlen(s)) < STR_ERRO_SIZE)
        strcat (strErro, s);
}
char *asm_ErroGet (void) {
    if (strErro[0])
        return strErro;
    else
        return NULL;
}
void asm_ErroReset (void) {
    erro = 0;
    strErro[0] = 0;
}

//-------------------------------------------------------------------
//--------------------------  GEN / EMIT  ---------------------------
//-------------------------------------------------------------------
//
void g (ASM *a, UCHAR c) {
    *a->p++ = c;
}
void g2 (ASM *a, UCHAR c1, UCHAR c2) {
    a->p[0] = c1;
    a->p[1] = c2;
    a->p += 2;
}
void g3 (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3) {
    a->p[0] = c1;
    a->p[1] = c2;
    a->p[2] = c3;
    a->p += 3;
}
void g4 (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4) {
    a->p[0] = c1;
    a->p[1] = c2;
    a->p[2] = c3;
    a->p[3] = c4;
    a->p += 4;
}
void asm_begin (ASM *a) {
    a->p[0] = 0x55; // 55 : push  %ebp
    // 89 e5  : mov     %esp,%ebp
    a->p[1] = 0x89;
    a->p[2] = 0xe5;
    a->p += 3;
}
void asm_end (ASM *a) {
    a->p[0] = 0xc9; // c9 : leave
    a->p[1] = 0xc3; // c3 : ret
    a->p += 2;
    if ((a->p - a->code) > a->size) {
        asm_Erro ("ASM ERRO: code > size\n");
        return;
    }
    asm_change_jump (a);
}
void asm_get_addr (ASM *a, void *ptr) {
    *(void**)a->p = ptr;
    a->p += sizeof(void*);
}
void asm_movl (ASM *a, long value, void *var) {
    //  c7 05     60 40 40 00     7b 00 00 00   movl    $0x7b,0x404060
    g2 (a,0xc7,0x05); asm_get_addr (a,var);
    *(long*)a->p = value;
    a->p += sizeof(long);
}
void asm_incl (ASM *a, void *var) {
    g2(a,0xff,0x05); asm_get_addr(a,var); // ff 05    00 20 40 00   :   incl   0x402000
}
void asm_cmpl (ASM *a, long value, void *var) {
    if (value == (char)value) { // char:  83 3d    70 40 40 00   64   cmpl   $0x64,0x404070
        g2 (a,0x83,0x3d);
        asm_get_addr(a, var);
        g(a,(char)value);
    } else { // long: 81 3d    60 40 40 00   e8 03 00 00   cmpl   $0x3e8,0x404060
        g2 (a,0x81,0x3d);
        asm_get_addr(a,var);
        *(long*)a->p = value;
        a->p += sizeof(long);
    }
}
//-------------------------------------------------------------------
// THANKS TO:
//
//   Fabrice Bellard: www.bellard.org
//
// CODE BASED:
// -------------------------------
//   LIB:  tcc-0.9.25
//   FILE: i386-gen.c
//   FUNC: void gjmp_addr (int a);
//   LINE: 568
// -------------------------------
//
// generate a jump to a fixed address
//
// NOTE: This jump is in the code[index]
//
//-------------------------------------------------------------------
//
static void asm_conditional_jump (ASM *a, char *name, int type) {
    ASM_jump *jump;

    if (name && (jump = (ASM_jump*)malloc (sizeof(ASM_jump))) != NULL) {
        jump->name = strdup (name);
        jump->pos  = (a->p - a->code); // the index
        jump->type = type;

        // add on top:
        jump->next = a->jump;
        a->jump = jump;

        // to change ...
        g(a,OP_NOP); g(a,OP_NOP);
        g(a,OP_NOP); g(a,OP_NOP); g(a,OP_NOP); g(a,OP_NOP);
    }
}
void asm_jmp (ASM *a, char *name) {
    ASM_jump *jump;

    if (name && (jump = (ASM_jump*)malloc (sizeof(ASM_jump))) != NULL) {

        g(a,OP_NOP); // change this in ( asm_change_jump ) to OPCODE: 0xe9

        jump->name = strdup (name);
        jump->pos  = (a->p - a->code); // the index
        jump->type = ASM_JUMP_JMP;

        // add on top:
        jump->next = a->jump;
        a->jump = jump;

        // to change ...
        g(a,OP_NOP); g(a,OP_NOP); g(a,OP_NOP); g(a,OP_NOP);
    }
}
void asm_jg (ASM *a, char *name) {
    asm_conditional_jump (a, name, ASM_JUMP_JG);
}
static void asm_change_jump (ASM *a) {
    ASM_label *label = a->label;

    while (label) {

        ASM_jump  *jump  = a->jump;

        while (jump) {

            if (!strcmp(label->name, jump->name)) {
                int jump_pos  = jump->pos;
                int label_pos = label->pos;

                switch (jump->type) {
                case ASM_JUMP_JMP:
                    {
                    *(UCHAR*)(a->code+jump_pos-1) = 0xe9; // OPCODE ( jmp )
                    *(int*)(a->code+jump_pos) = (int)(label_pos - jump_pos - 4);
                    }
                    break;

                case ASM_JUMP_JG:
                    {
                    int r = label_pos - jump_pos - 2;

                    if (r == (char)r) { // 2 bytes
                        // 7f 08                	 jg     4012b1 < _code + number >
                        //
                        *(UCHAR*)(a->code+jump_pos) = 0x7f;
                        *(UCHAR*)(a->code+jump_pos+1) = r;
                    } else { // 6 bytes
                        // 0f 8f    bb 00 00 00     jg     4013a7 < _code + number >
                        //
                        *(UCHAR*)(a->code+jump_pos) = 0x0f;
                        *(UCHAR*)(a->code+jump_pos+1) = 0x8f;
                        *(int*) (a->code+jump_pos+2) = (int)(label_pos - jump_pos - 6);
                    }
                    }
                    break;//: case ASM_JUMP_JG:

                }//: switch (jump->type)

            }//: if (!strcmp(label->name, jump->name))

            jump = jump->next;

        }//: while (jump)

        label = label->next;

    }//: while (label)

}//: static void asm_change_jump (ASM *a)

void asm_call (ASM *a, void *func, UCHAR argc) {
//void asm_call (ASM *a, void *func) {
    // b8   7a 13 40 00       mov    $0x40137a,%eax
    // ff d0                	call   *%eax
    //
    g(a,0xb8); asm_get_addr(a, func);
    g2(a,0xff,0xd0);
}
void asm_sub_esp (ASM *a, char c) {
    g3(a,0x83,0xec,(char)c); // 83 ec     08   sub  $0x8,%esp
}

void asm_popl_var (ASM *a, void *var) {
    // stack--;
    g2(a,0x8f,0x05); asm_get_addr(a, var); // 8f 05    60 40 40 00    popl   0x404060
}

//------------------------------------------------
void asm_imul_eax_esp (ASM *a) {
//    stack--;
    g(a,0x58);                  // 58               pop  %eax
    g4(a,0x0f,0xaf,0x04,0x24);  // 0f af 04 24      imul   (%esp),%eax
    g3(a,0x89,0x04,0x24);       // 89 04 24         mov    %eax,(%esp)
}
void asm_idivl_eax_esp (ASM *a) {
//    stack--;
//    stack--;
    g(a,0x59);        // 59       pop    %ecx
    g(a,0x58);        // 58       pop    %eax
    g(a,0x99);        // 99       cltd
    g2(a,0xf7,0xf9);  // f7 f9    idiv   %ecx
    g(a,0x50);        // 50       push   %eax
//    stack++;
}
void asm_add_eax_esp (ASM *a) {
//    stack--;
    g(a,0x58);              // 58         pop  %eax
    g3(a,0x01,0x04,0x24);   // 01 04 24   add   %eax,(%esp)
}
void asm_sub_eax_esp (ASM *a) {
//    stack--;
    g(a,0x58);            // 58           pop  %eax
    g3(a,0x29,0x04,0x24); // 29 04 24     sub    %eax,(%esp)
}
//------------------------------------------------

// push variable on: %esp:
//
void asm_pushl_var (ASM *a, void *var) {
//    stack++;
    g2(a,0xff,0x35); asm_get_addr(a,var); // ff 35    60 40 40 00   pushl   0x404060
}
// push number on: %esp:
//
void asm_push_number (ASM *a, long value) {
//    stack++;
    if (value == (char)value) {
        g(a,0x6a);  // 6a   64    push   $0x64
        *a->p = value;
        a->p += sizeof(char);
    } else {
        g(a,0x68); // 68    00 e1 f5 05    push  $0x5f5e100
        *(long*)a->p = value;
        a->p += sizeof(long);
    }
}

//-------------------------------------------------------------------
//###########################  MATH FLOAT  ##########################
//-------------------------------------------------------------------
//
// load a float value:
// USAGE:
//   asm_float_flds_value (a, 12345.00);
//
void asm_float_flds_value (ASM *a, float value) {
    // b8     9a 19 c9 42       	mov    $0x42c9199a,%eax
    //
    g(a,0xb8);
    *(float*)a->p = value;
    a->p += sizeof(float);

    // a3     00 20 40 00       	mov    %eax,0x402000
    //
    g(a,0xa3);
    *(void**)a->p = &asmFvalue;
    a->p += sizeof(void*);

    // FLDS VARIABLE ( asmFvalue ):
    //
    // d9 05    04 20 40 00    	flds   0x402004
    //
    g2 (a,0xd9,0x05);
    *(void**)a->p = &asmFvalue;
    a->p += sizeof(void*);
}

//
// load a float variable:
// USAGE:
//   asm_float_flds (a, &var_name);
//
void asm_float_flds (ASM *a, void *var) {
    // d9 05    04 20 40 00    	flds   0x402004
    //
    g2(a,0xd9,0x05);
    *(void**)a->p = var;
    a->p += sizeof(void*);
}

//
// multiply the stack float:
// USAGE:
//   asm_float_fmulp (a);
//
void asm_float_fmulp (ASM *a) { g2 (a,0xde,0xc9); } // de c9    fmulp  %st,%st(1)

//
// divide the stack float:
// USAGE:
//   asm_float_fmulp (a);
//
void asm_float_fdivp (ASM *a) { g2(a,0xde,0xf1); } // de f1    fdivp  %st,%st(1)

//
// add the stack float:
// USAGE:
//   asm_float_faddp (a);
//
void asm_float_faddp (ASM *a) { g2 (a,0xde,0xc1); } // de c1    faddp  %st,%st(1)

//
// sub the stack float:
// USAGE:
//   asm_float_faddp (a);
//
void asm_float_fsubp (ASM *a) { g2 (a,0xde,0xe1); } // de e1    fsubp  %st,%st(1)

//
// set the variable and CLEAR the stack:
// USAGE:
//   asm_float_fstps (a, &var_name);
//
void asm_float_fstps (ASM *a, void *var) {
    // d9 1d    00 20 40 00    	fstps  0x402000
    //
    g2(a,0xd9,0x1d);
    *(void**)a->p = var;
    a->p += sizeof(void*);
}

void asm_mov_reg_var (ASM *a, int reg, void *var) { // move: %register to variable
    if (reg >= 0 && reg <= 7) {
        switch (reg) {
        case EAX: g(a,0xa3);       break; // a3       10 40 40 00   mov   %eax, 0x404010
        case ECX: g2(a,0x89,0x0d); break; // 89 0d    60 40 40 00   mov   %ecx, 0x404060
        case EDX: g2(a,0x89,0x15); break; // 89 15    60 40 40 00   mov   %edx, 0x404060
        case EBX: g2(a,0x89,0x1d); break;//  89 1d    60 40 40 00   mov   %ebx, 0x404060
        default: return;
        }
        asm_get_addr(a,var);
    }
}
void asm_mov_var_reg (ASM *a, void *var, int reg) { // move: variable to %register
    if (reg >= 0 && reg <= 7) {
        switch (reg) {
        case EAX: g(a,0xa1);       break; // a1       60 40 40 00   mov   0x404060, %eax
        case ECX: g2(a,0x8b,0x0d); break;	// 8b 0d    70 40 40 00   mov   0x404070, %ecx
        case EDX: g2(a,0x8b,0x15); break; // 8b 15    70 40 40 00   mov   0x404070, %edx
        case EBX: g2(a,0x8b,0x1d); break; // 8b 1d    60 40 40 00   mov   0x404060, %ebx
        default: return;
        }
        asm_get_addr (a, var);
    }
}
