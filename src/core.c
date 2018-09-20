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
// Main Core Language:
//
// FILE:
//   core.c
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite: 20/07/2018 - 11:10
//
// BY:  Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
// FILE SIZE: 49.251
#include "summer.h"
#include <stdarg.h>
#include <time.h>

#ifdef USE_APPLICATION
    #include <GL/gl.h>
#endif

//-----------------------------------------------
//-----------------  PROTOTYPES  ----------------
//-----------------------------------------------
//
static void   word_int      (LEXER *l, ASM *a);
static void   word_var      (LEXER *l, ASM *a);
static void   word_if       (LEXER *l, ASM *a);
static void   word_for      (LEXER *l, ASM *a);
static void   word_break    (LEXER *l, ASM *a);
static void   word_module   (LEXER *l, ASM *a);
static void   word_import   (LEXER *l, ASM *a);
static void   word_function (LEXER *l, ASM *a);
//
//static void   word_DEFINE   (LEXER *l, ASM *a);
static void   word_IFDEF    (LEXER *l, ASM *a);
//
static void   expression    (LEXER *l, ASM *a);
//--------------  Expression:  --------------
static int    expr0         (LEXER *l, ASM *a);
static void   expr1         (LEXER *l, ASM *a);
static void   expr2         (LEXER *l, ASM *a);
static void   expr3         (LEXER *l, ASM *a);
static void   atom          (LEXER *l, ASM *a);
//-------------------------------------------
static void   do_block      (LEXER *l, ASM *a);
static int    stmt          (LEXER *l, ASM *a);
static int    see           (LEXER *l);
static void   core_ModuleAdd(char *module_name, char *func_name, char *proto, UCHAR *code);
static void   core_DefineAdd(char *name, int value);
F_STRING    * fs_new        (char *s);
static void   execute_call  (LEXER *l, ASM *a, TFunc *func);
// stdlib:
void  lib_info    (int arg);
int   lib_addi     (int a, int b);
float lib_addf     (float a, float b);
int   lib_arg     (int a, int b, int c, int d, int e);
void  lib_printf  (char *format, ...);
void  lib_log (char *s); // console.log();
TFunc *lib_get_func (char *name);
void disasm (char *name);
void time_fps (void);

extern void testDrawTriangle (void);

//-----------------------------------------------
//------------------  VARIABLES  ----------------
//-----------------------------------------------
//
TVar              Gvar [GVAR_SIZE]; // global:
int               asm_mode;
//
static MODULE   * Gmodule = NULL; // ... console.log(); ...
static TFunc    * Gfunc = NULL;  // store the user functions
static DEFINE   * Gdefine = NULL;
static ASM      * asm_function;
static ARG        argument [20];
static F_STRING * fs = NULL;

char            write_var_name[100];
char            write_func_name[100];

static char
    strErro     [STR_ERRO_SIZE],
    func_name   [100],
    array_break [20][20]   // used to word break
    ;

static int
    erro,
    loop_level,
    is_function,
    is_recursive,
    main_variable_type,
    var_type,
    argument_count,
    local_count,
    mov64_rdi_RBP
    ;

float _Fvalue_;

static TFunc stdlib[]={
  //-----------------------------------------------------------------
  // char*        char*   UCHAR*/ASM*             int   int   FUNC*
  // name         proto   code                    type  len   next
  //-----------------------------------------------------------------
  { "info",         "0i",       (UCHAR*)lib_info,       0,    0,    NULL },
  { "addi",         "iii",      (UCHAR*)lib_addi,       0,    0,    NULL },
  { "addf",         "fff",      (UCHAR*)lib_addf,       0,    0,    NULL },
  { "arg",          "iiiiii",   (UCHAR*)lib_arg,        0,    0,    NULL },
  { "printf",       "0s",       (UCHAR*)lib_printf,     0,    0,    NULL },
  { "get_func",     "ps",       (UCHAR*)lib_get_func,   0,    0,    NULL },
  { "fps",          "00",       (UCHAR*)time_fps,       0,    0,    NULL },
#ifdef USE_DISASM
  { "disasm",       "0s",       (UCHAR*)disasm,         0,    0,    NULL },
#endif
#ifdef USE_GA // Graphic Application API:
  { "gaInit",         "iiip",     (UCHAR*)gaInit,         0,    0,    NULL },
  { "gaRun",          "00",       (UCHAR*)gaRun,          0,    0,    NULL },
  { "gaBeginScene",   "00",       (UCHAR*)gaBeginScene,   0,    0,    NULL },
  { "gaEndScene",     "00",       (UCHAR*)gaEndScene,     0,    0,    NULL },
  { "gaText",         "0siii",    (UCHAR*)gaText,         0,    0,    NULL },
  { "gaFPS",          "i0",       (UCHAR*)gaFPS,          0,    0,    NULL },
  { "gaSetCall",      "0ps",      (UCHAR*)gaSetCall,      0,    0,    NULL },
  { "gaDisplayMouse", "0ii",      (UCHAR*)gaDisplayMouse, 0,    0,    NULL },
  { "gaButton",       "0iiiis",   (UCHAR*)gaButton,       0,    0,    NULL },
//  { "rate",           "ii",       (UCHAR*)rate,           0,    0,    NULL },
#endif
  //
  // Application API ... Only WIN32 ...:
  //
#ifdef USE_APPLICATION
  { "AppInit",        "iis",      (UCHAR*)AppInit,        0,    0,    NULL },
  { "AppRun",         "0p",       (UCHAR*)AppRun,         0,    0,    NULL },
  { "AppNewWindow",   "ppiiiis",  (UCHAR*)AppNewWindow,   0,    0,    NULL },
  { "AppNewRenderGL", "ppiiiis",  (UCHAR*)AppNewRenderGL, 0,    0,    NULL },
  { "AppNewButton",   "ppiiiis",  (UCHAR*)AppNewButton,   0,    0,    NULL },
  { "AppSetCall",     "0pp",      (UCHAR*)AppSetCall,     0,    0,    NULL },
  { "AppRender",      "00",       (UCHAR*)AppRender,      0,    0,    NULL },
  { "Sleep",          "0i",       (UCHAR*)Sleep,          0,    0,    NULL },
  // OpenGL:
  { "glBegin",        "0i",       (UCHAR*)glBegin,        0,    0,    NULL },
  { "glEnd",          "00",       (UCHAR*)glEnd,          0,    0,    NULL },
  { "glVertex2f",     "0ff",      (UCHAR*)glVertex2f,     0,    0,    NULL },
  { "glColor3f",      "0ff",      (UCHAR*)glColor3f,      0,    0,    NULL },
  { "glRotatef",      "0ffff",    (UCHAR*)glRotatef,      0,    0,    NULL },
  { "glPushMatrix",   "00",       (UCHAR*)glPushMatrix,   0,    0,    NULL },
  { "glPopMatrix",    "00",       (UCHAR*)glPopMatrix,    0,    0,    NULL },
  { "glClearColor",   "0ffff",    (UCHAR*)glClearColor,   0,    0,    NULL },
  { "glClear",        "0i",       (UCHAR*)glClear,        0,    0,    NULL },
#endif
  { NULL,             NULL,       NULL,                   0,    0,    NULL }
};

