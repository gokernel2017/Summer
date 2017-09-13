//-------------------------------------------------------------------
//
// VM (Virtual Machine).
//
//   Main Library.
//
// TANKS TO:
// ----------------------------------------------
//
//   01: God the creator of the heavens and the earth in the name of Jesus Christ.
//
// ----------------------------------------------
//
// FILE:
//   vm.c
//
// START DATE:
//   29/08/2017 - 17:42
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "vm.h"

#define STR_ERRO_SIZE 1024
#define STACK_SIZE    1024
#define ARG_SIZE      15

// global:
int     erro;
TValue  arg [ARG_SIZE];
TValue  ret;
int     arg_count;

static char strErro[STR_ERRO_SIZE];

static TValue   stack [STACK_SIZE];
static TValue   *sp = stack;
static int      flag;

void vm_run (ASM *vm) {
//    register UCHAR *code = vm->code;

    for (;;)
    switch (vm->code[vm->ip++]) {

case OP_PUSHL: // long
    sp++;
    sp->l = *(long*)(vm->code+vm->ip);
    vm->ip += sizeof(long);
    continue;

case OP_PUSHF: // float
    sp++;
    sp->f = *(float*)(vm->code+vm->ip);
    vm->ip += sizeof(float);
    continue;

case OP_PUSHVAR: {
    UCHAR i = (UCHAR)vm->code[vm->ip];
    sp++;
    switch (Gvar[i].type) {
    case TYPE_LONG:  sp->l = Gvar[i].value.l; break;
    case TYPE_FLOAT: sp->f = Gvar[i].value.f; break;
    }
    vm->ip++;
    } continue;

case OP_POPVAR: {
    UCHAR i = (UCHAR)vm->code[vm->ip];
    switch (Gvar[i].type) {
    case TYPE_LONG:  Gvar[i].value.l = sp->l; break;
    case TYPE_FLOAT: Gvar[i].value.f = sp->f; break;
    }
    vm->ip++;
    sp--;
    } continue;

case OP_INCVAR: {
    UCHAR index = (UCHAR)(vm->code[vm->ip]);
    if (Gvar[index].type==TYPE_LONG)
        Gvar[index].value.l++;
    vm->ip++;
    } continue;

case OP_MULL: sp[-1].l *= sp[0].l; sp--; continue;
case OP_DIVL: sp[-1].l /= sp[0].l; sp--; continue;
case OP_ADDL: sp[-1].l += sp[0].l; sp--; continue;
case OP_SUBL: sp[-1].l -= sp[0].l; sp--; continue;

case OP_MULF: sp[-1].f *= sp[0].f; sp--; continue;
case OP_DIVF: sp[-1].f /= sp[0].f; sp--; continue;
case OP_ADDF: sp[-1].f += sp[0].f; sp--; continue;
case OP_SUBF: sp[-1].f -= sp[0].f; sp--; continue;

case OP_CMPL:
    sp--;
    flag = sp[0].l - sp[1].l;
    sp--;
    continue;

case OP_JMP:
    vm->ip = *(unsigned short*)(vm->code+vm->ip);
    continue;

case OP_JEQ: // ==
    if (!flag)
        vm->ip = *(unsigned short*)(vm->code+vm->ip);
    else
        vm->ip += sizeof(unsigned short);
    continue;

case OP_JNE: // !=
    if (flag)
        vm->ip = *(unsigned short*)(vm->code+vm->ip);
    else
        vm->ip += sizeof(unsigned short);
    continue;

case OP_JG: // <
    if (flag > 0) 
        vm->ip = *(unsigned short*)(vm->code+vm->ip);
    else
        vm->ip += sizeof(unsigned short);
    continue;

case OP_PRINTVAR: {
    UCHAR i = (UCHAR)vm->code[vm->ip];
    switch (Gvar[i].type) {
    case TYPE_LONG:  printf ("%ld", Gvar[i].value.l); break;
    case TYPE_FLOAT: printf ("%f",  Gvar[i].value.f); break;
    }
    vm->ip++;
    } continue;

case OP_PRINTS: {
    UCHAR i = (UCHAR)(vm->code[vm->ip]);
    vm->ip++;    
    while (i--){
        if (vm->code[vm->ip]=='\\' && vm->code[vm->ip+1]=='n') {
            printf("%c", 10); // new line
            i--;
            vm->ip++;
        } else printf ("%c", vm->code[vm->ip]);

        vm->ip++;
    }
    } continue;

case OP_PRINTC:
    printf ("%c", vm->code[vm->ip++]);
    continue;

// call a VM Function
// 
case OP_CALLVM: {
    ASM *local = *(void**)(vm->code+vm->ip);
    vm->ip += sizeof(void*);
    arg_count = (UCHAR)(vm->code[vm->ip]); //printf ("CALL ARG_COUNT = %d\n", arg_count);
    vm->ip++;

    local->ip = 0;
    vm_run (local);
    } continue;

// call a C Function
// 
case OP_CALL: {
    long (*func)() = *(void**)(vm->code+vm->ip);
    float (*func_float)() = *(void**)(vm->code+vm->ip);
    vm->ip += sizeof(void*);
    arg_count = (UCHAR)(vm->code[vm->ip]); //printf ("CALL ARG_COUNT = %d\n", arg_count);
    vm->ip++;

    switch (arg_count) {
    case 0:
        if (Gvar[VAR_RET].type==TYPE_LONG)
            Gvar[VAR_RET].value.l = func();
        if (Gvar[VAR_RET].type==TYPE_FLOAT)
            Gvar[VAR_RET].value.f = func_float();

        // no return
        if (Gvar[VAR_RET].type==TYPE_NO_RETURN) func();
        break;
    case 1:
        if (Gvar[VAR_RET].type==TYPE_LONG)
            Gvar[VAR_RET].value.l = func(sp[0]);
        if (Gvar[VAR_RET].type==TYPE_FLOAT)
            Gvar[VAR_RET].value.f = func_float(sp[0]);

        // no return
        if (Gvar[VAR_RET].type==TYPE_NO_RETURN) func(sp[0]);

        sp--;
        break;
    case 2:
        arg[0] = sp[0]; sp--;
        arg[1] = sp[0]; sp--;

        if (Gvar[VAR_RET].type==TYPE_LONG) {
            // reverse order
            Gvar[VAR_RET].value.l = func(arg[1], arg[0]);
        }
        if (Gvar[VAR_RET].type==TYPE_FLOAT) {
            // reverse order
            Gvar[VAR_RET].value.f = func_float(arg[1], arg[0]);
        }
        if (Gvar[VAR_RET].type==TYPE_NO_RETURN) {
            // reverse order
            func (arg[1], arg[0]);
        }
        break;
/*
    case 3:
        if (ret_type != 'f') {
            arg[0] = sp[0]; sp--;
            arg[1] = sp[0]; sp--;
            arg[2] = sp[0]; sp--;
            // reverse order
            ret.l=func(arg[2], arg[1], arg[0]);
        } else {
            arg[0] = sp[0]; sp--;
            arg[1] = sp[0]; sp--;
            arg[2] = sp[0]; sp--;
            // reverse order
            ret.f=func_float(arg[2], arg[1], arg[0]);
        }
        break;
*/
    }//: switch (arg_count)
    } continue;

case OP_HALT:
    //printf ("OPCODE OP_HALT\n");
    return;
    }//: switch (vm->code[vm->ip++])

}//: void vm_run (VM *vm)

