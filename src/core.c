//-------------------------------------------------------------------
//
// SUMMER LANGUAGE:
//
// Main Core:
//
// FILE:
//   core.c
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite 01: 20/07/2018 - 11:10
//   rewrite 02: 23/03/2019 - 08:50
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
// File Size: 49.146
#include "summer.h"

//-----------------------------------------------
//-----------------  PROTOTYPES  ----------------
//-----------------------------------------------
//
static void word_int (LEXER *l, ASM *a);
static void word_var (LEXER *l, ASM *a); // int, float
static void word_asm (LEXER *l, ASM *a);
static void word_if (LEXER *l, ASM *a);
static void word_for (LEXER *l, ASM *a);
static void word_break (LEXER *l, ASM *a);
static void word_function (LEXER *l, ASM *a);
static void word_include (LEXER *l, ASM *a);
static void word_import (LEXER *l, ASM *a);
static void word_module (LEXER *l, ASM *a);
static void word_DEFINE (LEXER *l, ASM *a);
static void word_IFDEF (LEXER *l, ASM *a);

static int expr0 (LEXER *l, ASM *a);
static void expr1 (LEXER *l, ASM *a);
static void expr2 (LEXER *l, ASM *a);
static void expr3 (LEXER *l, ASM *a);
static void atom (LEXER *l, ASM *a);
static int stmt (LEXER *l, ASM *a);
static int see (LEXER *l);
static int ModuleIsLib (char *LibName);
static TFunc *ModuleFind (char *LibName, char *FuncName);

TFstring *fs_new (char *s);

static void execute_call (LEXER *l, ASM *a, TFunc *func);
static void DefineAdd (char *name, int value);

void lib_info (int arg);
int lib_printf (char *format, ...);
void lib_help (int i);
void lib_disasm (char *name);
int lib_func_add (int a, int b);
float lib_func_add_float (float a, float b);
void lib_print_int (int i);
void lib_print_float (float f);
void lib_func_float_int (float f, int i);
int arg4(int a, int b, int c, int d);
int arg5(int a, int b, int c, int d, int e);

char *lib_float2s (float f);

void CallBack (void (*call) (void));

#ifdef USE_SG
  #ifndef USE_SDL
void gl_Begin (int i) { glBegin (i); }
void gl_End (void) { glEnd (); }
void gl_PushMatrix (void) { glPushMatrix(); }
void gl_PopMatrix (void) { glPopMatrix(); }
void gl_Rotatef (float angle, float x, float y, float z) { glRotatef (angle,x,y,z); }
void gl_Translatef (float x, float y, float z) { glTranslatef (x,y,z); }
void gl_Color3ub (unsigned char r, unsigned char g, unsigned char b) { glColor3ub (r,g,b); }
void gl_Vertex2f (float x, float y) { glVertex2f (x,y); }
void gl_Vertex3f (float x, float y, float z) { glVertex3f (x,y,z); }
  #endif
void sgInitEvent (void) {
    TFunc *fi;

    if ((fi = FuncFind ("MouseUp")) != NULL) {
        sgSetEvent( (void(*)(TEvent*))fi->code, "onmouseup");
    }
    if ((fi = FuncFind ("MouseDown")) != NULL) {
        sgSetEvent( (void(*)(TEvent*))fi->code, "onmousedown");
    }
    if ((fi = FuncFind ("MouseMove")) != NULL) {
        sgSetEvent( (void(*)(TEvent*))fi->code, "onmousemove");
    }
//    sgSetEvent (onMouseMove, "onmousemove");
//    sgSetEvent (onMouseDown, "onmousedown");
//    sgSetEvent (onMouseUp,   "onmouseup");
}

#endif

static TFunc stdlib[]={
  //-----------------------------------------------------------------------------
  // char*        char*     UCHAR*                    int   int   int       FUNC*
  // name         proto     code                      type  len   sub_esp   next
  //-----------------------------------------------------------------------------
  { "info",       "0i",     (UCHAR*)lib_info,         0,    0,    0,        NULL },
  { "CallBack",   "0p",     (UCHAR*)CallBack,         0,    0,    0,        NULL },
  { "disasm",     "0s",     (UCHAR*)lib_disasm,       0,    0,    0,        NULL },
  { "float2s",    "pi",     (UCHAR*)lib_float2s,      0,    0,    0,        NULL },
  //
  { "printf",     "is.",    (UCHAR*)lib_printf,       0,    0,    0,        NULL },
  { "strcat",     "ss",     (UCHAR*)strcat,           0,    0,    0,        NULL },
  { "sprintf",    "iss.",   (UCHAR*)sprintf,          0,    0,    0,        NULL },
  { "malloc",     "pi",     (UCHAR*)malloc,           0,    0,    0,        NULL },
  //
  { "func_add",   "iii",    (UCHAR*)lib_func_add,     0,    0,    0,        NULL },
  { "addf",       "fff",    (UCHAR*)lib_func_add_float,   0,    0,    0,        NULL },
  { "printi",			"0i",		  (UCHAR*)lib_print_int,   0,    0,  	0,  			NULL },
  { "print_float","0f",     (UCHAR*)lib_print_float,0,    0,    0,        NULL },

  { "float_int",  "0fi",    (UCHAR*)lib_func_float_int,0,    0,    0,        NULL },

  { "arg4",			  "iiiii",  (UCHAR*)arg4,    	0,    0,  	0,  			NULL },
  { "arg5",			  "iiiiii", (UCHAR*)arg5,    	0,    0,  	0,  			NULL },
  // SDL:
#ifdef USE_APPLICATION
  { "app_Init",       "iis",    (UCHAR*)app_Init,    	    0,    0,  	0,  			NULL },
  { "app_Run",        "0p",     (UCHAR*)app_Run,    	    0,    0,  	0,  			NULL },
  { "app_NewButton",  "ppiiis", (UCHAR*)app_NewButton,    0,    0,  	0,  			NULL },
  { "app_NewEditor",  "ppiisi", (UCHAR*)app_NewEditor,    0,    0,  	0,  			NULL },
  { "app_SetSize",    "0pii",   (UCHAR*)app_SetSize,      0,    0,  	0,  			NULL },
  { "app_SetCall",    "0pp",    (UCHAR*)app_SetCall,      0,    0,  	0,  			NULL },
  { "app_SetEvent",   "0pps",   (UCHAR*)app_SetEvent,      0,    0,  	0,  			NULL },
  { "LOG",            "0*",     (UCHAR*)LOG,              0,    0,  	0,  			NULL },
#endif
#ifdef USE_SG
  { "sgInit",       "i0",     (UCHAR*)sgInit,    	    0,    0,  	0,  			NULL },
  { "sgRun",        "00",     (UCHAR*)sgRun,    	    0,    0,  	0,  			NULL },
  { "sgInitEvent",  "00",     (UCHAR*)sgInitEvent,    0,    0,  	0,  			NULL },
  { "sgSetEvent",   "0ps",    (UCHAR*)sgSetEvent,	    0,    0,  	0,  			NULL },
  { "sgDrawText",   "0siii",  (UCHAR*)sgDrawText,	    0,    0,  	0,  			NULL },
  { "sgDrawFloat",  "0f",     (UCHAR*)sgDrawFloat,    0,    0,  	0,  			NULL },
  { "sgBeginScene", "00",     (UCHAR*)sgBeginScene,   0,    0,    0,        NULL },
  { "sgEndScene",   "00",     (UCHAR*)sgEndScene,     0,    0,    0,        NULL },
  { "sgClear",      "00",     (UCHAR*)sgClear,	      0,    0,  	0,  			NULL },
  { "sgSet2D",      "00",     (UCHAR*)sgSet2D,	      0,    0,  	0,  			NULL },
  { "sgSet3D",      "00",     (UCHAR*)sgSet3D,	      0,    0,  	0,  			NULL },
  #ifndef USE_SDL
  { "draw_cube",    "0i",     (UCHAR*)draw_cube,      0,    0,  	0,  			NULL },
  { "draw_piso",    "00",     (UCHAR*)draw_piso,      0,    0,  	0,  			NULL },

  { "glBegin",      "0i",(UCHAR*)gl_Begin,0,0,0, NULL },
  { "glEnd",        "00",(UCHAR*)gl_End,0,0,0,NULL },
  { "glPushMatrix", "00",(UCHAR*)gl_PushMatrix,0,0,0,NULL},
  { "glPopMatrix",  "00",(UCHAR*)gl_PopMatrix,0,0,0,NULL},
  { "glRotatef",    "0ffff",(UCHAR*)gl_Rotatef,0,0,0,NULL},
  { "glTranslatef", "0fff",(UCHAR*)gl_Translatef,0,0,0,NULL},

  { "glColor3ub",   "0iii",(UCHAR*)gl_Color3ub,0,0,0,NULL},
  { "glVertex2f",   "0ff",(UCHAR*)gl_Vertex2f,0,0,0,NULL},
  { "glVertex3f",   "0fff",(UCHAR*)gl_Vertex3f,0,0,0,NULL},
  #endif // ! USE_SDL
#endif // USE_SG
  { NULL, NULL, NULL, 0, 0, 0, NULL }
};