void func_null (void) { printf ("FUNCTION: func_null\n"); }
TFunc func_null_default = { "func_null", "00", (UCHAR*)func_null, 0, 0, NULL };


//-----------------------------------------------
//------------------  FUNCTIONS  ----------------
//-----------------------------------------------
//
ASM * core_Init (unsigned int size) {
    static int init = 0;
    ASM *a;
    if (!init) {
        init = 1;
        if ((a            = asm_new (size))==NULL) return NULL;
        if ((asm_function = asm_new (size))==NULL) return NULL;

        core_ModuleAdd ("console", "log", "0s", (UCHAR*)lib_printf);

        #ifdef USE_APPLICATION
        //---------------------------------------
        // object : Gvar[0] | pointer
        // mx     : Gvar[1] | int : mouse_x
        // my     : Gvar[2] | int : mouse_y
        //---------------------------------------
        CreateVarInt ("object", 0);
        CreateVarInt ("mx", 0); // mouse_x
        CreateVarInt ("my", 0); // mouse_y
        CreateVarInt ("mb", 0); // mouse_button
        //
        CreateVarInt ("MOUSEMOVE", 512); // constant
        #endif // ! USE_APPLICATION

        #ifdef USE_JIT
        core_DefineAdd ("USE_JIT", 1);
        #endif
        #ifdef USE_VM
        core_DefineAdd ("USE_VM", 2);
        #endif

        #ifdef WIN32
        core_DefineAdd("WIN32", 1);
        #endif
        #ifdef __linux__
        core_DefineAdd("__linux__", 2);
        #endif

        return a;
    }
    return NULL;
}

void core_Finalize (void) {
/*
    MODULE *p = Gmodule;
    while (p) {
        TFunc *f = p->func;
        printf ("MODULE: '%s'\n", p->name);
        while (f) {
            printf ("  '%s'\n", f->name);
            f = f->next;
        }
        p = p->next;
    }
*/
}

char * FileOpen (const char *FileName) {
    FILE *fp;

    if ((fp = fopen (FileName, "rb")) != NULL) {
        char *str;
        int size, i;

        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        str = (char *)malloc (size + 5);
        if(!str){
            fclose (fp);
            return NULL;
        }
        i = fread(str, 1, size, fp);
        fclose(fp);
        str[i] = 0;
        str[i+1] = 0;

        return str;
    }
    else printf ("File Not Found: '%s'\n", FileName);

    return NULL;
}
void CreateVar (char *name, char *svalue) {
    TVar *v = Gvar;
    int i = 0;
    while (v->name) {
        if (!strcmp(v->name, name))
      return;
        v++;
        i++;
    }
    if (i < GVAR_SIZE) {
        if (strchr(svalue, '.')) {
            v->type = TYPE_FLOAT;
            v->value.f = atof(svalue);
        } else {
            v->type = TYPE_INT;
            v->value.i = atoi(svalue);
        }
        v->name = strdup(name);
        v->info = NULL;
    }
}

void CreateVarInt (char *name, int value) {
    TVar *v = Gvar;
    int i = 0;
    while (v->name) {
        if (!strcmp(v->name, name))
      return;
        v++;
        i++;
    }
    if (i < GVAR_SIZE) {
        v->name = strdup(name);
        v->type = TYPE_INT;
        v->value.i = value;
        v->info = NULL;
    }
}
TFunc *FuncFind (char *name) {
    // array:
    TFunc *lib = stdlib;
    while (lib->name) {
        if ((lib->name[0]==name[0]) && !strcmp(lib->name, name))
      return lib;
        lib++;
    }
    // linked list:
    TFunc *func = Gfunc;
    while (func) {
        if ((func->name[0]==name[0]) && !strcmp(func->name, name))
      return func;
        func = func->next;
    }
    return NULL;
}
int ArgumentFind (char *name) {
    int i;
    for(i=0;i<argument_count;i++)
        if (!strcmp(argument[i].name, name)) return i;
    return -1;
}

TFunc *ModuleFind (char *LibName, char *FuncName) {
    MODULE *p = Gmodule;
    while (p) {
        if (!strcmp(p->name, LibName)) {
            TFunc *f = p->func;
            while (f) {
                if (!strcmp(f->name, FuncName)) {
              return f;
                }
                f = f->next;
            }
        }
        p = p->next;
    }
    return NULL;
}

int ModuleIsLib (char *LibName) {
    MODULE *p = Gmodule;
    while (p) {
        if (!strcmp(p->name, LibName))
      return 1;
        p = p->next;
    }
    return 0;
}

int VarFind (char *name) {
    TVar *v = Gvar;
    int i = 0;
    while(v->name) {
        if (!strcmp(v->name, name))
      return i;
        v++;
        i++;
    }
    return -1;
}

