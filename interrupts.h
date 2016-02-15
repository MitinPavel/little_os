#ifndef INCLUDE_INTERRUPTS
#define INCLUDE_INTERRUPTS

/*                      I/O port */
#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

struct IDT 
{
	unsigned short size;
	unsigned int address;
} __attribute__((packed));

struct IDTDescriptor {
	/* The lowest 32 bits */
	unsigned short offset_low; // offset bits 0..15
	unsigned short segment_selector; // a code segment selector in GDT or LDT
	
	/* The highest 32 bits */
	unsigned char reserved; // Just 0.
	unsigned char type_and_attr; // type and attributes
	unsigned short offset_high; // offset bits 16..31
} __attribute__((packed));

void interrupts_install_idt();

void interrupts_remap_pic(int offset1, int offset2);

// Wrappers around ASM.
void interrupts_load_idt(unsigned int idt_address);
void interrupt_handler_33();

struct cpu_state {
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebp; 
	unsigned int esi; 
	unsigned int edi; 
} __attribute__((packed));

struct stack_state {
	unsigned int error_code;
	unsigned int eip;
	unsigned int cs;
	unsigned int eflags;
} __attribute__((packed));

void interrupt_handler(struct cpu_state cpu, unsigned int interrupt, struct stack_state stack);


#endif /* INCLUDE_INTERRUPTS */

