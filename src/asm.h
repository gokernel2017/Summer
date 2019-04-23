//-------------------------------------------------------------------
//
// MINI LANGUAGE:
//
// The Backend | JIT(x86) 32/64 bits:
//
// FILE:
//   asm.h
//
// START DATE: 23/03/2019 - 08:50
//
// BY: Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
#ifndef _ASM_H_
#define _ASM_H_

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------
//------------------  INCLUDE  ------------------
//-----------------------------------------------
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef WIN32
    #include <windows.h>
#endif
#ifdef __linux__
    #include <unistd.h>
    #include <sys/mman.h> // to: mprotect()
    #include <dlfcn.h>    // to: dlopen(), dlsym(), ... in file: cs_library.c
#endif

//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//
#define LIBIMPORT					extern
//
#define ASM_DEFAULT_SIZE	50000
#define UCHAR							unsigned char
#define ASM_FREE(aa)      asm_Free(aa); aa=NULL
//
#define PUSH_EAX  0x50 // push  %eax
#define POP_EAX   0x58 // pop   %eax
#define POP_ECX		0x59 // pop   %ecx
#define POP_EDX 	0x5a // pop   %edx
#define POP_EDI		0X5f // pop   %edi
#define POP_ESI		0X5e // pop   %esi
//
#define G2_MOV_EAX_ECX	0x89, 0xc1 // 89 c1 | mov   %eax, %ecx
#define G2_MOV_EAX_EDX	0x89, 0xc2 // 89 c2 | mov   %eax, %edx
#define G2_MOV_EAX_EBX  0X89, 0Xc3 // 89 c3 | mov   %eax, %ebx
#define G2_MOV_EAX_EDI	0x89, 0xc7 // 89 c7 | mov   %eax, %edi
#define G2_MOV_EAX_ESI  0x89, 0xc6 // 89 c6 | mov   %eax, %esi
#define G2_CMP_EAX_EDX  0x39, 0xc2 // 39 c2 |	cmp   %eax, %edx
#define G2_CMP_EAX_EBX  0X39, 0Xc3 // 39 c3 | cmp   %eax, %ebx
//
#define OP_NOP          0x90

enum {
    EAX = 0,
    ECX,
    EDX,
    EBX,
    ESP,
    EBP,
    ESI,
    EDI
};

enum { // jump type for change the labels
    ASM_JUMP_JMP = 1,
    ASM_JUMP_JNE,
    ASM_JUMP_JLE,
    ASM_JUMP_JGE,
    ASM_JUMP_JG,
    ASM_JUMP_JE,
    ASM_JUMP_JL,
    ASM_JUMP_LOOP
};

//-----------------------------------------------
//-------------------  STRUCT  ------------------
//-----------------------------------------------
//
typedef struct ASM ASM; // opaque struct in file: "asm.c"

//-----------------------------------------------
//--------------  GLOBAL VARIABLE  --------------
//-----------------------------------------------
//
LIBIMPORT int   erro;
LIBIMPORT ASM * asm_function;
//
// const EMIT opcode:
//
extern const UCHAR OP_mov_eax_ecx  [2]; // mov %eax, %ecx
extern const UCHAR OP_add_ecx_eax  [2]; // add %ecx, %eax
extern const UCHAR OP_imul_ecx_eax [3]; // imul %ecx, %eax
extern const UCHAR OP_imul_edx_ecx [3]; // imul %edx, %ecx
extern const UCHAR OP_sub_ecx_eax  [2]; // sub %ecx, %eax
extern const UCHAR OP_sub_edx_ecx  [2]; // sub %edx, %ecx

extern const UCHAR OP_push_eax [1]; // push  %rax
extern const UCHAR OP_pop_eax  [1]; // pop   %rax

extern const UCHAR OP_pop_ecx  [1]; // pop  %ecx
extern const UCHAR OP_pop_edx  [1]; // pop  %edx
extern const UCHAR OP_pop_edi  [1]; // pop  %edi

//-----------------------------------------------
//-----------------  PUBLIC API  ----------------
//-----------------------------------------------
//
LIBIMPORT ASM		*	asm_New						(unsigned int size, char *name);
LIBIMPORT void		asm_Free					(ASM *a);
LIBIMPORT void		asm_Reset					(ASM *a);
LIBIMPORT int			asm_SetExecutable_ASM	(ASM *a, unsigned int size);
LIBIMPORT int 		asm_SetExecutable_PTR (void *ptr, unsigned int size);
LIBIMPORT void		asm_Run						(ASM *a);
LIBIMPORT UCHAR * asm_GetCode				(ASM *a);
LIBIMPORT int 		asm_GetLen				(ASM *a);
LIBIMPORT void    asm_Label         (ASM *a, char *name);
LIBIMPORT void    asm_CodeCopy      (ASM *src, UCHAR *dest, unsigned int len);

LIBIMPORT void    asm_get_addr      (ASM *a, void *ptr); ///: 32/64 BITS OK

