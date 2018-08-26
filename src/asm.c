//-------------------------------------------------------------------
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
// THIS FILE IS PART OF SUMMER LANGUAGE:
//
// The back-end | JIT:
//
// FILE:
//   asm.c
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite: 20/07/2018 - 11:10
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "summer.h"

LIBIMPORT void    Erro    (char *format, ...);
LIBIMPORT char  * ErroGet (void);

static void asm_change_jump (ASM *a);

static int  stack;

void Run (ASM *a) {
    ( (void(*)())a->code ) ();
}

ASM * asm_new (unsigned int size) {
    ASM *a = (ASM*)malloc(sizeof(ASM));
    if (a && (a->code=(UCHAR*)malloc(size)) != NULL) {
        a->p     = a->code;
        a->label = NULL;
        a->jump  = NULL;
        a->size  = size;
        a->ip    = 0; // not used
        return a;
    }
    return NULL;
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

int set_executable (void *ptr, unsigned int size) {

    if (ErroGet()) return 1;

#ifdef WIN32
    unsigned long old_protect;

    if (!VirtualProtect(ptr, size, PAGE_EXECUTE_READWRITE, &old_protect)) {
        Erro ("ERROR: asm_set_executable() ... NOT FOUND - VirtualProtect()\n");
        return 1; // erro
    }
#endif

#ifdef __linux__
    unsigned long start, end, PageSize;

    PageSize = sysconf (_SC_PAGESIZE);
    start = (unsigned long)ptr & ~(PageSize - 1);
    end = (unsigned long)ptr + size;
    end = (end + PageSize - 1) & ~(PageSize - 1);
    if (mprotect((void *)start, end - start, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
        Erro ("ERROR: asm_set_executable() ... NOT FOUND - mprotec()\n");
        return 1; // erro
    }
#endif

    return 0; // no erro
}

int asm_set_executable (ASM *a, unsigned int size) {
    return set_executable (a->code, size);
}

void asm_get_addr (ASM *a, void *ptr) { // 32/64 BITS OK
    *(void**)a->p = ptr;
    //a->p += sizeof(void*);
    a->p += 4; // ! OK
}

int asm_get_len (ASM *a) {
    return (a->p - a->code);
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

                case ASM_JUMP_JGE:
                    {
                    int r = label_pos - jump_pos - 2;
                    if (r == (char)r) { // 2 bytes
                        // 7d 06                	jge    40165f < code + number>
                        //
                        *(UCHAR*)(a->code+jump_pos) = 0x7d;
                        *(UCHAR*)(a->code+jump_pos+1) = r;
                    } else {
                        // 0f 8d   d3 00 00 00    	jge    401733 < code+ number>
                        //
                        *(UCHAR*)(a->code+jump_pos) = 0x0f;
                        *(UCHAR*)(a->code+jump_pos+1) = 0x8d;
                        *(int*) (a->code+jump_pos+2) = (int)(label_pos - jump_pos - 6);
                    }
                    }
                    break;//: case JUMP_TYPE_JGE:

                case ASM_JUMP_JLE:
                    {
                    int r = label_pos - jump_pos - 2;
                    if (r == (char)r) { // 2 bytes
                        // 7e 06                	jle    40165a < code + number >
                        //
                        *(UCHAR*)(a->code+jump_pos) = 0x7e;
                        *(UCHAR*)(a->code+jump_pos+1) = r;
                    } else {
                        // 0f 8e    d3 00 00 00    	jle    401733 < code + number >
                        //
                        *(UCHAR*)(a->code+jump_pos) = 0x0f;
                        *(UCHAR*)(a->code+jump_pos+1) = 0x8e;
                        *(int*) (a->code+jump_pos+2) = (int)(label_pos - jump_pos - 6);
                    }
                    } break;//: case JUMP_JUMP_JLE: {


                case ASM_JUMP_JE:
                    {
                    int r = label_pos - jump_pos - 2;
                    if (r == (char)r) { // 2 bytes
                        //  40129a:	74 02     je    40129e <_my_loop+0xe>
                        //
                        *(char*)(a->code+jump_pos) = 0x74;
                        *(char*)(a->code+jump_pos+1) = r;
                    } else {
                        // 4012a2:	0f 84    96 00 00 00    je   40133e <_my_loop+0xa9>
                        //
                        *(char*)(a->code+jump_pos) = 0x0f;
                        *(char*)(a->code+jump_pos+1) = 0x84;
                        *(int*) (a->code+jump_pos+2) = (int)(label_pos - jump_pos - 6);
                    }
                    }
                    break;


                case ASM_JUMP_JNE:
                    {
                    int r = label_pos - jump_pos - 2;

                    if (r == (char)r) { // 2 bytes
                        // 75 08                	 jne     4012b1 < _code + number >
                        //
                        *(char*)(a->code+jump_pos) = 0x75;
                        *(char*)(a->code+jump_pos+1) = r;
                    } else {
                        // 0f 85    85 ed bf ff   jne    401293 < _code + number >
                        //
                        *(char*)(a->code+jump_pos) = 0x0f;
                        *(char*)(a->code+jump_pos+1) = 0x85;
                        *(int*) (a->code+jump_pos+2) = (int)(label_pos - jump_pos - 6);
                    }
                    }
                    break;

                }//: switch (jump->type)

            }//: if (!strcmp(label->name, jump->name))

            jump = jump->next;

        }//: while (jump)

        label = label->next;

    }//: while (label)

}//: asm_change_jump ()


