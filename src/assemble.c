//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
// Assemble:
//   ASSEMBLE START DATE: 02/04/2019 - 19:00
//
// FILE:
//   assemble.c
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite 01: 20/07/2018 - 11:10
//   rewrite 02: 23/03/2019 - 08:50
//
// IMPLEMENTED:
//   mov  %eax , 4 ( %esp )
//   mov  %edx , 4 ( %eax )
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#include "summer.h"

#define ARG_MAX	15

enum { // arg.tok[]
    T_ID = 1,
    T_REG,
    T_NUM,
    T_VAR,
    T_FUN,
    T_STR,
    T_LABEL,
    T_PRE			// #
};

//-----------------------------------------------
//-----------------  PROTOTYPES  ----------------
//-----------------------------------------------
//
static void execute_function (LEXER *l, ASM *a);
static int GetReg (char *s);
//
static int parse_assemble_4 (ASM *a);
static int parse_assemble_7 (ASM *a);
//static int op_mov_4 (ASM *a);
static int op_mov_5 (ASM *a);

struct ARG {
    char  *string;
    char  text  [ ARG_MAX+1 ] [ LEXER_TOKEN_SIZE + 1 ];
    int   tok   [ ARG_MAX+1 ];
    int   value [ ARG_MAX+1 ]; // register_number / variable_number
    int   count;
};
struct ASSEMBLE_4 {
    char  *name;
    char  a1;
    char  a2;
    char  a3;
}assemble_4[] = {
  //---------------------------------------------
  // ... PLEASE DONT CHANGE THIS ORDER ...
  //
  // See the function:
  //   static int parse_assemble_4 (ASM *a);
  //---------------------------------------------
  //
  { "mov", T_VAR,  ',',  T_REG },  // mov VAR , %REG
  { "mov", T_FUN,  ',',  T_REG },  // mov FUN , %REG

  { "mov", T_STR,  ',',  T_REG },  // mov STR , %REG

  { "mov", T_REG,  ',',  T_VAR },  // mov %REG , VAR
  { "cmp", T_REG,  ',',  T_VAR },  // cmp %REG , VAR
  { NULL, 0, 0, 0 }
};
//cmp   %eax , %edx

struct ASSEMBLE_7 {
    char  *name;
    char  a1;
    char  a2;
    char  a3;
    char  a4;
    char  a5;
    char  a6;
}assemble_7[] = {
  // mov  %eax , 4 ( %esp )
  { "mov", T_REG,  ',',  T_NUM, '(', T_REG, ')' },
  { NULL, 0, 0, 0, 0, 0, 0 }
};

struct ASSEMBLE_5 {
    char  *name;
    int   (*call) (ASM *a);
    char  a1;
    char  a2;
    char  a3;
    char  a4;
}assemble_5[] = {
  //
  // mov  0x4  (  %eax  )  ,  %eax
  //
  { "mov",  op_mov_5,  '$',    T_NUM,  ',', T_REG }, // mov $ NUM , %REG
  { NULL, NULL, 0, 0, 0, 0 }
};

//-----------------------------------------------
//-----------------  VARIABLES  -----------------
//-----------------------------------------------
//


static char *reg[] = {
    "%eax", "%ecx", "%edx", "%ebx", "%esp", "%ebp", "%esi", "%edi", // 32 bits
    "%rax", "%rcx", "%rdx", "%rsi", "%rdi", // 64 bits
    0 
};

static char *str;

static TFunc *fun;
static struct ARG arg;

int ifndef_true;

static int
    count
    ;

