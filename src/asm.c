//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
// The Backend | JIT(x86) 32/64 bits:
//
// FILE:
//   asm.c
//
// START DATE: 23/03/2019 - 08:50
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "asm.h"

#define REG_MAX		6
#define STR_ERRO_SIZE		1024

//char *ss = "\x81\x05\xE0\x5A\x47\x00\x01\x00\x00\x00\x11\x22\x33\x44\x55\x66";
//		emit (a, (UCHAR[]){ 0x48, 0x89, 0xe5 }, 3);

/*
#include "ChunkEmitters.c"

void emitByte(Byte **buf, Byte x) {
    *((*buf)++) = x;
}

void emitShort(Byte **buf, short x) {
    *((short *) *buf) = x;
    *buf += 2;
}

void emitInt(Byte **buf, int x) {
    *((int *) *buf) = x;
    *buf += 4;
}

void emitLong(Byte **buf, long x) {
    *((long *) *buf) = x;
    *buf += 8;
}

*/

static void asm_change_jump (ASM *a);

int erro; // global

static float asmFvalue = 1500.0;
float f1 = 100.55, f2 = 500.55;

static char strErro [STR_ERRO_SIZE + 1];
static char *REGISTER[REG_MAX] = { "%eax", "%ecx", "%edx", "%ebx", "%esi", "%edi" };
// used for expression in register:
static int	reg; // EAX, ECX, EDX, EBX, ESI, EDI

//
// const EMIT opcode:
//
const UCHAR OP_mov_eax_ecx  [2] = { 0x89, 0xc1 }; // mov	%eax, %ecx
const UCHAR OP_add_ecx_eax  [2] = { 0x01, 0xc8 }; // add %ecx, %eax
const UCHAR OP_imul_ecx_eax [3] = { 0x0f, 0xaf, 0xc1 }; // imul %ecx, %eax
const UCHAR OP_imul_edx_ecx [3] = { 0x0f, 0xaf, 0xca }; // imul %edx, %ecx

const UCHAR OP_sub_ecx_eax [2] = { 0x29, 0xc8 }; // sub %ecx, %eax
const UCHAR OP_sub_edx_ecx [2] = { 0x29, 0xd1 }; // sub %edx, %ecx

typedef struct ASM_label	ASM_label;
typedef struct ASM_jump		ASM_jump;

struct ASM { // opaque struct
    UCHAR     *p;
    UCHAR     *code;
    ASM_label *label;
    ASM_jump  *jump;
    int       size;
    char      name[20];
};
struct ASM_label {
    char      *name;
    int       pos;
    ASM_label *next;
};
struct ASM_jump {
    char      *name;
    int       pos;
    int       type;
    int       exist; // to check if this 'exist'
    ASM_jump  *next;
};

static void push_register (void) {
    if (reg < REG_MAX) reg++;
}

static void pop_register (void) {
    if (reg > 0) reg--;
}


//-------------------------------------------------------------------
//-------------------------  ASM PUCLIC API  ------------------------
//-------------------------------------------------------------------
//
ASM *asm_New (unsigned int size, char *name) {
    ASM *a = (ASM*)malloc(sizeof(ASM));
    if (a && (a->code=(UCHAR*)malloc(size)) != NULL) {
        a->p     = a->code;
        a->label = NULL;
        a->jump  = NULL;
        a->size  = size;
        if (name) {
            sprintf (a->name, name);
            printf ("Create ASM(%s)\n", a->name);
        }
        return a;
    }
    return NULL;
}

void asm_Free (ASM *a) {
    if (a) {
        printf ("Freeing ASM(%s)\n", a->name);
        asm_Reset (a);
        if (a->code) {
            free (a->code);
            a->code = NULL;
        }
        free (a);
    }
}