static void expression (LEXER *l, ASM *a) {
    if (l->tok==TOK_ID || l->tok==TOK_NUMBER) {
        char buf[100];
        TFunc *fi;
        int i;

        if (l->tok==TOK_NUMBER && strchr(l->token, '.'))
            main_variable_type = var_type = TYPE_FLOAT;
        else
            main_variable_type = var_type = TYPE_INT; // 0

        if (see(l)=='.') { // console.log();
            lex_save(l);
            sprintf (buf, "%s", l->token);
            lex(l); // .
            if (lex(l)==TOK_ID) { // ! FuncName:
                if ((fi = ModuleFind (buf, l->token)) != NULL) {
                    execute_call (l, a, fi);
              return;
                }
            }
            else lex_restore(l);
        }

        //
        // call a function without return:
        //   function_name (...);
        //
        if ((fi = FuncFind(l->token)) != NULL) {
            execute_call (l, a, fi);
      return;
        }

        if ((i = VarFind (l->token)) != -1) {
            int next = see(l);

            main_variable_type = var_type = Gvar[i].type;

            sprintf (write_var_name, "%s", Gvar[i].name);

            // increment var type int: a++;
            //
            if (next == TOK_PLUS_PLUS) {
                lex(l);
                #ifdef USE_VM
                emit_inc_var_int(a,(UCHAR)i);
                #endif
                #ifdef USE_JIT
                emit_incl (a,&Gvar[i].value.i);
                #endif
                return;
            }
            // increment var type int: a--;
            //
            if (next == TOK_MINUS_MINUS) {
                lex(l);
                #ifdef USE_VM
                emit_dec_var_int(a,(UCHAR)i);
                #endif
                #ifdef USE_JIT
                emit_decl (a,&Gvar[i].value.i);
                #endif
                return;
            }

            if (next=='=') {
                lex_save(l); // save the lexer position
                lex(l); // =
                if (lex(l)==TOK_ID) {

                    if (see(l)=='.' && ModuleIsLib(l->token)) {
                        lex_save(l); // save the lexer position
                        sprintf (buf, l->token);
                        lex(l); // .
                        if (lex(l)==TOK_ID) { // Module FuncName
                            if ((fi = ModuleFind(buf, l->token)) != NULL) {

                                execute_call (l, a, fi);

                                #ifdef USE_VM
                                // The function return is stored in variable VALUE( eax ) ... see in file: vm.c
                                emit_mov_eax_var(a,i);
                                #endif

                                #ifdef USE_JIT
                                //
                                // The function return is stored in register %eax ... types ! TYPE_FLOAT
                                //
                                //   HERE: copy the register ( %eax ) to the variable
                                //
                                if (main_variable_type != TYPE_FLOAT) {
                                    emit_mov_reg_var (a, EAX, &Gvar[i].value.i);
                                } else {

                                }
                                #endif //: #ifdef USE_JIT
                          return;
                            }
                        }
                    }
                    //
                    // call a function with return:
                    //   i = function_name (...);
                    //
                    if ((fi = FuncFind(l->token)) != NULL) {
                        execute_call (l, a, fi);

                        #ifdef USE_VM
                        // The function return is stored in variable VALUE( eax ) ... see in file: vm.c
                        emit_mov_eax_var(a,i);
                        #endif

                        #ifdef USE_JIT
                        //
                        // The function return is stored in register %eax ... types ! TYPE_FLOAT
                        //
                        //   HERE: copy the register ( %eax ) to the variable
                        //
                        if (main_variable_type != TYPE_FLOAT) {
                            emit_mov_reg_var (a, EAX, &Gvar[i].value.i);
                        } else {

                        }
                        #endif //: #ifdef USE_JIT
                  return;
                    }//: if ((fi = FuncFind(l->token)) != NULL)

                }
                lex_restore (l); // restore the lexer position

            }//: if (see(l)=='=')

        }//: if ((i = VarFind (l->token)) != -1)

        //---------------------------------------
        // Expression types:
        //   a * b + c * d;
        //   10 * a + 3 * b;
        //   i;
        //---------------------------------------
        //
        if (expr0(l,a) == -1) {
            #ifdef USE_VM
            emit_pop_eax (a); // %eax | eax.i := expression result
            emit_print_eax (a,main_variable_type);
            #endif

            #ifdef USE_JIT
            #if defined(__x86_64__)
            //
            // JIT 64 bits
            //
            if (main_variable_type == TYPE_INT) {

                // this "pop" the expression to register %eax and prepare to print the result.
                emit_expression_pop_64_int (a);

            } else if (main_variable_type == TYPE_FLOAT) {

                // this "pop" ( asm_float_fstps ) the expression and prepare to print the result.
                emit_expression_pop_64_float (a);

            }
            emit_call (a,printf,0,0); // display the result
            #else
            //
            // JIT 32 bits
            //
            if (main_variable_type == TYPE_INT) {
                emit_pop_eax (a); // %eax | eax.i := expression result
                emit_movl_ESP (a, (long)("%d\n"), 0); // pass argument 1
                emit_mov_eax_ESP (a, 4);              // pass argument 2
            } else if (main_variable_type == TYPE_FLOAT) {
                emit_movl_ESP (a, (long)("%f\n"), 0); // pass argument 1

                // send the result of (float expression) to: 4(%esp)
                //
                // dd 5c 24 04          	fstpl  0x4(%esp)
                g4(a,0xdd,0x5c,0x24,0x04); // pass argument 2
            }
            emit_call(a,printf,0,0); // display the result
            #endif
            #endif // #ifdef USE_JIT
        }// if (expr0(l,a) == -1)

    }
    else Erro ("%s: Expression ERRO(%d) - Ilegar Word (%s)\n", l->name, l->line, l->token);
}// expression ();


static void word_int (LEXER *l, ASM *a) {
    while (lex(l)) {
        if (l->tok == TOK_ID) {
            char name[255];
            int value = 0;

            strcpy (name, l->token); // save

            if (lex(l) == '=') {
                if (lex(l) == TOK_NUMBER)
                    value = atoi (l->token);
            }
            CreateVarInt (name, value);

        }
        if (l->tok == ';') break;
    }
    if (l->tok != ';') Erro ("ERRO: The word(float) need the char(;) on the end\n");

}//: word_int ()
static void word_var (LEXER *l, ASM *a) {
    while (lex(l)) {
        if (l->tok == TOK_ID) {
            char name[100];
            char svalue[100] = { '0', 0 };

            strcpy (name, l->token); // save

            if (lex(l) == '=') {
                if (lex(l) == TOK_NUMBER) {
                    sprintf (svalue, "%s", l->token);
                }
            }
            CreateVar (name, svalue);
        }
        if (l->tok == ';') break;
    }
    if (l->tok != ';') Erro ("ERRO: The word(float) need the char(;) on the end\n");

}//: word_int ()


static void word_if (LEXER *l, ASM *a) {
    //**** to "push/pop"
    static char array[20][20];
    static int if_count_total = 0;
    static int if_count = 0;
    int is_negative;

    if (lex(l) !='(') { Erro ("ERRO SINTAX (if) need char: '('\n"); return; }

    if_count++;
    sprintf (array[if_count], "IF%d", if_count_total++);

    while (!erro && lex(l)) { // pass arguments: if (a > b) { ... }
        is_negative = 0;

        if (l->tok == '!') { is_negative = 1; lex(l); }

        expr0(l,a);

        if (erro) {
            Erro ("<<<<<<<<<<  if erro >>>>>>>>>>>>>\n");
            return;
        }

        #ifdef USE_JIT
        if (l->tok == ')' || l->tok == TOK_AND_AND) emit_pop_eax (a); // 58     pop   %eax
        else                                        emit_pop_edx (a); // 5a     pop   %edx
        #endif

        switch (l->tok) {
        case ')': // if (a) { ... }
        case TOK_AND_AND:
            #ifdef USE_VM
            emit_push_int (a, 0); // ! to compare if zero
            emit_cmp_int (a);
            #endif
            #ifdef USE_JIT
            g2(a,0x85,0xc0); // 85 c0    test   %eax,%eax
            #endif
            if (is_negative == 0) emit_jump_je  (a,array[if_count]);
            else                  emit_jump_jne (a,array[if_count]);
            break;

        case '>':
            lex(l); expr0(l,a);
            #ifdef USE_VM
            emit_cmp_int (a);
            #endif
            #ifdef USE_JIT
            emit_pop_eax(a);
            emit_cmp_eax_edx(a);
            #endif
            emit_jump_jle (a,array[if_count]);
            break;
        case '<':
            lex(l); expr0(l,a);
            #ifdef USE_VM
            emit_cmp_int (a);
            #endif
            #ifdef USE_JIT
            emit_pop_eax(a);     // 58     : pop   %eax
            emit_cmp_eax_edx(a); // 39 c2  : cmp   %eax,%edx
            #endif
            emit_jump_jge (a,array[if_count]);
            break;

        case TOK_EQUAL_EQUAL: // ==
            lex(l); expr0(l,a);
            #ifdef USE_VM
            emit_cmp_int (a);
            #endif
            #ifdef USE_JIT
            emit_pop_eax(a);     // 58     : pop   %eax
            emit_cmp_eax_edx(a); // 39 c2  : cmp   %eax,%edx
            #endif
            emit_jump_jne(a,array[if_count]);
            break;

        case TOK_NOT_EQUAL: // !=
            lex(l); expr0(l,a);
            #ifdef USE_VM
            emit_cmp_int (a);
            #endif
            #ifdef USE_JIT
            emit_pop_eax(a);
            emit_cmp_eax_edx(a);
            #endif
            emit_jump_je (a,array[if_count]);
            break;
        }//: switch(tok)

        if (l->tok==')') break;
    }
    if (see(l)=='{') stmt (l,a); else Erro ("word(if) need start block: '{'\n");

    asm_label (a, array[if_count]);
    if_count--;

}// word_if ()

