.intel_syntax noprefix

.global main

.text

twice:
    mov rax, rdi
    add rax, rdi
    ret


main:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	lea rdi, [rip + read_int]
	lea rsi, [rbp - 4]
	call scanf

	mov rdi, [rbp - 4]
	call twice

	lea rdi, [rip + write_twice]
	mov rsi, [rbp - 4]
	// twice
	mov rdx, rax
	call printf

	add rsp, 16
	pop rbp
	mov rax, 0
	ret

.data
read_int: .asciz "%d"
write_twice: .asciz "%d * 2 = %d\n"

