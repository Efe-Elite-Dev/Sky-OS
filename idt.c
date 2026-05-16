#include "sky_subsystem.h"

struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void keyboard_handler_asm(void);
extern void mouse_handler(void); // mouse.c dosyasına eklediğimiz yeni C köprüsü!

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = ((base >> 16) & 0xFFFF);
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_load(uint32_t ptr_address) {
    asm volatile("lidt (%0)" : : "r"(ptr_address));
}

void default_exception_handler(void) {
    asm volatile (
        "movb $0x20, %al\n"
        "outb %al, $0x20\n"
        "outb %al, $0xA0\n"
    );
}

/**
 * 🐭 KORUMALI FARE ASSEMBLY HANDLER
 * mouse.c içindeki asıl donanım motorunu tetikler ve kesme durumunu sonlandırır.
 */
__attribute__((naked)) void mouse_handler_asm(void) {
    asm volatile(
        "pushal\n"
        "cld\n"
        "call mouse_handler\n"  // mouse.c içindeki köprüyü çağırıyor, çakışma bitti!
        "movb $0x20, %al\n"
        "outb %al, $0xA0\n"
        "outb %al, $0x20\n"
        "popal\n"
        "iretl\n"
    );
}

void init_idt(void) {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, (uint32_t)default_exception_handler, 0x08, 0x8E);
    }

    asm volatile (
        "movb $0x11, %al\n" "outb %al, $0x20\n" "outb %al, $0xA0\n" 
        "movb $0x20, %al\n" "outb %al, $0x21\n"                    
        "movb $0x28, %al\n" "outb %al, $0xA1\n"                    
        "movb $0x04, %al\n" "outb %al, $0x21\n"                    
        "movb $0x02, %al\n" "outb %al, $0xA1\n"
        "movb $0x01, %al\n" "outb %al, $0x21\n" "outb %al, $0xA1\n" 
        "movb $0x00, %al\n" "outb %al, $0x21\n" "outb %al, $0xA1\n" 
    );

    idt_set_gate(33, (uint32_t)keyboard_handler_asm, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)mouse_handler_asm, 0x08, 0x8E);

    idt_load((uint32_t)&idtp);
    asm volatile("sti");
}