//
// for (;;) { ... }
// for (i = 0; i < 100; i++) { ... }
//
static void word_for (LEXER *l, ASM *a) {
    //####### to "push/pop"
    //
    static char array[20][20];
    static int for_count_total = 0;
    static int for_count = 0;

    if (lex(l) != '(') {
        Erro ("%s: %d: ERRO FOR, dont found char: '('", l->name, l->line);
        return;
    }
    lex (l);

    // for (;;) { ... }
    //
    if (l->tok == ';' && lex(l) == ';') {
        if (lex(l) != ')') {
            Erro ("ERRO FOR, dont found char: ')'");
            return;
        }
        if (see(l) != '{') { // ! check block '{'
            Erro ("ERRO FOR, dont found char: '{'");
            return;
        }

loop_level++;  // <<<<<<<<<<  ! PUSH  >>>>>>>>>>

        for_count++;
        for_count_total++;
        sprintf (array[for_count], "FOR_%d", for_count_total);
        sprintf (array_break[loop_level], "FOR_END%d", for_count_total); // used for break
        asm_label(a, array[for_count]);

        stmt (l,a); //<<<<<<<<<<  block  >>>>>>>>>>

        emit_jump_jmp (a, array[for_count]);

        asm_label (a, array_break[loop_level]); // used to break
        for_count--;

loop_level--;  // <<<<<<<<<<  ! POP  >>>>>>>>>>
    } else {
        int i; // var index
        int type = 0; // <  >  ==  !=
        int inc = 0; // ++, --
        int var_count = -1, number_count = 0;

        // for (i = 10; i < 100; i++) { ... }
        i = expr0 (l,a);
        if (i != -1) {
            lex(l);
            if (!strcmp(Gvar[i].name, l->token)) {
                // < >  ==  !=
                type = lex(l);
                lex(l); // get number or variable
                var_count = VarFind (l->token);
                if (var_count == -1) {
                    number_count = atoi (l->token);
                }
                while (lex(l)) {
                    if (l->tok == TOK_PLUS_PLUS)   inc = TOK_PLUS_PLUS;
                    if (l->tok == TOK_MINUS_MINUS) inc = TOK_MINUS_MINUS;
                    if (l->tok == ')') break;
                }
                if (l->tok == ')' && see(l)=='{') {

loop_level++;
                    for_count++;
                    for_count_total++;
                    sprintf (array[for_count], "FOR_%d", for_count_total);
                    sprintf (array_break[loop_level], "FOR_END%d", for_count_total); // used for break

//-------------------------------------------------------------------
//<<<<<<<<<<<<<<<<<<<<<<<  " TOP OF LOOP "  >>>>>>>>>>>>>>>>>>>>>>>>>
//-------------------------------------------------------------------

                    asm_label (a, array[for_count]);

                    if (var_count == -1) {
                        asm_mov_value_eax (a, number_count);
                    } else {
                        sprintf (write_var_name, "%s", Gvar[var_count].name);
                        emit_mov_var_reg (a, &Gvar[var_count].value.i, EAX);
                    }

                    sprintf (write_var_name, "%s", Gvar[i].name);
                    emit_cmp_eax_var (a, &Gvar[i].value.i);

                    //
                    // ! Jump to: " END OF LOOP "
                    //
                    if (type == '>') emit_jump_jle (a, array_break[loop_level]);
                    else
                    if (type == '<') emit_jump_jge (a, array_break[loop_level]);
                    else
                    {
                        printf ("Not found: %d\n", type);
                        Erro ("< > == !=");
                    }

#ifdef USE_ASM
write_asm(" //<<<< for block >>>>");
#endif

                    //---------------------------------------------------------------
                    // process the block starting from string char: '{'
                    //---------------------------------------------------------------
                    //
                    stmt (l,a);  //<<<<<<<<<<  block  >>>>>>>>>>

                    sprintf (write_var_name, "%s", Gvar[i].name);

                    if (inc == TOK_PLUS_PLUS)
                        emit_incl (a, &Gvar[i].value.i);
                    else if (inc == TOK_MINUS_MINUS)
                        emit_decl (a, &Gvar[i].value.i);

                    //
                    // Jump to: " TOP OF LOOP "
                    //
                    emit_jump_jmp (a, array[for_count]);

                    asm_label(a, array_break[loop_level]); // used to break
                    for_count--;

//-------------------------------------------------------------------
//<<<<<<<<<<<<<<<<<<<<<<<  " END OF LOOP "  >>>>>>>>>>>>>>>>>>>>>>>>>
//-------------------------------------------------------------------

loop_level--;
                }// if (l->tok == ')' && see(l)=='{')
                else
                Erro ("%s: %d: USAGE: for(i = 1; i < 100; i++) { ... }\n", l->name, l->line);
            }
            else Erro ("%s: %d: USAGE: for(i = 1; i < 100; i++) { ... }\n", l->name, l->line);

        }// if (i != -1)
        else Erro ("%s: %d: USAGE: for(i = 1; i < 100; i++) { ... }\n", l->name, l->line);

    }// } else {

}//: word_for ()

static void word_break (LEXER *l, ASM *a) {
    if (loop_level) {
        emit_jump_jmp (a, array_break[loop_level]);
    }
    else Erro ("%s: %d: word 'break' need a loop", l->name, l->line);
}
/*
//
// define TOK_ID    100
//
static void word_DEFINE (LEXER *l, ASM *a) {
    char name[100];
    lex(l);
    strcpy (name,l->token);
    if (lex(l)==TOK_NUMBER) {
        core_DefineAdd(name,atoi(l->token));
    } else Erro("%s: %d: word (define) suport only number\n", l->name, l->line);
}
*/

static void word_IFDEF (LEXER *l, ASM *a) {
    char text[100];
    int _line_;
    DEFINE *o = Gdefine;

    lex(l);
    strcpy (text, l->token);
    _line_ = l->line;

    while (o) { // ! if exist
        if (!strcmp(o->name, l->token)) return;
        o = o->next;
    }
    while (lex(l) && l->tok != TOK_ENDIF) { } // ! execute not defined

    if (l->tok != TOK_ENDIF) {
        Erro ("%s: %d: ERRO LINE %d - ENDIF(%s) not found\n", l->name, l->line, _line_, text);
//        asm_Erro (buf);
    }
}


//
// library ("SDL", "sdl");
//
static void word_module (LEXER *l, ASM *a) {
    char FileName [100] = { 0 };
    char LibName  [100] = { 0 };
    void *lib = NULL;
    int count = 0;

    while (lex(l)) {
        if (l->tok==TOK_STRING) {
            if (count==0) {
                #ifdef WIN32
                sprintf (FileName, "%s.dll", l->token);
                #endif
                #ifdef __linux__
                sprintf (FileName, "%s.so", l->token);
                #endif
            } else if (count==1) {
                sprintf (LibName, "%s", l->token);
            }
            count++;
        }
        if (l->tok==')' || l->tok==';') break;
    }

    //
    // is module exist then return
    //
    MODULE *mod, *p = Gmodule;
    while (p) { // ! if exist
        if (!strcmp(p->name, LibName))
      return;
        p = p->next;
    }

    #ifdef WIN32
    lib = (void *)LoadLibrary (FileName);
    #endif
    #ifdef __linux__
    lib = dlopen (FileName, RTLD_LAZY); // RTLD_NOW
    #endif
    if (lib && (mod = (MODULE*) malloc(sizeof(MODULE))) != NULL) {
        mod->name = strdup(LibName);
        mod->lib = lib;
        mod->func = NULL;
        mod->next = NULL;
        // add on top
        mod->next = Gmodule;
        Gmodule = mod;
        printf ("Module Loaded: '%s'\n", FileName);
    } else {
        //Erro("%s: %d: File Not Found: '%s'\n", l->name, l->line, FileName);
        printf ("%s: %d: File Not Found: '%s'\n", l->name, l->line, FileName);
    }
}

