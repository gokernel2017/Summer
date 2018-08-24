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
// The back-end | VM:
//
// FILE:
//   vm.c
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite: 20/07/2018 - 11:10
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "summer.h"

#define STACK_SIZE    1024

static VALUE    stack [STACK_SIZE];
static VALUE  * sp = stack;
static VALUE    eax;

void Run (ASM *a) {
    vm_run (a);
}

//-------------------------------------------------------------------
//--------------------------  START VM RUN  -------------------------
//-------------------------------------------------------------------
//
void vm_run (ASM *a) {

    a->ip = 0;

    for (;;) {
    switch (a->code[a->ip++]) {

case OP_PUSH_INT: // size: 5
    sp++;
    sp->i = *(int*)(a->code+a->ip);
    a->ip += sizeof(int);
    continue;

case OP_PUSH_VAR: { // size: 2
    UCHAR i = (UCHAR)a->code[a->ip++];
    sp++;
    sp[0] = Gvar[i].value;
    } continue;

case OP_POP_VAR: { // size: 2
    UCHAR i = (UCHAR)a->code[a->ip++];
    Gvar[i].value = sp[0];
    sp--;
    } continue;

case OP_MUL_INT: sp[-1].i *= sp[0].i; sp--; continue; // size: 1
case OP_DIV_INT: sp[-1].i /= sp[0].i; sp--; continue; // size: 1
case OP_ADD_INT: sp[-1].i += sp[0].i; sp--; continue; // size: 1
case OP_SUB_INT: sp[-1].i -= sp[0].i; sp--; continue; // size: 1

case OP_POP_EAX:
    eax = sp[0];
    sp--;
    continue;

case OP_PRINT_EAX: {
    UCHAR i = (UCHAR)a->code[a->ip++];
    switch (i) {
    case TYPE_INT:   printf ("%d\n", eax.i); break;
    case TYPE_FLOAT: printf ("%f\n", eax.f); break;
    }
    } continue;

case OP_MOV_EAX_VAR: {
    UCHAR i = (UCHAR)a->code[a->ip++];
    Gvar[i].value = eax;
    } continue;

case OP_PUSH_STRING: {
    char *s = *(void**)(a->code+a->ip);
    a->ip += sizeof(void*);
    sp++;
    sp->s = s;
    } continue;

//
// call a C Function
//
case OP_CALL:
    {
    int (*func)() = *(void**)(a->code+a->ip);
    float (*func_float)() = *(void**)(a->code+a->ip);
    a->ip += sizeof(void*);
    UCHAR arg_count = (UCHAR)(a->code[a->ip++]);
    UCHAR return_type = (UCHAR)(a->code[a->ip++]);

    switch (arg_count) {
    case 0: // no argument
        if (return_type == TYPE_NO_RETURN) // 0 | no return
            func ();
        else if (return_type == TYPE_FLOAT)
            eax.f = func_float ();
        else
            eax.i = func ();
        break; //: case 0:

    case 1: // 1 argument
        if (return_type == TYPE_NO_RETURN) // 0 | no return
            func (sp[0]);
        else if (return_type == TYPE_FLOAT)
            eax.f = func_float (sp[0]);
        else
            eax.i = func (sp[0]);
        sp--;
        break; //: case 1:

    case 2: // 2 arguents
        if (return_type == TYPE_NO_RETURN) // 0 | no return
            func (sp[-1], sp[0]);
        else if (return_type == TYPE_FLOAT)
            eax.f = func_float (sp[-1], sp[0]);
        else
            eax.i = func (sp[-1], sp[0]);
        sp -= 2;
        break; //: case 2:

    case 3: // 3 arguents
        if (return_type == TYPE_NO_RETURN) // 0 | no return
            func (sp[-2], sp[-1], sp[0]);
        else if (return_type == TYPE_FLOAT)
            eax.f = func_float (sp[-2], sp[-1], sp[0]);
        else
            eax.i = func (sp[-2], sp[-1], sp[0]);
        sp -= 3;
        break; //: case 3:

    case 4: // 4 arguents
        if (return_type == TYPE_NO_RETURN) // 0 | no return
            func (sp[-3], sp[-2], sp[-1], sp[0]);
        else if (return_type == TYPE_FLOAT)
            eax.f = func_float (sp[-3], sp[-2], sp[-1], sp[0]);
        else
            eax.i = func (sp[-3], sp[-2], sp[-1], sp[0]);
        sp -= 4;
        break; //: case 4:

    case 5: // 5 arguents
        if (return_type == TYPE_NO_RETURN) // 0 | no return
            func (sp[-4], sp[-3], sp[-2], sp[-1], sp[0]);
        else if (return_type == TYPE_FLOAT)
            eax.f = func_float (sp[-4], sp[-3], sp[-2], sp[-1], sp[0]);
        else
            eax.i = func (sp[-4], sp[-3], sp[-2], sp[-1], sp[0]);
        sp -= 5;
        break; //: case 5:

    }//: switch (arg_count)

    } continue; //: case OP_CALL:

case OP_HALT: // size: 1
    a->ip = 0;
    //printf ("OP_HALT: >>>>>>> (sp - stack) = %d\n", (int)(sp - stack));
    return;
    }//: switch (a->code[a->ip++])
    }//: for (;;)

}//: vm_run ()
//
//-------------------------------------------------------------------
//---------------------------  END VM RUN  --------------------------
//-------------------------------------------------------------------