void asm_Reset (ASM *a) {

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

int asm_SetExecutable_PTR (void *ptr, unsigned int size) {

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

int asm_SetExecutable_ASM (ASM *a, unsigned int size) {
		if (size == 0)
				size = (a->p - a->code);
printf ("ASM SIZE(%s): %d\n", a->name, size);
    return asm_SetExecutable_PTR (a->code, size);
}

void asm_Run (ASM *a) {
    ( (void(*)()) a->code ) ();
}

void asm_get_addr (ASM *a, void *ptr) { ///: 32/64 BITS OK
    *(void**)a->p = ptr;
    //a->p += sizeof(void*);
    a->p += 4; // ! OK
}

void asm_Label (ASM *a, char *name) {
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

void asm_CodeCopy (ASM *src, UCHAR *dest, unsigned int len) {
    register int i;
    for (i = 0; i <= len; i++)
        dest[i] = src->code[i];
}

static void asm_change_jump (ASM *a) {
    
    ASM_label *label = a->label;
    while (label) {

        ASM_jump *jump  = a->jump;
        while (jump) {

            if (!strcmp(label->name, jump->name)) {
                int jump_pos  = jump->pos;
                int label_pos = label->pos;

                jump->exist = 1;

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

    ASM_jump *j  = a->jump;
    while (j) {
        if (j->exist==0) {
            Erro ("Label Not Found: '%s'\n", j->name);
        }
        j = j->next;
    }

}//: asm_change_jump ()

//-------------------------------------------------------------------
//---------------------------  GEN / EMIT  --------------------------
//-------------------------------------------------------------------
//
void g (ASM *a, UCHAR c) {
    *a->p++ = c;
}
void gen (ASM *a, UCHAR c) {
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
void g5 (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4, UCHAR c5) {
    a->p[0] = c1;
    a->p[1] = c2;
    a->p[2] = c3;
    a->p[3] = c4;
    a->p[4] = c5;
    a->p += 5;
}

void emit (ASM *a, const UCHAR opcode[], unsigned int len) {
    while (len--) {
				*a->p++ = *opcode++;
		}
}

void emit_begin (ASM *a) { //: 32/64 BITS OK
    #if defined(__x86_64__)
    // 55         : push  %rbp
    // 48 89 e5   : mov   %rsp,%rbp
    //-----------------------------
    a->p[0] = 0x55;
    a->p[1] = 0x48;
    a->p[2] = 0x89;
    a->p[3] = 0xe5;
    a->p += 4;
    emit_sub_esp(a,48); // 48 / 8 := 6
    #else
    // 55     : push  %ebp
    // 89 e5  : mov   %esp,%ebp
    //-----------------------------
    a->p[0] = 0x55;
    a->p[1] = 0x89;
    a->p[2] = 0xe5;
    a->p += 3;
    emit_sub_esp(a,100);
    #endif
}

void emit_end (ASM *a) { ///: 32/64 BITS OK
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

// 64 bits: ff 04 25 10 30 40 00 	incl   0x403010
// 32 bits: ff 05 04 30 40 00    	incl   0x403004

void emit_incl (ASM *a, void *var) { //: 32/64 BITS OK
    #if defined(__x86_64__)
    g3(a,0xff,0x04,0x25); asm_get_addr(a,var);  // ff 04 25   00 0a 60 00   : incl   0x600a00
    #else
    g2(a,0xff,0x05); asm_get_addr(a,var);       // ff 05      00 20 40 00   : incl   0x402000
    #endif
}

// mov $0x3e8, %eax
void emit_mov_long_reg (ASM *a, long value, int reg) {
    switch (reg) {
    case EAX: g(a,0xb8); break; // b8 	e8 03 00 00		|	mov $0x3e8, %eax
    case ECX: g(a,0xb9); break; // b9 	e8 03 00 00		| mov $0x3e8, %ecx
    case EDX: g(a,0xba); break; // ba 	e8 03 00 00		| mov $0x3e8, %edx
    case EBX: g(a,0xbb); break; // bb		e8 03 00 00		| mov $0x3e8, %ebx
    case ESI: g(a,0xbe); break; // be   e8 03 00 00   | mov $0x3e8, %esi
    case EDI: g(a,0xbf); break; // bf   e8 03 00 00   | mov $0x3e8, %edi
    default: return;
    }
    *(long*)a->p = value;
    a->p += 4; // ! OK
}

void emit_mov_var_reg (ASM *a, void *var, int reg) { ///: 32/64 BITS OK: Move variable to %register
    if (reg >= 0 && reg <= 7) {
        #if defined(__x86_64__)
        switch (reg) {
        case EAX: g3(a,0x8b,0x04,0x25); break; // 8b 04 25   00 0a 60 00 	mov    0x600a00,%eax
        case ECX: g3(a,0x8b,0x0c,0x25); break; // 8b 0c 25   00 0a 60 00 	mov    0x600a00,%ecx
        case EDX: g3(a,0x8b,0x14,0x25); break; // 8b 14 25   00 0a 60 00 	mov    0x600a00,%edx
        case EBX: g3(a,0x8b,0x1c,0x25); break; // 8b 1c 25   00 0a 60 00 	mov    0x600a00,%ebx
				case ESI: g3(a,0x8b,0x34,0x25); break; // 8b 34 25   20 7a 40 00  mov    0x407a20,%esi
  			case EDI: g3(a,0x8b,0x3c,0x25); break; // 8b 3c 25   30 7a 40 00  mov    0x407a30,%edi
        default: return;
        }
        asm_get_addr (a, var);
        #else
        switch (reg) {
        case EAX: g(a,0xa1);       break; // a1       60 40 40 00   mov   0x404060, %eax
        case ECX: g2(a,0x8b,0x0d); break;	// 8b 0d    70 40 40 00   mov   0x404070, %ecx
        case EDX: g2(a,0x8b,0x15); break; // 8b 15    70 40 40 00   mov   0x404070, %edx
        case EBX: g2(a,0x8b,0x1d); break; // 8b 1d    60 40 40 00   mov   0x404060, %ebx
				case ESI: g2(a,0x8b,0x35); break; // 8b 35 		38 54 40 00		mov   0x405438, %esi
				case EDI: g2(a,0x8b,0x3d); break; // 8b 3d 		34 54 40 00		mov   0x405434, %edi
        default: return;
        }
        asm_get_addr (a, var);
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

void emit_call (ASM *a, void *func, UCHAR arg_count, UCHAR return_type) {
    // b8   7a 13 40 00       mov    $0x40137a, %eax
    // ff d0                	call   *%eax
    //
    g(a,0xb8); asm_get_addr(a, func);
    g2(a,0xff,0xd0);
}

void emit_sub_esp (ASM *a, char c) { // 32/64 BITS OK
    #if defined(__x86_64__)
    g4(a,0x48,0x83,0xec,(char)c); // 48 83 ec   08   sub   $0x8,%rsp
    #else
    g3(a,0x83,0xec,(char)c);      // 83 ec      08   sub  $0x8,%esp
    #endif
}

void emit_movl_ESP (ASM *a, long value, UCHAR index) {
    #if defined(__x86_64__)
    //  67 c7 44 24     04    e8 03 00 00	movl   $0x3e8,0x4(%esp)
    g5(a,0x67,0xc7,0x44,0x24,(UCHAR)index);
    *(long*)a->p = value;
    a->p += 4;
    #else
    // c7 44 24   04   dc 05 00 00 	  movl    $0x5dc,0x4(%esp)
    g4(a,0xc7,0x44,0x24,(UCHAR)index);
    *(long*)a->p = value;
    a->p += 4;
    #endif
}

void emit_mov_eax_ESP (ASM *a, UCHAR index) {
    #if defined(__x86_64__)
    g5(a,0x67,0x89,0x44,0x24,(UCHAR)index); // 67 89 44 24       04    mov  %eax, 0x4(%esp)
    #else
    g4 (a,0x89,0x44,0x24,(UCHAR)index); // 89 44 24     04    mov    %eax,0x4(%esp)
    #endif
}

void emit_mov_edx_EAX (ASM *a, UCHAR index) {
    // 89 50      04             	mov    %edx,0x4(%eax)
    g3(a,0x89,0x50,(UCHAR)index);
}

void emit_mov_eax_EDX (ASM *a, UCHAR index) {
  // 32 bits
// 89 42 04             	mov    %eax,0x4(%edx)

// 64 bits
// 67 89 42 04          	mov    %eax,0x4(%edx)

}

//-------------------------------------------------------------------
//###########################  MATH FLOAT  ##########################
//-------------------------------------------------------------------
//
// load a float value:
// USAGE:
//   emit_float_flds_value (a, 12345.00);
//
void emit_float_flds_value (ASM *a, float value) {
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
//   emit_float_flds (a, &var_name);
//
void emit_float_flds (ASM *a, void *var) { //: 32/64 BITS OK
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
//   emit_float_fmulp (a);
//
void emit_float_fmulp (ASM *a) { g2 (a,0xde,0xc9); } // de c9    fmulp  %st,%st(1)

//
// divide the stack float:
// USAGE:
//   emit_float_fmulp (a);
//
void emit_float_fdivp (ASM *a) { g2(a,0xde,0xf1); } // de f1    fdivp  %st,%st(1)

//
// add the stack float:
// USAGE:
//   emit_float_faddp (a);
//
void emit_float_faddp (ASM *a) { g2 (a,0xde,0xc1); } // de c1    faddp  %st,%st(1)

//
// sub the stack float:
// USAGE:
//   emit_float_faddp (a);
//
void emit_float_fsubp (ASM *a) { g2 (a,0xde,0xe1); } // de e1    fsubp  %st,%st(1)

//
// set the variable and CLEAR the stack:
// USAGE:
//   emit_float_fstps (a, &var_name);
//
void emit_float_fstps (ASM *a, void *var) {
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


void asm_expression_reset (void) {
		reg = 0;
}

static void print_string (const char *s) {
    while (*s) {
        if (*s=='\\' && s[1]=='n') {
            printf("%c", 10); // new line
            s++;
        } else printf("%c", *s);
        s++;
    }
}

void emit_print_string (ASM *a, char *s) {
#ifdef __x86_64__
		#ifdef WIN32
		// argument 1: %ecx
		// b9		06 40 40 00       	mov    $0x404006,%ecx
		g(a,0xb9);
		#endif
		#ifdef __linux__
		// argument 1: %edi
		// bf 		06 40 40 00       	mov    $0x404006,%edi
		g(a,0xbf);
		#endif
		*(void**)a->p = s;
		a->p += 4;
		emit_call (a,print_string,0,0);
#else // 32 bits
/*
            //  c7 44 24    04    00 30  40 00	    movl   $0x403000,0x4(%esp)
            //
            g4(a,0xc7,0x44,0x24,0);
            *(void**)a->p = s->s;
            a->p += sizeof(long);
            asm_call (a, print_string, 0);
*/
#endif

}


void emit_pop_print_result (ASM *a, int var_type, int new_line) {
    #if defined(__x86_64__)
        #ifdef WIN32
        // the result of expression is in register: %eax
        // argument 1 = %ecx
        // argument 2 = %edx
        if (var_type == 0) { // TYPE_LONG
            // argument 1:
            // b9 		00 20 40 00       	mov    $0x402000,%ecx
            g(a,0xb9);
            if (new_line)
                asm_get_addr(a,(void*)"%d\n");
            else
                asm_get_addr(a,(void*)"%d ");
            // argument 2:
            g2(a,0x89,0xc2); //	mov    %eax,%edx
            emit_call(a,printf,0,0);
        }
        if (var_type == 1) { // TYPE_FLOAT | WINDOWS 64
            emit_float_fstps (a, &asmFvalue); // store expression result | TYPE_FLOAT

            // f3 0f 10 04 25 		20 7a 40 00			movss  0x407a20,%xmm0
            g5(a,0xf3, 0x0f, 0x10, 0x04, 0x25); asm_get_addr(a, &asmFvalue);

            //---------------------------------------------------
            // f3 0f 5a c0          	cvtss2sd %xmm0,%xmm0
            g4(a,0xf3, 0x0f, 0x5a, 0xc0);

            // 66 0f 28 c8          	movapd %xmm0,%xmm1
            g4(a,0x66, 0x0f, 0x28, 0xc8);

            // 66 48 0f 7e c2       	movq   %xmm0,%rdx
            g5(a,0x66, 0x48, 0x0f, 0x7e, 0xc2);
            //---------------------------------------------------

            // argument 1: "string"
            // b9 		00 20 40 00       	mov    $0x402000,%ecx
            g(a,0xb9);
            if (new_line)
                asm_get_addr(a, &("%f\n"));
            else
                asm_get_addr(a, &("%f "));
            emit_call(a,printf,0,0);
        }
        #endif
        #ifdef __linux__
        // the result of expression is in register: %eax
        // argument 1 = %edi
        // argument 2 = %esi
        if (var_type == 0) { // TYPE_LONG
            // argument 1:
            // bf     e8 03 00 00       	mov    $0x3e8,%edi
            g(a,0xbf);
            if (new_line)
                asm_get_addr(a,(void*)"%d\n");
            else
                asm_get_addr(a,(void*)"%d ");
            // argument 2:
            g2(a,G2_MOV_EAX_ESI);
            emit_call(a,printf,0,0);
        }
        if (var_type == 1) { // TYPE_FLOAT | LINUX 64
            emit_float_fstps (a, &asmFvalue); // store expression result | TYPE_FLOAT
            emit_mov_var_reg(a, &asmFvalue, EAX);

            // 89 45 fc             	mov    %eax,-0x4(%rbp)
            g3(a,0x89, 0x45,0xfc);

            // f3 0f 10 45 fc       	movss  -0x4(%rbp),%xmm0
            g5(a,0xf3, 0x0f, 0x10, 0x45, 0xfc);

            //----------------------------------------------
            // 0f 14 c0             	unpcklps %xmm0,%xmm0
            //----------------------------------------------

            // 0f 5a c0             	cvtps2pd %xmm0,%xmm0
            g3(a,0x0f, 0x5a, 0xc0);

            // argument 1:
            // b8    00 20 40 00       	mov    $0x402000,%eax
            g(a,0xb8);
            if (new_line)
                asm_get_addr(a,(void*)"%f\n");
            else
                asm_get_addr(a,(void*)"%f ");
            g2(a,G2_MOV_EAX_EDI); // 89 c7   : mov   %eax,%edi

            emit_call(a,printf,0,0);
        }
        #endif
    #else // 64 bits
    #endif
}

void emit_expression_push_long (ASM *a, long value) {
		emit_mov_long_reg (a, value, reg); // mov $1000, %eax
		push_register();
}

void emit_expression_push_var (ASM *a, void *var) {
		emit_mov_var_reg (a, var, reg);
		push_register();
}

void emit_expression_add_long (ASM *a) {
		pop_register();
		if (reg == ECX && reg-1 == EAX) {
				EMIT(a,OP_add_ecx_eax); // add %ecx, %eax
		}
}

void emit_expression_sub_long (ASM *a) { 
		pop_register();
		// DEBUG !
    printf ("  sub %s, %s\n", REGISTER[reg], REGISTER[reg-1]);
		if (reg == ECX && reg-1 == EAX) {
				EMIT(a,OP_sub_ecx_eax); // sub %ecx, %eax
		}
		else
		if (reg == EDX && reg-1 == ECX) {
				EMIT(a,OP_sub_edx_ecx); // sub %edx, %ecx
		}
}


void emit_expression_mul_long (ASM *a) {
    pop_register();
    if (reg == ECX && reg-1 == EAX) {
        EMIT(a,OP_imul_ecx_eax); // imul %ecx, %eax
    }
    else
    if (reg == EDX && reg-1 == ECX) {
        EMIT(a,OP_imul_edx_ecx); // imul %edx, %ecx
    }
}

void emit_expression_div_long (ASM *a) {
    pop_register();
    if (reg == ECX) {
        // DEBUG !
        printf ("  cltd\n");
        printf ("  idiv %s\n", REGISTER[reg]);
        // emit ...
        g(a,0x99);        // 99       cltd
        g2(a,0xf7,0xf9);  // f7 f9    idiv   %ecx
    } else {
        Erro ("asm Division erro ... please use as first expression !!!\n");
    }
}

UCHAR * asm_GetCode (ASM *a) {
    return a->code;
}

int asm_GetLen (ASM *a) {
    return (a->p - a->code);
}

void emit_cmp_eax_var (ASM *a, void *var) {
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

void emit_jump_jmp (ASM *a, char *name) {
    ASM_jump *jump;

    if (name && (jump = (ASM_jump*)malloc (sizeof(ASM_jump))) != NULL) {

        g(a,OP_NOP); // change this in ( asm_change_jump ) to OPCODE: 0xe9

        jump->name = strdup (name);
        jump->pos  = (a->p - a->code); // the index
        jump->type = ASM_JUMP_JMP;
        jump->exist = 0;

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
        jump->exist = 0;

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
void emit_jump_jg (ASM *a, char *name) {
    asm_conditional_jump (a, name, ASM_JUMP_JG);
}
void emit_jump_jl (ASM *a, char *name) {
    asm_conditional_jump (a, name, ASM_JUMP_JL);
}

//-------------------------------------------------------------------
//------------------------------  ERRO  -----------------------------
//-------------------------------------------------------------------
//
void Erro (char *format, ...) {
    char msg[1024] = { 0 };
    va_list ap;

    va_start (ap,format);
    vsprintf (msg, format, ap);
    va_end (ap);
    if ((strlen(strErro) + strlen(msg)) < STR_ERRO_SIZE)
        strcat (strErro, msg);
    erro++;
}
char *ErroGet (void) {
    if (strErro[0])
        return strErro;
    else
        return NULL;
}
void ErroReset (void) {
    erro = 0;
    strErro[0] = 0;
}