// emit / gen:
LIBIMPORT void 		gen								(ASM *a, UCHAR c); // emit 1 opcode
LIBIMPORT void    g (ASM *a, UCHAR c);
LIBIMPORT void    g2 (ASM *a, UCHAR c1, UCHAR c2);
LIBIMPORT void    g3 (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3);
LIBIMPORT void    g4 (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4);
LIBIMPORT void    g5 (ASM *a, UCHAR c1, UCHAR c2, UCHAR c3, UCHAR c4, UCHAR c5);
#define 					EMIT(a,op)				emit(a, op, sizeof(op))
LIBIMPORT void		emit 							(ASM *a, const UCHAR opcode[], unsigned int len);
LIBIMPORT void 		emit_begin				(ASM *a);							// 32/64 BITS OK
LIBIMPORT void 		emit_end 					(ASM *a);							// 32/64 BITS OK
LIBIMPORT void 		emit_incl					(ASM *a, void *var);	// 32/64 BITS OK
LIBIMPORT void    emit_decl         (ASM *a, void *var); //: 32/64 BITS OK
LIBIMPORT void		emit_mov_long_reg	(ASM *a, long value, int reg); // mov $1000, %eax
LIBIMPORT void 		emit_mov_var_reg	(ASM *a, void *var, int reg); // 32/64 BITS OK: Move variable to %register
LIBIMPORT void		emit_mov_reg_var	(ASM *a, int reg, void *var); // 32/64 BITS OK: Move %register to variable
LIBIMPORT void    emit_mov_EBP_eax  (ASM *a, UCHAR c); // mov 4(%ebp), %eax
LIBIMPORT void    emit_mov_EAX_eax  (ASM *a, UCHAR c); // mov 4(%eax), %eax
//
LIBIMPORT void emit_func_arg_number_float0 (ASM *a, float f);
LIBIMPORT void emit_func_arg_number_float1 (ASM *a, float f);
LIBIMPORT void emit_func_arg_number_float2 (ASM *a, float f);
LIBIMPORT void emit_func_arg_number_float3 (ASM *a, float f);
LIBIMPORT void emit_func_arg_number_float4 (ASM *a, float f);

LIBIMPORT void emit_func_arg_var_float0 (ASM *a, void *var);
LIBIMPORT void emit_func_arg_var_float1 (ASM *a, void *var);
LIBIMPORT void emit_func_arg_var_float2 (ASM *a, void *var);
LIBIMPORT void emit_func_arg_var_float3 (ASM *a, void *var);
LIBIMPORT void emit_func_arg_var_float4 (ASM *a, void *var);

LIBIMPORT void 		emit_call 				(ASM *a, void *func, UCHAR arg_count, UCHAR return_type);
LIBIMPORT void 		emit_sub_esp 			(ASM *a, char c); // 32/64 BITS OK
LIBIMPORT void    emit_movl_ESP     (ASM *a, long value, UCHAR index);
LIBIMPORT void    emit_mov_eax_ESP  (ASM *a, UCHAR index); // mov  %eax, 4(%esp)
LIBIMPORT void    emit_mov_edx_EAX  (ASM *a, UCHAR index); // mov  %edx, 4(%eax)
//89 43 04             	mov    %eax,0x4(%edx)
// compare / jumps:
LIBIMPORT void    emit_cmp_eax_var  (ASM *a, void *var);
LIBIMPORT void    emit_jump_jmp     (ASM *a, char *name);
LIBIMPORT void    emit_jump_je      (ASM *a, char *name);
LIBIMPORT void    emit_jump_jne     (ASM *a, char *name);
LIBIMPORT void    emit_jump_jle     (ASM *a, char *name);
LIBIMPORT void    emit_jump_jge     (ASM *a, char *name);
LIBIMPORT void    emit_jump_jg      (ASM *a, char *name);
LIBIMPORT void    emit_jump_jl      (ASM *a, char *name);

//-------------------------------------------------------------------
//###########################  MATH FLOAT  ##########################
//-------------------------------------------------------------------
//
// load a float value:
// USAGE:
//   asm_float_flds_value (a, 12345.00);
//
LIBIMPORT void emit_float_flds_value (ASM *a, float value);

//
// load a float variable:
// USAGE:
//   asm_float_flds (a, &var_name);
//
LIBIMPORT void emit_float_flds (ASM *a, void *var); //: 32/64 BITS OK

//
// multiply the stack float:
// USAGE:
//   asm_float_fmulp (a);
//
LIBIMPORT void emit_float_fmulp (ASM *a); //  fmulp  %st,%st(1)

//
// divide the stack float:
// USAGE:
//   asm_float_fmulp (a);
//
LIBIMPORT void emit_float_fdivp (ASM *a); // fdivp  %st,%st(1)

//
// add the stack float:
// USAGE:
//   asm_float_faddp (a);
//
LIBIMPORT void emit_float_faddp (ASM *a); // faddp  %st,%st(1)

//
// sub the stack float:
// USAGE:
//   asm_float_faddp (a);
//
LIBIMPORT void emit_float_fsubp (ASM *a); // fsubp  %st,%st(1)

//
// set the variable and CLEAR the stack:
// USAGE:
//   asm_float_fstps (a, &var_name);
//
LIBIMPORT void emit_float_fstps (ASM *a, void *var);

#define emit_push_float emit_float_flds_value
#define emit_add_float  emit_float_faddp
#define emit_mul_float  emit_float_fmulp
#define emit_div_float	emit_float_fdivp
#define emit_sub_float	emit_float_fsubp

//-------------------------------------------------------------------
//###########################  MATH FLOAT  ##########################
//-------------------------------------------------------------------

LIBIMPORT void emit_print_string (ASM *a, char *s);

// expression:
LIBIMPORT void asm_expression_reset (void);
LIBIMPORT void emit_pop_print_result (ASM *a, int var_type, int new_line);
LIBIMPORT void emit_expression_push_long (ASM *a, long value);
LIBIMPORT void emit_expression_push_var (ASM *a, void *var);
//
LIBIMPORT void emit_expression_add_long (ASM *a);
LIBIMPORT void emit_expression_mul_long (ASM *a); // imul %ecx, %eax
LIBIMPORT void emit_expression_div_long (ASM *a);
LIBIMPORT void emit_expression_sub_long (ASM *a);
// erro:
LIBIMPORT void 		Erro 			(char *format, ...);
LIBIMPORT char	*	ErroGet 	(void);
LIBIMPORT void 		ErroReset	(void);

#ifdef __cplusplus
}
#endif
#endif // ! _ASM_H_

