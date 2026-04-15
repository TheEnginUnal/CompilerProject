.data
x:
    .quad 5

.data
y:
    .quad 0

.text
.global main
main:
    pushq %rbp
    movq %rsp, %rbp

    movq $5, %r10
    movq %r10, x
    movq $0, %r10
    movq %r10, y
    movq x, %r10
    movq $3, %r11
    imulq %r11, %r10
    movq $10, %r11
    addq %r11, %r10
    movq %r10, y
    movq y, %r10
    movq $20, %r11
    cmpq %r11, %r10
    setg %al
    movzbq %al, %r10
    cmpq $0, %r10
    je .L0
    movq y, %r10
    movq %r10, %rdi
    pushq %r10
    pushq %r11
    call print_integer
    popq %r11
    popq %r10
    jmp .L1
.L0:
    movq x, %r10
    movq %r10, %rdi
    pushq %r10
    pushq %r11
    call print_integer
    popq %r11
    popq %r10
.L1:

    movq $0, %rax
    popq %rbp
    ret