//
// import ("sdl", "SDL_init", "0i");
//
static void word_import (LEXER *l, ASM *a) {
    char LibName[100];
    char FuncName[100];
    char proto[100];
    MODULE *p = Gmodule;
    int count = 0;

    while (lex(l)) {
        if (l->tok==TOK_STRING) {
            if (count==0) {
                sprintf (LibName, "%s", l->token);
            } else if (count==1) {
                sprintf (FuncName, "%s", l->token);
            } else if (count==2) {
                sprintf(proto, "%s", l->token);
            }
            count++;
        }
        if (l->tok==')' || l->tok==';') break;
    }

    while (p) {
        if (!strcmp(p->name, LibName)) {
            #ifdef WIN32
            void *fp = (void*)GetProcAddress ((HMODULE)p->lib, FuncName);
            #endif
            #ifdef __linux__
            void *fp = dlsym (p->lib, FuncName);
            #endif
            if (fp) {
                TFunc *func;
                if ((func = (TFunc*) malloc (sizeof(TFunc))) != NULL) {
                    func->name = strdup (FuncName);
                    func->proto = strdup (proto);
                    func->type = FUNC_TYPE_NATIVE_C;
                    func->len = 0;
                    func->code = (UCHAR*)fp;

                    // add function on top
                    func->next = p->func;
                    p->func = func;
                }
            }
            else printf ("Function Not Found: '%s'\n", FuncName);//Erro("%s: %d: USAGE: import(%csdl%c, %SDL_Init%c, %c0i%c\n", '"', '"', '"');
      return;
        }
        p = p->next;
    }//: while (p)
    //else Erro("%s: %d: USAGE: import(%csdl%c, %SDL_Init%c, %c0i%c\n", '"', '"', '"');
}

static void word_function (LEXER *l, ASM *a) {
    TFunc *func;
    char name[255], proto[255] = { '0', 0, 0, 0, 0, 0, 0, 0 };
    int i;

    lex(l);

    strcpy (name, l->token);

    // if exist ... return
    //
    if (FuncFind(name)!=NULL) {
        int brace = 0;

        printf ("Function exist: ... REBOBINANDO '%s'\n", name);

        while (lex(l) && l->tok != ')');

        if (see(l)=='{') { } else Erro ("word(if) need start block: '{'\n");

        while (lex(l)){
            if (l->tok == '{') brace++;
            if (l->tok == '}') brace--;
            if (brace <= 0) break;
        }

  return;
    }

    // PASSA PARAMETROS ... IMPLEMENTADA APENAS ( int ) ... AGUARDE
    //
    // O analizador de expressao vai usar esses depois...
    //
    // VEJA EM ( expr3() ):
    // ---------------------
    // Funcoes usadas:
    //     ArgumentFind();
    //     asm_push_argument();
    // ---------------------
    //
    argument_count = 0;
    while (lex(l)) {

        if (l->tok==TOK_INT) {
            argument[argument_count].type[0] = TYPE_INT; // 0
            //strcpy (argument[argument_count].type, "int");
            if (lex(l)==TOK_ID) {
                strcpy (argument[argument_count].name, l->token);
                strcat (proto, "i");
                argument_count++;
            }
        }
/*
        else if (l->tok==TOK_FLOAT) {
            argument[argument_count].type[0] = TYPE_FLOAT; // 1
            //strcpy (argument[argument_count].type, "int");
            if (lex(l)==TOK_ID) {
                strcpy (argument[argument_count].name, l->token);
                strcat (proto, "f");
                argument_count++;
            }
        }
*/
        else if (l->tok==TOK_ID) {
            argument[argument_count].type[0] = TYPE_UNKNOW;
            strcpy (argument[argument_count].name, l->token);
            strcat (proto, "i");
            argument_count++;
        }

        if (l->tok=='{') break;
    }
    if (argument_count==0) {
        proto[1] = '0';
        proto[2] = 0;
    }
    if (l->tok=='{') l->pos--; else { Erro("Word Function need char: '{'"); return; }

    is_function = 1;
    local_count = 0;
    mov64_rdi_RBP = 0;
    strcpy (func_name, name);

#ifdef USE_ASM
write_asm(".globl %s\n%s:", name, name);
#endif

    // compiling to buffer ( f ):
    //
    asm_reset (asm_function);
    asm_begin (asm_function);
    //stmt (l,a); // here start from char: '{'
    stmt (l,asm_function); // here start from char: '{'
    asm_end (asm_function);

    if (erro) return;

    int len = asm_get_len (asm_function);

#ifdef USE_ASM
write_asm("// %s | len: %d", name, len);
#endif

#ifdef USE_JIT

    // new function:
    //
    func = (TFunc*) malloc (sizeof(TFunc));
    func->name = strdup (func_name);
    func->proto = strdup (proto);
    func->type = FUNC_TYPE_COMPILED;
    func->len = len;
    func->code = (UCHAR*) malloc (func->len);
    // NOW: copy the buffer ( f ):
    for (i=0;i<func->len;i++) {
        func->code[i] = asm_function->code[i];
    }

    //-------------------------------------------
    // HACKING ... ;)
    // Resolve Recursive:
    // change 4 bytes ( func_null ) to this
    //-------------------------------------------
    if (is_recursive)
    for (i=0;i<func->len;i++) {
        // b8     7a 13 40 00       mov    $0x40137a,%eax
        //
        // unsigned char: 184
        //
        if (func->code[i] == 0xb8 && *(void**)(func->code+i+1) == func_null) {

            i++; // ... HERE: 4 bytes ...

            // ff d0    call   *%eax
            //
            if (*(func->code+i+4) == 0xff && *(func->code+i+5) == 0xd0) {
                if (*(void**)(func->code+i) == func_null) {
                    *(void**)(func->code+i) = func->code; //<<< change here
                    i += 5;
                }
            }
        }
    }

    set_executable (func->code, len);

#endif
#ifdef USE_VM
    ASM *vm;
    if ((vm = asm_new (len + 5)) != NULL) {
        // new function:
        //
        func = (TFunc*) malloc (sizeof(TFunc));
        func->name = strdup (func_name);
        func->proto = strdup (proto);
        func->type = FUNC_TYPE_VM;
        func->len = len;
        // NOW: copy the buffer ( f ):
        for (i=0;i<func->len;i++) {
            vm->code[i] = asm_function->code[i];
        }
        vm->code[func->len  ] = 0;
        vm->code[func->len+1] = 0;
        vm->code[func->len+2] = 0;

        //-------------------------------------------
        // HACKING ... ;)
        // Resolve Recursive:
        // change 4 bytes ( func_null ) to this
        //-------------------------------------------
        if (is_recursive)
        for (i=0;i<func->len;i++) {
            if (vm->code[i]==OP_CALL && *(void**)(vm->code+i+1) == func_null) {
                vm->code[i] = OP_CALL_VM;     //<<<<<<<  change here  >>>>>>>
                *(void**)(vm->code+i+1) = vm; //<<<<<<<  change here  >>>>>>>
                i += 5;
            }
        }

        func->code = (UCHAR*)vm;

    } else {
        is_function = is_recursive = argument_count = *func_name = mov64_rdi_RBP = 0;
        return;
    }
#endif

    // add on top:
    func->next = Gfunc;
    Gfunc = func;

    is_function = is_recursive = argument_count = *func_name = mov64_rdi_RBP = 0;

}//:word_function ()

