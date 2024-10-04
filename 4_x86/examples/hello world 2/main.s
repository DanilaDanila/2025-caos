.intel_syntax noprefix

.global _start

.set SYSCALL_WRITE, 1
.set STDOUT, 1
.set SYSCALL_EXIT, 60
.set MAGIC_EXIT_CODE, 152


.data
    msg: .string "omg what is going!?\n"
    msg_len: .long $ - msg



.text

_start:
    // write(STDOUT, buffer, count)
    mov rax, SYSCALL_WRITE
    mov rdi, STDOUT
    lea rsi, [rip+msg]
    mov rdx, [rip+msg_len]
    syscall

    mov rax, SYSCALL_EXIT
    mov rdi, MAGIC_EXIT_CODE
    syscall

