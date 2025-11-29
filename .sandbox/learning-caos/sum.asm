; Функция sum принимает два int32_t аргумента:
; int sum(int a, int b)
; Аргументы: a в edi, b в esi (системный V calling convention для x86-64/Linux)
; Возвращает: сумму в eax

section .text
    global sum                ; экспортируем функцию для линковки

sum:
    mov rax, rdi              ; первый аргумент (a) — в eax (возврат)
    add rax, rsi              ; второй аргумент (b) — прибавляем к eax
    add rax, rsi              ; второй аргумент (b) — прибавляем к eax
    ret
