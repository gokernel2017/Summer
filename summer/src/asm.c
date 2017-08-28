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
void asm_set_executable (void *ptr, unsigned long len) {
#ifdef WIN32
    unsigned long old_protect;

    if (!VirtualProtect(ptr, len, PAGE_EXECUTE_READWRITE, &old_protect)) {
        printf ("ERROR: asm_set_executable() ... NOT FOUND - VirtualProtect()\n");
        exit (-1);
    }
#endif

#ifdef __linux__
    unsigned long start, end, PageSize;

    PageSize = sysconf (_SC_PAGESIZE);
    start = (unsigned long)ptr & ~(PageSize - 1);
    end = (unsigned long)ptr + len;
    end = (end + PageSize - 1) & ~(PageSize - 1);
    if (mprotect((void *)start, end - start, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
        printf ("ERROR: asm_set_executable() ... NOT FOUND - mprotec()\n");
        exit (-1);
    }
#endif
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

void asm_call (ASM *a, void *func) {
    // b8   7a 13 40 00       mov    $0x40137a,%eax
    // ff d0                	call   *%eax
    //
    g(a,0xb8); asm_get_addr(a, func);
    g2(a,0xff,0xd0);
}