//-----------------------------------------------
//------------------  VARIABLES  ----------------
//-----------------------------------------------
//
TVar Gvar [GVAR_SIZE]; // global:
ASM *asm_function;
static ASM *asm_include1, *asm_include2;
static TFunc *Gfunc = NULL;
static TModule *Gmodule = NULL;
static TFstring *fs = NULL;
static TDefine *Gdefine = NULL;
static TArg argument [20];
static char
  func_name [100],
  array_break [20][20]   // used to word break
  ;
static int
  is_function,
  is_recursive,
  loop_level,
  main_variable_type,
  var_type,
  argument_count,
  local_count
  ;
float _Fvalue_;

static int expr0 (LEXER *l, ASM *a) {
  if (l->tok == TOK_ID) {
    //---------------------------------------
    //
    // Expression type:
    //
    //   i = a * b + c;
    //
    //---------------------------------------
    if (see(l)=='=') {
      int i;
      if ((i = VarFind(l->token)) != -1) {
        lex_save (l); // save the lexer position
        if (lex(l)=='=') {
          lex(l);
          expr1(l,a);
          if (Gvar[i].type==TYPE_FLOAT) {
            emit_float_fstps (a, &Gvar[i].value.f);
          } else {
            emit_mov_reg_var (a, EAX, &Gvar[i].value.l);
          }
          return i;
        }
        else lex_restore (l); // restore the lexer position
      }// if ((i = VarFind(l->token)) != -1)
    }// if (see(l)=='=')
  }
  expr1(l, a);
  return -1;
}
static void expr1 (LEXER *l, ASM *a) { // '+' '-' : ADDITION | SUBTRACTION
  int op;
  expr2(l, a);
  while ((op=l->tok) == '+' || op == '-') {
    lex(l);
    expr2(l,a);
    if (main_variable_type==TYPE_FLOAT) {
      if (op=='+') emit_add_float(a);
      if (op=='-') emit_sub_float(a);
    } else { // TYPE_LONG
      if (op=='+') emit_expression_add_long(a);
      if (op=='-') emit_expression_sub_long(a);
    }
  }
}

static void expr2 (LEXER *l, ASM *a) { // '*' '/' : MULTIPLICATION | DIVISION
  int op;
  expr3(l,a);
  while ((op=l->tok) == '*' || op == '/') {
    lex(l);
    expr3(l, a);
    if (main_variable_type==TYPE_FLOAT) {
      if (op=='*') emit_mul_float(a);
      if (op=='/') emit_div_float(a);
    } else { // TYPE_LONG
      if (op=='*') emit_expression_mul_long(a);
      if (op=='/') emit_expression_div_long(a);
    }
  }
}

static void expr3 (LEXER *l, ASM *a) { // '('
  if (l->tok=='(') {
    lex(l);
    expr0(l,a);
    if (l->tok != ')') {
      Erro("ERRO )\n");
    }
    lex(l);
  }
  else atom(l,a);
}

