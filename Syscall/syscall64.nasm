global main

SECTION .data 

msg: db "Hello world from x86_64 machine" , 0Ah, 0h
len_msg: equ $ - msg

SECTION .text
main:
	mov rax,1  ;pass the write id num
	mov rdi,1  ;select the output file
	mov rsi,msg
	mov rdx,len_msg
	syscall ; to switch to kernal mode and call write

	mov r15,rax

	mov rax,60
	mov rdi,r15
	syscall

