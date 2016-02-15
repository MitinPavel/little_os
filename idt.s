global  interrupts_load_idt

; Loads the interrupt descriptor table (IDT).
interrupts_load_idt:
	mov eax, [esp + 4]
	lidt [eax]
	ret