ASM *asm_new (unsigned long size) {
    ASM *vm = (ASM*)malloc (sizeof(ASM));

    if (vm && (vm->code = (UCHAR*)malloc(size)) != NULL) {
        vm->p = vm->code;
        vm->label = NULL;
        vm->jump = NULL;
        vm->ip = 0;
        return vm;
    }
    return NULL;
}
void asm_label (ASM *vm, char *name) {
    if (name) {
        ASM_label *lab;
        ASM_label *l = vm->label;

        // find if exist:
        while (l) {
            if (!strcmp(l->name, name)) {
                printf ("Label Exist: '%s'\n", l->name);
                return;
            }
            l = l->next;
        }

        if ((lab = (ASM_label*)malloc(sizeof(ASM_label))) != NULL) {
            lab->name = strdup (name);
            lab->pos  = (vm->p - vm->code); // the index

            // add on top:
            lab->next = vm->label;
            vm->label = lab;
        }
    }
}

void vme (ASM *vm, UCHAR c) {
    *vm->p++ = c;
}

void vme_pushl (ASM *vm, long value) {
    *vm->p++ = OP_PUSHL;
    *(long*)vm->p = value;
    vm->p += sizeof(long);
}
void vme_pushf (ASM *vm, float value) {
    *vm->p++ = OP_PUSHF;
    *(float*)vm->p = value;
    vm->p += sizeof(float);
}
void vme_popvar (ASM *vm, UCHAR i) {
    *vm->p++ = OP_POPVAR;
    *vm->p++ = i;
}
void vme_addl (ASM *vm) {
    *vm->p++ = OP_ADDL;
}
void vme_mull (ASM *vm) {
    *vm->p++ = OP_MULL;
}