//
// function_name (a, b, c + d);
//
static void execute_call (LEXER *l, ASM *a, TFunc *func) {
    int count = 0;
#ifdef USE_JIT
    int pos = 0, size = 4, is_float = 0, is_string = 0; // Used in: JIT 32 bits
#endif
    int return_type = TYPE_INT;

    // no argument
    if (func->proto && func->proto[1] == '0') {
        while (lex(l))
            if (l->tok == ')' || l->tok == ';') break;
    } else {
        // get next: '('
        if (lex(l)!='(') { Erro ("Function need char: '('\n"); return; }

        while (lex(l)) {

            if (l->tok==TOK_ID || l->tok==TOK_NUMBER || l->tok==TOK_STRING || l->tok=='(') {
            
                if (l->tok==TOK_STRING)
                    is_string = 1;

                main_variable_type = var_type = TYPE_INT;

                //
                // The result of expression is store in the "stack".
                //
                expr0 (l,a);

                #ifdef USE_VM
                // ... none ...
                #endif

                #ifdef USE_JIT

                if (var_type == TYPE_INT) {
                    emit_pop_eax (a); // ... pop the "stack" and store in %eax ... := expression result TYPE_INT
                }

                #if defined(__x86_64__)
                //-----------------------------------
                // JIT 64 bits
                // %edi, %esi, %edx, %ecx, %r8 and %r9
                //
                if (var_type != TYPE_FLOAT) {
                    if (count==0) {         // argument 1
                        emit_mov_eax_edi (a);
                    } else if (count==1) {  // argument 2
                        emit_mov_eax_esi (a);
                    } else if (count==2) {  // argument 3
                        emit_mov_eax_edx (a);
                    } else if (count==3) {  // argument 4
                        emit_mov_eax_ecx (a);
                    } else if (count==4) {  // argument 5
                        emit_mov_eax_r8d (a);
                    } else if (count==5) {  // argument 6
                        emit_mov_eax_r9d (a);
                    }
                } else {
                    if (is_float==0) { // Function argument float

                        //  dd 1c 25    f8 09 60 00 	fstpl  0x6009f8
                        //
                        asm_float_fstps (a, &_Fvalue_); // store expression result | TYPE_FLOAT

                        emit_mov_var_reg(a, &_Fvalue_, EAX);

                        // 89 45 fc             	mov    %eax,-0x4(%rbp)
                        g3(a,0x89, 0x45,0xfc);
                        // f3 0f 10 45 fc       	movss  -0x4(%rbp),%xmm0
                        g4(a,0xf3, 0x0f, 0x10, 0x45);
                        g(a,(char)-4);

                        // 0f 5a c0             	cvtps2pd %xmm0,%xmm0
                        g3(a,0x0f, 0x5a, 0xc0);
                    }
                    is_float++;
                }
                #else
                //-----------------------------------
                // JIT 32 bits
                if (var_type != TYPE_FLOAT) {
                    emit_mov_eax_ESP (a, pos);
                    size = 4;
                    pos += size;
                } else { // JIT 32 bits: pass function argument float

                    // send the result of (float expression) to: 4(%esp)
                    //
                    // dd 5c 24   04    fstpl  0x4(%esp)
                    // d9 5c 24   04    fstps  0x4(%esp)
                    //
                    if (is_string)
                        g4 (a,0xdd,0x5c,0x24, (char)pos); // fstpl
                    else
                        g4 (a,0xd9,0x5c,0x24, (char)pos); // fstps
                    if (is_string) size = 8;
                    pos += size;
                }
                #endif
                #endif // ! USE_JIT

                if (count++ > 15) break;
            }
            if (l->tok == ')' || l->tok == ';') break;
        }
    }

    if (count > 6) {
        Erro ("%s:%d: - Call Function(%s) the max arguments is: 5\n", l->name, l->line, func->name);
  return;
    }

    sprintf (write_func_name, "%s", func->name);

    if (func->proto) {
        if (func->proto[0] == '0') return_type = TYPE_NO_RETURN;
        if (func->proto[0] == 'f') return_type = TYPE_FLOAT;
    }
    if (func->type == FUNC_TYPE_VM) {
        //
        // THE_SUMMER_BASE_CODE: <<<<<<<<<<  Not implemented  >>>>>>>>>>
        //
        // here: fi->code ==  ASM*
        //
        #ifdef USE_VM
        emit_call_vm (a, (ASM*)(func->code), (UCHAR)count, return_type);
        #endif
    } else {
        emit_call (a, (void*)func->code, (UCHAR)count, return_type);
    }
}

