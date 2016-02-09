#include "interrupts.h"

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
	interrupts_init_descriptor(1, 1);

	idt.address = (int) &idt_descriptors;
	idt.size = sizeof(struct IDTDescriptor) * 256;
	interrupts_load_idt((int) &idt);
}

