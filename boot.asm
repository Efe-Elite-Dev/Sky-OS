; Sky-OS Saf Çekirdek Önyükleyici Tanımı
MBALIGN  equ  1 << 0            ; Hizalama kuralları
MEMINFO  equ  1 << 1            ; Bellek haritasını oku
FLAGS    equ  MBALIGN | MEMINFO ; Multiboot bayrakları
MAGIC    equ  0x1BADB002        ; İşlemcinin tanıyacağı sihirli numara
CHECKSUM equ -(MAGIC + FLAGS)   ; Kontrol toplamı

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bootstrap_stack, nobits
align 16
stack_bottom:
    resb 16384 ; Çekirdek için 16 KB'lık saf RAM alanı ayırıyoruz
stack_top:

section .text
global _start
extern kernel_main

_start:
    mov esp, stack_top
    call kernel_main ; İşte burası! Bizim C dilindeki ana motora zıplıyoruz.

    cli
.hang:
    hlt
    jmp .hang