static int expr0 (LEXER *l, ASM *a) {

    if (l->tok == TOK_ID) {
        int i;
        //---------------------------------------
        //
        // Expression type:
        //
        //   i = a * b + c;
        //
        //---------------------------------------
        if (see(l)=='=') {
            if ((i=VarFind(l->token)) != -1) {
                lex_save (l); // save the lexer position
                if (lex(l) == '=') {
                    lex(l);
                    expr1(l,a);
                    sprintf (write_var_name, "%s", Gvar[i].name);
                    #ifdef USE_VM
                    // Copia o TOPO DA PILHA ( sp ) para a variavel ... e decrementa sp++.
                    emit_pop_var (a,i);
                    #endif
                    #ifdef USE_JIT
                    if(main_variable_type==TYPE_FLOAT) {
                        asm_float_fstps (a, &Gvar[i].value.f); // Gvar[1]
                    } else {
                        // Copia o TOPO DA PILHA ( %ESP ) para a variavel ... e estabiliza a PILHA
                        emit_pop_var (a, &Gvar[i].value.i);
                    }
                    #endif
              return i;
                } else {
                    lex_restore (l); // restore the lexer position
                }
            }//: if ((i=VarFind(l->token)) != -1)
        }//: if (see(l)=='=')
    }
    expr1(l,a);
    return -1;
}
static void expr1 (LEXER *l, ASM *a) { // '+' '-' : ADDITION | SUBTRACTION
    int op;
    expr2(l,a);
    while ((op=l->tok) == '+' || op == '-') {
        lex(l);
        expr2(l,a);
        if (var_type==TYPE_FLOAT) {
            if (op=='+') emit_add_float (a);
        } else { // INT
            if (op=='+') emit_add_int (a);
            if (op=='-') emit_sub_int (a);
        }

    }
}
static void expr2 (LEXER *l, ASM *a) { // '*' '/' : MULTIPLICATION | DIVISION
    int op;
    expr3(l,a);
    while ((op=l->tok) == '*' || op == '/') {
        lex(l);
        expr3(l,a);
        if (var_type==TYPE_FLOAT) {
            if (op=='*') emit_mul_float (a);
        } else { // INT
            if (op=='*') emit_mul_int (a);
            if (op=='/') emit_div_int (a);
        }
    }
}
static void expr3 (LEXER *l, ASM *a) { // '('
    if (l->tok=='(') {
        lex(l); expr0(l,a);
        if (l->tok != ')') {
            Erro ("ERRO )\n");
        }
        lex(l);
    }
    else atom(l,a); // atom:
}
static void atom (LEXER *l, ASM *a) { // expres

    if (l->tok==TOK_STRING) {
        F_STRING *s = fs_new (l->token);
        if (s) {
            #ifdef USE_VM
            emit_push_string (a, s->s);
            #endif
            #ifdef USE_JIT
            // this is a constans :
            // 68    00 20 40 00       	push   $0x402000
            //
            g(a, 0x68); asm_get_addr(a,s->s);
            #endif
        }
  return;
    }
    if (l->tok==TOK_ID) {
        int i;
#ifdef USE_JIT
        TFunc *fi;
        //
        // push the pointer of function:
        //
        // NO CALL THE FUNCTION
        //
        if ((fi = FuncFind (l->token)) != NULL) {
            if (see(l) == '(') {
                // "execute the function"
                execute_call (l,a,fi);
                // and ... push the result
                emit_push_eax(a);
                lex (l);
            } else if (see(l) != '(') {
                //
                // push the real function pointer
                //
                emit_mov_var_reg (a, &fi->code, EAX);
                emit_push_eax(a);
                lex (l);
            } 
        }
        else
#endif
        // push a argument function:
        //
        if (is_function && (i=ArgumentFind(l->token))!=-1) {
            // function argument:
            //
            // e.offsetX, e.offsetY, e.which;
            //
            if (argument[i].type[0]==TYPE_UNKNOW && see(l)=='.') {
                if (lex(l) && lex(l)) { // .offsetX;

                    #if defined(__x86_64__)
                    if (mov64_rdi_RBP==0) {
                        mov64_rdi_RBP = 1;
                        // 48 89 7d f8          	mov    %rdi,-0x8(%rbp)
                        g4(a,0x48,0x89,0x7d,0xf8);
                    }
//                    printf ("push argument (%s.%s)\n", argument[i].name, l->token);
                    #endif

                    if (!strcmp(l->token, "offsetX")) {
                        #if defined(__x86_64__)
// 48 8b 45 f8          	mov    -0x8(%rbp),%rax
// 8b 40 08             	mov    8(%rax),%eax
                        g4(a,0x48,0x8b,0x45,0xf8);
                        g3(a,0x8b,0x40,8);
                        emit_push_eax(a);
                        #else
                        // 8b 45 08             	mov    8(%ebp),%eax
                        // 8b 40 04             	mov    4(%eax),%eax
                        g3(a,0x8b,0x45,8);
                        g3(a,0x8b,0x40,4);
                        emit_push_eax(a);
                        #endif

                        lex(l);
                    }
                    else
                    if (!strcmp(l->token, "offsetY")) {
                        #if defined(__x86_64__)
// 48 8b 45 f8          	mov    -0x8(%rbp),%rax
// 8b 40 0c             	mov    12(%rax),%eax
                        g4(a,0x48,0x8b,0x45,0xf8);
                        g3(a,0x8b,0x40,12);
                        emit_push_eax(a);
                        #else
                        // 8b 45 08             	mov    8(%ebp),%eax
                        // 8b 40 04             	mov    8(%eax),%eax
                        g3(a,0x8b,0x45,8);
                        g3(a,0x8b,0x40,8);
                        emit_push_eax(a);
                        #endif

                        lex(l);
                    }
                    else
                    if (!strcmp(l->token, "which")) {
                        #if defined(__x86_64__)
// 48 8b 45 f8          	mov    -0x8(%rbp),%rax
// 8b 40 0c             	mov    16(%rax),%eax
                        g4(a,0x48,0x8b,0x45,0xf8);
                        g3(a,0x8b,0x40,16);
                        emit_push_eax(a);
                        #else
                        // 8b 45 08             	mov    8(%ebp),%eax
                        // 8b 40 04             	mov    12(%eax),%eax
                        g3(a,0x8b,0x45,8);
                        g3(a,0x8b,0x40,12);
                        emit_push_eax(a);
                        #endif

                        lex(l);
                    }
                    else Erro("%s %d | ERRO(%s.%s) - Only implemented( .offsetX, .offsetY )", l->name, l->line, argument[i].name, l->token);
                }
            }
            else {
//            emit_push_arg (a, i);
                lex(l);
            }
        }
        else if ((i=VarFind(l->token))!=-1) {
            var_type = Gvar[i].type;

            #ifdef USE_JIT
            if (main_variable_type == TYPE_FLOAT && var_type != TYPE_FLOAT) {
                // db 05    70 40 40 00    	fildl  0x404070
                //g2(a,0xdb,0x05); asm_get_addr(a, &Gvar[i].value.i);
                Erro ("%s: %d: Float and Integer ... Not Permited: '%s' ;)\n", l->name, l->line, Gvar[i].name);
            } else {
                if (var_type == TYPE_FLOAT) {
                    asm_float_flds (a, &Gvar[i].value.f);
                } else {
                    emit_push_var (a, &Gvar[i].value.i);
                }
            }
            #endif
            #ifdef USE_VM
            emit_push_var (a,i);
            #endif

            lex(l);

        }
        else Erro ("%s: %d: - Var Not Found '%s'", l->name, l->line, l->token);
    }
    else if (l->tok==TOK_NUMBER) {
        if (strchr(l->token, '.'))
            var_type = TYPE_FLOAT;

        if (var_type==TYPE_FLOAT) {
            emit_push_float (a, atof(l->token));
//printf ("NUMBER FLOAT(%f)\n", atof(l->token));
        } else {
            emit_push_int (a, atoi(l->token));
        }

        lex(l);
    }
    else Erro("%s: %d Expression atom - Ilegal Word (%s)\n", l->line, l->token);

}//: atom ()


void do_block (LEXER *l, ASM *a) {
    while (!erro && l->tok && l->tok != '}') {
        stmt (l,a);
    }
    l->tok = ';';
}

