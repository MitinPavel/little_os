#include "io.h"

/* ------------------------------------------------------------------------- */
/* ---------------------- Serial ports --------------------------------------*/
/* ------------------------------------------------------------------------- */

/* The I/O ports */

/* All the I/O ports are calculated relative to the data port. This is because
 * all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
 * order, but they start at different values.
 */

#define SERIAL_COM1_BASE                0x3F8      /* COM1 base port */

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

/* The I/O port commands */

/* SERIAL_LINE_ENABLE_DLAB:
 * Tells the serial port to expect first the highest 8 bits on the data port,
 * then the lowest 8 bits will follow
 */
#define SERIAL_LINE_ENABLE_DLAB         0x80

/** serial_configure_baud_rate:
 *  Sets the speed of the data being sent. The default speed of a serial
 *  port is 115200 bits/s. The argument is a divisor of that number, hence
 *  the resulting speed becomes (115200 / divisor) bits/s.
 *
 *  @param com      The COM port to configure
 *  @param divisor  The divisor
 */
void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
{
	outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
	outb(SERIAL_DATA_PORT(com),         (divisor >> 8) & 0x00FF);
	outb(SERIAL_DATA_PORT(com),         divisor & 0x00FF);
}

/** serial_configure_line:
 *  Configures the line of the given serial port. The port is set (0x03) to have a
 *  data length of 8 bits, no parity bits, one stop bit and break control
 *  disabled.
 *
 * d	Enables (d = 1) or disables (d = 0) DLAB (Divisor Latch Access Bit)
 * b	If break control is enabled (b = 1) or disabled (b = 0)
 * prty	The number of parity bits to use
 * s	The number of stop bits to use (s = 0 equals 1, s = 1 equals 1.5 or 2)
 * dl	Describes the length of the data
 *
 *  @param com  The serial port to configure
 */
void serial_configure_line(unsigned short com)
{
	/* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
	 * Content: | d | b | prty  | s | dl  |
	 * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
 	*/
	outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

/** serial_is_transmit_fifo_empty:
 *  Checks whether the transmit FIFO queue is empty or not for the given COM
 *  port.
 *
 *  @param  com The COM port
 *  @return 0 if the transmit FIFO queue is not empty
 *          1 if the transmit FIFO queue is empty
 */
int serial_is_transmit_fifo_empty(unsigned int com)
{
	/* 0x20 = 0010 0000 */
	return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void serial_write_char(unsigned int com, char a) 
{
	while (serial_is_transmit_fifo_empty(com) == 0);


	outb(SERIAL_DATA_PORT(com), a);
} 

void test_serial_port()
{
	serial_configure_baud_rate(SERIAL_COM1_BASE, 4);
        serial_configure_line(SERIAL_COM1_BASE);
	serial_write_char(SERIAL_COM1_BASE, 'h');
	serial_write_char(SERIAL_COM1_BASE, 'e');
	serial_write_char(SERIAL_COM1_BASE, 'l');
	serial_write_char(SERIAL_COM1_BASE, 'l');
	serial_write_char(SERIAL_COM1_BASE, 'o');
	serial_write_char(SERIAL_COM1_BASE, '\n');
}

/* ------------------------------------------------------------------------- */
/* ----------------------  Framebuffer --------------------------------------*/
/* ------------------------------------------------------------------------- */

#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

/** fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos The new position of the cursor
 */
void fb_move_cursor(unsigned short pos)
{
	outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
        outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
        outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
        outb(FB_DATA_PORT,    pos & 0x00FF);
}

/**************************************************************************/
int sum_of_three(int arg1, int arg2, int arg3)
{
	return arg1 + arg2 + arg3;
}

/********************** Framebuffer ****************************************/

#define FB_GREEN     2
#define FB_DARK_GREY 8
#define FRAMEBUFFER_ADDRESS 0x000B8000

/** fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 *
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
	char *fb = (char *) FRAMEBUFFER_ADDRESS;
	fb[i] = c;
        fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

void fb_write_simple()
{
	fb_write_cell(0, 'A', FB_GREEN, FB_DARK_GREY);
}

/***************************************************************************/

void os_main()
{
	fb_write_simple();
	test_serial_port();
}


