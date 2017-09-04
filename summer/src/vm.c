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
#define VAR_SIZE      255

enum { // variable type:
    TYPE_LONG = 1,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_POINTER
};

// global:
int erro;

static char strErro[STR_ERRO_SIZE];

static VALUE  stack [STACK_SIZE];
static VALUE  *sp = stack;
static VAR    var [VAR_SIZE];
static int    flag;

void vm_run (ASM *vm) {
    register UCHAR *code = vm->code;

    for (;;)
    switch (code[vm->ip++]) {

case OP_PUSHL: // long
    sp++;
    sp->l = *(long*)(code+vm->ip);
    vm->ip += sizeof(long);
    continue;

case OP_PUSHF: // float
    sp++;
    sp->f = *(float*)(code+vm->ip);
    vm->ip += sizeof(float);
    continue;

case OP_PUSHVAR: {
    UCHAR i = (UCHAR)code[vm->ip];
    sp++;
    switch (var[i].type) {
    case TYPE_LONG:  sp->l = var[i].value.l; break;
    case TYPE_FLOAT: sp->f = var[i].value.f; break;
    }
    vm->ip++;
    } continue;

case OP_POPVAR: {
    UCHAR i = (UCHAR)code[vm->ip];
    switch (var[i].type) {
    case TYPE_LONG:  var[i].value.l = sp->l; break;
    case TYPE_FLOAT: var[i].value.f = sp->f; break;
    }
    vm->ip++;
    sp--;
    } continue;

case OP_INCVAR:
    var[ (UCHAR)(code[vm->ip]) ].value.l++;
    vm->ip++;
    continue;

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
    vm->ip = *(unsigned short*)(code+vm->ip);
    continue;

case OP_JEQ: // ==
    if (!flag)
        vm->ip = *(unsigned short*)(code+vm->ip);
    else
        vm->ip += sizeof(unsigned short);
    continue;

case OP_JNE: // !=
    if (flag)
        vm->ip = *(unsigned short*)(code+vm->ip);
    else
        vm->ip += sizeof(unsigned short);
    continue;

case OP_JG: // <
    if (flag > 0) 
        vm->ip = *(unsigned short*)(code+vm->ip);
    else
        vm->ip += sizeof(unsigned short);
    continue;

case OP_PRINTVAR: {
    UCHAR i = (UCHAR)code[vm->ip];
    switch (var[i].type) {
    case TYPE_LONG:  printf ("%ld", var[i].value.l); break;
    case TYPE_FLOAT: printf ("%f",  var[i].value.f); break;
    }
    vm->ip++;
    } continue;

case OP_PRINTS: {
    UCHAR i = (UCHAR)(code[vm->ip]);
    vm->ip++;    
    while (i--)
        printf ("%c", code[vm->ip++]);
    } continue;

case OP_PRINTC:
    printf ("%c", code[vm->ip++]);
    continue;

case OP_HALT:
    printf ("\nOpcode HALT - (sp - stack): %d\n", (sp - stack));
    return;
    }//: switch (code[vm->ip++])

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
void vm_var (char *name, int type) {
    VAR *v = var;
    int i = 0;
    while (v->name) {
        if (!strcmp(v->name, name))
      return;
        v++;
        i++;
    }
    if (i < VAR_SIZE) {
        v->name = strdup(name);
        v->type = type;
    }
}

void vm_label (ASM *vm, char *name) {
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

int vm_var_find (char *name) {
    VAR *v = var;
    int i = 0;
    while(v->name) {
        if (!strcmp(v->name, name))
      return i;
        v++;
        i++;
    }
    return -1;
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