int alex (void) {
    register char *p = arg.text[count];
    int pre = 0; // '#'

    *p = 0;

label_top:
    if (*str <= 32) {
        if (*str==0) return 0;
        str++;
        goto label_top;
    }

    //################  STRING  #################
    //
    if (*str == '"') {
        str++; // '"'
        while (*str && *str != '"' && *str != '\r' && *str != '\n') {
            *p++ = *str++;
        }
        *p = 0;
        if (*str=='"') str++;// else Erro("%s: Line: %d - String erro", l->name, l->line);
        return (arg.tok[count] = T_STR);
    }

    //##########  WORD, IDENTIFIER ...  #########
    //
    if ((*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z') || *str == '_' || *str == '%' || *str == '#') {
        if (*str=='#') pre = 1;
        *p++ = *str++;
        while ((*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z') || (*str >= '0' && *str <= '9') || *str=='_') {
            *p++ = *str++;
        }
        *p = 0;

        if (pre) {
            return (arg.tok[count] = T_PRE);
        }

        if (*str == ':') {
            str++;
            return (arg.tok[count] = T_LABEL);
        }

        char *text = arg.text[count];
					
        if (text[0] == '%' && (text[1] == 'e' || text[1] == 'r') && text[4] == 0) {
            if ((arg.value[count] = GetReg(text)) != -1)
                return (arg.tok[count] = T_REG);
        }
				
        if ((arg.value[count] = VarFind(text)) != -1) {
            return (arg.tok[count] = T_VAR);
        }

        if ((fun = FuncFind(text)) != NULL) {
            return (arg.tok[count] = T_FUN);
        }

        return (arg.tok[count] = T_ID);
    }

    //#################  NUMBER  ################
    //
    if (*str >= '0' && *str <= '9') {
        while ((*str >= '0' && *str <= '9') || *str == '.') {
            *p++ = *str++;
        }
        *p = 0;
        arg.value[count] = atoi(arg.text[count]);
        return (arg.tok[count] = T_NUM);
    }

    // comment:
    if (*str == '/' && str[1]=='/') {
        return 0;
    }

    *p++ = *str;
    *p = 0;
    return (arg.tok[count] = *str++);
}


static int store_arg (char *text, LEXER *l) {
    arg.string = text;
    str = text;
    count = 0;
    while (alex()) { if (count < ARG_MAX) count++; }
    return (arg.count = count);
}

void aparse (char *text, LEXER *l, ASM *a) {
    static int IFDEF_ENDIF_block_count = 0, defined_not = 0;

    if (store_arg(text, l)==0)
  return;

    //---------------------------------------------------------------
    //-------------------  START: #ifdef / #endif  ------------------
    //---------------------------------------------------------------

    //------------  process "not_defined" ... THIS RETURN -----------
    // ...

    if (arg.tok[0]==T_PRE && arg.count==1 && !strcmp(arg.text[0], "#endif")) {
        if (defined_not)
            IFDEF_ENDIF_block_count--;
    }

    if (arg.tok[0]==T_PRE && arg.count==2 && !strcmp(arg.text[0], "#ifdef")) {
        if (is_defined(arg.text[1])==0)
            defined_not = 1;
        if (defined_not)
            IFDEF_ENDIF_block_count++;
// if (defined_not==0) printf ("DEFINED(%s) line: %d, block_count = %d\n", arg.text[1], l->line, block_count);
    }

    if (IFDEF_ENDIF_block_count == 0 && defined_not)
        defined_not = 0;

    //<<<<<<<<<<<<<<<<<<<<<<<<  RETURN HERE  >>>>>>>>>>>>>>>>>>>>>>>>
    //
    if (defined_not)
  return;

    //  ...
    //------------  process "not_defined" ... THIS RETURN -----------

    if (arg.tok[0]==T_PRE && arg.count==1 && !strcmp(arg.text[0], "#endif")) {
        ifndef_true = 0;
  return;
    }

    if (ifndef_true == 1)
  return;

    if (arg.tok[0]==T_PRE && arg.count==2 && !strcmp(arg.text[0], "#ifdef")) {
        proc_ifdef (arg.text[1]);
  return;
    }

    //---------------------------------------------------------------
    //--------------------  END: #ifdef / #endif  -------------------
    //---------------------------------------------------------------

    if (arg.tok[0]==T_LABEL) {
        asm_Label (a, arg.text[0]);
  return;
    }

    // execute a function
    // SYNTAX: function_name arg1 arg2 arg3 ...
    //
    if (arg.tok[0] == T_FUN && fun != NULL) {
        execute_function(l,a);
        return;
    }

    if (arg.count == 4 && parse_assemble_4(a))
        return;

    if (arg.count == 7 && parse_assemble_7(a))
        return;

    if (arg.count == 5) {
        struct ASSEMBLE_5 *o = assemble_5;
        while (o->name) {
            if (!strcmp(o->name, arg.text[0])) {
                if (arg.tok[1] == o->a1 && arg.tok[2] == o->a2 && arg.tok[3] == o->a3 && arg.tok[4] == o->a4) {
                    if (o->call(a)) return;
                }
            }
            o++;
        }
    }


    // with 2 arguments:
    //
    if (arg.count == 2) {
        // call function_name
        //
        if (!strcmp(arg.text[0], "call") && arg.tok[1] == T_FUN && fun != NULL) {
            emit_call(a,fun->code);
            return;
        }
        if (!strcmp(arg.text[0], "incl") && arg.tok[1] == T_VAR) {
            emit_incl(a, &Gvar[ arg.value[1] ].value.l);
            return;
        }
        if (!strcmp(arg.text[0], "jmp")) {
            emit_jump_jmp (a, arg.text[1]);
            return;
        }
        if (!strcmp(arg.text[0], "jge")) {
            emit_jump_jge (a, arg.text[1]);
            return;
        }
        if (!strcmp(arg.text[0], "jle")) {
            emit_jump_jle (a, arg.text[1]);
            return;
        }
        if (!strcmp(arg.text[0], "jl")) {
            emit_jump_jl (a, arg.text[1]);
            return;
        }
        if (!strcmp(arg.text[0], "jg")) {
            emit_jump_jg (a, arg.text[1]);
            return;
        }

// 32 bits
//  39 c1                	cmp    %eax,%ecx
//  39 c2                	cmp    %eax,%edx

// 7d 0d                	jge    40151f <_funcao+0x1f>

    }

    Erro ("%s: %d - ASSEMBLY Ilegal Opcode: '%s'\n", l->name, l->line, text);
}

void Assemble (LEXER *l, ASM *a) {
    register int c, comment = 0;
    register char *p;
    char text [LEXER_TOKEN_SIZE+1];
    p = text;
    *p = 0;

    int line = l->line;

    if (lex(l) != '{') { // create function
        char name[255], proto[255] = { '0', '0', 0, 0, 0, 0, 0, 0 };
        int i;
        TFunc *func;
        UCHAR *code;

        strcpy (name, l->token);
        if (lex(l) != '{') {
            Erro ("%s: %d - Need Start Block '{' | ASM USAGE: asm { ... }\n", l->name, line);
            return;
        }
        // if exist ... return
        if (FuncFind(name) != NULL) {
            printf ("Function exist: ... REBOBINANDO '%s'\n", name);
            while (lex(l) && l->tok != '}') {}
      return;
        }
        asm_Reset(asm_function);
        emit_begin(asm_function);
        comment = 0;
        while ((c = l->text[l->pos++])) {
            if (c=='/' && l->text[l->pos]=='/') comment = 1;
            if (c == '\n' || c == '|' || (c == '}' && comment==0)) {
                *p = 0;
                if (text[0] != 0) {
                    aparse(text, l, asm_function);
                }
                p = text;
                *p = 0;
                comment = 0;
                if (c=='\n') l->line++;
            } else *p++ = c;
            if (c == '}' && comment==0) break;
        }
        emit_end (asm_function);

        if (c != '}') Erro ("ASM ERRO ... Need Close Block: '}'\n");

        if (erro) return;

        // new function:
        //
        if ((func = (TFunc*) malloc (sizeof(TFunc))) == NULL) {
            printf ("Create Function(%s) Erro\n", name);
            return;
        }
        func->name = strdup (name);
        func->proto = strdup (proto);
        func->type = FUNC_TYPE_COMPILED;
        func->len = asm_GetLen(asm_function);
        func->code = (UCHAR*) malloc (func->len);

        code = asm_GetCode(asm_function);

        // NOW: copy the buffer ( f ):
        for (i=0;i<func->len;i++) {
            func->code[i] = code[i];
        }

        asm_SetExecutable_PTR (func->code, func->len);

        FuncAdd (func);

    } else {
        comment = 0;
        while ((c = l->text[l->pos++])) {
            if (c=='/' && l->text[l->pos]=='/') comment = 1;
            if (c == '\n' || c == '|' || (c == '}' && comment==0)) {
                *p = 0;
                if (text[0] != 0) {
                    aparse(text, l, a);
                }
                p = text;
                *p = 0;
                comment = 0;
                if (c=='\n') l->line++;
            } else *p++ = c;
            if (c == '}' && comment==0) break;
        }
        if (c != '}') Erro ("ASM ERRO ... Need Close Block: '}'\n");
    }
}

static int GetReg (char *s) {
    int i = 0;
    while (reg[i]) {
        if (!strcmp(reg[i], s)) {
            return i;
        }
        i++;
    }
    return -1;
}

// mov $ NUM , %REG
//
static int op_mov_5 (ASM *a) {
    if (arg.tok[1] == '$') {
        if (arg.tok[2] == T_NUM && arg.tok[3] == ',' && arg.tok[4] == T_REG) {
            emit_mov_long_reg (a, atol(arg.text[2]), arg.value[4]);
            return 1;
        }
    }
    return 0;
}

static int parse_assemble_4 (ASM *a) {
    int i = 0;
    struct ASSEMBLE_4 *o = assemble_4;
    while (o->name) {
        if (!strcmp(o->name, arg.text[0]) && arg.tok[1] == o->a1 && arg.tok[2] == o->a2 && arg.tok[3] == o->a3) {
            //-----------------------------------
            // mov var , %eax
            //
            //   arg.text[0] = "mov"
            //   arg.tok[1]  = T_VAR
            //   arg.tok[2]  = ','
            //   arg.tok[3]  = T_REG
            //-----------------------------------
            // 00: mov VAR , %REG
            // 01: mov %REG , VAR
            // 02: cmp %REG , VAR
            switch (i) {
            // if (arg.tok[1] == T_VAR && arg.tok[2]==',' && arg.tok[3]==T_REG)
            case 0: emit_mov_var_reg (a, &Gvar[ arg.value[1] ].value.l, arg.value[3]); return 1;
            case 1: { printf ("funcao(%s) para registro(%s)\n", fun->name, reg[arg.value[3]] ); emit_mov_var_reg (a, &fun->code, arg.value[3]); return 1; }

            case 2: // mov T_STR , T_REG
                {
                TFstring *s = fs_new(arg.text[1]);
                if (s) {
                    emit_mov_var_reg(a, &(s->s), arg.value[3]);
                }
                }
                return 1;

            case 3: emit_mov_reg_var (a, arg.value[1], &Gvar[ arg.value[3] ].value.l); return 1;
            case 4: emit_cmp_eax_var (a, &Gvar[ arg.value[3] ].value.l); return 1;
            }
        }
        o++; i++;
    }
    return 0;
}

//  { "mov", T_REG,  ',',  T_NUM, '(', T_REG, ')' },
static int parse_assemble_7 (ASM *a) {
    struct ASSEMBLE_7 *o = assemble_7;
    while (o->name) {
        if (!strcmp(o->name, arg.text[0])
            && arg.tok[1] == o->a1 && arg.tok[2] == o->a2 && arg.tok[3] == o->a3
            && arg.tok[4] == o->a4 && arg.tok[5] == o->a5 && arg.tok[6] == o->a6)
        {
            //
            // mov  %eax , 4 ( %esp )
            //
            if (arg.value[1]==EAX && arg.value[5]==ESP) {
                emit_mov_eax_ESP (a, arg.value[3]);
                return 1;
            }
            //
            // mov  %edx , 4 ( %eax )
            //
            if (arg.value[1]==EDX && arg.value[5]==EAX) {
                emit_mov_edx_EAX (a, arg.value[3]);
                return 1;
            }
        }
        o++;
    }
    return 0;
}

// INFO: Windows X64 BITS functions arguments:
// arg 1 = %ecx
// arg 2 = %edx
// arg 3 = %r8d
// arg 4 = %r9d
//
// INFO: Linux X64 BITS functions arguments:
// arg 1 = %edi
// arg 2 = %esi
// arg 3 = %edx
// arg 4 = %ecx
// arg 5 = %r8
// arg 6 = %r9

static void execute_function (LEXER *l, ASM *a) {
    // add 100 200
    if (arg.count > 1) {
        int i;
        for (i = 1; i < arg.count; i++) {
        #ifdef __x86_64__
            #ifdef WIN32
            if (i == 1) {
                if (arg.tok[1] == T_NUM)
                    emit_mov_long_reg (a, atol(arg.text[1]), ECX);
                if (arg.tok[1] == T_VAR) {
                    if (Gvar[ arg.value[1] ].type != TYPE_FLOAT)
                        emit_mov_var_reg (a, &Gvar[ arg.value[1] ].value.l, ECX);
                    else
                        emit_func_arg_var_float0 (a, &Gvar[ arg.value[1] ].value.f);
                }
                if (arg.tok[1] == T_STR) {
                    TFstring *s = fs_new (arg.text[1]);
                    if (s) {
//                        emit_mov_long_reg (a,(long)(s->s), ECX);
                        emit_mov_var_reg (a, &(s->s), RCX);
printf ("ASM pasaando string\n");
                    }
                }
            }
            if (i == 2) {
                if (arg.tok[2] == T_NUM)
                    emit_mov_long_reg(a,atol(arg.text[2]),EDX);
                if (arg.tok[2] == T_VAR) {
                    if (Gvar[ arg.value[2] ].type != TYPE_FLOAT)
                        emit_mov_var_reg (a, &Gvar[ arg.value[2] ].value.l, EDX);
                    else
                        emit_func_arg_var_float1 (a, &Gvar[ arg.value[2] ].value.f);
                }
                if (arg.tok[2] == T_STR) {
                    TFstring *s = fs_new (arg.text[2]);
                    if (s) {
                        emit_mov_long_reg (a,(long)(s->s), EDX);
printf ("ASM pasaando string\n");
                    }
                }

            }
            if (i == 3) {
                if (arg.tok[2] == T_NUM) {
                    if (strchr(arg.text[2], '.'))
                        emit_mov_long_reg(a,atol(arg.text[2]),EDX);
                    else
                        emit_func_arg_number_float2 (a, atof(arg.text[2]));
                }
                if (arg.tok[3] == T_VAR) {
                    if (Gvar[ arg.value[3] ].type != TYPE_FLOAT) {
                        emit_mov_var_reg (a, &Gvar[ arg.value[3] ].value.l, EAX);
                        // 41 89 c0   mov    %eax,%r8d
                        g3(a,0x41,0x89,0xc0);
                    } else {
//                        emit_func_arg_var_float2 (a, &Gvar[ arg.value[3] ].value.f);
                        emit_mov_var_reg(a, &Gvar[ arg.value[3] ].value.f, EAX);
                        // 41 89 c0   mov    %eax,%r8d
                        g3(a,0x41,0x89,0xc0);
                    }
                }
            }
            #endif // WIN32

            #ifdef __linux__
            if (i == 1) {
                if (arg.tok[1] == T_NUM)
                    emit_mov_long_reg(a,atol(arg.text[1]),EDI);
                if (arg.tok[1] == T_VAR)
                    emit_mov_var_reg (a, &Gvar[ arg.value[1] ].value.l, EDI);
            }
            if (i == 2) {
                if (arg.tok[2] == T_NUM)
                    emit_mov_long_reg(a,atol(arg.text[2]),ESI);
                if (arg.tok[2] == T_VAR)
                    emit_mov_var_reg (a, &Gvar[ arg.value[2] ].value.l, ESI);
            }
            #endif // __linux__
        #else // __x86_64__
            // 32 bits
            if (arg.tok[i] == T_NUM)
                emit_movl_ESP(a, atol(arg.text[i]), (i - 1)*4);
            if (arg.tok[i] == T_VAR) {
                emit_mov_var_reg (a, &Gvar[ arg.value[i] ].value.l, EAX);
                emit_mov_eax_ESP (a, (i - 1)*4);
            }
        #endif
        }
    }
    emit_call(a,fun->code);
}
// line 412