//-------------------------------------------------------------------
//--------------------------  GEN / EMIT  ---------------------------
//-------------------------------------------------------------------
//
void asm_begin (ASM *a) { // 32/64 BITS OK
    #if defined(__x86_64__)
    // 55         : push  %rbp
    // 48 89 e5   : mov   %rsp,%rbp
    //-----------------------------
    a->p[0] = 0x55;
    a->p[1] = 0x48;
    a->p[2] = 0x89;
    a->p[3] = 0xe5;
    a->p += 4;
    #else
    // 55     : push  %ebp
    // 89 e5  : mov   %esp,%ebp
    //-----------------------------
    a->p[0] = 0x55;
    a->p[1] = 0x89;
    a->p[2] = 0xe5;
    //
    // 83 ec 100  : sub  $100, %esp
    //-----------------------------
    a->p[3] = 0x83;
    a->p[4] = 0xec;
    a->p[5] = 100;
    a->p += 6;
    #endif
}
void asm_end (ASM *a) { // 32/64 BITS OK
    #if defined(__x86_64__)
    a->p[0] = 0xc9; // c9 : leaveq
    a->p[1] = 0xc3; // c3 : retq
    a->p += 2;
    #else
    a->p[0] = 0xc9; // c9 : leave
    a->p[1] = 0xc3; // c3 : ret
    a->p += 2;
    #endif
    if ((a->p - a->code) > a->size) {
        Erro ("ASM ERRO: code > size\n");
        return;
    }
    asm_change_jump (a);
}

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
void emit_jump_jmp (ASM *a, char *name) {
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
void emit_jump_je (ASM *a, char *name) {
    asm_conditional_jump (a, name, ASM_JUMP_JE);
}
void emit_jump_jne (ASM *a, char *name) {
    asm_conditional_jump (a, name, ASM_JUMP_JNE);
}
void emit_jump_jle (ASM *a, char *name) {
    asm_conditional_jump (a, name, ASM_JUMP_JLE);
}
void emit_jump_jge (ASM *a, char *name) {
    asm_conditional_jump (a, name, ASM_JUMP_JGE);
}

// push number on: %esp:
//
void emit_push_int (ASM *a, int value) {
    stack++;
    if (value == (char)value) {
        g(a,0x6a);  // 6a   64    push   $0x64
        *a->p = value;
        a->p += sizeof(char);
    } else {
        g(a,0x68); // 68    00 e1 f5 05    push  $0x5f5e100
        // 32/64 BITS
        *(int*)a->p = value;
        a->p += sizeof(int);
    }
}


// push variable on: %esp:
//
void emit_push_var (ASM *a, void *var) {
    stack++;
    #if defined(__x86_64__)
    g3(a,0xff,0x34,0x25); asm_get_addr(a,var);  // ff 34 25   60 40 40 00   pushq   0x404060
    #else
    g2(a,0xff,0x35); asm_get_addr(a,var);       // ff 35      60 40 40 00   pushl   0x404060
    #endif
}
void emit_pop_var (ASM *a, void *var) { // 32/64 BITS OK
    stack--;
    #if defined(__x86_64__)
    g3(a,0x8f,0x04,0x25); asm_get_addr(a,var);  // 8f 04 25   60 40 40 00    popq   0x404060
    #else
    g2(a,0x8f,0x05); asm_get_addr(a,var);       // 8f 05      60 40 40 00    popl   0x404060
    #endif
}

//------------------------------------------------
// long expression math:
//------------------------------------------------
void asm_imul_eax_esp (ASM *a) {
    stack--;
    g(a,0x58);                  // 58               pop  %eax
    g4(a,0x0f,0xaf,0x04,0x24);  // 0f af 04 24      imul   (%esp),%eax
    g3(a,0x89,0x04,0x24);       // 89 04 24         mov    %eax,(%esp)
}
void asm_idivl_eax_esp (ASM *a) {
    stack--;
    stack--;
    g(a,0x59);        // 59       pop    %ecx
    g(a,0x58);        // 58       pop    %eax
    g(a,0x99);        // 99       cltd
    g2(a,0xf7,0xf9);  // f7 f9    idiv   %ecx
    g(a,0x50);        // 50       push   %eax
//    stack++;
}
void asm_add_eax_esp (ASM *a) {
    stack--;
    g(a,0x58);              // 58         pop  %eax
    g3(a,0x01,0x04,0x24);   // 01 04 24   add   %eax,(%esp)
}
void asm_sub_eax_esp (ASM *a) {
    stack--;
    g(a,0x58);            // 58           pop  %eax
    g3(a,0x29,0x04,0x24); // 29 04 24     sub    %eax,(%esp)
}
//------------------------------------------------

void emit_call (ASM *a, void *func, UCHAR arg_count, UCHAR return_type) {
//void asm_call (ASM *a, void *func) {
    // b8   7a 13 40 00       mov    $0x40137a,%eax
    // ff d0                	call   *%eax
    //
    g(a,0xb8); asm_get_addr(a, func);
    g2(a,0xff,0xd0);
}

void emit_pop_eax (ASM *a) {
    stack--;
    g(a,0x58);  // 58     pop   %eax
}

void emit_movl_ESP (ASM *a, long value, UCHAR index) {
    // c7 44 24   04   dc 05 00 00 	  movl    $0x5dc,0x4(%esp)
    g4(a,0xc7,0x44,0x24,(UCHAR)index);
    *(long*)a->p = value;
    a->p += sizeof(long);
}

void emit_mov_eax_ESP (ASM *a, UCHAR index) {
    g4 (a,0x89,0x44,0x24,(UCHAR)index); // 89 44 24     04    mov    %eax,0x4(%esp)
}

void emit_mov_var_reg (ASM *a, void *var, int reg) { // move: variable to %register
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

void emit_mov_reg_var (ASM *a, int reg, void *var) { // 32/64 BITS OK - move: %register to variable
    if (reg >= 0 && reg <= 7) {
        #if defined(__x86_64__)
        switch (reg) {
        case EAX: g3(a, 0x89, 0x04, 0x25);  break; // 89 04 25    28 0b 60 00 	mov  %eax, 0x600b28
        case ECX: g3(a, 0x89, 0x0c, 0x25);  break; // 89 0c 25    28 0b 60 00 	mov  %ecx, 0x600b28
        case EDX: g3(a, 0x89, 0x14, 0x25);  break; // 89 14 25    28 0b 60 00 	mov  %edx, 0x600b28
        case EBX: g3(a, 0x89, 0x1c, 0x25);  break; // 89 1c 25    28 0b 60 00 	mov  %ebx, 0x600b28
        default: return;
        }
        asm_get_addr(a,var);
        #else
        switch (reg) {
        case EAX: g(a,0xa3);       break; // a3       10 40 40 00   mov   %eax, 0x404010
        case ECX: g2(a,0x89,0x0d); break; // 89 0d    60 40 40 00   mov   %ecx, 0x404060
        case EDX: g2(a,0x89,0x15); break; // 89 15    60 40 40 00   mov   %edx, 0x404060
        case EBX: g2(a,0x89,0x1d); break;//  89 1d    60 40 40 00   mov   %ebx, 0x404060
        default: return;
        }
        asm_get_addr(a,var);
        #endif
    }
}

void emit_cmp_eax_edx (ASM *a) { g2(a,0x39,0xc2); }  // 39 c2  : cmp   %eax,%edx

//#endif // ! USE_JIT
