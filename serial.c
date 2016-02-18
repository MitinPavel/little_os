#include "io.h"
#include "serial.h"

/* ------------------------------------------------------------------------- */
/* ---------------------- Serial ports --------------------------------------*/
/* ------------------------------------------------------------------------- */

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

void serial_write(const char* data, int len)
{
	int i = 0;
	for (i = 0; i < len; i++) {
		serial_write_char(SERIAL_COM1_BASE, data[i]);
	}
}
 

void test_serial_port()
{
	serial_configure_baud_rate(SERIAL_COM1_BASE, 4);
        serial_configure_line(SERIAL_COM1_BASE);

	char str[] = "test serial port\n";
	serial_write(str, 17);
}