static int stmt (LEXER *l, ASM *a) {

    lex (l);

    switch (l->tok) {
    case '{':
        l->level++;
        //----------------------------------------------------
        do_block (l,a); //<<<<<<<<<<  no recursive  >>>>>>>>>>
        //----------------------------------------------------
        return 1;
    case TOK_INT:       word_int      (l,a); return 1;
    case TOK_VAR:       word_var      (l,a); return 1;
    case TOK_IF:        word_if       (l,a); return 1;
    case TOK_FOR:       word_for      (l,a); return 1;
    case TOK_BREAK:     word_break    (l,a); return 1;
    case TOK_MODULE:    word_module   (l,a); return 1;
    case TOK_IMPORT:    word_import   (l,a); return 1;
    case TOK_FUNCTION:  word_function (l,a); return 1;
    //
    case TOK_IFDEF:     word_IFDEF    (l,a); return 1;
    //
    default:            expression    (l,a); return 1;
    case '}': l->level--; return 1;
    case ';':
    case '#':
    case TOK_ENDIF:
        return 1;
    case 0: return 0;
    }
    return 1;
}
static int see (LEXER *l) {
    char *s = l->text+l->pos;
    while (*s) {
        if (*s=='\n' || *s==' ' || *s==9 || *s==13) {
            s++;
        } else {
            if (s[0]=='=' && s[1]=='=') return TOK_EQUAL_EQUAL;
            if (s[0]=='+' && s[1]=='+') return TOK_PLUS_PLUS;
            if (s[0]=='-' && s[1]=='-') return TOK_MINUS_MINUS;
            return *s;
        }
    }
    return 0;
}
F_STRING *fs_new (char *s) {
    static int count = 0;
    F_STRING *p = fs, *n;

    while (p) {
        if (!strcmp(p->s,s)) return p;
        p = p->next;
    }

    if ((n = (F_STRING*)malloc(sizeof(F_STRING)))==NULL) return NULL;
    n->s = strdup(s);

//printf ("FIXED: %p\n", &n->s);

    n->i = count++;
    // add on top
    n->next = fs;
    fs = n;

    return n;
}

void core_ModuleAdd (char *module_name, char *func_name, char *proto, UCHAR *code) {
    MODULE *mod, *p = Gmodule;
    TFunc *func;

    while (p) {
        if (!strcmp(module_name, p->name)) {
            if ((func = (TFunc*) malloc (sizeof(TFunc))) != NULL) {
                func->name = strdup (func_name);
                func->proto = strdup (proto);
                func->type = FUNC_TYPE_NATIVE_C;
                func->len = 0;
                func->code = code;

                // add on top;
                func->next = p->func;
                p->func = func;
          return;
            }
        }
        p = p->next;
    }
    //
    // create: MODULE and FUNCTION.
    //
    if ((mod = (MODULE*) malloc(sizeof(MODULE))) != NULL) {
        if ((func = (TFunc*) malloc (sizeof(TFunc))) != NULL) {
            func->name = strdup (func_name);
            func->proto = strdup (proto);
            func->type = FUNC_TYPE_NATIVE_C;
            func->len = 0;
            func->code = code;

            mod->name = strdup(module_name);

            mod->func = NULL;

            // add function on top
            func->next = mod->func;
            mod->func = func;

            // add module on top
            mod->next = Gmodule;
            Gmodule = mod;
        }
    }
}
void core_DefineAdd (char *name, int value) {
    DEFINE *o = Gdefine, *n;
    while (o) {
        if (!strcmp(o->name, name)) return;
        o = o->next;
    }
    if ((n = (DEFINE*)malloc(sizeof(DEFINE)))==NULL) return;
    n->name = strdup(name);
    n->value = value;
    // add on top
    n->next = Gdefine;
    Gdefine = n;
}


int Parse (LEXER *l, ASM *a, char *text, char *name) {
    lex_set (l, text, name);
    ErroReset ();
    asm_reset (a);
    asm_begin (a);
    while (!erro && stmt(l,a)) {
        // ... compiling ...
    }
    asm_end (a);
    if (l->level) { // { ... }
        Erro ("\nERRO: LEXER->level { ... }: %d\n", l->level);
    }
    return erro;
}

void lib_log (char *s) {
    printf ("%s\n", s);
}

void lib_info (int arg) {
    switch (arg) {
    case 1: {
        TVar *v = Gvar;
        int i = 0;
        printf ("VARIABLES:\n---------------\n");
        while (v->name) {
            if (v->type==TYPE_INT)   printf ("Gvar[%d](%s) = %d\n", i, v->name, v->value.i);
            else
            if (v->type==TYPE_FLOAT) printf ("Gvar[%d](%s) = %f\n", i, v->name, v->value.f);
            v++; i++;
        }
        } break;

    case 2: {
        TFunc *fi = stdlib;
        printf ("FUNCTIONS:\n---------------\n");
        while (fi->name) {
            if(fi->proto){
                char *s=fi->proto;
                if (*s=='0') printf ("void  ");
                else
                if (*s=='i') printf ("int   ");
                else
                if (*s=='f') printf ("float ");
                else
                if (*s=='s') printf ("char  *");
                else
                if (*s=='p') printf ("void * ");
                printf ("%s (", fi->name);
                s++;
                while(*s){
                    if (*s=='i') printf ("int");
                    else
                    if (*s=='f') printf ("float");
                    else
                    if (*s=='s') printf ("char *");
                    s++;
                    if(*s) printf (", ");
                }
                printf (");\n");
            }
            fi++;
        }
        fi = Gfunc;
        while(fi){
            if(fi->proto) printf ("%s ", fi->proto);
            printf ("%s\n", fi->name);
            fi = fi->next;
        }

        }
        break;

    default:
        printf ("USAGE(%d): info(1);\n\nInfo Options:\n 1: Variables\n 2: Functions\n 3: Defines\n 4: Words\n",arg);
    }
}
int lib_addi (int a, int b) {
    return a + b;
}
float lib_addf (float a, float b) {
printf ("a: %f   b: %f\n", a, b);
    return a + b;
}

int lib_arg (int a, int b, int c, int d, int e) {
  printf ("arg1: %d\n", a);
  printf ("arg2: %d\n", b);
  printf ("arg3: %d\n", c);
  printf ("arg4: %d\n", d);
  printf ("arg5: %d\n", e);
  return a + b + c + d + e;
}
void lib_printf (char *format, ...) {
    char msg[1024] = { 0 };
    register unsigned int i;
    va_list ap;
    int new_line = 0;

    va_start (ap,format);
    vsprintf (msg, format, ap);
    va_end (ap);

    for (i = 0; i < strlen(msg); i++) {
        if (msg[i] == '\\' && msg[i+1] == 'n') { // new line
            putc (10, stdout); // new line
            new_line = 1;
            i++;
        }
        else if (msg[i] == '\\' && msg[i+1] == 't') { // tab
            putc ('\t', stdout); // tab
            i++;
        } else {
            putc (msg[i], stdout);
        }
    }
    if (new_line==0)
        printf ("\n");
}

TFunc *lib_get_func (char *name) {
    TFunc *f;
    f = FuncFind (name);
    if (f && f->type == FUNC_TYPE_COMPILED) {
printf ("f existe\n");
        return f;
    }
    return NULL;
}


void write_asm (char *format, ...) {
    if (is_function && asm_mode) {
        char msg[1024] = { 0 };
        va_list ap;
        va_start (ap,format);
        vsprintf (msg, format, ap);
        va_end (ap);
        printf ("%s\n", msg);
    }
}

int fps, old, t;
void time_fps (void) {
    fps++;
    t = time (NULL);
    if (old != t) {
        old = t;
        printf ("FPS: %d\n", fps); fps = 0;
    }
}


void Erro (char *format, ...) {
    char msg[1024] = { 0 };
    va_list ap;

    va_start (ap,format);
    vsprintf (msg, format, ap);
    va_end (ap);

    erro++;
    if ((strlen(strErro) + strlen(msg)) < STR_ERRO_SIZE)
        strcat (strErro, msg);
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

