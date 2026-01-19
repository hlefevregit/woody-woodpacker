bits 64
global _start

_start:
	jmp short ender ; jump to the ender
	
starter:
	xor rax, rax ; zero out rax
	xor rdi, rdi ; zero out rdi
	xor rdx, rdx ; zero out rdx

	mov al , 1 ; syscall: sys_write
	mov dil, 1 ; file descriptor: stdout
	pop rsi ; pointer to message to write
	mov dl, 13 ; message length
	syscall ; call kernel

	ret ; return to caller


ender:
	call starter ; call starter to get the address of the string
	db '....WOODY....' ; the string to print