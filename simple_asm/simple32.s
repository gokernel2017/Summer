//-------------------------------------------------------------------
//
// Simple Assembly Example: 32 bits:
//
// FILE:
//   simple32.s
//
// COMPILE:
//   gcc simple32.s -o simple32 -Wall
//
//-------------------------------------------------------------------
//
.data

//
// declare variable:
//
.globl a
  .align 4
a:
  .long 155

//
// Labels Texts:
//
LC0:
	.ascii "Value a: %d\12\0"

//
// Section Text: ... functions here:
//
.text

.globl _main
_main:
  push  %ebp
  mov   %esp, %ebp
  sub   $100, %esp
  mov   a, %eax
  movl  $LC0, (%esp)
  mov   %eax, 4(%esp)
  call  _printf
  mov   $0, %eax
  leave
  ret