static void atom (LEXER *l, ASM *a) { // expres
    if (l->tok == TOK_STRING) {
        TFstring *s = fs_new (l->token);
        if (s)
            emit_expression_push_long (a, (long)(s->s));
        lex(l);
        return;
    }

    if (l->tok == TOK_ID) {
        int i;
        TFunc *fi;
        TDefine *def = Gdefine;

        // process MACROS: ... ONLY NUMBERS
        while (def) {
            if (!strcmp(def->name, l->token)) {
                emit_expression_push_long (a, def->value);
                lex(l);
                return;
            }
            def = def->next;
        }

        //
        // push the pointer of function:
        //
        // NO CALL THE FUNCTION
        //
        if ((fi = FuncFind (l->token)) != NULL) {
            if (see(l)=='(') {
                // "execute the function"
                execute_call (l,a,fi);
                // and ... push the result in %eax
                lex (l);
            } else if (see(l) != '(') {
                //
                // push the real function pointer
                //
                emit_mov_var_reg (a, &fi->code, EAX);
                lex (l);
            } 
        }
        else
        // push a argument function:
        //
        if (is_function && (i=ArgumentFind(l->token))!=-1) {
            // function argument:
            //
            // e.offsetX, e.offsetY, e.which;
            //
            if (argument[i].type[0]==TYPE_UNKNOW) {

                if (see(l)=='.' && lex(l) && lex(l)) { // .offsetX;
                    // ... NEED IMPLEMENTATION

                    if (!strcmp(l->token, "offsetX")) { // 8
                    #if defined(__x86_64__)
                        #ifdef WIN32
                        //48 8b 45 10          	mov    16(%rbp),%rax
                        g4(a,0x48,0x8b,0x45,0x10);
                        #endif
                        #ifdef __linux__
                        // 48 8b 45 f8          	mov    -0x8(%rbp),%rax
                        g4(a,0x48,0x8b,0x45,0xf8);
                        #endif
                        //8b 40 08             	mov    8(%rax),%eax
                        g3(a,0x8b,0x40,8);
                    #else
                        // 32 bits
                        // 8b 45 08             	mov    0x8(%ebp),%eax
                        // 8b 40    04             	mov    0x4(%eax),%eax
                        g3(a,0x8b,0x45,0x08);
                        g3(a,0x8b,0x40,4);
                    #endif
                        lex(l);
                    }
                    else
                    if (!strcmp(l->token, "offsetY")) { // 12
                    #if defined(__x86_64__)
                        #ifdef WIN32
                        //48 8b 45 10          	mov    16(%rbp),%rax
                        g4(a,0x48,0x8b,0x45,0x10);
                        #endif
                        #ifdef __linux__
                        // 48 8b 45 f8          	mov    -0x8(%rbp),%rax
                        g4(a,0x48,0x8b,0x45,0xf8);
                        #endif
                        //8b 40 12             	mov    12(%rax),%eax
                        g3(a,0x8b,0x40,12);
                    #else
                        // 32 bits
                        // 8b 45 08             	mov    0x8(%ebp),%eax
                        // 8b 40    08             	mov    0x8(%eax),%eax
                        g3(a,0x8b,0x45,0x08);
                        g3(a,0x8b,0x40,8);
                    #endif
                        lex(l);
                    }
                    else Erro("%s %d | ERRO(%s.%s) - Only implemented( .offsetX, .offsetY )", l->name, l->line, argument[i].name, l->token);

                } else {
                    if (i == 0 && argument_count == 1 && argument[0].type[0] == TYPE_UNKNOW) {
                    #if defined(__x86_64__)
                        #ifdef WIN32
                        // 48 8b 45 10          	mov    0x10(%rbp),%rax
                        // 8b 00               	mov    (%rax),%eax
                        // PONTEIRO
                        g4(a,0x48,0x8b,0x45,16);
                        g2(a,0x8b,0x00);
                        #endif
                        #ifdef __linux__
                        //48 8b 45 f8          	mov    -0x8(%rbp),%rax
                        // 8b 00                	mov    (%rax),%eax
                        // PONTEIRO
                        g4(a,0x48,0x8b,0x45,0xf8);
                        g2(a,0x8b,0x00);
                        #endif
                    #else
                        // 32 bits
                        // 8b 45 08             	mov    0x8(%ebp),%eax
                        // 8b 40    04             	mov    0x4(%eax),%eax
                        // PONTEIRO
                        g3(a,0x8b,0x45,0x08);
//                        g3(a,0x8b,0x40,4);
                    #endif
                    }
                    lex(l);
                }
            } // if (argument[i].type[0]==TYPE_UNKNOW)
            else {
                if (i==0 && argument[0].type[0]==TYPE_LONG) {
//                    printf ("----------------- long dentro da funcaoFUNCAO ARGUMETO(INT) (%s)\n", argument[0].name);
                #if defined(__x86_64__)
                    #ifdef WIN32
                    // 8b 45 10             	mov  16(%rbp),%eax // 16
                    g3(a,0x8b,0x45,16);
                    #endif
                    #ifdef __linux__
                    //8b 45 fc             	mov    -4(%rbp),%eax
                    g3(a,0x8b,0x45,0xfc);
                    #endif
                #else
                    // 32 bits
                    // 8b 45 08             	mov    8(%ebp),%eax
                    g3(a,0x8b,0x45,8);
                #endif
                }
                lex(l);
            }
        }
        else
				if ((i = VarFind(l->token)) != -1) {
            var_type = Gvar[i].type;

            if (main_variable_type==TYPE_FLOAT && var_type != TYPE_FLOAT) {
            #ifdef __x86_64__
                // db 04 25     10 30 40 00 	fildl  0x403010
                g3(a,0xdb,0x04,0x25); asm_get_addr(a, &Gvar[i].value.l);
            #else
                // db 05    70 40 40 00    	fildl  0x404070
                g2(a,0xdb,0x05); asm_get_addr(a, &Gvar[i].value.l);
            #endif
            } else {
                if (var_type==TYPE_FLOAT) {
                    emit_float_flds (a, &Gvar[i].value.f);
                } else {
										emit_expression_push_var (a, &Gvar[i].value.l);
                }
            }

            lex(l);
        }
        else Erro ("%s: %d: - Ilegal Word '%s'\n", l->name, l->line, l->token);
		}
		else if (l->tok==TOK_NUMBER) {
        if (strchr(l->token, '.'))
            var_type = TYPE_FLOAT;

        if (var_type==TYPE_FLOAT) {
            emit_push_float(a, atof(l->token));
        } else {
						emit_expression_push_long (a, atol(l->token));
        }
        lex(l);
    }
    else {
				printf ("ATOM ERRO TOKEN(%s) ... SAINDO\n", l->token);
				Erro("%s: %d Expression atom - Ilegal Word (%s)\n", l->line, l->token);
		}
}// atom()

void expression (LEXER *l, ASM *a) {

    if (l->tok==TOK_STRING) { // OK !
        TFstring *s = fs_new (l->token);
        if (s) {
            emit_print_string (a, s->s);
        }
        return;
    }

    if (l->tok==TOK_ID || l->tok==TOK_NUMBER) {
        TFunc *fi;
        int i, next;

        next = see(l);

        if (next=='.' && ModuleIsLib(l->token)) { // ! console.log();
            char buf[100];
            lex_save(l);
            sprintf (buf, "%s", l->token); // TModule.name: := "console"
            lex(l); // .
            if (lex(l)==TOK_ID && (fi = ModuleFind (buf, l->token)) != NULL) { // ! FuncName in TModule
                execute_call (l, a, fi);
          return;
            }
            else lex_restore(l);
        }

        // call a function without return:
        //   function_name (...);
        //
        if ((fi = FuncFind(l->token)) != NULL) {
            execute_call (l, a, fi);
            return;
        }

        if (l->tok==TOK_NUMBER && strchr(l->token, '.'))
            main_variable_type = var_type = TYPE_FLOAT;
        else
            main_variable_type = var_type = TYPE_LONG; // 0

        if ((i = VarFind(l->token)) != -1) {

            main_variable_type = var_type = Gvar[i].type;

            if (next=='=') {
                lex_save(l); // save the lexer position
                lex(l); // =
                if (lex(l)==TOK_ID) {
/*
                    if (see(l)=='.' && ModuleIsLib(l->token)) {
                        lex_save(l); // save the lexer position
                        sprintf(buf, "%s", l->token);
                        lex(l); // .
                        if (lex(l)==TOK_ID && (fi = ModuleFind(buf, l->token)) != NULL) { // Module FuncName

                            execute_call(l, a, fi);

                            //
                            // The function return is stored in register %eax ... types ! TYPE_FLOAT
                            //
                            //   HERE: copy the register ( %eax ) to the variable
                            //
                            if (main_variable_type != TYPE_FLOAT) {
                                emit_mov_reg_var(a, EAX, &Gvar[i].value.i);
                            } else {

                            }

                      return;
                        }
                    }
*/
                    //
                    // call a function with return:
                    //   i = function_name (...);
                    //
                    if ((fi = FuncFind(l->token)) != NULL) {
                        execute_call(l, a, fi);

                        //
                        // The function return is stored in register %eax ... types ! TYPE_FLOAT
                        //
                        //   HERE: copy the register ( %eax ) to the variable
                        //
                        // OBS:
                        //   todo retorno eh armazenado em %eax, exceto tipo (float e double).
                        //
                        if (Gvar[i].type==TYPE_FLOAT) {
                            emit_float_fstps(a, &Gvar[i].value.f);
                        } else {
                            emit_mov_reg_var(a, EAX, &Gvar[i].value.l);
                        }

                        return;
                    }//: if ((fi = FuncFind(l->token)) != NULL)

                }
                lex_restore(l); // restore the lexer position

            }// if (see(l)=='=')

        }// if ((i = VarFind(l->token)) != -1)

        //---------------------------------------
        // Expression types:
        //   a * b + c * d;
        //   10 * a + 3 * b;
        //   i;
        //---------------------------------------
        //
        asm_expression_reset(); // reg = 0;
        if (expr0(l,a) == -1) { // Expression: a + b;
            int new_line = 0;
            if (l->tok==';')
                new_line = 1;
            // display the expression result
            emit_pop_print_result (a, main_variable_type, new_line);
        }
    }
    else Erro("%s: %d | Expression ERRO - Ilegar Word (%s)\n", l->name, l->line, l->token);
}

