asm func_name {
 push   %rbp
 mov    %rsp, %rbp
 sub    $48, %rsp
 mov    %ecx, 16(%rbp)
 nop
0x8b, 0x4, 0x25, 0015: push   %eax            | 0x50 
0xf8, 0x42, 0x0, 0x89, 0xc2, 0021: mov    $70, %eax       | 0xb8 0x46 0x0 0x0 0x0 
0x39, 0xc2, 0x7e, 0x10, 0030: nop                    | 0x90 
 nop
 nop
 nop
0x8b, 0x45, 0x10, 0037: push   %eax            | 0x50 
 pop    %ecx
 mov    $4201008, %eax
 call   *%eax
 leaveq
 retq
}
Exiting With Sucess:
