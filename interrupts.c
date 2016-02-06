#include "interrupts.h"

#define INTERRUPTS_DESCRIPTOR_COUNT 256 

static struct IDTDescriptor idt_descriptors[INTERRUPTS_DESCRIPTOR_COUNT];

void interrupts_install_idt()
{
	idt_descriptors[0].offset_low = 1;
}