static void do_block (LEXER *l, ASM *a) {
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
  case TOK_INT: word_int (l,a); return 1;
  case TOK_VAR: word_var (l,a); return 1;
  case TOK_ASM: word_asm (l,a); return 1;
  case TOK_IF: word_if (l,a); return 1;
  case TOK_FOR: word_for (l,a); return 1;
  case TOK_BREAK: word_break (l,a); return 1;
  case TOK_FUNCTION: word_function (l,a); return 1;
  case TOK_INCLUDE: word_include (l,a); return 1;
  case TOK_MODULE: word_module (l,a); return 1;
  case TOK_IMPORT: word_import (l,a); return 1;
  case TOK_DEFINE: word_DEFINE(l,a); return 1;
  case TOK_IFDEF: word_IFDEF (l,a); return 1;
  default: expression (l,a); return 1;
  case '}': l->level--; return 1;
  case '#':
  case ';':
  case ',':
  case TOK_ENDIF:
    return 1;
  case 0: return 0;
  }
  return 1;
}

int core_Parse (LEXER *l, ASM *a, char *text, char *name) {
  lex_set (l, text, name);
  ErroReset ();
  asm_Reset (a);
  emit_begin (a);
  while (!erro && stmt(l,a)) {
    // ... compiling ...
  }
  emit_end (a);
  if (l->level) // { ... }
    Erro ("\nERRO: LEXER->level { ... }: %d\n", l->level);
  if (l->ifdef_block)
    Erro ("%s: #ifdef block need close ...\n", l->name);
  return erro;
}

