#include "interrupts.h"
#include "io.h"

#include "serial.h"
#include "keyboard.h"

#define INTERRUPTS_DESCRIPTOR_COUNT 256 

#define INTERRUPTS_PIC1_OFFSET 0x20
#define INTERRUPTS_PIC2_OFFSET 0x28
#define INTERRUPTS_PIC2_END INTERRUPTS_PIC2_OFFSET + 7

#define INTERRUPTS_PIC1_COMMAND_PORT 0x20
#define INTERRUPTS_PIC2_COMMAND_PORT 0xA0
#define INTERRUPTS_PIC_ACKNOWLEDGE 0x20

#define INTERRUPTS_KEYBOARD 33 

struct IDTDescriptor idt_descriptors[INTERRUPTS_DESCRIPTOR_COUNT];
struct IDT idt;

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
	idt_descriptors[index].type_and_attr =	(0x01 << 7) |			// P
						(0x00 << 6) | (0x00 << 5) |	// DPL
						0xe;				// 0b1110=0xE 32-bit interrupt gate
}

void interrupts_install_idt()
{
	interrupts_init_descriptor(INTERRUPTS_KEYBOARD, (unsigned int) interrupt_handler_33);


	idt.address = (int) &idt_descriptors;
	idt.size = sizeof(struct IDTDescriptor) * INTERRUPTS_DESCRIPTOR_COUNT;
	interrupts_load_idt((int) &idt);

	interrupts_remap_pic(INTERRUPTS_PIC1_OFFSET, INTERRUPTS_PIC2_OFFSET);
}

/* PIC ***********************************************************************/

/*
	From: http://wiki.osdev.org/PIC

	Reinitialize the PIC controllers, giving them specified vector offsets
	rather than 8h and 70h, as configured by default.
*/

#define INTERRUPTS_ICW1_ICW4		0x01		/* ICW4 (not) needed */
#define INTERRUPTS_ICW1_SINGLE		0x02		/* Single (cascade) mode */
#define INTERRUPTS_ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define INTERRUPTS_ICW1_LEVEL		0x08		/* Level triggered (edge) mode */
#define INTERRUPTS_ICW1_INIT		0x10		/* Initialization - required! */

#define INTERRUPTS_ICW4_8086		0x01		/* 8086/88 (MCS-80/85) mode */
#define INTERRUPTS_ICW4_AUTO		0x02		/* Auto (normal) EOI */
#define INTERRUPTS_ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define INTERRUPTS_ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define INTERRUPTS_ICW4_SFNM		0x10		/* Special fully nested (not) */

/** pic_acknowledge:
     *  Acknowledges an interrupt from either PIC 1 or PIC 2.
     *
     *  @param num The number of the interrupt
     */
void interrupts_pic_acknowledge(unsigned int interrupt)
{
	if (interrupt < INTERRUPTS_PIC1_OFFSET || interrupt > INTERRUPTS_PIC2_END) {
		return;
	}

	if (interrupt < INTERRUPTS_PIC2_OFFSET) {
		outb(INTERRUPTS_PIC1_COMMAND_PORT, INTERRUPTS_PIC_ACKNOWLEDGE);
	} else {
		outb(INTERRUPTS_PIC2_COMMAND_PORT, INTERRUPTS_PIC_ACKNOWLEDGE);
	}
}

/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/

void interrupts_remap_pic(int offset1, int offset2)
{
	outb(PIC1_COMMAND, INTERRUPTS_ICW1_INIT+INTERRUPTS_ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	outb(PIC2_COMMAND, INTERRUPTS_ICW1_INIT+INTERRUPTS_ICW1_ICW4);
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)

	outb(PIC1_DATA, INTERRUPTS_ICW4_8086);
	outb(PIC2_DATA, INTERRUPTS_ICW4_8086);

        // Setup Interrupt Mask Register (IMR)
	outb(PIC1_DATA, 0xFD); // 1111 1101 - Enable IRQ 1 only (keyboard).
	outb(PIC2_DATA, 0xFF);

	asm("sti"); // Enable interrupts.
}

/* Interrupt handlers ********************************************************/

void interrupt_handler(__attribute__((unused)) struct cpu_state cpu, unsigned int interrupt, __attribute__((unused)) struct stack_state stack)
{
	unsigned char scan_code;
	unsigned char ascii;

	switch (interrupt){
		case INTERRUPTS_KEYBOARD:

			scan_code = keyboard_read_scan_code();

			if (scan_code <= KEYBOARD_MAX_ASCII) {
				ascii = keyboard_scan_code_to_ascii[scan_code];
				serial_configure_baud_rate(SERIAL_COM1_BASE, 4);
				serial_configure_line(SERIAL_COM1_BASE);
				char str[1];
				str[0] = ascii;
				serial_write(str, 1);
			}

			interrupts_pic_acknowledge(interrupt);

			break;
		default:
			break;
    }
}

