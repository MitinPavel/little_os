#include "interrupts.h"
#include "io.h"

#include "serial.h"

#define INTERRUPTS_DESCRIPTOR_COUNT 256 

static struct IDTDescriptor idt_descriptors[INTERRUPTS_DESCRIPTOR_COUNT];
static struct IDT idt;

void interrupts_init_descriptor(int index, unsigned int address)
{
	idt_descriptors[index].offset_high = (address >> 16) & 0xFFFF; // offset bits 0..15
	idt_descriptors[index].offset_low = (address & 0xFFFF); // offset bits 16..31

	idt_descriptors[index].segment_selector = 0x08; // The second (code) segment selector in GDT: one segment is 64b.
	idt_descriptors[index].reserved = 0x00; // Reserved.

	/*
	   Bit:     | 31              16 | 15 | 14 13 | 12 | 11     10 9 8   | 7 6 5 | 4 3 2 1 0 |
	   Content: | offset high        | P  | DPL   | S  | D and  GateType | 0 0 0 | reserved

		P	If the handler is present in memory or not (1 = present, 0 = not present). Set to 0 for unused interrupts or for Paging.
		DPL	Descriptor Privilige Level, the privilege level the handler can be called from (0, 1, 2, 3).
		S	Storage Segment. Set to 0 for interrupt gates.
		D	Size of gate, (1 = 32 bits, 0 = 16 bits).
	*/
	idt_descriptors[index].type_and_attr = (0x01 << 7) |			// P
                                               (0x00 << 6) | (0x00 << 5) |	// DPL
                                               (0x00 << 4) |			// S
                                               0x0f;				// D + GateType 1111 (0b1111=0xf 32-bit trap gate) TODO 110?
}

void interrupts_install_idt()
{
	interrupts_init_descriptor(1, (unsigned int) interrupt_handler_1);

	idt.address = (int) &idt_descriptors;
	idt.size = sizeof(struct IDTDescriptor) * 256;
	interrupts_load_idt((int) &idt);

	interrupts_clear_imr_mask(0);
	interrupts_set_imr_mask(1);
	interrupts_clear_imr_mask(2);
	interrupts_clear_imr_mask(3);
	interrupts_clear_imr_mask(4);
	interrupts_clear_imr_mask(5);
	interrupts_clear_imr_mask(6);
	interrupts_clear_imr_mask(7);
	interrupts_clear_imr_mask(8);
	interrupts_clear_imr_mask(9);
	interrupts_clear_imr_mask(10);
	interrupts_clear_imr_mask(11);
	interrupts_clear_imr_mask(12);
	interrupts_clear_imr_mask(13);
	interrupts_clear_imr_mask(14);
	interrupts_clear_imr_mask(15);
}

/* PIC ***********************************************************************/

/*
	From: http://wiki.osdev.org/PIC

	Reinitialize the PIC controllers, giving them specified vector offsets
	rather than 8h and 70h, as configured by default.
*/

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/

void interrupts_remap_pic(int offset1, int offset2)
{
	unsigned char a1, a2;

	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();

	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

// Interrupt Mask Register (IMR)
void interrupts_set_imr_mask(unsigned char irq_line)
{
	unsigned short port;
	unsigned char value;

	if (irq_line < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		irq_line -= 8;
	}

	value = inb(port) | (1 << irq_line);
	outb(port, value);        
}

void interrupts_clear_imr_mask(unsigned char irq_line)
{
	unsigned short port;
	unsigned char value;
 

	if (irq_line < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		irq_line -= 8;
	}

	value = inb(port) & ~(1 << irq_line);
	outb(port, value);        
}


/* Interrupt handlers ********************************************************/

void interrupt_handler(struct cpu_state cpu, struct stack_state stack, unsigned int interrupt)
{
	unsigned int i = cpu.eax + stack.cs + interrupt;
        

        serial_configure_baud_rate(SERIAL_COM1_BASE, 4);
        serial_configure_line(SERIAL_COM1_BASE);
        char str[] = "hello :)\n";
        str[i] = 'a';
        serial_write(str, 9);

}