//
// function_name (a, b, c + d);
//
static void execute_call (LEXER *l, ASM *a, TFunc *func) {
    int count = 0, is_float = 0;
		#if !defined(__x86_64__)
    int pos = 0, size = 4, is_string = 0;
		#endif

    // no argument
    if (func->proto && func->proto[1] == '0') {
        while (lex(l))
            if (l->tok == ')' || l->tok == ';') break;
    } else {

        // get next: '('
        if (lex(l)!='(') { Erro ("Function need char: '('\n"); return; }

        while (lex(l)) {

            if (l->tok==TOK_ID || l->tok==TOK_NUMBER || l->tok==TOK_STRING || l->tok=='(') {

                #if !defined(__x86_64__)
                if (l->tok==TOK_STRING)
                    is_string = 1;
                #endif

                // process function argumento float ... NEED IMPLEMENTATION
                #ifdef __x86_64__
                int i;
                if (l->tok==TOK_NUMBER && strchr(l->token, '.')) {
                    if (is_float==0) emit_func_arg_number_float0 (a,atof(l->token));
                    if (is_float==1) emit_func_arg_number_float1 (a,atof(l->token));
                    if (is_float==2) emit_func_arg_number_float2 (a,atof(l->token));
                    if (is_float==3) emit_func_arg_number_float3 (a,atof(l->token));
                    if (is_float==4) emit_func_arg_number_float4 (a,atof(l->token));
                    is_float++;
              continue;
                }
                if ((i = VarFind(l->token)) != -1) {
                    if (Gvar[i].type==TYPE_FLOAT) {
                        if (is_float==0) emit_func_arg_var_float0 (a,&Gvar[i].value.f);
                        if (is_float==1) emit_func_arg_var_float1 (a,&Gvar[i].value.f);
                        if (is_float==2) emit_func_arg_var_float2 (a,&Gvar[i].value.f);
                        if (is_float==3) emit_func_arg_var_float3 (a,&Gvar[i].value.f);
                        if (is_float==4) emit_func_arg_var_float4 (a,&Gvar[i].value.f);
                        is_float++;
              continue;
                    }
                }
                #endif

                main_variable_type = var_type = TYPE_LONG;

								asm_expression_reset(); // reg = 0;
                // The result of expression is store in the register: %eax.
                expr0 (l,a);

                #if defined(__x86_64__)
                if (var_type != TYPE_FLOAT) {
								    // 4 arquments
								    if (count <= 4) {
										    gen(a,PUSH_EAX);
                    }
                } else {
                    // ... float type ...
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
                        g4 (a,0xdd,0x5c,0x24,(char)pos); // fstpl
                    else
                        g4 (a,0xd9,0x5c,0x24,(char)pos); // fstps
                    if (is_string) size = 8;
                    pos += size;
                }
                #endif

                if (count++ > 10) break;
            }
            if (l->tok == ')' || l->tok == ';') break;
        }
    }

    if (count > 5) {
        Erro ("%s:%d: - IMPLEMENTED MAX ARGUMENTS 5 ... Call Function(%s) the max arguments is: 4\n", l->name, l->line, func->name);
  return;
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

		// pass 64 bits arquments:
		#if defined(__x86_64__)
		if (count > 0) {
        int i;
				for (i = count-1; i >= 0; i--) {
						if (i == 0) { // argument 1
								#ifdef WIN32
								gen(a,POP_ECX);
								#else
								gen(a,POP_EDI);
								#endif
						}
						else if (i == 1) { // argument 2
								#ifdef WIN32
								gen(a,POP_EDX);
								#else
								gen(a,POP_ESI);
								#endif
						}
						else if (i == 2) { // argument 3
								#ifdef WIN32
                // 58                   	pop    %rax
                // 41 89 c0             	mov    %eax,%r8d
                g4(a,0x58,0x41,0x89,0xc0);
								#else
                g(a,POP_EDX);
								#endif
						}
						else if (i == 3) { // argument 4
								#ifdef WIN32
                // 58                   	pop    %rax
                // 41 89 c1             	mov    %eax,%r9d
                g4(a,0x58,0x41,0x89,0xc1);
								#else
                g(a,POP_ECX);
								#endif
						}
						else if (i == 4) { // argument 5
								#ifdef WIN32
                // 58                   	pop    %rax
                g(a,0x58);
                // 48 89 44 24    20       	mov    %rax,0x20(%rsp)
                g5(a,0x48,0x89,0x44,0x24,64); // 32
								#else
                // 41 58                	pop    %r8
                g2(a,0x41,0x58);
								#endif
						}
				}
		}
		#endif

    if (func->type == FUNC_TYPE_MODULE) {
		    emit_call_direct (a, func->code);
printf ("Call direct(%s)\n", func->name);
    } else {
		    emit_call (a, func->code);
    }
}

void core_ModuleAdd (char *module_name, char *func_name, char *proto, UCHAR *code) {
    TModule *mod, *p = Gmodule;
    TFunc *func;

    while (p) {
        if (!strcmp(module_name, p->name)) {
/*
            TFunc *f = p->func;
            // if function exist then return 
            while (f) {
                if (!strcmp(f->name, func_name)) {
                    printf("FUNCTION EXIST: %s.%s()\n", p->name, f->name);
                    return;
                }
                f = f->next;
            }
*/
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
    if ((mod = (TModule*) malloc(sizeof(TModule))) != NULL) {
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

ASM * core_Init (unsigned int size) {
  static int init = 0;
  if (!init) {
    ASM *a;
    init = 1;
    if ((a =            asm_New(size)) == NULL) return NULL;
    if ((asm_function = asm_New(size)) == NULL) return NULL;
    if ((asm_include1 = asm_New(size)) == NULL) return NULL;
    if ((asm_include2 = asm_New(size)) == NULL) return NULL;

    if (asm_SetExecutable_ASM(asm_include1, size - 2) != 0) return NULL;
    if (asm_SetExecutable_ASM(asm_include2, size - 2) != 0) return NULL;

    core_ModuleAdd ("console", "log", "0s", (UCHAR*)lib_printf);

    #ifdef WIN32
    DefineAdd("WIN32", 1);
    #endif
    #ifdef __linux__
    DefineAdd("__linux__", 1);
    #endif

    #ifdef __x86_64__
      // 64 bits
      DefineAdd("__x86_64__", 1);
      #ifdef WIN32
      DefineAdd("WINDOWS_64", 1);
      #endif
      #ifdef __linux__
      DefineAdd("LINUX_64", 1);
      #endif
    #else // 64 bits
      // 32 bits
      DefineAdd("__x86_32__", 1);
      #ifdef WIN32
      DefineAdd("WINDOWS_32", 1);
      #endif
      #ifdef __linux__
      DefineAdd("LINUX_32", 1);
      #endif
    #endif // 32 bits

    printf ("core_Init >>>>>>> ASM CREATED:\nasm_main\nasm_function\nasm_include1\nasm_include2\n");

    return a;
  }
  return NULL;
}

void core_Finalize (void) {
  // free Fixed String:
  while (fs != NULL) {
    TFstring *temp = fs->next;
    if (fs->s) {
      //printf ("FSTRING(%s)\n", fs->s);
      free(fs->s);
    }
    free(fs);
    fs = temp;
  }
  ASM_FREE (asm_function);
  ASM_FREE (asm_include1);
  ASM_FREE (asm_include2);
}

static int see (LEXER *l) {
  char *s = l->text+l->pos;
  while (*s) {
    if (*s=='\n' || *s==' ' || *s==9 || *s==13) {
      s++;
    } else {
      if (s[0]=='=' && s[1]=='=') return TOK_EQUAL_EQUAL;
      if (s[0]=='+' && s[1]=='+') return TOK_PLUS_PLUS;
      if (s[0]=='+' && s[1]=='=') return TOK_PLUS_EQUAL;
      if (s[0]=='-' && s[1]=='-') return TOK_MINUS_MINUS;
      return *s;
    }
  }
  return 0;
}

void CreateVar (char *name, char *svalue) {
  TVar *v = Gvar;
  int i = 0;
  while (v->name) {
    if (!strcmp(v->name, name))
      return;
    i++;
    v++;
  }
  if (i < GVAR_SIZE) {
    if (strchr(svalue, '.')) {
      v->type = TYPE_FLOAT;
      v->value.f = atof(svalue);
    } else {
      v->type = TYPE_LONG;
      v->value.l = atol(svalue);
    }
    v->name = strdup(name);
    v->info = NULL;
  }
}

void CreateVarLong (char *name, long l) {
  TVar *v = Gvar;
  int i = 0;
  while (v->name) {
    if (!strcmp(v->name, name))
      return;
    i++;
    v++;
  }
  if (i < GVAR_SIZE) {
    v->name = strdup(name);
    v->type = TYPE_LONG;
    v->value.l = l;
    v->info = NULL;
  }
}

void FuncAdd (TFunc *func) {
  // add on top:
  func->next = Gfunc;
  Gfunc = func;
}

static void DefineAdd (char *name, int value) {
  TDefine *o = Gdefine, *n;
  while (o) {
    if (!strcmp(o->name, name))
      return;
    o = o->next;
  }
  if ((n = (TDefine*)malloc(sizeof(TDefine)))==NULL)
    return;
  n->name = strdup(name);
  n->value = value;
  // add on top
  n->next = Gdefine;
  Gdefine = n;
}

int is_defined (char *name) {
  TDefine *p = Gdefine;
  while (p) {
    if (!strcmp(p->name, name))
      return 1;
    p = p->next;
  }
  return 0;
}

void proc_ifdef (char *name) {
  TDefine *p = Gdefine;
  while (p) {
    if (!strcmp(p->name, name))
      return;
    p = p->next;
  }
  ifndef_true = 1;
}

char * FileOpen (const char *FileName) {
  FILE *fp;
  if ((fp = fopen (FileName, "rb")) != NULL) {
    char *str;
    int size, i;
    fseek (fp, 0, SEEK_END);
    size = ftell(fp);
    fseek (fp, 0, SEEK_SET);
    str = (char *)malloc (size + 5);
    if (!str) {
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

int ArgumentFind (char *name) {
  int i;
  for (i = 0; i < argument_count; i++)
    if (!strcmp(argument[i].name, name))
      return i;
  return -1;
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

int VarFind (char *name) {
  TVar *v = Gvar;
  int i = 0;
  while(v->name) {
    if (!strcmp(v->name, name))
      return i;
    i++;
    v++;
  }
  return -1;
}

TFunc *ModuleFind (char *LibName, char *FuncName) {
    TModule *p = Gmodule;
//    TModule *pnew = NULL;
    while (p) {
        if (!strcmp(p->name, LibName)) {
            TFunc *f = p->func;
//            pnew = p;
            while (f) {
                if (!strcmp(f->name, FuncName)) {
              return f;
                }
                f = f->next;
            }
        }
        p = p->next;
    }
/*
    //
    // ! create new, if function exist in library:
    //
    if (pnew != NULL) {
        #ifdef WIN32
        void *fp = (void*)GetProcAddress ((HMODULE)pnew->lib, FuncName);
        #endif
        #ifdef __linux__
        void *fp = dlsym (pnew->lib, FuncName);
        #endif
        TFunc *fn;
        if (fp && (fn = (TFunc*) malloc (sizeof(TFunc))) != NULL) {
            fn->name = strdup (FuncName);
            fn->proto = strdup ("ii");
            fn->type = FUNC_TYPE_MODULE;
            fn->len = 0;
            fn->code = (UCHAR*)fp;
            //
            // add in Gmodule ... on top
            //
            fn->next = pnew->func;
            pnew->func = fn;
      return fn;
        }
    }
*/
    return NULL;
}

int ModuleIsLib (char *LibName) {
    TModule *p = Gmodule;
    while (p) {
        if (!strcmp(p->name, LibName))
      return 1;
        p = p->next;
    }
    return 0;
}

TFstring *fs_new (char *s) {
  static int count = 0;
  TFstring *p = fs, *n;
  while (p) {
    if (!strcmp(p->s,s))
      return p;
    p = p->next;
  }
  if ((n = (TFstring*)malloc(sizeof(TFstring)))==NULL)
    return NULL;
  n->s = strdup(s);

//printf ("FIXED: %p\n", &n->s);
  n->i = count++;
  // add on top
  n->next = fs;
  fs = n;
  return n;
}

static void word_int (LEXER *l, ASM *a) {
  while (lex(l)) {
    if (l->tok==TOK_ID) {
      char name[255];
      int value = 0;
      strcpy (name, l->token); // save
      if (lex(l)=='=') {
        if (lex(l)==TOK_NUMBER)
          value = atoi (l->token);
      }
      if (is_function) {
/*
                //---------------------------------------------------
                // this is temporary ...
                // in function(word_function) changes ...
                //---------------------------------------------------
                if (local_count < LOCAL_MAX) {
                    sprintf (local[local_count].name, "%s", name);
                    local[local_count].type = TYPE_LONG;
                    local[local_count].value.l = value;
                    local[local_count].info = NULL;
                    local_count++;
                }
                else Erro ("Variable Local Max %d\n", LOCAL_MAX);
                // ... need implementation ...
*/
      }
      else CreateVarLong (name, value);
    }
    if (l->tok == ';') break;
  }
  if (l->tok != ';') Erro ("ERRO: The word(int) need the char(;) on the end\n");
}// word_int()

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
    if (l->tok != ';') Erro ("ERRO: The word(var) need the char(;) on the end\n");

}//: word_var ()

static void word_asm (LEXER *l, ASM *a) {
		Assemble (l,a);
}

static void word_function (LEXER *l, ASM *a) {
    TFunc *func;
    UCHAR *code;
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
            argument[argument_count].type[0] = TYPE_LONG; // 0
            //strcpy (argument[argument_count].type, "int");
            if (lex(l)==TOK_ID) {
                strcpy (argument[argument_count].name, l->token);
                strcat (proto, "i");
                argument_count++;
            }
        }
        else if (l->tok==TOK_FLOAT) {
            argument[argument_count].type[0] = TYPE_FLOAT; // 1
            //strcpy (argument[argument_count].type, "int");
            if (lex(l)==TOK_ID) {
                strcpy (argument[argument_count].name, l->token);
                strcat (proto, "f");
                argument_count++;
            }
        }
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
    } else {
    }
    if (l->tok=='{') l->pos--; else { Erro("Word Function need char: '{'"); return; }

    is_function = 1;
    local_count = 0;
    strcpy (func_name, name);

    // compiling to buffer ( f ):
    //
    asm_Reset (asm_function);
    emit_begin (asm_function);


    #ifdef __x86_64__
        #ifdef WIN32
        if (argument_count == 1) {
            if (argument[0].type[0] == TYPE_UNKNOW) {
                // posicao 8
                // 48 89 4d 10          	mov    %rcx,0x10(%rbp) // 16
                // ARGUMENT POINTER:
                g4(asm_function,0x48,0x89,0x4d,16);
            } else if (argument[0].type[0] == TYPE_LONG) {
                // posicao 8
                // 89 4d 10             	mov    %ecx,0x10(%rbp) // 16
                g3(asm_function,0x89,0x4d,16);
                g(asm_function,OP_NOP);
            }
            else Erro ("%s: %d - USAGE: function func_name (arg) { ... }\n", l->name, l->line);
        }
        #endif
        #ifdef __linux__
        if (argument_count == 1) {
            if (argument[0].type[0] == TYPE_UNKNOW) {
                // PONTEIRO:
                // 48 89 7d f8          	mov    %rdi,-0x8(%rbp)
                g4(asm_function,0x48,0x89,0x7d,0xf8);
            } else if (argument[0].type[0] == TYPE_LONG) {
                // LONG
                // 89 7d fc             	mov    %edi,-0x4(%rbp)
                g3(asm_function,0x89,0x7d,0xfc);
            }
            else Erro ("%s: %d - USAGE: function func_name (arg) { ... }\n", l->name, l->line);
        }
        #endif
    #endif
    stmt (l,asm_function); // here start from char: '{'
    emit_end (asm_function);

    if (erro) return;

    int len = asm_GetLen (asm_function);

    // new function:
    //
    func = (TFunc*) malloc (sizeof(TFunc));
    func->name = strdup (func_name);
    func->proto = strdup (proto);
    func->type = FUNC_TYPE_COMPILED;
    func->len = len;
    func->code = (UCHAR*) malloc (func->len);

    code = asm_GetCode(asm_function);

    // NOW: copy the buffer ( f ):
    for (i=0;i<func->len;i++) {
        func->code[i] = code[i];
    }

    //-------------------------------------------
    // HACKING ... ;)
    // Resolve Recursive:
    // change 4 bytes ( func_null ) to this
    //-------------------------------------------
/*
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
*/

    asm_SetExecutable_PTR (func->code, len);

    // add on top:
    func->next = Gfunc;
    Gfunc = func;

    is_function = is_recursive = argument_count = *func_name = 0;

}// word_function ()

static void word_break (LEXER *l, ASM *a) {
    if (loop_level) {
        emit_jump_jmp (a, array_break[loop_level]);
    }
    else Erro ("%s: %d: word 'break' need a loop", l->name, l->line);
}

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
        asm_Label(a, array[for_count]);

        stmt (l,a); //<<<<<<<<<<  block  >>>>>>>>>>

        emit_jump_jmp (a, array[for_count]);

        asm_Label (a, array_break[loop_level]); // used to break
        for_count--;

loop_level--;  // <<<<<<<<<<  ! POP  >>>>>>>>>>
    } else {
        int i; // var index
        int type = 0; // <  >  ==  !=
        int inc = 0; // ++, --
        int var_count = -1, number_count = 0;

        // for (i = 10; i < 100; i++) { ... }
        asm_expression_reset(); // reg = 0;
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

                    asm_Label (a, array[for_count]);

                    if (var_count == -1) {
                        emit_mov_long_reg(a, number_count, EAX);
                    } else {
                        emit_mov_var_reg (a, &Gvar[var_count].value.l, EAX);
                    }
                    emit_cmp_eax_var (a, &Gvar[i].value.l);

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

                    //---------------------------------------------------------------
                    // process the block starting from string char: '{'
                    //---------------------------------------------------------------
                    //
                    stmt (l,a);  //<<<<<<<<<<  block  >>>>>>>>>>

                    if (inc == TOK_PLUS_PLUS)
                        emit_incl (a, &Gvar[i].value.l);
                    else if (inc == TOK_MINUS_MINUS)
                        emit_decl (a, &Gvar[i].value.l);

                    //
                    // Jump to: " TOP OF LOOP "
                    //
                    emit_jump_jmp (a, array[for_count]);

                    asm_Label(a, array_break[loop_level]); // used to break
                    for_count--;

//-------------------------------------------------------------------
//<<<<<<<<<<<<<<<<<<<<<<<  " END OF LOOP "  >>>>>>>>>>>>>>>>>>>>>>>>>
//-------------------------------------------------------------------

loop_level--;
                }// if (l->tok == ')' && see(l)=='{')
                else Erro ("%s: %d: USAGE: for(i = 1; i < 100; i++) { ... }\n", l->name, l->line);
            }
            else Erro ("%s: %d: USAGE: for(i = 1; i < 100; i++) { ... }\n", l->name, l->line);

        }// if (i != -1)
        else Erro ("%s: %d: USAGE: for(i = 1; i < 100; i++) { ... }\n", l->name, l->line);

    }// } else {

}// word_for ()

static void word_if (LEXER *l, ASM *a) {
    //**** to "push/pop"
    static char array[20][20];
    static int if_count_total = 0;
    static int if_count = 0;

    int line = l->line;
    if (lex(l) !='(') { Erro ("%s: %d - ERRO SINTAX (if) need char: '('\n", l->name, line); return; }

    if_count++;
    sprintf (array[if_count], "IF%d", if_count_total++);

    while (!erro && lex(l)) { // pass arguments: if (a > b) { ... }
        int is_negative = 0, _tok_;

        if (l->tok == '!') { is_negative = 1; lex(l); }

        asm_expression_reset(); // reg = 0;
        expr0(l,a); // Expression result in register: %eax

        if (erro) {
            Erro ("<<<<<<<<<<  if erro >>>>>>>>>>>>>\n");
            return;
        }

        _tok_ = l->tok;
        if (l->tok == ')' || l->tok == TOK_AND_AND) {
            // ... none ..
        } else {
            g2(a,G2_MOV_EAX_EDX); // "save" %eax in %ebx
            asm_expression_reset(); // reg = 0;
            lex(l); expr0(l,a);
            g2(a,G2_CMP_EAX_EDX);
        }

        switch (_tok_) {
        case ')': // if (a) { ... }
        case TOK_AND_AND:
            g2(a,0x85,0xc0); // 85 c0    test   %eax,%eax
            if (is_negative == 0) emit_jump_je  (a,array[if_count]);
            else                  emit_jump_jne (a,array[if_count]);
            break;

        case '>':
            emit_jump_jle (a,array[if_count]);
            break;

        case '<':
            emit_jump_jge (a,array[if_count]);
            break;

        case TOK_EQUAL_EQUAL: // ==
            emit_jump_jne(a,array[if_count]);
            break;

        case TOK_NOT_EQUAL: // !=
            emit_jump_je (a,array[if_count]);
            break;

        default:
            Erro ("%s: %d: Word ( if ) compare not implemented: '%c'\n", l->name,l->line, _tok_);
            return;
        }//: switch(tok)

        if (l->tok==')') break;
    }
    if (see(l)=='{') stmt (l,a); else Erro ("word(if) need start block: '{'\n");

    asm_Label (a, array[if_count]);
    if_count--;

}// word_if ()

static void include1 (LEXER *l, ASM *a) {
    LEXER lexer;
    char *text;
    if ((text = FileOpen(l->token)) != NULL) {
        if (core_Parse(&lexer, asm_include1, text, l->token) == 0) {
            asm_Run (asm_include1); //<<<<<<<<<<  execute the JIT here  >>>>>>>>>>
        }
        else Erro ("FILE %s: %d '%s'\n", lexer.name, lexer.line, l->token);
        free (text);
    }
    else Erro ("%s %d | File Not Found: '%s'\n", l->name, l->line, l->token);
}
static void include2 (LEXER *l, ASM *a) {
    LEXER lexer;
    char *text;
    if ((text = FileOpen(l->token)) != NULL) {
        if (core_Parse(&lexer, asm_include2, text, l->token) == 0) {
            asm_Run (asm_include2); //<<<<<<<<<<  execute the JIT here  >>>>>>>>>>
        }
        else Erro ("FILE %s: %d '%s'\n", lexer.name, lexer.line, l->token);
        free (text);
    }
    else Erro ("%s %d | File Not Found: '%s'\n", l->name, l->line, l->token);
}

static void word_include (LEXER *l, ASM *a) {
    static int icount = 1;
    if (lex(l)==TOK_STRING) {
        if (icount==1 || icount==2) {
            int i = icount++;
            if (i == 1) {
                include1(l,a);
            } else if (i==2){
                include2(l,a);
            }
            icount--;
        }
        else Erro ("%s: %d Max include 2\n", l->name, l->line);
    }
    else Erro ("%s: %d - INCLUDE USAGE: include %cfile_name%c\n", l->name, l->line, '"', '"' );
}

//
// define TOK_ID  100
//
static void word_DEFINE (LEXER *l, ASM *a) {
  if (lex(l)==TOK_ID) {
    char name[100];
    strcpy (name,l->token);
    if (lex(l)==TOK_NUMBER) {
      DefineAdd(name,atoi(l->token));
    }
    else Erro("%s: %d | USAGE: #define NUMBER\n", l->name, l->line);
  }
}

static void word_IFDEF (LEXER *l, ASM *a) {
    char text[100];
    int _line_;
    TDefine *o = Gdefine;

    _line_ = l->line;
    if (lex(l)==TOK_ID) {
        strcpy (text, l->token);

        while (o) { // ! if exist
            if (!strcmp(o->name, l->token)) return;
            o = o->next;
        }
        while (lex(l)) { // ! execute not defined
            if (l->ifdef_block==0) break;
        }

        if (l->tok != TOK_ENDIF) {
            Erro ("%s: %d: ERRO LINE %d - ENDIF(%s) not found\n", l->name, l->line, _line_, text);
        }
    }
    else Erro ("%s: %d: USAGE: ifdef name\n", l->name, _line_);
}

//
// library ("SDL", "sdl");
//
static void word_module (LEXER *l, ASM *a) {
    char FileName [100] = { 0 };
    char LibName  [100] = { 0 };
    void *lib = NULL;
    int count = 0;

#ifdef __linux__
    #ifdef __x86_64__
    Erro ("%s: %d | MODULE Not For Linux 64 bits ... PLEASE WAIT IMPLEMENTATION !!!\n", l->name, l->line);
    return;
    #endif
#endif

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
    TModule *mod, *p = Gmodule;
    while (p) { // ! if exist
        if (!strcmp(p->name, LibName))
      return;
        p = p->next;
    }

    #ifdef WIN32
    lib = (void *)LoadLibrary (FileName);
    #endif
    #ifdef __linux__
    lib = dlopen (FileName, RTLD_NOW); // RTLD_LAZY); // RTLD_NOW
    #endif
    if (lib && (mod = (TModule*) malloc(sizeof(TModule))) != NULL) {
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
// import ("sdl", "SDL_init", "0i", 0);
// import (0, "SDL_init", "0i", 0);
//
static void word_import (LEXER *l, ASM *a) {
    char LibName[100] = { '0', 0 };
    char FuncName[100] = { 0 };
    char proto[100] = { '0', '0', 0 };
    int sub = 0;
    TModule *p = Gmodule;
    void *fp = NULL;
    TFunc *func = NULL;
    int count = 0;

#ifdef __linux__
    #ifdef __x86_64__
    Erro ("%s: %d | IMPORT Not For Linux 64 bits ... PLEASE WAIT IMPLEMENTATION !!!\n", l->name, l->line);
    return;
    #endif
#endif

    while (lex(l)) {
        if (l->tok==TOK_STRING || l->tok==TOK_NUMBER) {
            if (count==0) {
                sprintf (LibName, "%s", l->token);
            } else if (count==1) {
                sprintf (FuncName, "%s", l->token);
            } else if (count==2) {
                sprintf(proto, "%s", l->token);
            } else if (count==3) { // sub_esp
                sub = atoi(l->token);
            }
            count++;
        }
        if (l->tok==')' || l->tok==';') break;
    }

    if (LibName[0]=='0') {

        TFunc *f = Gfunc;
        // if function exist then return 
        while (f) {
            if (!strcmp(f->name, FuncName)) {
                printf("FUNCTION EXIST: %s()\n", f->name);
                return;
            }
            f = f->next;
        }

        #ifdef WIN32
        fp = (void*)GetProcAddress ((HMODULE)p->lib, FuncName);
        #endif
        #ifdef __linux__
        fp = dlsym (p->lib, FuncName);
        #endif
        if (fp && (func = (TFunc*) malloc (sizeof(TFunc))) != NULL) {
            func->name = strdup (FuncName);
            func->proto = strdup (proto);
            func->type = FUNC_TYPE_MODULE;
            func->len = 0;
            func->sub_esp = sub;
            func->code = fp;

            //
            // add in Gfunc ... on top
            // USAGE:
            //   SDL_SetVideoMode (...);
            //
            func->next = Gfunc;
            Gfunc = func;
        }
        else printf ("Function Not Found: '%s'\n", FuncName);//Erro("%s: %d: USAGE: import(%csdl%c, %SDL_Init%c, %c0i%c\n", '"', '"', '"');
        return;
    }

    while (p) {
        if (!strcmp(p->name, LibName)) {
/*
            TFunc *f = p->func;
            // if function exist then return 
            while (f) {
                if (!strcmp(f->name, FuncName)) {
                    printf("FUNCTION EXIST: %s.%s()\n", p->name, f->name);
                    return;
                }
                f = f->next;
            }
*/
            #ifdef WIN32
            fp = (void*)GetProcAddress ((HMODULE)p->lib, FuncName);
            #endif
            #ifdef __linux__
            fp = dlsym (p->lib, FuncName);
            #endif
            if (fp && (func = (TFunc*) malloc (sizeof(TFunc))) != NULL) {
                func->name = strdup (FuncName);
                func->proto = strdup (proto);
                func->type = FUNC_TYPE_MODULE;
                func->len = 0;
                func->sub_esp = sub;
                func->code = fp;

                //
                // add in Gmodule ... on top
                // USAGE:
                //   sdl.SDL_SetVideoMode (...);
                //
                func->next = p->func;
                p->func = func;
            }
            else printf ("Function Not Found: '%s'\n", FuncName);//Erro("%s: %d: USAGE: import(%csdl%c, %SDL_Init%c, %c0i%c\n", '"', '"', '"');
            return;
        }
        p = p->next;
    }//: while (p)
}



void lib_info (int arg) {
    switch (arg) {
    case 1: {
        TVar *v = Gvar;
        int i = 0;
        printf ("VARIABLES:\n---------------\n");
        while (v->name) {
            if (v->type==TYPE_LONG)   printf ("Gvar[%d](%s) = %ld\n", i, v->name, v->value.l);
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
                if (*s=='0') printf ("void   ");
                else
                if (*s=='i') printf ("int    ");
                else
                if (*s=='f') printf ("float  ");
                else
                if (*s=='s') printf ("char   *");
                else
                if (*s=='p') printf ("void * ");
                printf ("%s (", fi->name);
                s++;
                while(*s){
                    if (*s=='0') printf ("void");
										else
                    if (*s=='i') printf ("int");
                    else
                    if (*s=='f') printf ("float");
                    else
                    if (*s=='s') printf ("char *");
                    else
                    if (*s=='p') printf ("void *");
                    else
                    if (*s=='.') printf ("...");
                    s++;
                    if(*s) printf (", ");
                }
                printf (");\n");
            }
            fi++;
        }
        fi = Gfunc;
        while(fi){
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
                    if (*s=='0') printf ("void");
										else
                    if (*s=='i') printf ("int");
                    else
                    if (*s=='f') printf ("float");
                    else
                    if (*s=='s') printf ("char *");
                    else
                    if (*s=='p') printf ("void *");
                    else
                    if (*s=='.') printf ("...");
                    s++;
                    if(*s) printf (", ");
                }
                printf (");\n");
            }
            fi = fi->next;
        }
        TModule *p = Gmodule;
        while (p) {
            TFunc *f = p->func;
            printf ("MODULE: '%s'\n", p->name);
            while (f) {
                printf ("  '%s'\n", f->name);
                f = f->next;
            }
            p = p->next;
        }

				}
        break;

    case 3: {
        TDefine *o = Gdefine;
        printf ("ALL DEFINES:\n---------------\n");
        while (o) {
            printf ("  %s : %d\n", o->name, o->value);
            o = o->next;
        }
        } break;

    default:
        printf ("USAGE(%d): info(1);\n\nInfo Options:\n 1: Variables\n 2: Functions\n 3: Defines\n 4: Words\n",arg);
    }
}

