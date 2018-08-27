//-------------------------------------------------------------------
//
// Simple Assembly Example: 64 bits:
//
// FILE:
//   simple64.s
//
// COMPILE:
//   gcc simple64.s -o simple64 -Wall
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

.globl main
main:
  push  %rbp
  mov   %rsp, %rbp
  movl  $LC0, %edi
  mov   a, %eax
  mov   %eax, %esi
  call  printf
  mov   $0, %eax
  leave
  ret