void vme_addf (ASM *vm) {
    *vm->p++ = OP_ADDF;
}
void vme_mulf (ASM *vm) {
    *vm->p++ = OP_MULF;
}

void asm_end (ASM *vm) {
    ASM_label *label = vm->label;

    *vm->p++ = OP_HALT;

    //-----------------------
    // change jump:
    //-----------------------
    //
    while (label) {

        ASM_jump *jump = vm->jump;

        while (jump) {
            if (!strcmp(label->name, jump->name)) {

                *(unsigned short*)(vm->code+jump->pos) = label->pos;

            }
            jump = jump->next;
        }
        label = label->next;
    }
}
void vme_printvar (ASM *vm, UCHAR index) {
    *vm->p++ = OP_PRINTVAR;
    *vm->p++ = index;
}
void vme_printc (ASM *vm, char c) {
    *vm->p++ = OP_PRINTC;
    *vm->p++ = c;
}
void vme_prints (ASM *vm, UCHAR size, const char *str) {
    *vm->p++ = OP_PRINTS;
    *vm->p++ = size;
    while (*str) {
        *vm->p++ = *str++;
    }
}

void vme_pushvar (ASM *vm, UCHAR index) {
    *vm->p++ = OP_PUSHVAR;
    *vm->p++ = index;
}
void vme_incvar (ASM *vm, UCHAR index) {
    *vm->p++ = OP_INCVAR;
    *vm->p++ = index;
}


void vme_jmp (ASM *vm, char *name) {
    ASM_jump *jump;

    if (name && (jump = (ASM_jump*)malloc (sizeof(ASM_jump))) != NULL) {

        *vm->p++ = OP_JMP;

        jump->name = strdup (name);
        jump->pos  = (vm->p - vm->code); // the index

        // add on top:
        jump->next = vm->jump;
        vm->jump = jump;

        // to change ...
        *(unsigned short*)vm->p = (jump->pos+2); // the index
        vm->p += sizeof(unsigned short);
    }
}
void vme_jg (ASM *vm, char *name) {
    ASM_jump *jump;

    if (name && (jump = (ASM_jump*)malloc (sizeof(ASM_jump))) != NULL) {

        *vm->p++ = OP_JG;

        jump->name = strdup (name);
        jump->pos  = (vm->p - vm->code); // the index

        // add on top:
        jump->next = vm->jump;
        vm->jump = jump;

        // to change ...
        *(unsigned short*)vm->p = (jump->pos+2); // the index
        vm->p += sizeof(unsigned short);
    }
}

void vme_cmpl (ASM *vm) {
    *vm->p++ = OP_CMPL;
}

void asm_call (ASM *vm, void *func, UCHAR argc) {
    *vm->p++ = OP_CALL;
    *(void**)vm->p = func;
    vm->p += sizeof(void*);
    *vm->p++ = argc;
}
void asm_callvm (ASM *vm, void *func, UCHAR argc) {
    *vm->p++ = OP_CALLVM;
    *(void**)vm->p = func;
    vm->p += sizeof(void*);
    *vm->p++ = argc;
}

void vme_argc (ASM *vm, UCHAR c) {
    *vm->p++ = c;
}

void asm_reset (ASM *vm) {

    vm->p = vm->code;

    // reset ASM_label:
    while (vm->label != NULL) {
        ASM_label *temp = vm->label->next;
        if (vm->label->name)
            free (vm->label->name);
        free (vm->label);
        vm->label = temp;
    }
    // reset ASM_jump:
    while (vm->jump != NULL) {
        ASM_jump *temp = vm->jump->next;
        if (vm->jump->name)
            free(vm->jump->name);
        free (vm->jump);
        vm->jump = temp;
    }

    vm->label = NULL;
    vm->jump  = NULL;
    vm->ip = 0;
}

void asm_Erro (char *s) {
    erro = 1;
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