int lib_printf (char *format, ...) {
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
    }
    else putc (msg[i], stdout);
  }
  if (!new_line)
    printf ("\n");
  return i;
}

void lib_disasm (char *name) {
  if (name) {
    if (!strcmp(name, "main")) {
      ASM *_a_ = GetAsmMain();
      Disasm (asm_GetCode(_a_), name, asm_GetLen(_a_));
    } else {
      TFunc *func = FuncFind(name);
      if (func && func->type == FUNC_TYPE_COMPILED)
        Disasm (func->code, name, func->len);
    }
  }
}

char buf[20];
char *lib_float2s (float f) {
    sprintf (buf, "%f", f);
    return buf;
}

void lib_func_float_int (float f, int i) {
  printf ("f: %f, i: %d\n", f, i);
}


int lib_func_add (int a, int b) {
  printf ("a: %d, b: %d = %d\n", a, b, a+b);
  return a + b;
}
float lib_func_add_float (float a, float b) {
  printf ("a: %f, b: %f = %f\n", a, b, a+b);
  return a + b;
}


void lib_print_int (int i) {
  printf ("%d\n", i);
}
void lib_print_float (float f) {
  printf ("Float: %f\n", f);
}

int arg4 (int a, int b, int c, int d) {
  printf ("\nFunction Builtin: 'arg4'\n");
  printf ("a: %d, b: %d, c: %d, d: %d\n", a,b,c,d);
  printf ("arg4 result = %d\n\n", a+b+c+d);
  return a + b + c + d;
}

int arg5 (int a, int b, int c, int d, int e) {
  printf ("\nFunction Builtin: 'arg5'\n");
  printf ("a: %d, b: %d, c: %d, d: %d, e: %d\n", a,b,c,d,e);
  printf ("arg5 result = %d\n\n", a+b+c+d+e);
  return a + b + c + d + e;
}

void CallBack (void (*call)(void)) {
    call();
}

// lines: 1437

