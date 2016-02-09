global  interrupts_load_idt

; Loads the interrupt descriptor table (IDT).
interrupts_load_idt:
        lidt [esp + 4]
        ret