ASM *asm_new (unsigned int size) {
    ASM *a = (ASM*)malloc (sizeof(ASM));
    if (a && (a->code = (UCHAR*)malloc(size)) != NULL) {
        a->p = a->code;
        a->label = NULL;
        a->jump  = NULL;
        a->size  = size;
        a->ip    = 0;
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
    a->ip = 0;
}

void asm_begin (ASM *a) {
}

void asm_end (ASM *a) {
    ASM_label *label = a->label;

    emit_halt(a);

    //-----------------------
    // change jump:
    //-----------------------
    //
    while (label) {

        ASM_jump *jump = a->jump;

        while (jump) {
            if (!strcmp(label->name, jump->name)) {

                *(unsigned short*)(a->code+jump->pos) = label->pos;

            }
            jump = jump->next;
        }
        label = label->next;
    }
}

int asm_get_len (ASM *a) {
    return (a->p - a->code);
}

//-------------------------------------------------------------------
//------------------------------  EMIT  -----------------------------
//-------------------------------------------------------------------
//
void emit_push_int (ASM *a, int i) {
    *a->p++ = OP_PUSH_INT;
    *(int*)a->p = i;
    a->p += sizeof(int);
}
void emit_push_var (ASM *a, UCHAR i) {
    *a->p++ = OP_PUSH_VAR;
    *a->p++ = i;
}
void emit_pop_var (ASM *a, UCHAR i) {
    *a->p++ = OP_POP_VAR;
    *a->p++ = i;
}
void emit_mul_int (ASM *a) {
    *a->p++ = OP_MUL_INT;
}
void emit_div_int (ASM *a) {
    *a->p++ = OP_DIV_INT;
}
void emit_add_int (ASM *a) {
    *a->p++ = OP_ADD_INT;
}
void emit_sub_int (ASM *a) {
    *a->p++ = OP_SUB_INT;
}
void emit_pop_eax (ASM *a) {
    *a->p++ = OP_POP_EAX;
}
void emit_print_eax (ASM *a, UCHAR type) {
    *a->p++ = OP_PRINT_EAX;
    *a->p++ = type;
}

void emit_mov_eax_var (ASM *a, UCHAR index) {
    *a->p++ = OP_MOV_EAX_VAR;
    *a->p++ = index;
}

void emit_push_string (ASM *a, char *s) {
    *a->p++ = OP_PUSH_STRING;
    *(void**)a->p = s;
    a->p += sizeof(void*);
}

void emit_call (ASM *a, void *func, UCHAR arg_count, UCHAR return_type) {
    *a->p++ = OP_CALL;
    *(void**)a->p = func;
    a->p += sizeof(void*);
    *a->p++ = arg_count;
    *a->p++ = return_type;
}

void emit_halt (ASM *a) {
    *a->p++ = OP_HALT;
}

