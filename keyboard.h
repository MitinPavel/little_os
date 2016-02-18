#ifndef INCLUDE_KEYBOARD_H
#define INCLUDE_KEYBOARD_H

static char scan_code_to_ascii[256] =
{
	0x0, 0x0, '1', '2', '3', '4', '5', '6',
	'7', '8', '9', '0', '-', '=', 0x0, 0x0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
	'o', 'p', '[', ']', '\n', 0x0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`', 0x0, '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0x0, '*',
	0x0, ' ', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, '7',
	'8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.'
};

unsigned char keyboard_read_scan_code(void);

#endif /* INCLUDE_KEYBOARD_H */
