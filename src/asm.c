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
// FILE SIZE: 24.862
#include "summer.h"

static void asm_change_jump (ASM *a);

static float  asmFvalue;
static int    stack;

void Run (ASM *a) {
    ( (void(*)())a->code ) ();
}

ASM * asm_new (unsigned int size) {
    ASM *a = (ASM*)malloc(sizeof(ASM));

    #ifdef WIN32
    if (a && (a->code=(UCHAR*)malloc(size)) != NULL) {
        a->p     = a->code;
        a->label = NULL;
        a->jump  = NULL;
        a->size  = size;
        //a->ip    = 0; // not used
        return a;
    }
    #endif
    #ifdef __linux__
    if (a && (a->code = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MMAP_ANON, -1, 0)) != MAP_FAILED) {
        a->p     = a->code;
        a->label = NULL;
        a->jump  = NULL;
        a->size  = size;
        //a->ip    = 0; // not used
        return a;
    }
    #endif

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
//    if (mprotect((void *)start, end - start, PROT_READ | PROT_EXEC) == -1) {
        Erro ("ERROR: asm_set_executable() ... NOT FOUND - mprotec()\n");
        return 1; // erro
    }
#endif

    return 0; // no erro
}

int set_executable2 (void *ptr, unsigned int size) {

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
    if (mprotect((void *)start, end - start, PROT_READ | PROT_EXEC) == -1) {
        Erro ("ERROR: asm_set_executable() ... NOT FOUND - mprotec()\n");
        return 1; // erro
    }
#endif

    return 0; // no erro
}

int asm_set_executable (ASM *a, unsigned int size) {
    return set_executable (a->code, size);
}

