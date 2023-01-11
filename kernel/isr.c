#include "isr.h"
#include "tty.h"
#include "io.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler(uint8_t n, isr_t handler)
{
	interrupt_handlers[n] = handler;
}

void isr_handler(registers_t *regs)
{
	if (interrupt_handlers[regs->int_no]) {
		isr_t handler = interrupt_handlers[regs->int_no];
		handler(*regs);
	} else {
		printk("unhandled interrupt: %d\n", regs->int_no);
	}
}