void asm_get_addr (ASM *a, void *ptr) { ///: 32/64 BITS OK
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

#ifdef USE_ASM
write_asm("%s:", name);
#endif

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
//------------------------------------------------------------

// 7c fe                	jl     401293 <label>
// 0f 8c    5a ff ff ff    	jl     401293 <label>
                case ASM_JUMP_JL:
                    {
                    int r = label_pos - jump_pos - 2;

                    if (r == (char)r) { // 2 bytes
                        //  7c fe     jl    40129e <_my_loop+0xe>
                        //
                        *(char*)(a->code+jump_pos) = 0x7c;
                        *(char*)(a->code+jump_pos+1) = r;
                    } else {
                        // 0f 8c    96 00 00 00    jl   40133e <_my_loop+0xa9>
                        //
                        *(char*)(a->code+jump_pos) = 0x0f;
                        *(char*)(a->code+jump_pos+1) = 0x8c;
                        *(int*) (a->code+jump_pos+2) = (int)(label_pos - jump_pos - 6);
                    }
                    }
                    break;

//------------------------------------------------------------

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
void asm_begin (ASM *a) { //: 32/64 BITS OK
    #if defined(__x86_64__)
#ifdef USE_ASM
write_asm(" push\t%%rbp\n mov\t%%rsp,%%rbp");
#endif
    // 55         : push  %rbp
    // 48 89 e5   : mov   %rsp,%rbp
    //-----------------------------
    a->p[0] = 0x55;
    a->p[1] = 0x48;
    a->p[2] = 0x89;
    a->p[3] = 0xe5;
    a->p += 4;
    emit_sub_esp (a,16);
    #else
#ifdef USE_ASM
write_asm(" push\t%%ebp\n mov\t%%esp,%%ebp");
#endif
    // 55     : push  %ebp
    // 89 e5  : mov   %esp,%ebp
    //-----------------------------
    a->p[0] = 0x55;
    a->p[1] = 0x89;
    a->p[2] = 0xe5;
    a->p += 3;
    emit_sub_esp (a,100);
    #endif
}
void asm_end (ASM *a) { ///: 32/64 BITS OK
    #if defined(__x86_64__)
    a->p[0] = 0xc9; // c9 : leaveq
    a->p[1] = 0xc3; // c3 : retq
    a->p += 2;
    #else
#ifdef USE_ASM
write_asm(" leave\n ret\n");
#endif
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

#ifdef USE_ASM
write_asm(" jmp\t%s", name);
#endif

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
#ifdef USE_ASM
write_asm(" jle\t%s", name);
#endif
    asm_conditional_jump (a, name, ASM_JUMP_JLE);
}
void emit_jump_jge (ASM *a, char *name) {
#ifdef USE_ASM
write_asm(" jge\t%s", name);
#endif
    asm_conditional_jump (a, name, ASM_JUMP_JGE);
}


void emit_jump_jg (ASM *a, char *name) {
    asm_conditional_jump (a, name, ASM_JUMP_JG);
}
void emit_jump_jl (ASM *a, char *name) {
    asm_conditional_jump (a, name, ASM_JUMP_JL);
}

// push number on: %esp:
//
void emit_push_int (ASM *a, int value) {
#ifdef USE_ASM
write_asm(" push\t$%d", value);
#endif
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
void emit_push_var (ASM *a, void *var) { //: 32/64 BITS OK
#ifdef USE_ASM
write_asm(" push\t%s", write_var_name);
#endif
    stack++;
    #if defined(__x86_64__)
    g3(a,0xff,0x34,0x25); asm_get_addr(a,var);  // ff 34 25   60 40 40 00   pushq   0x404060
    #else
    g2(a,0xff,0x35); asm_get_addr(a,var);       // ff 35      60 40 40 00   pushl   0x404060
    #endif
}
void emit_pop_var (ASM *a, void *var) { ///: 32/64 BITS OK
#ifdef USE_ASM
write_asm(" pop\t%s", write_var_name);
#endif
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

//-------------------------------------------------------------------
//###########################  MATH FLOAT  ##########################
//-------------------------------------------------------------------
//
// load a float value:
// USAGE:
//   asm_float_flds_value (a, 12345.00);
//
/*
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
*/
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
    a->p += 4;

    // a3     00 20 40 00       	mov    %eax,0x402000
    //
    #if defined(__x86_64__)
    g3(a,0x89,0x04,0x25);
    #else
    g(a, 0xa3);
    #endif
    *(void**)a->p = &asmFvalue;
    a->p += 4;

    // FLDS VARIABLE ( asmFvalue ):
    //
    // d9 05    04 20 40 00    	flds   0x402004
    //
    #if defined(__x86_64__)
    // d9 04 25    04 b4 60 00 	flds   0x60b404
    g3 (a,0xd9,0x04,0x25);
    #else
    g2 (a,0xd9,0x05);
    #endif
    *(void**)a->p = &asmFvalue;
    a->p += 4;
}

//
// load a float variable:
// USAGE:
//   asm_float_flds (a, &var_name);
//
void asm_float_flds (ASM *a, void *var) { //: 32/64 BITS OK
    #if defined(__x86_64__)
    // d9 04 25    04 b4 60 00 	flds   0x60b404
    //
    g3 (a,0xd9,0x04,0x25);
    #else
    // d9 05    04 20 40 00    	flds   0x402004
    //
    g2(a,0xd9,0x05);
    #endif
    *(void**)a->p = var;
    a->p += 4;
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
    #if defined(__x86_64__)
    // d9 1c 25    00 b4 60 00 	fstps  0x60b400
    g3(a,0xd9,0x1c,0x25);
    #else
    g2(a,0xd9,0x1d);
    #endif
    *(void**)a->p = var;
//    a->p += sizeof(void*);
    a->p += 4;
}
//-------------------------------------------------------------------
//###########################  MATH FLOAT  ##########################
//-------------------------------------------------------------------


void emit_call (ASM *a, void *func, UCHAR arg_count, UCHAR return_type) {
#ifdef USE_ASM
write_asm(" call\t%s", write_func_name);
#endif
    //
    // b8   7a 13 40 00       mov    $0x40137a,%eax
    // ff d0                	call   *%eax
    //
    g(a,0xb8); asm_get_addr(a, func); // %eax
    g2(a,0xff,0xd0);                  // %eax

}
void emit_call_direct (ASM *a, void *func, UCHAR arg_count, UCHAR return_type) {
#ifdef __linux__
#if defined(__x86_64__)
    // call init
    //uintptr_t rel = (uintptr_t)func - (uintptr_t)a->p - 5;
    *a->p++ = 0xe8;
    *(long*)a->p = ((void*)func - (void*)a->p - 4);
    a->p += 4;
#endif
#endif
}

void emit_push_eax (ASM *a) {
#ifdef USE_ASM
write_asm(" push\t%%eax");
#endif
    stack--;
    g(a,0x50); // 50   push   %eax
}

void emit_pop_eax (ASM *a) {
#ifdef USE_ASM
write_asm(" pop\t%%eax");
#endif
    stack--;
    g(a,0x58);  // 58     pop   %eax
}
void emit_pop_edx (ASM *a) {
#ifdef USE_ASM
write_asm(" pop\t%%edx");
#endif
    stack--;
    g(a,0x5a);// 5a     pop   %edx
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

void emit_mov_var_reg (ASM *a, void *var, int reg) { ///: 32/64 BITS OK: Move variable to %register
    if (reg >= 0 && reg <= 7) {
        #if defined(__x86_64__)
        switch (reg) {
        case EAX: g3(a,0x8b,0x04,0x25); break; // 8b 04 25   00 0a 60 00 	mov    0x600a00,%eax
        case ECX: g3(a,0x8b,0x0c,0x25); break; // 8b 0c 25   00 0a 60 00 	mov    0x600a00,%ecx
        case EDX: g3(a,0x8b,0x14,0x25); break; // 8b 14 25   00 0a 60 00 	mov    0x600a00,%edx
        case EBX: g3(a,0x8b,0x1c,0x25); break; // 8b 1c 25   00 0a 60 00 	mov    0x600a00,%ebx
        default: return;
        }
        asm_get_addr (a, var);
        #else
        switch (reg) {
        case EAX: g(a,0xa1);       break; // a1       60 40 40 00   mov   0x404060, %eax
        case ECX: g2(a,0x8b,0x0d); break;	// 8b 0d    70 40 40 00   mov   0x404070, %ecx
        case EDX: g2(a,0x8b,0x15); break; // 8b 15    70 40 40 00   mov   0x404070, %edx
        case EBX: g2(a,0x8b,0x1d); break; // 8b 1d    60 40 40 00   mov   0x404060, %ebx
        default: return;
        }
        asm_get_addr (a, var);
        #endif
#ifdef USE_ASM
write_asm(" mov\t%s,%%eax", write_var_name);
#endif
    }
}

void emit_mov_reg_var (ASM *a, int reg, void *var) { ///: 32/64 BITS OK: Move %register to variable
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

void emit_sub_esp (ASM *a, char c) { // 32/64 BITS OK
#ifdef USE_ASM
write_asm(" sub\t$%d,%%esp", c);
#endif
    #if defined(__x86_64__)
    g4(a,0x48,0x83,0xec,(char)c); // 48 83 ec   08   sub   $0x8,%rsp
    #else
    g3(a,0x83,0xec,(char)c);      // 83 ec      08   sub  $0x8,%esp
    #endif
}


void emit_cmp_eax_edx (ASM *a) {
#ifdef USE_ASM
write_asm(" cmp\t%%eax,%%edx");
#endif
    g2(a,0x39,0xc2); // 39 c2  : cmp   %eax,%edx
}

void emit_incl (ASM *a, void *var) { //: 32/64 BITS OK
#ifdef USE_ASM
write_asm(" incl\t%s", write_var_name);
#endif
    #if defined(__x86_64__)
    g3(a,0xff,0x04,0x25); asm_get_addr(a,var);  // ff 04 25   00 0a 60 00   : incl   0x600a00
    #else
    g2(a,0xff,0x05); asm_get_addr(a,var);       // ff 05      00 20 40 00   : incl   0x402000
    #endif
}

void emit_decl (ASM *a, void *var) { //: 32/64 BITS OK
    #if defined(__x86_64__)
    g3(a,0xff,0x0c,0x25); asm_get_addr(a, var);  // ff 0c 25   cc 0a 60 00   decl  0x600acc
    #else
    g2(a,0xff,0x0d); asm_get_addr(a, var);  // ff 0d      00 20 40 00   decl  0x402000
    #endif
}


void emit_expression_pop_64_int (ASM *a) {
    // argument 1:
    // this is a constans :
    // b8    00 20 40 00       	mov    $0x402000,%eax
    g(a,0xb8); asm_get_addr(a,(void*)"%d\n");
    g2(a,G2_MOV_EAX_EDI); // 89 c7   : mov   %eax,%edi

    // argument 2:
    emit_pop_eax (a); // %eax | eax.i := expression result
    g2(a,G2_MOV_EAX_ESI); // 89 c6   : mov   %eax,%esi
}
void emit_expression_pop_64_float (ASM *a) {
    //  dd 1c 25    f8 09 60 00 	fstpl  0x6009f8
    asm_float_fstps (a, &asmFvalue); // store expression result | TYPE_FLOAT
    emit_mov_var_reg(a, &asmFvalue, EAX);

    // 89 45 fc             	mov    %eax,-0x4(%rbp)
    g3(a,0x89, 0x45,0xfc);
    // f3 0f 10 45 fc       	movss  -0x4(%rbp),%xmm0
    g4(a,0xf3, 0x0f, 0x10, 0x45);
    g(a,(char)-4);

    // 0f 5a c0             	cvtps2pd %xmm0,%xmm0
    g3(a,0x0f, 0x5a, 0xc0);

    // argument 1:
    // b8    00 20 40 00       	mov    $0x402000,%eax
    g(a,0xb8); asm_get_addr(a,"%f\n");
    g2(a,G2_MOV_EAX_EDI); // 89 c7   : mov   %eax,%edi
}

void emit_cmp_eax_var (ASM *a, void *var) {
#ifdef USE_ASM
write_asm(" cmp\t%%eax,%s", write_var_name);
#endif
    #if defined(__x86_64__)
    g3(a,0x39,0x04,0x25);
    *(void**)a->p = var;
    a->p += 4; // ! OK
    #else
    //	39 05      60 40 40 00    	cmp    %eax,0x404060
    g2(a,0x39,0x05);
    *(void**)a->p = var;
    a->p += 4; // ! OK
    #endif
}
void asm_mov_value_eax (ASM *a, long value) {
#ifdef USE_ASM
write_asm(" mov\t$%d,%%eax", value);
#endif
    // b8   e8 03 00 00       	mov    $0x3e8,%eax
    //
    g(a,0xb8);
    *(long*)a->p = value;
    a->p += 4; // ! OK
}


void emit_mov_eax_edi (ASM *a) { g2(a,G2_MOV_EAX_EDI); }
void emit_mov_eax_esi (ASM *a) { g2(a,G2_MOV_EAX_ESI); }
void emit_mov_eax_edx (ASM *a) { g2(a,G2_MOV_EAX_EDX); }
void emit_mov_eax_ecx (ASM *a) { g2(a,G2_MOV_EAX_ECX); }
void emit_mov_eax_r8d (ASM *a) { g3(a,G3_MOV_EAX_r8d); }
void emit_mov_eax_r9d (ASM *a) { g3(a,G3_MOV_EAX_r9d); }
